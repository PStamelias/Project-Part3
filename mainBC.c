#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funBC.h"
int main(int argc,char** argv){
	int version=version_found(argv);
	char* inputfileoriginalspace=NULL;
	char* inputfilenewspace=NULL;
	char* queryfileoriginalspace=NULL;
	char* queryfilenewspace=NULL;
	int K;
	int L;
	char* outputfile=NULL;
	char* labelsofinputdataset=NULL;
	char* labelsofquerydataset=NULL;
	if(version==0){/*B*/
		for(int i=1;i<14;i++){
			if(!strcmp("-d",argv[i])){
				inputfileoriginalspace=malloc((strlen(argv[i+1])+1)*sizeof(char));
				strcpy(inputfileoriginalspace,argv[i+1]);
			}
			else if(!strcmp("-i",argv[i])){
				inputfilenewspace=malloc((strlen(argv[i+1])+1)*sizeof(char));
				strcpy(inputfilenewspace,argv[i+1]);
			}
			else if(!strcmp("-q",argv[i])){
				queryfileoriginalspace=malloc((strlen(argv[i+1])+1)*sizeof(char));
				strcpy(queryfileoriginalspace,argv[i+1]);
			}
			else if(!strcmp("-s",argv[i])){
				queryfilenewspace=malloc((strlen(argv[i+1])+1)*sizeof(char));
				strcpy(queryfilenewspace,argv[i+1]);
			}
			else if(!strcmp("-k",argv[i]))
				K=atoi(argv[i+1]);
			else if(!strcmp("-L",argv[i])){
				L=atoi(argv[i+1]);
			}
			else if(!strcmp("-o",argv[i])){
				outputfile=malloc((strlen(argv[i+1])+1)*sizeof(char));
				strcpy(outputfile,argv[i+1]);
			}
		}
	}
	else{/*C*/
		for(int i=1;i<=11;i++){
			if(!strcmp("-d",argv[i])){
				inputfileoriginalspace=malloc((strlen(argv[i+1])+1)*sizeof(char));
				strcpy(inputfileoriginalspace,argv[i+1]);
			}
			else if(!strcmp("-q",argv[i])){
				queryfileoriginalspace=malloc((strlen(argv[i+1])+1)*sizeof(char));
				strcpy(queryfileoriginalspace,argv[i+1]);
			}
			else if(!strcmp("-l1",argv[i])){
				labelsofinputdataset=malloc((strlen(argv[i+1])+1)*sizeof(char));
				strcpy(labelsofinputdataset,argv[i+1]);
			}
			else if(!strcmp("-l2",argv[i])){
				labelsofquerydataset=malloc((strlen(argv[i+1])+1)*sizeof(char));
				strcpy(labelsofquerydataset,argv[i+1]);
			}
			else if(!strcmp("-o",argv[i])){
				outputfile=malloc((strlen(argv[i+1])+1)*sizeof(char));
				strcpy(outputfile,argv[i+1]);
			}
			else if(!strcmp("-EMD",argv[i]))
				continue;
		}
	}
	if(version==0){
		free_memory1(inputfileoriginalspace,inputfilenewspace,queryfileoriginalspace,queryfilenewspace,outputfile);
	}
	else{
		free_memory2(inputfileoriginalspace,queryfileoriginalspace,labelsofinputdataset,labelsofquerydataset,outputfile);
	}
	return 0;
}
