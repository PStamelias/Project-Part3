#include "structB.h"

typedef struct P_info {
	int img_num;
	float P;
} Pnode;


int* initialization_kmeans(int, image_node*, int, int);
double randfrom(double, double);
void Lloyds(int *, image_node*, image_node *, int, int, int);
int up_value_fun(int);
void update(int *, image_node*, image_node*, int, int, int);
double silhouette(int, int, image_node *, int, int *, int, image_node*, int);
void printresults(FILE*, int, int, int *, int, image_node *, image_node*, double);
double objfunction(int, int *, int, image_node *, image_node*);
void exit_memory_Cluster(FILE*, FILE*, char*, char*, char*, int*, image_node*, int, int*, image_node*, int);
