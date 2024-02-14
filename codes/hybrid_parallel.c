#include <mpi.h>
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

// void read_dataset(float percentage, int rank, int size) {

//     FILE* file = fopen("iris_noisy_2.csv","r");
//     if (file == NULL) {
//         fprintf(stderr, "Error opening file.\n");
//         MPI_Abort(MPI_COMM_WORLD, 1);
//     }

//     int partial_nr_ROWS = (int)(ROWS * percentage / size);
//     char buffer[130];
//     int row = 0;
//     int count = 0;
    
//     #pragma omp parallel
//     while (fgets(buffer, 130, file)) {
//         if (row % size == rank) {
//             char *token = strtok(buffer, ",");
//             int col = 0;
//             while (token) {
//                 float n = atof(token);
//                 dataset[row / size][col] = n;
//                 token = strtok(NULL, ",");
//                 col++;
//             }
//             count++;
//         }
//         if (count >= partial_nr_ROWS) break;
//         row++;
//     }
//     // Since each process has access to the dataset variable, 
//     // each process can directly store the values it reads in the correct position of the matrix
//     // without needing to gather data from other processes.
//     fclose(file);
// }

void read_dataset(float percentage, int rank, int size) {
    FILE* file = fopen("iris_noisy_2.csv","r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int partial_nr_ROWS = (int)(ROWS * percentage / size);
    char buffer[130];
    int row = 0;
    int count = 0;
    int finished = 0; // Flag condiviso per terminare il loop

    // Calcoliamo il numero di righe da elaborare per ciascun processo MPI
    int start_row = rank * partial_nr_ROWS;
    int end_row = (rank + 1) * partial_nr_ROWS;

    // Leggiamo e processiamo le righe del file in modo parallelo con OpenMP
    #pragma omp parallel default(none) shared(file, start_row, end_row, rank, size, count, finished, dataset) private(buffer, row)
    {
        #pragma omp for schedule(static)
        for (row = start_row; row < end_row; row++) {
            if (!finished) { // Verifica del flag condiviso
                if (fgets(buffer, 130, file) == NULL) {
                    #pragma omp critical
                    finished = 1; // Imposta il flag se il file è finito
                }
                else {
                    char *token = strtok(buffer, ",");
                    int col = 0;
                    while (token) {
                        float n = atof(token);
                        dataset[row / size][col] = n;
                        token = strtok(NULL, ",");
                        col++;
                    }
                    #pragma omp atomic
                    count++;
                }
            }
        }
    }

    fclose(file);
}


double euclidean_distance(int a, int b) {
    double sum = 0;
    #pragma omp parallel for
    for (int i = 0; i < COLS; i++) {
        sum += pow(dataset[a][i] - centroids[b][i], 2);
    }
    return sqrt(sum);}

void kmeans(int rank, int size, double *sum_iter_kmeans_time) {
    
    double t1, t2, t_per_iter;

    // Initialize centroids randomly
    if (rank == 0) {
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < K; i++) {
            for (int j = 0; j < COLS; j++) {
                centroids[i][j] = dataset[rand() % ROWS][j];
            }
        }
    }

    // Broadcast initial centroids to all processes
    MPI_Bcast(centroids, K*COLS, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    int iter = 10;
    int tt = 0;   
    //int changed = 1;

    t1 = MPI_Wtime();

    while (tt<iter) {
        // Assign points to closest centroid
        int start = rank * ROWS / size;
        int end = (rank + 1) * ROWS / size;

        #pragma omp parallel for
        for (int i = start; i < end; i++) {
            double min_distance = DBL_MAX;
            int min_index = 0;
            for (int j = 0; j < K; j++) {
                double distance = euclidean_distance(i, j);

                #pragma omp critical
                if (distance < min_distance) {
                    min_distance = distance;
                    min_index = j;
                }
            }

            #pragma omp critical
            if (cluster[i] != min_index) {
                cluster[i] = min_index;
            }
        }

        // Gather cluster assignments from all processes
        // int global_changed;
        // MPI_Allreduce(&changed, &global_changed, 1, MPI_INT, MPI_LOR,
        //               MPI_COMM_WORLD);
        // changed = global_changed;

        // Recompute centroids
        
        double local_sum[K][COLS] = {{0}};
        int local_count[K] = {0};

        #pragma omp parallel for
        for (int i=start;i<end;i++){
            for(int j=0;j<COLS;j++){
                #pragma omp critical
                local_sum[cluster[i]][j]+=dataset[i][j];
            }
            #pragma omp critical
            local_count[cluster[i]]++;
        }

        double global_sum[K][COLS]={{0}};
        int global_count[K]={0};

        MPI_Allreduce(local_sum,
                    global_sum,
                    K*COLS,
                    MPI_DOUBLE,
                    MPI_SUM,
                    MPI_COMM_WORLD);

        
        MPI_Allreduce(local_count,
                    global_count,
                    K,
                    MPI_INT,
                    MPI_SUM,
                    MPI_COMM_WORLD);

        #pragma omp parallel for collapse(2)
        for (int i = 0; i < K; i++) {
            if (global_count[i] != 0) {
                    centroids[i][j] = global_sum[i][j] / global_count[i];
            }
        }

        FILE *fi = fopen("centroids.csv", "w");
        if (fi == NULL) {
            printf("Failed to open the file for writing.");
            
        }

        // Write the list elements to the file
        #pragma omp parallel for
        for(int i = 0;i<K;i++){
            for(int j = 0;j<=3;j++){
                fprintf(fi," %f, ",centroids[i][j]);
            }
            fprintf(fi,"\n");
        }

        // Close the file
        fclose(fi);   
        tt++;
    }
    t2 = (MPI_Wtime()-t1);
    t_per_iter = t2 / iter;
    
    MPI_Allreduce(&t_per_iter, sum_iter_kmeans_time, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
}

int main(int argc, char **argv) {
    double t1, t2, t3;
    double reading_time, kmean_time, total_time;
    double max_reading, max_kmeans, max_total;

    double sum_iter_kmeans_time;
    double avg_iter_kmeans_time;

    int rank, size;
    
    const char *nomeFile = "Time Results Hybrid.csv";
    eliminaContenutoCSV(nomeFile);

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    
    // for (float i = 0.10; i < 1.1; i+=0.10){   
    //     int partial_nr_ROWS = (int)(ROWS * i);

    //     time1 = MPI_Wtime();

    //     read_dataset(i, rank, size);

    //     time2 = MPI_Wtime();

    //     kmeans(rank, size, &sum_iter_kmeans_time);
        
    //     time3 = MPI_Wtime();

    //     reading_time = time2 - time1;
    //     kmean_time = time3 - time2;
    //     total_time = time3 - time1;

    //     // Using MPI_Reduce in order to calculate il max time for each phase
    //     MPI_Reduce( &reading_time,
    //                 &max_reading,
    //                 1,
    //                 MPI_DOUBLE,
    //                 MPI_MAX,
    //                 0,
    //                 MPI_COMM_WORLD);
    //     MPI_Reduce( &kmean_time,
    //                 &max_kmeans,
    //                 1,
    //                 MPI_DOUBLE,
    //                 MPI_MAX,
    //                 0,
    //                 MPI_COMM_WORLD);
    //     MPI_Reduce( &total_time,
    //                 &max_total,
    //                 1,
    //                 MPI_DOUBLE,
    //                 MPI_MAX,
    //                 0,
    //                 MPI_COMM_WORLD);
        
    //     if (rank==0){
    //         avg_iter_kmeans_time = (sum_iter_kmeans_time / size);
    //         printf("\n---------- PERCENTAGE OF DATABASE: %f ----------\n", i);
    //         printf("\nI/O time (s): %f", max_reading);
    //         printf("\nK-means algorithm time (s): %f", max_kmeans);
    //         printf("\nTotal time (s): %f", max_total);
    //         printf("\nAverage iteration time in k-means (s): %f", avg_iter_kmeans_time);
    //         salvaTempisticheCSV(nomeFile, partial_nr_ROWS, max_reading, max_kmeans, max_total, sum_iter_kmeans_time);
    //     }
    // }

    #pragma omp parallel
    for (int i = 1; i < 11; i++){
        float x = 0.10*i;  
        int partial_nr_ROWS = (int)(ROWS * x);

        t1 = MPI_Wtime();

        read_dataset(x, rank, size);

        t2 = MPI_Wtime();

        kmeans(rank, size, &sum_iter_kmeans_time);
        
        t3 = MPI_Wtime();

        #pragma omp barrier
        reading_time = t2 - t1;
        kmean_time = t3 - t2;
        total_time = t3 - t1;

        // Using MPI_Reduce in order to calculate il max time for each phase
        MPI_Reduce( &reading_time,
                    &max_reading,
                    1,
                    MPI_DOUBLE,
                    MPI_MAX,
                    0,
                    MPI_COMM_WORLD);
        MPI_Reduce( &kmean_time,
                    &max_kmeans,
                    1,
                    MPI_DOUBLE,
                    MPI_MAX,
                    0,
                    MPI_COMM_WORLD);
        MPI_Reduce( &total_time,
                    &max_total,
                    1,
                    MPI_DOUBLE,
                    MPI_MAX,
                    0,
                    MPI_COMM_WORLD);
        
        if (rank==0){
            avg_iter_kmeans_time = (sum_iter_kmeans_time / size);
            printf("\n---------- PERCENTAGE OF DATABASE: %f ----------\n", x);
            printf("\nI/O time (s): %f", max_reading);
            printf("\nK-means algorithm time (s): %f", max_kmeans);
            printf("\nTotal time (s): %f", max_total);
            printf("\nAverage iteration time in k-means (s): %f", avg_iter_kmeans_time);
            salvaTempisticheCSV(nomeFile, partial_nr_ROWS, max_reading, max_kmeans, max_total, sum_iter_kmeans_time);
        }
    }
    MPI_Finalize();
    return 0;
}
