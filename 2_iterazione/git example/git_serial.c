/*  This is an implementation of the k-means clustering algorithm (aka Lloyd's algorithm) using MPI (message passing interface). */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<math.h>
#include<errno.h>
#include<string.h>

#define MAX_ITERATIONS 50

/* Definition of the constant */
#define numOfElements 150
#define COLS 4
#define numOfClusters 3

double dataset[numOfElements][COLS];

// int numOfClusters = 0;
// int numOfElements = 0;
// int num_of_processes = 0;

/* This function goes through that data points and assigns them to a cluster */
void assign2Cluster(double k_sepal_len[], double k_sepal_wid[], double k_petal_len[], double k_petal_wid[], double recv_sepal_len[], double recv_sepal_wid[], double recv_petal_len[], double recv_petal_wid[], int assign[])
{
	double min_dist = 10000000;
	double sepal_len=0, sepal_wid=0, petal_len=0, petal_wid=0, temp_dist=0;
	int k_min_index = 0;
	for(int i = 0; i < (numOfElements) + 1; i++)
	{
		for(int j = 0; j < numOfClusters; j++)
		{
			sepal_len = fabs(recv_sepal_len[i] - k_sepal_len[j]);
			sepal_wid = fabs(recv_sepal_wid[i] - k_sepal_wid[j]);
			petal_len = fabs(recv_petal_len[i] - k_petal_len[j]);
			petal_wid = fabs(recv_petal_wid[i] - k_petal_wid[j]);
			temp_dist = sqrt((sepal_len*sepal_len) + (sepal_wid*sepal_wid) + (petal_len*petal_len) + (petal_wid*petal_wid));
			// new minimum distance found
			if(temp_dist < min_dist)
			{
				min_dist = temp_dist;
				k_min_index = j;
			}
		}

		// update the cluster assignment of this data points
		assign[i] = k_min_index;
	}
}

/* Recalcuate k-means of each cluster because each data point may have
   been reassigned to a new cluster for each iteration of the algorithm */
void calcKmeans(double k_means_sepal_len[], double k_means_sepal_wid[], double k_means_petal_len[], double k_means_petal_wid[], double data_sepal_len_points[], double data_sepal_wid_points[], double data_petal_len_points[], double data_petal_wid_points[], int k_assignment[])
{
	double total_sepal_len = 0;
	double total_sepal_wid = 0;
    double total_petal_len = 0;
	double total_petal_wid = 0;
	int numOfpoints = 0;

	for(int i = 0; i < numOfClusters; i++)
	{
		total_sepal_len = 0;
		total_sepal_wid = 0;
        total_petal_len = 0;
		total_petal_wid = 0;
		numOfpoints = 0;

		for(int j = 0; j < numOfElements; j++)
		{
			if(k_assignment[j] == i)
			{
				total_sepal_len += data_sepal_len_points[j];
				total_sepal_wid += data_sepal_wid_points[j];
                total_petal_len += data_petal_len_points[j];
				total_petal_wid += data_petal_wid_points[j];
				numOfpoints++;
			}
		}

		if(numOfpoints != 0)
		{
			k_means_sepal_len[i] = total_sepal_len / numOfpoints;
			k_means_sepal_wid[i] = total_sepal_wid / numOfpoints;
            k_means_petal_len[i] = total_petal_len / numOfpoints;
			k_means_petal_wid[i] = total_petal_wid / numOfpoints;
		}
	}

}

int main(int argc, char *argv[])
{
	// buffers
	double *k_means_sepal_len = NULL;		// k means corresponding sepal length values
	double *k_means_sepal_wid = NULL;		// k means corresponding sepal width values
    double *k_means_petal_len = NULL;		// k means corresponding petal length values
	double *k_means_petal_wid = NULL;		// k means corresponding petal width values
	int *k_assignment = NULL;		        // each data point is assigned to a cluster
	double *data_sepal_len_points = NULL;
	double *data_sepal_wid_points = NULL;
    double *data_petal_len_points = NULL;
	double *data_petal_wid_points = NULL;
	
    printf("The value of k is %d.\n", numOfClusters);

    // allocate memory for arrays
    k_means_sepal_len = (double *)malloc(sizeof(double) * numOfClusters);
    k_means_sepal_wid = (double *)malloc(sizeof(double) * numOfClusters);
    k_means_petal_len = (double *)malloc(sizeof(double) * numOfClusters);
    k_means_petal_wid = (double *)malloc(sizeof(double) * numOfClusters);

    if(k_means_sepal_len == NULL || k_means_sepal_wid == NULL || k_means_petal_len == NULL || k_means_petal_wid == NULL)
    {
        perror("malloc");
        exit(-1);
    }

    printf("Reading input data from file...\n\n");

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
        data_sepal_len_points[i] = dataset[i][0];
        data_sepal_wid_points[i] = dataset[i][1];
        data_petal_len_points[i] = dataset[i][2];
        data_petal_wid_points[i] = dataset[i][3];
        printf("-------------------------------------------\n");
        printf("ALL POINTS\n");
        printf("Point # %d: %f, %f, %f, %f\n",i , data_sepal_len_points[i], data_sepal_wid_points[i], data_petal_len_points[i], data_petal_wid_points[i]);
        // assign all point to the cluster 0
        // initial setting (i = index of points)
        k_assignment[i] = 0;
    }

    // randomly select initial k-means point (4 coordinates)
    time_t t;
    srand((unsigned) time(&t));
    // the 3 random points selected as centroids
    int random;
    for(int i = 0; i < numOfClusters; i++) {
        random = rand() % numOfElements;
        k_means_sepal_len[i] = data_sepal_len_points[random];
        k_means_sepal_wid[i] = data_sepal_wid_points[random];
        k_means_petal_len[i] = data_petal_len_points[random];
        k_means_petal_wid[i] = data_petal_wid_points[random];
    }

    printf("Running k-means algorithm for %d iterations...\n\n", MAX_ITERATIONS);
    for(int i = 0; i < numOfClusters; i++)
    {
        printf("Initial K-means: (%f, %f, %f, %f)\n", k_means_sepal_len[i], k_means_sepal_wid[i], k_means_petal_len[i], k_means_petal_wid[i]);
    }

    for (int run = 0; run < MAX_ITERATIONS; run++) {
        assign2Cluster(k_means_sepal_len, k_means_sepal_wid, k_means_petal_len, k_means_petal_wid, data_sepal_len_points, data_sepal_wid_points, data_petal_len_points, data_petal_wid_points, k_assignment);
	    calcKmeans(k_means_sepal_len, k_means_sepal_wid, k_means_petal_len, k_means_petal_wid, data_sepal_len_points, data_sepal_wid_points, data_petal_len_points, data_petal_wid_points, k_assignment);
    }

    // assign the data points to a cluster
	//assign2Cluster(k_means_sepal_len, k_means_sepal_wid, k_means_petal_len, k_means_petal_wid, recv_sepal_len, recv_sepal_wid, recv_petal_len, recv_petal_wid, recv_assign);
	
    printf("--------------------------------------------------\n");
    printf("FINAL RESULTS:\n");
    for(int i = 0; i < numOfClusters; i++)
    {
        printf("Cluster #%d: (%f, %f, %f, %f)\n", i, k_means_sepal_len[i], k_means_sepal_wid[i], k_means_petal_len[i], k_means_petal_wid[i]);
    }
    printf("--------------------------------------------------\n");

    //assign2Cluster(k_means_sepal_len, k_means_sepal_wid, k_means_petal_len, k_means_petal_wid, data_sepal_len_points, data_sepal_wid_points, data_petal_len_points, data_petal_wid_points, k_assignment);

    printf("--------------------------------------------------\n");
    printf("CLUSTERS:\n");
    for(int i = 0; i < numOfElements; i++)
    {
        printf("Point #%d is assigned to Cluster #%d\n", i, k_assignment[i]);
    }
    printf("RESULTS:");
    int zero = 0;
    int first = 0;
    int second = 0;
    for(int i = 0; i<numOfElements; i++){
        if(k_assignment[i] == 0){
            zero++;
        }
        if(k_assignment[i] == 1){
            first++;
        }
        if(k_assignment[i] == 2){
            second++;
        }
    } 
    printf("\nPoints in cluster 0 are %d",zero);
    printf("\nPoints in cluster 1 are %d",first);
    printf("\nPoints in cluster 2 are %d",second);
    printf("--------------------------------------------------\n");

    // deallocate memory and clean up
    free(k_means_sepal_len);
    free(k_means_sepal_wid);
    free(k_means_petal_len);
    free(k_means_petal_wid);

    free(data_sepal_len_points);
    free(data_sepal_wid_points);
    free(data_petal_len_points);
    free(data_petal_wid_points);

    free(k_assignment);
}