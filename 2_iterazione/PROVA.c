#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define N 150 // Numero di punti del dataset iris
#define M 4 // Numero di attributi per ogni punto
#define K 3 // Numero di cluster per il k-means

void init(int *cluster_assignments, float *points, int n, int m, int k) {
    int i, j;
    int cluster_sizes[k];
    float centroids[k][m];
    for (i = 0; i < k; i++) {
        cluster_sizes[i] = 0;
        for (j = 0; j < m; j++) {
            centroids[i][j] = 0.0;
        }
    }
    srand(time(NULL));
    for (i = 0; i < n; i++) {
        cluster_assignments[i] = rand() % k;
        cluster_sizes[cluster_assignments[i]]++;
        for (j = 0; j < m; j++) {
            centroids[cluster_assignments[i]][j] += points[i*m + j];
        }
    }
    for (i = 0; i < k; i++) {
        for (j = 0; j < m; j++) {
            centroids[i][j] /= cluster_sizes[i];
        }
    }
    for (i = 0; i < n; i++) {
        float min_dist = INFINITY;
        int closest_cluster = -1;
        for (j = 0; j < k; j++) {
            float dist = 0.0;
            int l;
            for (l = 0; l < m; l++) {
                float diff = points[i*m + l] - centroids[j][l];
                dist += diff*diff;
            }
            if (dist < min_dist) {
                min_dist = dist;
                closest_cluster = j;
            }
        }
        cluster_assignments[i] = closest_cluster;
    }
}

void kmeans(int rank, int size, int *cluster_assignments, float *points, int n, int m, int k) {
    int i, j, iter, closest_cluster, num_points_in_cluster;
    float centroids[k][m], sum_points[k][m];
    int cluster_sizes[k];
    MPI_Status status;
    for (i = 0; i < k; i++) {
        cluster_sizes[i] = 0;
        for (j = 0; j < m; j++) {
            centroids[i][j] = 0.0;
            sum_points[i][j] = 0.0;
        }
    }
    srand(time(NULL) + rank);
    for (i = rank; i < n; i += size) {
        cluster_assignments[i] = rand() % k;
        cluster_sizes[cluster_assignments[i]]++;
        for (j = 0; j < m; j++) {
            centroids[cluster_assignments[i]][j] += points[i*m + j];
        }
    }
    MPI_Allreduce(centroids, centroids, k*m, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(cluster_sizes, cluster_sizes, k, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    for (i = 0; i < k; i++) {
        if (cluster_sizes[i] > 0) {
            for (j = 0; j < m; j++) {
                centroids[i][j] = 0.0;
                sum_points[i][j] = 0.0;
            }
            cluster_sizes[i] = 0;
        }
        for (i = rank; i < n; i += size) {
            float min_dist = INFINITY;
            closest_cluster = -1;
            for (j = 0; j < k; j++) {
                float dist = 0.0;
                int l;
                for (l = 0; l < m; l++) {
                    float diff = points[i*m + l] - centroids[j][l];
                    dist += diff*diff;
                }
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_cluster = j;
                }
            }
            cluster_assignments[i] = closest_cluster;
            cluster_sizes[closest_cluster]++;
            for (j = 0; j < m; j++) {
                sum_points[closest_cluster][j] += points[i*m + j];
            }
        }
        MPI_Allreduce(cluster_sizes, cluster_sizes, k, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(sum_points, centroids, k*m, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
        for (i = 0; i < k; i++) {
            if (cluster_sizes[i] > 0) {
                for (j = 0; j < m; j++) {
                    centroids[i][j] /= cluster_sizes[i];
                }
            } else {
                for (j = 0; j < m; j++) {
                    centroids[i][j] = INFINITY;
                }
            }
        }
        int done = 0;
        for (i = 0; i < n; i++) {
            float min_dist = INFINITY;
            closest_cluster = -1;
            for (j = 0; j < k; j++) {
                float dist = 0.0;
                int l;
                for (l = 0; l < m; l++) {
                    float diff = points[i*m + l] - centroids[j][l];
                    dist += diff*diff;
                }
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_cluster = j;
                }
            }
            if (cluster_assignments[i] != closest_cluster) {
                cluster_assignments[i] = closest_cluster;
                done = 1;
            }
        }
        MPI_Allreduce(&done, &done, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        if (!done) {
            break;
        }
    }
    if (rank == 0) {
        printf("Cluster assignments:\n");
        for (i = 0; i < n; i++) {
            printf("%d ", cluster_assignments[i]);
        }
        printf("\n");
    }
    MPI_Finalize();
    return 0;
}
