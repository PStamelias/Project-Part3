#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funBC.h"
int version_found(char** argv){
	int found=0;
	for(int i=1;i<14;i++){
		if(!strcmp("-EMD",argv[i])){
			found=1;
			break;
		}
	}
	return found;
}
void free_memory1(char* inputfileoriginalspace,char* inputfilenewspace,char* queryfileoriginalspace,char* queryfilenewspace,char* outputfile){
	free(inputfileoriginalspace);
	free(inputfilenewspace);
	free(queryfileoriginalspace);
	free(queryfilenewspace);
	free(outputfile);
}
void free_memory2(char* inputfileoriginalspace,char* queryfileoriginalspace,char* labelsofinputdataset,char* labelsofquerydataset,char* outputfile){
	free(inputfileoriginalspace);
	free(queryfileoriginalspace);
	free(labelsofinputdataset);
	free(labelsofquerydataset);
	free(outputfile);
}