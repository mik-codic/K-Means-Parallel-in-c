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

void read_db(){
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
    // Initialize centroids randomly
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < COLS; j++) {
            centroids[i][j] = dataset[rand() % ROWS][j];
        }
    }

    int changed = 1;
    while (changed) {
        changed = 0;

        // Assign points to closest centroid
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
    }
}

int main() {
    read_db();
    kmeans();
    int c = 0;
    int g = 0;
    int f = 0;
    for (int i = 0; i < ROWS; i++) {
        printf("Point: ");
        for (int j = 0; j < COLS; j++) {
            printf("%f ", dataset[i][j]);
        }
        printf("Cluster: %d\n", cluster[i]);
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
    printf("cluster 0: %d, cluster 1: %d, cluster 2: %d",c,g,f);

    printf("TEST:\n %f", dataset[1][1]);

    return 0;
}