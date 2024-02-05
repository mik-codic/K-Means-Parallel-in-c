#include <mpi.h>
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


double euclidean_distance(int a, int b) {
    double sum = 0;
    for (int i = 0; i < COLS; i++) {
        sum += pow(dataset[a][i] - centroids[b][i], 2);
    }
    return sqrt(sum);}

void kmeans(int rank, int size) {
    
    double t1, t2;
    int rd = 0;

    // Initialize centroids randomly
    if (rank == 0) {
        
        for (int i = 0; i < K; i++) {
            rd = rand()%ROWS;
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
        //printf("%d` iteration of the kmeans",tt);
        

        // Assign points to closest centroid
        int start = rank * ROWS / size;
        int end = (rank + 1) * ROWS / size;
        for (int i = start; i < end; i++) {
            double min_distance = DBL_MAX;
            int min_index = 0;
            for (int j = 0; j < K; j++) {
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

        // Gather cluster assignments from all processes
        // int global_changed;
        // MPI_Allreduce(&changed, &global_changed, 1, MPI_INT, MPI_LOR,
        //               MPI_COMM_WORLD);
        // changed = global_changed;

        // Recompute centroids
        
        double local_sum[K][COLS] = {{0}};
        int local_count[K] = {0};

        for (int i=start;i<end;i++){
            for(int j=0;j<COLS;j++){
                local_sum[cluster[i]][j]+=dataset[i][j];
                }
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

          for (int i = 0; i < K; i++) {
              for (int j = 0; j < COLS; j++) {
                  centroids[i][j] = global_sum[i][j] / global_count[i];
              }
          }
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
        tt++;
    }
    t2 = (MPI_Wtime()-t1) / iter;

    printf("\nProcess %d takes %f seconds to complete the k-means algorithm.", rank, t2);
}

int main(int argc, char **argv) {
    double time1, time2, time3;
    double reading_time, kmean_time, total_time;
    double max_reading, max_kmeans, max_total;
    int rank, size;
    
    const char *nomeFile = "Time Results MPI.csv";
    eliminaContenutoCSV(nomeFile);


    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (float i = 0.10; i < 1.1; i+=0.10){   
        int partial_nr_ROWS = (int)(ROWS * i);

        time1 = MPI_Wtime();

        read_dataset(i);

        time2 = MPI_Wtime();

        kmeans(rank, size);
        
        time3 = MPI_Wtime();

        reading_time = time2 - time1;
        kmean_time = time3 - time2;
        total_time = time3 - time1;

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
            printf("\n---------- PERCENTAGE OF DATABASE: %f ----------\n", i);
            printf("\nI/O time (s): %f", max_reading);
            printf("\nK-means algorithm time (s): %f", max_kmeans);
            printf("\nTotal time (s): %f", max_total);
            salvaTempisticheCSV(nomeFile, partial_nr_ROWS, max_reading, max_kmeans, max_total);
        }
    }
    MPI_Finalize();
    return 0;
}
