#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>

#define ROWS 9730
#define COLS 4
#define K 3

double dataset[ROWS][COLS];
int cluster[ROWS];
double centroids[K][COLS];

void salvaTempisticheCSV(const char *nomeFile, int numeroDati, double ioTime, double kmeansTime, double totTime, double avgIterTime) {
    FILE *file = fopen(nomeFile, "a");
    if (file == NULL) {
        printf("Impossible to open the file %s.\n", nomeFile);
        return;
    }
    //fprintf(file, "#Data,I/O Time,Kmeans Time,Total Time\n");
    fprintf(file, "%d, %.6lf, %.6lf, %.6lf, %.6lf \n", numeroDati, ioTime, kmeansTime, totTime, avgIterTime);
    fclose(file);
}

void eliminaContenutoCSV(const char *nomeFile) {

    FILE *file = fopen(nomeFile, "r");
    if (file == NULL) {
        printf("Impossible to open the file %s.\n", nomeFile);
        return;
    }

    char primaRiga[1024];
    if (fgets(primaRiga, sizeof(primaRiga), file) == NULL) {
        printf("Errore nella lettura della prima riga.\n");
        fclose(file);
        return;
    }
    fclose(file);

    file = fopen(nomeFile, "w");
    if (file == NULL) {
        printf("Impossibile aprire il file %s in modalità di sovrascrittura.\n", nomeFile);
        return;
    }

    fprintf(file, "%s", primaRiga);
    fclose(file);
}

void read_dataset(float percentage) {
    FILE* file = fopen("iris_noisy_2.csv","r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        exit(1);
    }

    int partial_nr_ROWS = (int)(ROWS * percentage);
    char buffer[130];
    int row = 0;
    int count = 0;

    // Leggiamo e processiamo le righe del file in modo parallelo con OpenMP
    #pragma omp parallel default(none) shared(file, partial_nr_ROWS, count, dataset) private(buffer, row)
    {
        int tid = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        // Calcoliamo quale riga deve essere letta da questo thread
        int start_row = tid * partial_nr_ROWS / num_threads;
        int end_row = (tid + 1) * partial_nr_ROWS / num_threads;

        // Spostiamo la posizione del file per ogni thread
        for (int i = 0; i < start_row; i++) {
            if (fgets(buffer, 130, file) == NULL) {
                break;
            }
        }

        // Leggiamo il file e memorizziamo i dati
        for (row = start_row; row < end_row; row++) {
            if (fgets(buffer, 130, file) == NULL) {
                break;
            }
            char *token = strtok(buffer, ",");
            int col = 0;
            while (token) {
                float n = atof(token);
                dataset[row][col] = n;
                token = strtok(NULL, ",");
                col++;
            }
            #pragma omp atomic
            count++;
        }
    }

    fclose(file);
}

// double euclidean_distance(double *a, double *b) {
//     double sum = 0;
//     for (int i = 0; i < COLS; i++) {
//         sum += pow(a[i] - b[i], 2);
//     }
//     return sqrt(sum);
// }

double euclidean_distance(int a, int b) {
    double sum = 0;
    for (int i = 0; i < COLS; i++) {
        sum += pow(dataset[a][i] - centroids[b][i], 2);
        if (b == 3) {
            printf("\n%f\n", sum);
        }
        //printf(" %f ", centroids[3][i]);
        //printf("\n");
    }
    if (sum < 0) {
        printf("CIAO ERRORE!!!!");
    }
    return sqrt(sum);
}

void kmeans(double *sum_iter_kmeans_time) {

    // Initialize centroids randomly
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < COLS; j++) {
            centroids[i][j] = dataset[rand() % ROWS][j];
        }
    }
    
    //Iterations
    int iter = 10;
    int tt = 0;

    // All timers
    double time1 = 0;
    double time2 = 0;
    double max_t = 0;

    //Threads ids
    int tid = 0;
    int id_max = 0;

    #pragma omp parallel
    while (tt<iter) {

        // Start the timer in order to calculate how a thread takes to compute an iteration of the algorithm
        time1 = omp_get_wtime();

        // Assign points to closest centroid
        #pragma omp for schedule(static, ROWS/omp_get_num_threads()) 
        for (int i = 0; i < ROWS; i++) {
            double min_distance = DBL_MAX;
            int min_index = 0;
            
            for (int j = 0; j < K; j++) {
                //double distance = euclidean_distance(dataset[i], centroids[j]);
                double distance = euclidean_distance(i, j);
                if (distance < min_distance) {
                    min_distance = distance;
                    min_index = j;
                }
            }
            if (cluster[i] != min_index) {
                cluster[i] = min_index;
            }
        }

        // Recalculate centroids
        int count[K] = {0};
        double sum[K][COLS] = {{0}};

        #pragma omp parallel
        {
            int count_private[K] = {0}; // Private copy of the count array for each thread
            double sum_private[K][COLS] = {{0}}; // Private copy of the sum array for each thread

            #pragma omp for
            for (int i = 0; i < ROWS; i++) {
                count_private[cluster[i]]++;
                for (int j = 0; j < COLS; j++) {
                    sum_private[cluster[i]][j] += dataset[i][j];
                }
            }

            #pragma omp critical // Enter a critical section to merge the private arrays into the global arrays
            {
                for (int i = 0; i < K; i++) {
                    count[i] += count_private[i];
                    for (int j = 0; j < COLS; j++) {
                        sum[i][j] += sum_private[i][j];
                    }
                }
            }
        }

        #pragma omp single
        for (int i = 0; i < K; i++) {
            if (count[i] > 0) {
                for (int j = 0; j < COLS; j++) {
                    centroids[i][j] = sum[i][j] / count[i];
                }
            }
        }
        tt++;
        time2 = (omp_get_wtime() - time1) / iter;
        *sum_iter_kmeans_time += time2;
        // Get the id of the thread (used only to indentify the thread that takes more time)
        tid = omp_get_thread_num();

        // Calculate the max time
        #pragma omp single
        if(time2 > max_t){
            max_t = time2;
            id_max = tid;
        }
    } 
    // Write the final centroids to a file
    FILE *fi = fopen("centroids.csv", "w");
    if (fi == NULL) {
        printf("Failed to open the file for writing.");
    }

    // Write the list elements to the file
    for(int i = 0;i<K;i++){
        for(int j = 0;j<COLS;j++){
            fprintf(fi," %f, ",centroids[i][j]);
        }
        fprintf(fi,"\n");
    }
    // Close the file
    fclose(fi); 
    
}

int main(int argc, char **argv) {
    double t1, t2, t3;
    double reading_time, kmean_time, total_time;
    printf("CIAOOOO1");
    // double max_reading = 0;
    // double max_kmeans = 0;
    // double max_total = 0;

    double sum_iter_kmeans_time;
    double avg_iter_kmeans_time;

    int num_threads;

    const char *nomeFile = "Time Results OMP.csv";
    eliminaContenutoCSV(nomeFile);

    //omp_set_dynamic(0);
    omp_set_num_threads(4);
    //int iter; 
    

    #pragma omp parallel private(t1, t2, t3)
    for (int i = 10; i < 11; i++){
        float x = 0.10*i;
        int partial_nr_ROWS = (int)(ROWS * x);

        num_threads = omp_get_num_threads();

        t1 = omp_get_wtime();
        printf("CIAOOOO1");
        read_dataset(x);

        t2 = omp_get_wtime();
        printf("CIAOOOO2");
        kmeans(&sum_iter_kmeans_time);

        t3 = omp_get_wtime();

        #pragma omp barrier
        reading_time = t2 - t1;
        kmean_time = t3 - t2;
        total_time = t3 - t1;

        
        // // Getting higher time from each threads:
        // // I/O Time
        
        // if(reading_time > max_reading){
        //     max_reading = reading_time;
        // }
        // // Kmeans Time
        
        // if(kmean_time > max_kmeans){
        //     max_kmeans = kmean_time;
        // }
        // // Total Time
       
        // if(total_time > max_total){
        //     max_total = total_time;
        // }
        // AVG Iter Kmeans Time
        
        avg_iter_kmeans_time = sum_iter_kmeans_time / num_threads;

        #pragma omp single 
        {
        printf("\n---------- PERCENTAGE OF DATABASE: %f ----------\n", x);
        printf("\nNumber of threads: %d", num_threads);
        printf("\nThe I/O PHASE took %f seconds", reading_time);
        printf("\nThe K-MEANS ALGORITHM took %f seconds", kmean_time);
        printf("\nThe COMPLETE ALGORITHM took %f seconds\n", total_time);
        salvaTempisticheCSV(nomeFile, partial_nr_ROWS, reading_time, kmean_time, total_time, avg_iter_kmeans_time);
        }    
    }
    return 0;
}
