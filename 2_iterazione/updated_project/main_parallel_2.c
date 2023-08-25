#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>

#define ROWS 30001
#define COLS 4
#define K 3

double dataset[ROWS][COLS];
int cluster[ROWS];
double centroids[K][COLS];

void read_dataset() {
FILE* file = fopen("generated_data.csv","r");
    
    char buffer[130];
    int row = 0;
    while (fgets(buffer, 130, file)) {
        
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

double euclidean_distance(int a, int b) {
    double sum = 0;
    for (int i = 0; i < COLS; i++) {
        sum += pow(dataset[a][i] - centroids[b][i], 2);
    }
    return sqrt(sum);}

void kmeans() {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
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
    int changed = 1;
    while (tt<iter) {
        printf("%d` iteration of the kmeans",tt);
        changed = 0;

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
                changed = 1;
            }
        }

        // Gather cluster assignments from all processes
        int global_changed;
        MPI_Allreduce(&changed, &global_changed, 1, MPI_INT, MPI_LOR,
                      MPI_COMM_WORLD);
        changed = global_changed;

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
}

int main(int argc, char **argv) {
    printf("\nbefore init");

    MPI_Init(&argc, &argv);
    printf("\nafter init");

    read_dataset();
    printf("\nafter read data");

    kmeans();
    printf("\nafter kmeans ");


    MPI_Finalize();
    int c = 0;
    int g = 0;
    int f = 0;
    for (int i = 0; i < ROWS; i++) {
        //printf("\nPoint: ");
        for (int j = 0; j < COLS; j++) {
        //    printf("%f ", dataset[i][j]);
        }
        //printf("Cluster: %d\n", cluster[i]);
        if(cluster[i] == 0){
            c++;
        }
        if(cluster[i] == 1){
            g++;
        }
        if(cluster[i] == 2){
            f++;
        }
    }
    
    //printf("\ncluster 0: %d, cluster 1: %d, cluster 2: %d\n",c,g,f);

    //printf("\nfinalized ");

    return 0;
}
