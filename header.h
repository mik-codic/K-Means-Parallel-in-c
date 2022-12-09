
void print_data(Point data[],int c);
float euclidean_distance(Point data[], int a, int b);  //euclidean distance for distances in the datasets
float euclidean_distance_(Point data[],Point mean);
float * most_distant_k_point(Point dataset[],int rnd);
float * most_distant_k_point_(Point dataset[],Point mean, int array_len);
void change(Dict *dict, int in, float new_value);
void get_index (Dict *dict, int new_index, float new_value);
//int get_max_value(Dict dict[], int array_len);
//float * get_max_value(Dict dict[], int array_len);
float sort_array (Dict dict[], int array_len, int position);
int return_index_dict (Dict *dict, float new_value);
int * get_initial_cluster(Dict dict1[], Dict dict2[], Point points[]);
int return_position_dict (Dict dict[], int new_index);

int Get_rand_element();
void assign(Point *a, Point b);
int max_in_array(float distances[],Dict dict[], int array_len);
Point compute_mean_point(Point dataset[], int array_len); 
int compute_k_initial_point(Point data[], Point p[]);





#include"functions.c"



