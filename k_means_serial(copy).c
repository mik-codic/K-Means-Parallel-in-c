//CIAOOOOOOOOOOOOOO

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "functions.c"



int main(){
FILE *fp;

int number_of_data = 150;
int value_of_k = 4;

Point *data;
Point *p;
data = (Point*) malloc(number_of_data*sizeof(Point));

int i = 0;
fp = fopen("iris.csv", "r");
    if (fp == NULL) {
        fprintf(stderr, "Error reading file\n");
        return 1;
    }
    while (i < number_of_data)
{
    float number;
    fscanf(fp, "%f,%f,%f,%f,%s", &data[i].sep_l,&data[i].sep_w,&data[i].pet_l,&data[i].pet_w, data[i].label);
    i++;
}

//printf("ENTER THE NUMBER OF POINTS (value of k [0<k<7]): ");
//scanf("%d", &value_of_k);


p = (Point*) malloc(value_of_k*sizeof(Point));

int * final_indexes;
final_indexes = compute_k_initial_point(data, number_of_data, p, value_of_k);


// for(int i = 0; i < value_of_k ; i++)
// {
//     print_data(p,i);
// }
// float d = 0;
// for(int i = 0; i < value_of_k; i++)
// {
    
//     for(int j = i; j < value_of_k; j++)
//     {
//         d = euclidean_distance_(p[i],p[j]);
//         printf("\ndistanza tra %d e %d: %f", i, j, d);
//     }
// }

//matrix: on the column we have the k cluster and on the rows all the indexes of the points in those cluster
//rows: indexes of points
//columns: clusters
int *cluster_matrix = (int*) malloc(number_of_data*value_of_k*sizeof(int)); 

populate_cluster(data, number_of_data, final_indexes, value_of_k, cluster_matrix);




// 

// for (int i = 0; i < rows; i++)
// {
//     for (int j = 0; j < columns; j++)
//     {
//         *(a+ i*columns +j) = i+j;
//     }    
// }
// printf("The array elements are:\n");
// for (int i = 0; i < rows; i++) {
//     for (int j = 0; j < columns; j++) {
//         printf("%d ", *(a + i*columns + j));
//     }
//     printf("\n");
// }
// printf("\n");
// for (int j = 0; j< rows; j++){
//     printf("%d\n", *(a+j*columns) );
// }

//COME STAMPARE LA PRIMA COLONNA COMPLETA

 printf("%d\n", *(cluster_matrix+ 0*value_of_k) ); //0
// printf("%d\n", *(a+ 1*columns) ); //1
// printf("%d\n", *(a+ 2*columns) ); //2
// printf("%d\n", *(a+ 3*columns) ); //3
// printf("\n");
// printf("%d\n", *(a+ 0*columns +1) ); //1
// printf("%d\n", *(a+ 1*columns +1) ); //2
// printf("%d\n", *(a+ 2*columns +1) ); //3
// printf("%d\n", *(a+ 3*columns +1) ); //4


free(cluster_matrix);












fclose(fp);
return 0;

free(p);
free(data);
}

