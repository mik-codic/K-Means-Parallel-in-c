#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>

#define ROWS 150
#define COLS 4
#define K 3

double dataset[ROWS][COLS];
int cluster[ROWS];
double centroids[K][COLS];

void read_dataset() {
    FILE* file = fopen("iris.csv","r");
    
    char buffer[80];
    int row = 0;
    while (fgets(buffer, 80, file)) {
        
        // get all the values in a row
        char *token = strtok(buffer, ",");
        int col = 0;
        while (token) {
            
            // Just printing each integer here but handle as needed
            float n = atof(token);
            // printf("%f\n", n);
            
            dataset[row][col] = n;
            token = strtok(NULL, ",");
            col++;          
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
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Initialize centroids randomly
    if (rank == 0) {
        for (int i = 0; i < K; i++) {
            for (int j = 0; j < COLS; j++) {
                centroids[i][j] = dataset[rand() % ROWS][j];
            }
        }
    }

    // Broadcast initial centroids to all processes
    MPI_Bcast(centroids, K*COLS, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int changed = 1;
    while (changed) {
        changed = 0;

        // Assign points to closest centroid
        int start = rank * ROWS / size;
        int end = (rank + 1) * ROWS / size;

        #pragma omp parallel for reduction(||:changed)
        for (int i = start; i < end; i++) {
            double min_distance = DBL_MAX;
            int min_index = -1;
            for (int j = 0; j < K; j++) {
                double distance = euclidean_distance(dataset[i], 
centroids[j]);
                if (distance < min_distance) {
                    min_distance = distance;
                    min_index = j;
                }
            }

            if (cluster[i] != min_index) {
                cluster[i] = min_index;
                changed = 1;
            }
        }

        // Gather cluster assignments from all processes
        int global_changed;
        MPI_Allreduce(&changed, &global_changed, 1, MPI_INT, MPI_LOR, 
MPI_COMM_WORLD);
        
        changed = global_changed;
            // Recompute centroids
        if (changed) {
            double local_sum[K][COLS] = {{0}};
            int local_count[K] = {0};

            // parallelize this loop with OpenMP
            #pragma omp parallel for shared(local_sum, local_count) 
schedule(static)
            for (int i = start; i < end; i++) {
                for (int j = 0; j < COLS; j++) {
                    #pragma omp atomic
                    local_sum[cluster[i]][j] += dataset[i][j];
                }
                #pragma omp atomic
                local_count[cluster[i]]++;
            }

            double global_sum[K][COLS] = {{0}};
            int global_count[K] = {0};

            MPI_Allreduce(local_sum, global_sum, K*COLS, MPI_DOUBLE, 
MPI_SUM, MPI_COMM_WORLD);
            MPI_Allreduce(local_count, global_count, K, MPI_INT, MPI_SUM, 
MPI_COMM_WORLD);

            for (int i = 0; i < K; i++) {
                for (int j = 0; j < COLS; j++) {
                    centroids[i][j] = global_sum[i][j] / global_count[i];
                }
            }
        }
    }
    // Gather cluster assignments from all processes
    int local_counts[K] = {0};
    for (int i = start; i < end; i++) {
        local_counts[cluster[i]]++;
    }

    int global_counts[K];
    MPI_Reduce(local_counts, global_counts, K, MPI_INT, MPI_SUM, 0, 
MPI_COMM_WORLD);

    // Print cluster assignments
    if (rank == 0) {
        printf("Cluster assignments:\n");
        for (int i = 0; i < K; i++) {
            printf("Cluster %d: %d points\n", i, global_counts[i]);
        }
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    read_dataset();
    kmeans();


    printf("\nafter read data\n");
    // int c = 0;
    // int g = 0;
    // int f = 0;
    // for (int i = 0; i < ROWS; i++) {
    //     printf("\nPoint: ");
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
    
    //printf("\ncluster 0: %d, cluster 1: %d, cluster 2: %d\n",c,g,f);

    MPI_Finalize();

    return 0;
}
