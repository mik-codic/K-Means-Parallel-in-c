#include <stdio.h>
#include <math.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
// Define a Point structure to store 4D points
typedef struct {
    double x, y, z, w;
} Point;


#define numOfElements 150
#define COLS 4
#define numOfClusters 3
double data[numOfElements][COLS];

int collection_of_index[3];
// Function to calculate the distance between two 4D points
double distance(Point a, Point b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2) + pow(a.w - b.w, 2));
}

// Function to find the point in the dataset that maximizes the sum of the distances to the current set of selected points
Point find_best_point(Point *dataset, int n_points, Point *selected_points, int n_selected_points) {
    Point best_point = dataset[0];
    int index = 0;
    double max_sum = 0;
    for (int i = 0; i < n_points; i++) {
        double sum = 0;
        for (int j = 0; j < n_selected_points; j++) {
            sum += distance(dataset[i], selected_points[j]);
        }
        if (sum > max_sum) {
            max_sum = sum;
            best_point = dataset[i];
            index = i;
        }
    }            
    printf("\nIndex: %d",index);
    collection_of_index[n_selected_points] = index;

    return best_point;
}
int main() {

    double *k_means_sepal_len = NULL;		// k means corresponding sepal length values
	double *k_means_sepal_wid = NULL;		// k means corresponding sepal width values
    double *k_means_petal_len = NULL;		// k means corresponding petal length values
	double *k_means_petal_wid = NULL;		// k means corresponding petal width values
	int *k_assignment = NULL;		        // each data point is assigned to a cluster
	double *data_sepal_len_points = NULL;
	double *data_sepal_wid_points = NULL;
    double *data_petal_len_points = NULL;
	double *data_petal_wid_points = NULL;
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
            
            data[row][col] = n;
            token = strtok(NULL, ",");
            col++;          
        }
        row++;
    }
    fclose(file);

    // allocate memory for an array of data points
    data_sepal_len_points = (double *)malloc(sizeof(double) * numOfElements);
    data_sepal_wid_points = (double *)malloc(sizeof(double) * numOfElements);
    data_petal_len_points = (double *)malloc(sizeof(double) * numOfElements);
    data_petal_wid_points = (double *)malloc(sizeof(double) * numOfElements);
    k_assignment = (int *)malloc(sizeof(int) * numOfElements);

    if(data_sepal_len_points == NULL || data_sepal_wid_points == NULL || data_petal_len_points == NULL || data_petal_wid_points == NULL || k_assignment == NULL)
    {
        perror("malloc");
        exit(-1);
    }

    // now read in points and fill the arrays
    for (int i = 0; i<numOfElements; i++) {
        data_sepal_len_points[i] = data[i][0];
        data_sepal_wid_points[i] = data[i][1];
        data_petal_len_points[i] = data[i][2];
        data_petal_wid_points[i] = data[i][3];
        printf("-------------------------------------------\n");
        printf("ALL POINTS\n");
        printf("Point # %d: %f, %f, %f, %f\n",i , data_sepal_len_points[i], data_sepal_wid_points[i], data_petal_len_points[i], data_petal_wid_points[i]);
        // assign all point to the cluster 0
        // initial setting (i = index of points)
        k_assignment[i] = 0;
    }

    Point dataset[numOfElements];

    for(int i = 0;i<numOfElements;i++){
        dataset[i].x = data[i][0];
        dataset[i].y = data[i][1];
        dataset[i].z = data[i][2];
        dataset[i].w = data[i][3];
        
    }
    printf("\nmy point %f",dataset[0].x);
    
    int n_points = sizeof(dataset) / sizeof(Point);
    printf("\nn points = %d", n_points);
    // Find the n points that maximize the sum of the distances between them
    int n = 3;
    Point selected_points[n];
    selected_points[0] = dataset[0]; // Start with an arbitrary point
    for (int i = 1; i < n; i++) {
        selected_points[i] = find_best_point(dataset, n_points, selected_points, i);
    }
    for(int i= 0;i<3;i++){
        printf("\nbabyyyy %d",collection_of_index[i]);
    }
    // Print the n points that maximize the sum of the distances between them
    printf("The %d points that maximize the sum of the distances between them are:\n", n);
    for (int i = 0; i < n; i++) {
        printf("(%f, %f, %f, %f)\n", selected_points[i].x,
               selected_points[i].y,
               selected_points[i].z,
               selected_points[i].w);
    }

    return 0;
}