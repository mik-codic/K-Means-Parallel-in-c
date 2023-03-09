#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ROWS 150
#define COLS 4
#define K 3



// double iris[ROWS][COLS] = {
//     {5.1, 3.5, 1.4, 0.2},
//     {4.9, 3.0, 1.4, 0.2},
// };
double iris[ROWS][COLS];
double centroids[K][COLS] = {
    {6.0, 2.7, 4.3, 1.3},
    {5.0, 2.0, 3.6, 1.4},
    {4, 2.0, 3.8, 1.2}
};

int cluster[ROWS];

double euclidean_distance(double a[], double b[]) {
    double sum = 0;
    for (int i = 0; i < COLS; i++) {
        sum += pow(a[i] - b[i], 2);
    }
    return sqrt(sum);
}

void assign_clusters() {
    for (int i = 0; i < ROWS; i++) {
        double min_distance = euclidean_distance(iris[i], centroids[0]);
        int min_index = 0;
        for (int j = 1; j < K; j++) {
            double distance = euclidean_distance(iris[i], centroids[j]);
            if (distance < min_distance) {
                printf("\n yes");
                min_distance = distance;
                min_index = j;
                
            }
            
        }
        cluster[i] = min_index;
    }
}

void update_centroids() {
    int count[K] = {0};
    double sum[K][COLS] = {{0}};
    for (int i = 0; i < ROWS; i++) {
        count[cluster[i]]++;
        for (int j = 0; j < COLS; j++) {
            sum[cluster[i]][j] += iris[i][j];
        }
    }
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < COLS; j++) {
            centroids[i][j] = sum[i][j] / count[i];
        }
    }
    printf("count in clusters: %d_%d_%d",count[0],count[1],count[2]);

}

int main() {

    FILE *file = fopen("iris.csv", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            fscanf(file, "%lf,", &iris[i][j]);
        }
    }
    fclose(file);

    for (int i = 0; i < 100; i++) {
        assign_clusters();
        update_centroids();
    }
    for (int i = 0; i < ROWS; i++) {
        printf("%d and n %d\n", cluster[i],i);
    }
    return 0;
}
