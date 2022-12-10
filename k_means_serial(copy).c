//CIAOOOOOOOOOOOOOO

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "functions.c"



int main(){
FILE *fp;

int number_of_data = 150;
int value_of_k;

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

printf("ENTER THE NUMBER OF POINTS (value of k [0<k<7]): ");
scanf("%d", &value_of_k);


p = (Point*) malloc(value_of_k*sizeof(Point));
compute_k_initial_point(data, number_of_data, p, value_of_k);

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
    

fclose(fp);
return 0;

free(p);
free(data);
}

