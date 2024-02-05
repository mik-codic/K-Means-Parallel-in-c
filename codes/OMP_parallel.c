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

void salvaTempisticheCSV(const char *nomeFile, int numeroDati, double ioTime, double kmeansTime, double totTime) {
    FILE *file = fopen(nomeFile, "a");
    if (file == NULL) {
        printf("Impossible to open the file %s.\n", nomeFile);
        return;
    }
    //fprintf(file, "#Data,I/O Time,Kmeans Time,Total Time\n");
    fprintf(file, "%d,%.6lf,%.6lf,%.6lf\n", numeroDati, ioTime, kmeansTime, totTime);
    fclose(file);
}

void eliminaContenutoCSV(const char *nomeFile) {

    FILE *file = fopen(nomeFile, "r");
    if (file == NULL) {
        printf("Impossibile aprire il file %s.\n", nomeFile);
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
    int partial_nr_ROWS = (int)(ROWS * percentage);
    char buffer[130];
    int row = 0;
    while (fgets(buffer, 130, file)) {
        
        // get all the values in a row
        char *token = strtok(buffer, ",");
        int col = 0;
        while (token) {
            
            // Just printing each integer here but handle as needed
            float n = atof(token);
            
            dataset[row][col] = n;
            token = strtok(NULL, ",");
            col++;          
        }
        if (row == partial_nr_ROWS) {
            break;
        }
        row++;
    }
    fclose(file);
}

double euclidean_distance(double *a, double *b) {
    double sum = 0;
    for (int i = 0; i < COLS; i++) {
        sum += pow(a[i] - b[i], 2);
    }
    return sqrt(sum);
}

void kmeans() {

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
            int min_index = -1;
            for (int j = 0; j < K; j++) {
                double distance = euclidean_distance(dataset[i], centroids[j]);
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
        
        // Get the id of the thread (used only to indentify the thread that takes more time)
        tid = omp_get_thread_num();

        printf("\nA single thread (nr: %d) takes %f seconds to complete the k-means algorithm.", tid, time2);

        // Calculate the max time
        #pragma omp single
        if(time2 > max_t){
            max_t = time2;
            id_max = tid;
        }
    }
    printf("\n\n[MAX] A single thread (nr: %d) takes %f seconds to complete the k-means algorithm.\n", id_max, max_t);
 
    // Write the final centroids to a file
    FILE *fi = fopen("centroids.csv", "w");
    if (fi == NULL) {
        printf("Failed to open the file for writing.");
    }

    // Write the list elements to the file
    for(int i = 0;i<K;i++){
        for(int j = 0;j<=3;j++){
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
    int nr;

    const char *nomeFile = "Time Results OMP.csv";
    eliminaContenutoCSV(nomeFile);

    omp_set_dynamic(0);
    omp_set_num_threads(4);
    //int iter;

    for (float i = 0.10; i < 1.1; i+=0.10){
        int partial_nr_ROWS = (int)(ROWS * i);
        t1 = omp_get_wtime();

        read_dataset(i);

        t2 = omp_get_wtime();

        kmeans();

        t3 = omp_get_wtime();

        reading_time = t2 - t1;
        kmean_time = t3 - t2;
        total_time = t3 - t1;

        nr = omp_get_num_threads();
        printf("\nNumber of threads: %d \n", nr);
        printf("\nThe I/O PHASE took %f seconds\n", reading_time);
        printf("\nThe K-MEANS ALGORITHM took %f seconds\n", kmean_time);
        printf("\nThe COMPLETE ALGORITHM took %f seconds\n", total_time);
        salvaTempisticheCSV(nomeFile, partial_nr_ROWS, reading_time, kmean_time, total_time);
    }

    printf("\n\n\n-------------------PRINTING ALL POINTS AND THEIR ASSIGNED CLUSTER-------------------\n\n\n");


    // Let's see how the clusters are populated!
    // int c = 0;
    // int g = 0;
    // int f = 0;
    // for (int i = 0; i < ROWS; i++) {
    //     printf("Point: ");
    //     for (int j = 0; j < COLS; j++) {
    //         printf("%f ", dataset[i][j]);
    //     }
    //     printf("Cluster: %d\n", cluster[i]);
    //     if(cluster[i] == 0){
    //         c++;
    //     }
    //     if(cluster[i] == 1){
    //         g++;
    //     }
    //     if(cluster[i] == 2){
    //         f++;
    //     }

    // }
    //printf("cluster 0: %d, cluster 1: %d, cluster 2: %d",c,g,f);
    return 0;
}
