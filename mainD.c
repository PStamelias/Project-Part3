#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "funD.h"
int main(int argc,char** argv){
	char* input_file_original_space=NULL;
	char* input_file_new_space=NULL;
	char* output_file=NULL;
	char* configuration_file=NULL;
	char* clusters_file=NULL;
	for(int i=1;i<10;i++){
		if(!strcmp("-d",argv[i])){
			input_file_original_space=malloc((strlen(argv[i+1])+1)*sizeof(char));
			strcpy(input_file_original_space,argv[i+1]);
		}
		else if(!strcmp("-i",argv[i])){
			input_file_new_space=malloc((strlen(argv[i+1])+1)*sizeof(char));
			strcpy(input_file_new_space,argv[i+1]);
		}
		else if(!strcmp("-n",argv[i])){
			clusters_file=malloc((strlen(argv[i+1])+1)*sizeof(char));
			strcpy(clusters_file,argv[i+1]);
		}
		else if(!strcmp("-c",argv[i])){
			configuration_file=malloc((strlen(argv[i+1])+1)*sizeof(char));
			strcpy(configuration_file,argv[i+1]);
		}
		else if(!strcmp("-o",argv[i])){
			output_file=malloc((strlen(argv[i+1])+1)*sizeof(char));
			strcpy(output_file,argv[i+1]);
		}
	}
	free_memory(input_file_original_space,input_file_new_space,output_file,configuration_file,clusters_file);
	return 0;
}
