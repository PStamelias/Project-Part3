#include <stdio.h>
#include <stdlib.h>
#include "funD.h"
void free_memory(char* input_file_original_space,char* input_file_new_space,char* output_file,char* configuration_file,char* clusters_file){
	free(input_file_original_space);
	free(input_file_new_space);
	free(output_file);
	free(configuration_file);
	free(clusters_file);
}