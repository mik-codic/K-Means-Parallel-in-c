
#ifndef HEADER_FILE
#define HEADER_FILE
#include<string.h>
struct point
{
    float sep_l;
    float sep_w;
    float pet_l;
    float pet_w;
    char label[50];
    /* data */
};
typedef struct point Point;

struct dict
{
    int index;
    float value;
};
typedef struct dict Dict; 

float euclidean_distance(Point data[],int a, int b)
{
    float eu_dist = 0;
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;
    x = pow((data[a].sep_l - data[b].sep_l),2);
    y = pow((data[a].sep_w - data[b].sep_w),2);
    z = pow((data[a].pet_l - data[b].pet_l),2);
    w = pow((data[a].pet_w - data[b].pet_w),2);

    eu_dist = (x+y+z+w);
    return eu_dist;
}
float euclidean_distance_(Point x, Point y)
{
    float eu_dist = 0;
    float a = 0;
    float b = 0;
    float z = 0;
    float w = 0;
    a = pow((x.sep_l - y.sep_l),2);
    b = pow((x.sep_w - y.sep_w),2);
    z = pow((x.pet_l - y.pet_l),2);
    w = pow((x.pet_w - y.pet_w),2);

    eu_dist = (a+b+z+w);
    return eu_dist;
}

void print_data(Point data[], int c)
{
    int i = c;
    printf("\n data point %d \n sep_l: %f \n sep_w: %f\n pet_l: %f \n pet_w: %f\n", i, data[i].sep_l, data[i].sep_w, data[i].pet_l, data[i].pet_w);

}
int Get_rand_element()
{
    srand(time(NULL));   // Initialization, should only be called once.
    int r = rand() % 149;
    return r;
}

void assign(Point *a, Point b)
{
    a->sep_l = b.sep_l;
    a->sep_w = b.sep_w;
    a->pet_l = b.pet_l;
    a->pet_w = b.pet_w;
    //return *a;
}

float * most_distant_k_point(Point dataset[], Point mean, int array_len)
{   

    float * distances_values_big = (float *) malloc(sizeof(float)*array_len);
    float * distances_values_small = (float *) malloc(sizeof(float)*7);

    float tmp = 0;
    float dist = 0;

    //100 is a random choice
    if (array_len > 100) 
    {
        for(int i = 0; i < array_len; i++)
        {   
            dist = euclidean_distance_(dataset[i], mean);  
            if(dist >= tmp)
            {
                tmp = dist;
            }
            distances_values_big[i] = dist;      
        }

        return distances_values_big;

    } else {
        for(int i = 0; i < array_len; i++)
        {
            dist = euclidean_distance_(dataset[i], mean);  
            if(dist >= tmp)
            {
                tmp = dist;
            }
            distances_values_small[i] = dist;        
        }

        return distances_values_small;

    }
    free(distances_values_big);
    free(distances_values_small);
}



void change(Dict *dict, int in, float new_value)
{
    dict->index = in;
    dict->value = new_value;
}

void get_index (Dict *dict, int new_index, float new_value) {
    if (dict->value == new_value) {
        dict->index = new_index;
    }    
}

int return_index_dict (Dict *dict, float new_value, int array_len)
{
    //if (array_len > 100) {
    for (int i = 0; i < array_len; i++)
    {
        if (dict[i].value == new_value)
        {
            return(dict[i].index);
        }
    }
    // } else {
    //     for (int i = 0; i < array_len; i++)
    //     {
    //         if (dict[i].value == new_value)
    //         {
    //             return(dict[i].index);
    //         }
    //     }
    // } 
    return 0;   

}


int return_position_dict (Dict dict[], int new_index, int array_len)
{
    //if (array_len > 100) {
    for (int i = 0; i < array_len; i++)
    {
        if (dict[i].index == new_index)
        {
            return(i);
        }
    }
    // } else {
    return 0;
    // }
}


float sort_array_with_dict (Dict dict[], int array_len, int position, char* type_of_sort) {

    float *indici;

    indici = (float*) malloc(array_len*sizeof(float));
    
    
    float tmp_value = 0;
    float max = 0;
    int counter = 0;
    float pos = 0;

    for (int l = 0; l < array_len; l++)
    {
        indici[l] = dict[l].value;
    }

    for (int i = 0; i < array_len-1; i++)
    {
        counter = i;

        for (int j = i+1; j < array_len; j++)
        {
            if (strcmp(type_of_sort,"MAX") == 0) {
                if (indici[j] < indici[counter]) {
                    counter = j;
                }
            } else if (strcmp(type_of_sort,"MIN") == 0) {
                if (indici[j] > indici[counter]) {
                    counter = j;
                }
            }
        }

        tmp_value = indici[i];
        indici[i] = indici[counter];
        indici[counter] = tmp_value;        
    }

    pos = indici[position];

    return pos;

    free(indici);
}

int * get_initial_cluster(Dict dict1[], Dict dict2[], Point points[], int value_k)
{
    float * distances;
    int * top_index;

    top_index = (int*) malloc (value_k*sizeof(int));
    
    int top_seven = 7;
    int counter = 0;
    
    float max_value = 0;
    int index = 0;

    //Save in "top_index" the first point in "points", because it is the farthest from the initial mean
    Point initial_point = points[0];
    distances = most_distant_k_point(points, initial_point, 7);
    for (int j = 0; j<7; j++) {
        change(&dict1[j], dict2[top_seven-j].index, distances[j]);
    }
    //values in distances are calculated wrt the first point (the farthest) and so the value in distance for it is 0.0 (the distance with itself)   
    top_index[0] = return_index_dict(dict1, 0.0, top_seven);


    for (int i = 1; i < value_k; i++)
    {
        Point initial_point = points[counter];
        distances = most_distant_k_point(points, initial_point, 7);
        for (int j = 0; j<7; j++) {
            change(&dict1[j], dict2[top_seven-j].index, distances[j]);
        }
        
        max_value = sort_array_with_dict(dict1, 7, 6, "MAX");
        index = return_index_dict(dict1, max_value, top_seven);

        for (int l = 0; l < value_k; l++)
        {
            if(index == top_index[l]) {
                max_value = sort_array_with_dict(dict1, 7, 5, "MAX"); //if the max distance is for a Point already in the list, get max -1 (position 5)
                index = return_index_dict(dict1, max_value, top_seven);
                if (index == top_index[l]) {
                    max_value = sort_array_with_dict(dict1, 7, 4, "MAX"); //if the max distance is for a Point already in the list, get max -2 (position 4)
                    index = return_index_dict(dict1, max_value, top_seven);
                    if (index == top_index[l]) {
                        max_value = sort_array_with_dict(dict1, 7, 3, "MAX"); //if the max distance is for a Point already in the list, get max -3 (position 3)
                        index = return_index_dict(dict1, max_value, top_seven);
                }
                }
            }
        }

        top_index[i] = index;
        counter = return_position_dict(dict1, index, top_seven);
    }

    return top_index;
    free(top_index);
}

int  max_in_array(float distances[], Dict dict[], int array_len)
{   
    float max = 0;
    float tmp = 0;
    int cc = 0;
    
   
        for(int i = 0; i < array_len; i++)
        {   
                
            tmp = distances[i];
            if(tmp >= max)
            {
                max = tmp;                
                if(cc < 8)
                {
                    change(&dict[cc], i, max);
                    cc++;             
                }
            }
        }
    return 1;
}

Point compute_mean_point(Point dataset[], int array_len)
{
    Point x;
    float a = 0;
    float b = 0;
    float c = 0;
    float d = 0;
    
    for(int i = 0; i < array_len; i++)
        {
            a = a+dataset[i].sep_l;
            b = b+dataset[i].sep_w;
            c = c+dataset[i].pet_l;
            d = d+dataset[i].pet_w;
        }
    a = a/(array_len-1);
    b = b/(array_len-1);
    c = c/(array_len-1);
    d = d/(array_len-1);
    x.sep_l = a;
    x.sep_w = b;
    x.pet_l = c;
    x.pet_w = d;

    return x;
       
}

int * compute_k_initial_point(Point data[], int num_of_data, Point p[], int value_k)
{
    
    Dict *dict;  //n = 8
    Dict *top_dict;  //n = 7

    Point *refined_data;  //n = 7

    dict = (Dict*) malloc(8*sizeof(Dict));    
    top_dict = (Dict*) malloc(7*sizeof(Dict));


    Point mean;
    float *distances;
    
    int top_seven = 7;

    refined_data = (Point*) malloc(7*sizeof(Point));

    mean = compute_mean_point(data, num_of_data);                   //computing the mean point of the dataset

    printf("\nthe mean virtual point is:\n ");
    print_data(&mean, 0);

    distances = most_distant_k_point(data, mean, num_of_data);
    max_in_array(distances, dict, num_of_data);

    for(int i = 0; i < top_seven; i++)
    {
        assign(&refined_data[i],data[dict[top_seven - i].index]);        
    }

    int * top_final;
    top_final = get_initial_cluster(top_dict, dict, refined_data, value_k);

    
    // PRINT INDEX OF TOP 4 POINTS
    printf("\n TOP %d INDEX POINTS: \n", value_k);
    for (int i = 0; i < value_k; i++)
    {
        printf("\n%d \n", top_final[i]);
    }

    
    for(int i = 0; i < value_k; i++)
    {
        assign(&p[i],data[top_final[i]]);       
    }
    return top_final;


    free(dict);
    free(top_dict);
    free(refined_data);
}

void add_to_cluster(int matrix_cluster[], int cluster, int index_to_add, int number_of_index_in_matrix){
    
    *(matrix_cluster+ number_of_index_in_matrix*cluster) = index_to_add;    
}

void populate_cluster(Point data[], int num_data, int initial_points_indexes[], int k_value, int cluster_matrix []) {

    Dict *dict;
    //start from the first (initial_points_indexes[0])
    float * distances;
    float min_value;

    int counter = 0;

    int index;

    distances = (float*) malloc(num_data*sizeof(float));

    dict = (Dict*) malloc (num_data*sizeof(Dict));

    for (int i = 0; i < num_data; i++)
    {
        distances[i] = euclidean_distance(data, initial_points_indexes[0], i);
        change(&dict[i], i, distances[i]);
    }

    min_value = sort_array_with_dict(dict, num_data, num_data-2, "MIN");
    index = return_index_dict(dict, min_value, num_data);

    add_to_cluster(cluster_matrix, 0, index, counter++);

    
    
    // for (int i = 0; i<num_data; i++) {
    //     printf("\n%d, %f\n", dict[i].index, dict[i].value);
    // }


    
}




#endif
