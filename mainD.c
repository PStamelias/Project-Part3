#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "structD.h"
#define MAX_LENGTH_WORD 500

int main(int argc, char** argv) {

	srand(time(NULL));

	int arguments_number = argc-1;

	FILE* inptr;
	FILE* newinptr;
  FILE* outptr;
	image_node* image_table = NULL;
	image_node* new_image_table=NULL;
	int* K_clusters_num = NULL; //apouhkeyoyme ta arxika K kentroeidh poy ua paroyme apo ton kmeans++
	int* New_K_clusters_num=NULL;

	int number_of_images;
	int distances;
	int new_number_of_images;
	int new_distances;

	char* input_file;
	char* inputfilenewspace;
	char* configuration_file;
	char* output_file;
	char* classes_as_clusters;


	for (int pos = 1; pos <= arguments_number-1; ) {

	 if(strcmp(argv[pos],"-d") == 0) {
		 input_file=malloc((strlen(argv[pos+1])+1)*sizeof(char));
		 strcpy(input_file,argv[pos+1]);
	 } else if(strcmp(argv[pos],"-c") == 0) {
		 configuration_file=malloc((strlen(argv[pos+1])+1)*sizeof(char));
		 strcpy(configuration_file,argv[pos+1]);
	 } else if(strcmp(argv[pos],"-o") == 0) {
		 output_file=malloc((strlen(argv[pos+1])+1)*sizeof(char));
		 strcpy(output_file,argv[pos+1]);
	 } else if(strcmp(argv[pos],"-i") == 0) {
		 inputfilenewspace=malloc((strlen(argv[pos+1])+1)*sizeof(char));
		 strcpy(inputfilenewspace,argv[pos+1]);
	 } else if(strcmp(argv[pos],"-n") == 0) {
		 classes_as_clusters=malloc((strlen(argv[pos+1])+1)*sizeof(char));
		 strcpy(classes_as_clusters,argv[pos+1]);
	 }
	 pos = pos + 2;
	}

  //Diabazw tis parametroys apo to clusters.conf:
  FILE *conf_fp;
	conf_fp = fopen(configuration_file, "r");
  //gia systades(clustering):
  int num_of_clusters;
  fscanf(conf_fp,"%*s %d\n", &num_of_clusters);

	fclose(conf_fp);
  ////


  //------------------
	inptr = fopen(input_file,"r");
	newinptr = fopen(inputfilenewspace,"r");
	outptr = fopen(output_file,"a");
	input_info(inptr, &number_of_images, &distances);
	input_info(newinptr,&new_number_of_images,&new_distances);
	image_table = image_creation(inptr, number_of_images, distances);
	new_image_table = image_creation_new(newinptr,new_number_of_images,new_distances);
	K_clusters_num = initialization_kmeans(num_of_clusters, image_table, number_of_images, distances); //efarmozoyme ton kmeans++
  //o K_clusters_num exei tis ueseis toy image_table poy exoyn ta prwta num_of_clusters kentra
	New_K_clusters_num = initialization_kmeans(num_of_clusters,new_image_table,new_number_of_images,new_distances);
  //------------------


  //--For original space---
  image_node *kentroeidh = malloc(num_of_clusters*sizeof(image_node)); //ston pinaka kentroeidh ua exoyme ta pixels
  for (int i = 0; i < num_of_clusters; i++)                            //twn num_of_clusters kentroeidwn
  	kentroeidh[i].pixels = malloc(distances*sizeof(int));

	for (int i = 0; i < num_of_clusters; i++) {  /*ta prwta kentroeidh poy briskoyme me ton kmeans++ einai eikones toy Dataset,*/
		int position = K_clusters_num[i];          /*opote arxika bazoyme se kaue uesh toy pinaka kentroeidh, ta pixels ths kaue eikonas*/
		for (int j = 0; j < distances; j++) {      /*toy Dataset h opoia einai kentroeides*/
			kentroeidh[i].pixels[j] = image_table[position].pixels[j];
		}
	}

  int *assignments = malloc(number_of_images*sizeof(int));  /*ston pinaka assignments ua exw akeraioys sto [0,num_of_clusters-1]*/
                                                            /*p.x. an sthn uesh assignments[2] exw thn timh 3,ayto shmainei oti*/
                                                            /*h eikona image_table[2] exei anateuei sto kentroeides kentroeidh[3]*/
	clock_t t;
	double time_taken;

  //Classic-Lloyd's
	t = clock();
	for (int i = 0; i < 100; i++) {  //anti gia 100 ,na bazame 10 gia pio grhgora
    //printf("i=%d\n",i);
		for (int j = 0; j < number_of_images; j++) //arxikopoiw kaue fora ton assignments me -1
			assignments[j] = -1;
			//BHMA ANAUESHS:
		Lloyds(assignments, image_table, kentroeidh, number_of_images, num_of_clusters, distances);
			//BHMA UPDATE:
		update(assignments, kentroeidh, image_table, number_of_images, num_of_clusters, distances);

	}
	t = clock() - t;
	time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds

  fprintf(outptr,"%s\n","ORIGINAL SPACE");
	printresults(outptr,num_of_clusters,number_of_images,assignments,distances,kentroeidh,image_table,time_taken);
	double objres = objfunction(number_of_images,assignments,distances,kentroeidh,image_table);
	fprintf(outptr,"Value of Objective Function: %f\n",objres);
	//-----------------------


	//--For new space--------
	image_node *kentroeidh_new = malloc(num_of_clusters*sizeof(image_node));
	for (int i = 0; i < num_of_clusters; i++)
		kentroeidh_new[i].pixels = malloc(new_distances*sizeof(int));

	for (int i = 0; i < num_of_clusters; i++) {  /*ta prwta kentroeidh poy briskoyme me ton kmeans++ einai eikones toy Dataset,*/
		int position = New_K_clusters_num[i];          /*opote arxika bazoyme se kaue uesh toy pinaka kentroeidh, ta pixels ths kaue eikonas*/
		for (int j = 0; j < new_distances; j++) {      /*toy Dataset h opoia einai kentroeides*/
			kentroeidh_new[i].pixels[j] = new_image_table[position].pixels[j];
		}
	}

	int *new_assignments = malloc(new_number_of_images*sizeof(int));  /*ston pinaka assignments ua exw akeraioys sto [0,num_of_clusters-1]*/
																														/*p.x. an sthn uesh assignments[2] exw thn timh 3,ayto shmainei oti*/
																														/*h eikona image_table[2] exei anateuei sto kentroeides kentroeidh[3]*/
	clock_t t1;
	double time_taken1;

	//Classic-Lloyd's
	t1 = clock();
	for (int i = 0; i < 100; i++) { //anti gia 100 ,na bazame 10 gia pio grhgora
		//printf("i=%d\n",i);
		for (int j = 0; j < new_number_of_images; j++) //arxikopoiw kaue fora ton assignments me -1
			new_assignments[j] = -1;
			//BHMA ANAUESHS:
		Lloyds(new_assignments, new_image_table, kentroeidh_new, new_number_of_images, num_of_clusters, new_distances);
			//BHMA UPDATE:
		update(new_assignments, kentroeidh_new, new_image_table, new_number_of_images, num_of_clusters, new_distances);

	}
	t1 = clock() - t1;
	time_taken1 = ((double)t1)/CLOCKS_PER_SEC; // in seconds

	fprintf(outptr,"%s\n","NEW SPACE");
	printresults(outptr,num_of_clusters,new_number_of_images,new_assignments,new_distances,kentroeidh_new,new_image_table,time_taken1);
	objres = objfunction(new_number_of_images,new_assignments,new_distances,kentroeidh_new,new_image_table);
	fprintf(outptr,"Value of Objective Function: %f\n",objres);
	//-----------------------

  fclose(newinptr);
  free(inputfilenewspace);
	free(classes_as_clusters);
	for(int i = 0; i < new_number_of_images; i++)
		free(new_image_table[i].pixels);
	free(new_image_table);
	free(New_K_clusters_num);
	for (int i = 0; i < num_of_clusters; i++)
		free(kentroeidh_new[i].pixels);
	free(kentroeidh_new);
	free(new_assignments);
	exit_memory_Cluster(inptr,outptr,input_file,configuration_file,output_file,K_clusters_num,image_table,number_of_images,assignments,kentroeidh,num_of_clusters);
	return 0;
}
