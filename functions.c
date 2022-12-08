
#ifndef HEADER_FILE
#define HEADER_FILE

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
    // int till = (int) sizeof(data)/sizeof(data[0]);
    // printf("this is till :%d",till);
    // for(int i = 0; i < c; i++){
    //     printf("\n data point %d \n sep_l: %f \n sep_w: %f", i, data[i].sep_l,data[i].sep_w);
    // }
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
float * most_distant_k_point(Point dataset[],int rnd)
{   
    static float distances_values[149];
    float tmp = 0;
    float dist = 0;
    int k_points[4];


    for(int i = 0; i < 149; i++)
    {   dist = euclidean_distance(dataset,rnd,i);  
        if(euclidean_distance(dataset,i,rnd) >= tmp)
        {
            tmp = dist;
        }
        distances_values[i] = dist;
        
    }
    return distances_values;
}
float * most_distant_k_point_(Point dataset[], Point mean, int array_len)
{   
    
    static float distances_values_big[150];
    static float distances_values_small[7];

    float * distance_big = (float *) malloc(sizeof(float)*150);
    float * distance_small = (float *) malloc(sizeof(float)*7);

    float tmp = 0;
    float dist = 0;

    if (array_len == 150) 
    {
        for(int i = 0; i < 150; i++)
        {   
            dist = euclidean_distance_(dataset[i], mean);  
            if(dist >= tmp)
            {
                tmp = dist;
            }
            distance_big[i] = dist;      
        }

        return distance_big;

    } else {
        for(int i = 0; i < 7; i++)
        {
            dist = euclidean_distance_(dataset[i], mean);  
            if(dist >= tmp)
            {
                tmp = dist;
            }
            distance_small[i] = dist;        
        }

        return distance_small;

    }
    free(distance_big);
    free(distance_small);
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

int  max_in_array(float distances[], Dict dict[], int array_len)
{   
    float max = 0;
    float tmp = 0;
    int cc = 0;
    
    if (array_len == 150) 
    {
        for(int i = 0; i < 150; i++)
        {   
                
            tmp = distances[i];
            if(tmp >= max)
            {
                //printf("\n PRIMA: %f", max);
                max = tmp;
                //printf("\n DOPO: %f", max);

                printf("\nmax (top 7): %f",max);
                if(cc < 8)
                {
                    change(&dict[cc], i, max);
                    cc++;             
                }
            }
        }

    } else {
        for(int i = 0; i < 7; i++)
        {   
            //printf("\n PRIMA: %f", distances[i]);
            tmp = distances[i];
            //printf("\n DOPO: %f", tmp);

            if(tmp > max)
            {
                //printf("\n PRIMA: %f", max);
                max = tmp;
                //printf("\n DOPO: %f", max);

                printf("\nmax (top 4): %f",max);
                if(cc < 4)
                {
                    //printf("\n PRECEDENTE CHANGE: %d, %f", dict[i].index, dict[i].value);
                    change(&dict[cc], i, max);
                    cc++;
                    //printf("\n DOPO IL CHANGE: %d, %f", dict[i].index, dict[i].value);          
                }          
            //index = i;                
            } else {
                if (tmp > dict[cc-1].value && cc > 0) {
                    if(cc < 7)
                    {
                        change(&dict[cc-1], i, tmp);
                        cc++;
                    }      
                } else if (tmp > dict[cc-2].value && cc > 1) {
                    if(cc < 7)
                    {
                        change(&dict[cc-2], i, tmp);
                        cc++;
                    }    
                } else if (tmp > dict[cc-3].value && cc > 2) {
                    if(cc < 7)
                    {
                        change(&dict[cc-3], i, tmp);
                        cc++;
                    }    
                } else if (tmp > dict[cc-4].value && cc > 3) {
                    if(cc < 7)
                    {
                        change(&dict[cc-4], i, tmp);
                        cc++;
                    }    
                }
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
    if (array_len == 150) {
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
    } else {
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
    
    
}

int compute_k_initial_point(Point data[],Point p[])
{
    Dict dict[8];
    Dict top_dict[4];
    Dict new[7];
    Point mean;
    float *distances1;
    float *distances2;
    int top_seven = 7;
    int top_four = 4;
    int c = 0;
    float f = 0;
    float f_1 = 0;
    Point refined_data[7];
    mean = compute_mean_point(data, 150);                    //computing the mean point of the dataset
    printf("\nthe mean virtual point is:\n ");
    print_data(&mean, 0);
    distances1 = most_distant_k_point_(data, mean, 150);
    max_in_array(distances1, dict, 150);

    for(int i = 0; i < top_seven; i++)
    {
        //printf("\nPRIMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
        //print_data(refined_data,i);
        //printf("\nCIAOOOOOOOO %d, in posizione %d in DICT \n", dict[n_points-i].index, (n_points-i));
        assign(&refined_data[i],data[dict[top_seven - i].index]);
        //printf("\nREDEFINED_DATA\n");
        //print_data(refined_data,i);

        //REFINED_DATA ORDINE DECRESCENTE
    }

    for (int i = 0; i<7; i++) {
        printf("DIO CANEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
        print_data(refined_data,i);
    }

    Point ciao = compute_mean_point(refined_data, 7);
    distances2 = most_distant_k_point_(refined_data, ciao, 7);
    for (int j = 0; j<7; j++) {
        change(&new[j], dict[top_seven-j].index, distances2[j]);
    }
    printf("\nCIAOOOOOOOOOOOO\n");
    print_data(&ciao, 0);
    max_in_array(distances2, top_dict, 7);


    for(int i = 0; i<7; i++) {
        for (int j = 0; j<4; j++) {
            get_index(&top_dict[j], new[i].index, new[i].value);
        }
    }

    for (int l = 0; l<4; l++) {
        printf("\n %d, %f \n",top_dict[l].index, top_dict[l].value);
    }

    for(int i = 0; i < top_four; i++)
    {
        //printf("\nPORCO DIO %d \n",i);
        
        
        //printf("\nDISTANCES 2:\n");
        for (int l = 0; l<7; l++) {
            //printf("\n%f", distances2[l]);
            //print_data(refined_data, l);
        }
       
        
        //printf("\npd:%d ",dict[n_points].index);
        assign(&p[i],data[top_dict[i].index]);
        //print_data(p, i);
        // for(int j = 0; j < top_seven; j++)
        // {
        //     assign(&refined_data[i],data[dict[top_seven-j].index]);
        // }
        //print_data(refined_data, i);
        // if(i > 1)
        // {   printf("\nciaoo\n");
        //     f = euclidean_distance_(p[i-1],refined_data[0]);
        //     f_1 = euclidean_distance_(p[i],refined_data[0]);
        //     if(f < f_1)
        //     {
        //         assign(&p[i], refined_data[c+1]);
        //         printf("\nporco dio");
                
        //     }

        // } 
    }
    //int *kk;
    return 1;
}


#endif
