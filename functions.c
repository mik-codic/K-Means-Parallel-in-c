
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
    float eu_dist;
    float x, y, z, w;
    x = pow((data[a].sep_l - data[b].sep_l),2);
    y = pow((data[a].sep_w - data[b].sep_w),2);
    z = pow((data[a].pet_l - data[b].pet_l),2);
    w = pow((data[a].pet_w - data[b].pet_w),2);

    eu_dist = sqrt(x+y+z+w);
    return eu_dist;
}
float euclidean_distance_(Point x, Point y)
{
    float eu_dist;
    float a, b, z, w;
    a = pow((x.sep_l - y.sep_l),2);
    b = pow((x.sep_w - y.sep_w),2);
    z = pow((x.pet_l - y.pet_l),2);
    w = pow((x.pet_w - y.pet_w),2);

    eu_dist = a+b+z+w;
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
    printf("\n data point %d \n sep_l: %f \n sep_w: %f\n pet_l: %f \n pet_w: %f\n", i, data[i].sep_l,data[i].sep_w,data[i].pet_l,data[i].pet_w);

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


    for(int i = 0; i<= 149; i++)
    {   dist = euclidean_distance(dataset,rnd,i);  
        if(euclidean_distance(dataset,i,rnd) >= tmp)
        {
            tmp = dist;
        }
        distances_values[i] = dist;
        
    }
    return distances_values;
}
float * most_distant_k_point_(Point dataset[],Point mean)
{   
    static float distances_values[149];
    float tmp = 0;
    float dist = 0;
    int k_points[3];


    for(int i = 0; i<= 149; i++)
    {   dist = euclidean_distance_(dataset[i],mean);  
        if(dist >= tmp)
        {
            tmp = dist;
        }
        distances_values[i] = dist;
        
    }
    return distances_values;
}
void change(Dict *dict[],int ind, int in, float value)
{
    dict[ind]->index = in;
    dict[ind]->value = value;
}
int  max_in_array(float distances[], Dict dict[])
{   
    float max = 0;
    float tmp;
    int index ;
    static int k_points[7];
    int cc = 0;
    
    
    for(int i = 0;i<150;i++)
    {   
            
        tmp = distances[i];
        if(tmp >= max)
        {
            max = tmp;
            printf("max: %f",max);
            if(cc <= 10)
            {
            //k_points[cc] = i;
                dict[cc].value = max;
                dict[cc].index = i;
                //change(&dict,cc,i,max);

                cc++;
                
            }
            
               
          //index = i;
               
        }
    }return 1;
        
    
    //return dict;
}    
Point compute_mean_point(Point dataset[])
{   Point x;
    float a, b, c, d;
    for(int i = 0; i<= 149;i++)
    {
        a = a+dataset[i].sep_l;
        b = b+dataset[i].sep_w;
        c = c+dataset[i].pet_l;
        d = d+dataset[i].pet_w;
    }
    a = a/149;
    b = b/149;
    c = c/149;
    d = d/149;
    x.sep_l = a;
    x.sep_w = b;
    x.pet_l = c;
    x.pet_w = d;
    return x;
}
int compute_k_initial_point(Point data[],Point p[])
{
    Dict dict[8];
    Point mean;
    float *distances;
    int n_points = 7;
    int c = 0;
    float f ,f_1 = 0;
    Point refined_data[7];
    mean = compute_mean_point(data);                    //computing the mean point of the dataset
    printf("\nthe mean virtual point is:\n ");
    print_data(&mean,0);
    distances = most_distant_k_point_(data,mean);
    max_in_array(distances, dict);
    for(int i = 0; i < n_points;i++)
    {
        assign(&refined_data[i],data[dict[n_points-i].index]);
    }

    for(int i = 0; i<4;i++)
    {

        printf("\nPORCO DIO %d \n",i);
        distances = most_distant_k_point_(refined_data,refined_data[0]);
        max_in_array(distances, dict);
        printf("\npd:%d ",dict[n_points].index);
        assign(&p[i],data[dict[n_points].index]);
        print_data(p, i);
        for(int j = 0; j <= n_points;j++)
        {
            assign(&refined_data[i],data[dict[n_points-j].index]);
        }
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
