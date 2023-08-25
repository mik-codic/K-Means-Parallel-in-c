#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>

#define ROWS 9730
#define COLS 4
#define K 3

int labels[ROWS];
double dataset[ROWS][COLS];

int cluster[ROWS];
double centroids[K][COLS];

void read_db(){    
    printf("start of read_db function");
    FILE* file = fopen("/Users/edoardomaines/Desktop/UNI/HPC_Project/K-Means-Parallel-in-c/2_iterazione/updated_project/iris_noisy_2.csv","r");
    if(file == NULL){
        printf("\nfuck it doesn't work");
        exit(-1);
    }
    
    char buffer[120];
    int row = 0;
    while (fgets(buffer, 120, file)) {
        
        // get all the values in a row
        char *token = strtok(buffer, ",");
        
        int col = 0;
        while (token != NULL) {
            
            int count_label = 0;
            char *endptr;
            int max_names = 100;
            int max_name_len = 30;
            char labels[max_names][max_name_len];
            double n = strtod(token, &endptr); //token is the number converted to float 
            for(int m = 0; m<max_name_len;m++){
                labels[count_label][m] = endptr[m];
                //printf("\nlabel: %s",endptr);
            }
            
            
            dataset[row][col] = n;
            token = strtok(NULL, ",");
            col++;  
            if (col == 5){
                col = 0;
            }    
            if(row == ROWS-1){
                row = 0;
            }    
            count_label++;
        }
        row++;
    }
    fclose(file);
    printf("\nuscita da read_db");
}

double euclidean_distance(int a, int b) {
    double sum = 0;
    for (int i = 0; i < COLS; i++) {
        sum += pow(dataset[a][i] - centroids[b][i], 2);
    }
    return sqrt(sum);
}

void kmeans() {
    // Initialize centroids randomly
    int rd = 0;
    for (int i = 0; i < K; i++) {
        rd = rand()%ROWS;
        //printf("\n rand %d",rd);
        for (int j = 0; j < COLS; j++) {
            centroids[i][j] = dataset[rd%ROWS][j];
        }
    }

    int changed = 1;
    int iter = 10;
    int tt = 0;                

    while (tt < iter) {
        changed = 0;

        // Assign points to closest centroid
        for (int i = 0; i < ROWS; i++) {
            double min_distance = 3.5;

            
            int min_index = 0;
            for (int j = 0; j < K; j++) {
                double distance = euclidean_distance(i, j);
                //printf("\n %f",&dataset[i]);
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

        // Recalculate centroids
        int count[K] = {0};
        double sum[K][COLS] = {{0}};
        for (int i = 0; i < ROWS; i++) {
            count[cluster[i]]++;
            for (int j = 0; j < COLS; j++) {
                sum[cluster[i]][j] += dataset[i][j];
            }
        }
        for (int i = 0; i < K; i++) {
            if (count[i] > 0) {
                for (int j = 0; j < COLS; j++) {
                    centroids[i][j] = sum[i][j] / count[i];
                
                }
            }
        }
        int list_size = sizeof(cluster) / sizeof(cluster[0]);

    // Open the file for writing
    FILE *fi = fopen("centroids.csv", "w");
    if (fi == NULL) {
        printf("\nFailed to open the file for writing.");
        
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
        
        tt += 1;
    }
}
void test_using_labels(){
    FILE* file = fopen("/Users/michelepresutto/Desktop/unity_repo/untitled folder/K-Means-Parallel-in-c/2_iterazione/generated_labels.csv","r");
    if(file == NULL){
        printf("\nfuck it doesn't work");
        exit(-1);
    }
    
    char buffer[120];
    int row = 0;
    int conteggio_test = 0;
    while (fgets(buffer, 20, file)) {
        char *token = strtok(buffer, ",");
        //printf("\nlabels:%s",token);
        labels[row] = atoi(token);
        row++;
    }
    for(int i=0;i <= ROWS;i++){
        if (labels[i] == cluster[i]){
            //printf("\nt:%d = %d",labels[i],cluster[i]);
            conteggio_test++;
        }
        
    }
    printf("\nconteggio test: %2d ",(conteggio_test));
    
    int list_size = sizeof(cluster) / sizeof(cluster[0]);

    // Open the file for writing
    FILE *fil = fopen("output.csv", "w");
    if (fil == NULL) {
        printf("\nFailed to open the file for writing.");
        
    }

    // Write the list elements to the file
    for (int i = 0; i < list_size; i++) {
        fprintf(fil, "%d\n", cluster[i]);
        if (i < list_size - 1) {
            fprintf(fil, ",");
        }
    }

    // Close the file
    fclose(fil);

}


int main() {
    printf("debug start main");
    read_db();
    printf("\ndebug after reading db");
    printf("\ndata %2f",dataset[0][0]);
    kmeans();

    int c = 0;
    int g = 0;
    int f = 0;
    for (int i = 0; i < ROWS; i++) {
        //printf("Point: ");
        for (int j = 0; j < COLS; j++) {
            //printf("%f ", dataset[i][j]);
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
    //printf("cluster 0: %d, cluster 1: %d, cluster 2: %d",c,g,f);
    //test_using_labels();

    return 0;
}