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

void read_dataset() {
FILE* file = fopen("iris_noisy_2.csv","r");
    
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
    
    int i, j;

    double local_sum[K][COLS] = {{0}};
    int local_count[K] = {0};

    double global_sum[K][COLS]={{0}};
    int global_count[K]={0};

    //OMP definition of nr of threads
    // nthreads = omp_get_max_threads();
    
    int rd = 0;

    // Initialize centroids randomly
    for (i = 0; i < K; i++) {
        rd = rand()%ROWS;
        for (j = 0; j < COLS; j++) {
            centroids[i][j] = dataset[rand() % ROWS][j];
        }
    }

    #pragma omp parallel 
    {
        int tid = omp_get_thread_num();
        int nrthr =  omp_get_num_threads();
        int start = tid * ROWS / nrthr;
        int end = (tid+1) * ROWS / nrthr;

        int iter = 10;
        int tt = 0; 

        
        
         
        while (tt<iter) {
            printf(" %d iteration of the kmeans",tt);

            // Assign points to closest centroid
            #pragma omp for

            for (i = start; i < end; i++) {
                double min_distance = DBL_MAX;
                int min_index = 0;
                for (j = 0; j < K; j++) {
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
            
            
            #pragma omp for 
                //reduction(+:local_count, local_sum)
            for (i=start;i<end;i++){
                for(j=0;j<COLS;j++){
                    local_sum[cluster[i]][j]+=dataset[i][j];
                    }
                    local_count[cluster[i]]++;
            }
            #pragma omp critical 
            for (i = 0; i < K; i++) {
                for (j = 0; j < COLS; j++) {
                    global_sum[i][j] += local_sum[i][j];
                }
                global_count[i] += local_count[i];
            } 

            // MPI_Allreduce(local_sum,
            //                 global_sum,
            //                 K*COLS,
            //                 MPI_DOUBLE,
            //                 MPI_SUM,
            //                 MPI_COMM_WORLD);

            
            // MPI_Allreduce(local_count,
            //                 global_count,
            //                 K,
            //                 MPI_INT,
            //                 MPI_SUM,
            //                 MPI_COMM_WORLD);
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
    

    
    

    // Broadcast initial centroids to all processes
      
    //int changed = 1;

    

    printf("\nProcess %d takes %f seconds to complete the k-means algorithm.");
}

int main(int argc, char **argv) {
    int iter;
    
    printf("\nbefore init");

    


    read_dataset();
    printf("\nafter read data");


    kmeans();
    printf("\nafter kmeans ");

    printf("\nThe K-means algorithm took %f seconds");

    // int c = 0;
    // int g = 0;
    // int f = 0;
    // for (int i = 0; i < ROWS; i++) {
    //     //printf("\nPoint: ");
    //     for (int j = 0; j < COLS; j++) {
    //     //    printf("%f ", dataset[i][j]);
    //     }
    //     //printf("Cluster: %d\n", cluster[i]);
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

    //printf("\nfinalized ");

    return 0;
}
