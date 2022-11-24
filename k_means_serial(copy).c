#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "functions.c"


int main(){
FILE *fp;
Point data[150];

int i = 0;
fp = fopen("iris.csv", "r");
    if (fp == NULL) {
        fprintf(stderr, "Error reading file\n");
        return 1;
    }
    while (i < 150)
{
    float number;
    fscanf(fp, "%f,%f,%f,%f,%s", &data[i].sep_l,&data[i].sep_w,&data[i].pet_l,&data[i].pet_w, data[i].label);
    i++;
}
//Visualizing an example of dataset
printf("\n %f, %f, %f,%f,%s",data[149].sep_l, data[149].sep_w,data[149].pet_l,data[149].pet_w, data[149].label);

Point p[4];
compute_k_initial_point(data, p);
for(int i = 0; i < 4 ; i++)
{
    print_data(p,i);
}
float c = 0;
float d = 0;
for(int i = 0; i < 4; i++)
{
    
    for(int j = i; j<4; j++)
    {
        d = euclidean_distance_(p[i],p[j]);
        printf("\ndistanza tra %d e %d: %f",i,j,d);
    }
}


printf("\n%f", c);
    

// Point mean;
// float *distances;
// float *distances_second;

// Dict dict[8];
// Dict second_dict[8];

// int n_points = 7;
// Point refined_data[n_points];
// mean = compute_mean_point(data);                    //computing the mean point of the dataset
// printf("\nthe mean virtual point is:\n ");
// print_data(&mean,0);

// distances = most_distant_k_point_(data,mean);
// //int *kk;
// max_in_array(distances, dict);
// for(int i = 0; i <= n_points;i++)
// {
//     printf("\nindex :%d    values%f\n",dict[i].index,dict[i].value);
// }

// for(int i = 0; i <= n_points;i++)
// {
//     assign(&refined_data[i],data[dict[n_points-i].index]);
// }
// distances_second = most_distant_k_point_(data,refined_data[0]);
// print_data(&refined_data[0],0);
// print_data(&data[118],0);
// for(int i = 0; i <= n_points;i++)
// {
//     printf("\n second distances%f\n",distances_second[i]);
// }

// max_in_array(distances, second_dict);
// for(int i = 0; i <= n_points;i++)
// {
//     printf("\nindex :%d    values%f\n",second_dict[i].index,second_dict[i].value);
// }
// float fff;
// //second_refined_data
// fff = euclidean_distance_(data[13],data[118]);
// printf("%f",fff); //113 and 118 

// distances_third = most_distant_k_point_(data,refined_data[0]);




// for(int i = 0; i <= 7;i++)
// {   float dis,tm;
//     for(int j=1; j < 7;i++){
//         dis = euclidean_distance_(data[dict[i].index], data[dict[j].index]);
//         if(dis >= tm){
//             tm = dis;
//         }
//     }     

// }
//second_distances = most_distant_k_point(data[dict[].index],)
// if(max_in_array(distances, dict) != 1){
//     printf("Error in max_in_array()");
// }                       //need to make a struct for (float distances, int index) for reference on the dataset
//printf("\n max distances from center \n index %d, and value %f",dict[9].index,dict[9].value);

// printf(":\n ");
// for(int i = 0; i <= 10;i++)
// {
//     printf("\n%d",kk[i]);
// }

// float dist, md;

//dist = euclidean_distance(data,1,50);
//print_data(data,3);
// Point ass;
// Point *cptr = &ass;
// assign(cptr,data[1]) ;
// printf("\nthis is assign ass:\n");
// print_data(&ass,0);

//md = most_distant_k_point(data);
//int rd = Get_rand_element();
// float *tt;
// tt = most_distant_k_point(data,rd);
// int kk;
// kk = max_in_array(tt);

// print_data(&mean,0);
// printf("ecco il massimo indice%d",kk);
// for(int i = 0; i <= 149;i++)
// {
//     printf("\n%f",tt[i]);
// }
// printf("Random point\n");
// print_data(data,rd);
//printf("\ndistance between data point 0 and 149 : %f",dist);
//printf("\n\nmost distant %f",md);
fclose(fp);
return 0;
}

