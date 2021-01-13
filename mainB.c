#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
#include "structB.h"
#define MAX_LENGTH_WORD 500

//ALLAGH
int main(int argc,char** argv) {

	int arguments_number = argc-1; /*krataw to plhuos twn orismatwn(afairwntas to orisma ./lsh)*/

	int K,L,N;    /*to K prepei na einai toylaxiston 2(sthn prajh prepei na einai gyrw sto 4)*/
  double R;
  N=1;
  int number_of_images=0; /*poses eikones exei to Dataset moy(to input_file moy dhladh)*/
  int distances = 0; /*poses einai oi diastaseis enos dianysmatos(mias eikonas dhladh)*/
  int table_size = 0;

	image_node* image_table; /*einai enas pinakas megeuoys number_of_images poy periexei*/
	                         /*oles tis eikones poy diabasthkan apo to input_file*/

	bucket*** bucket_ptr_table; /*ua periexei toys ejwterikoys L pinakes katakermatismoy toy LSH*/

	char* input_file;
	char* query_file;
	char* output_file;
	char* inputfilenewspace;
	char* queryfilenewspace;

	if(arguments_number != 0) { /*an exoyn douei ta orismata sthn grammh entolwn*/

    	 K = -1; /*arxikopoiw ta K, L se periptwsh poy den*/
		 L = -1; /*paroyn times apo thn grammh entolwn*/
		 for (int pos = 1; pos <= arguments_number-1; ) {  /*tsekarw ta pos = 1,3,5,7,9,11,13 giati se aytes tis*/
                                                       /*ueseis ua briskontai ta -d,-q,-k,-L,-o,-N,-R*/
			if(strcmp(argv[pos],"-d") == 0) {   /* an exw -d ayto shmainei oti to */
				input_file=malloc((strlen(argv[pos+1])+1)*sizeof(char)); /*epomeno orisma einai to input_file*/
				strcpy(input_file,argv[pos+1]);
			} else if(strcmp(argv[pos],"-q") == 0) { /*an exw -q*/
				query_file=malloc((strlen(argv[pos+1])+1)*sizeof(char));
				strcpy(query_file,argv[pos+1]);
			} else if(strcmp(argv[pos],"-o") == 0) { /*an exw -o*/
				output_file=malloc((strlen(argv[pos+1])+1)*sizeof(char));
				strcpy(output_file,argv[pos+1]);
			} else if(strcmp(argv[pos],"-k") == 0) { /*an exw -k*/
				K = atoi(argv[pos+1]);
			} else if(strcmp(argv[pos],"-L") == 0) { /*an exw -L*/
				L = atoi(argv[pos+1]);
			} else if(strcmp(argv[pos],"-i") == 0) {
				inputfilenewspace=malloc((strlen(argv[pos+1])+1)*sizeof(char));
				strcpy(inputfilenewspace,argv[pos+1]);
			} else if(strcmp(argv[pos],"-s") == 0) {
				queryfilenewspace=malloc((strlen(argv[pos+1])+1)*sizeof(char));
				strcpy(queryfilenewspace,argv[pos+1]);
			}
			pos = pos + 2;
		 }

		 if((K == -1)&&(L == -1)) { /*an den douhke timh sta K,L tote*/
			 K = 4;                   /*sta K,L,N,R dinontai default times*/
			 L = 5;
			 N = 1;
		 }


	} else {  /*an den douhke kanena orisma sthn grammh entolwn kai ta zhtaei*/
            /*to programma kata thn diarkeia ths ekteleshs toy*/
		 printf("Give the input_file:");
		 input_file=malloc(MAX_LENGTH_WORD*sizeof(char));
		 scanf("%s",input_file);

		 printf("Give the query_file:");
		 query_file=malloc(MAX_LENGTH_WORD*sizeof(char));
		 scanf("%s",query_file);

		 printf("Give the output_file:");
		 output_file=malloc(MAX_LENGTH_WORD*sizeof(char));
		 scanf("%s",output_file);

		 printf("%s\n","For default values for k,L,N,R give -1 at k,L");

		 printf("Give parameter k:");
		 scanf("%d",&K);
		 printf("Give parameter L:");
		 scanf("%d",&L);

		 if((K == -1)&&(L == -1)) { /*an sta K,L douhhke -1 tote sta*/
			 K = 4;                   /*K,L,N,R dinontai default times*/
			 L = 5;
			 N = 1;
			 R = 10000;
		 } else {    /*alliws synexise kanonika kai pare apo ton xrhsth kai tis times twn N kai R*/
			 printf("Give parameter N:");
			 scanf("%d",&N);
			 printf("Give parameter R:");
			 scanf("%lf",&R);
		 }

	}
	/*creating L hashtables*/
  FILE *fp;

  fp = fopen(input_file,"r"); /*anoigw to arxeio input_file to opoio periexei to synolo eikonwn moy(Dataset)*/

	input_info(fp, &number_of_images, &distances); /*diabasa apo to input_file(diekths fp) toys 4 prwtoys akeraioys*/
	                                               /*kai phra poses eikones exei kai poses diastaseis exei ena dianysma(eikona)*/

	image_table = image_creation(fp, number_of_images, distances);

  srand(time(NULL));

	int w = 0; /*Uelw prwta na ypologisw to w*/
						 /*to w einai = (mesh apostash toy kaue shmeioy apo ton plhsiestero geitona)xparagwn*/
             /*opoy o paragwn einai kapoios akeraios ariumos sto diasthma [4,10]*/

  int deigma = 1000; /*gia na brw to w pairnw ena deigma poy einai oi 1000 prwtes eikones*/
	int min_dist = 0;

  for (int i = 0; i < deigma; ++i) {
  	/*min_dist = briskw gia mia eikona thn apostash ths apo ton plhsiestero geitona ths sto deigma*/
		min_dist = brute_force(image_table, i, deigma, distances);
		w = w + min_dist;

  }

	w = w/deigma;
  w = w*4; //to w bgainei peripoy 50000

	int*** s_L_tables = create_s_numbers_ofLtables(L, K, w, distances);  /*o s_L_tables perilambanei ta s sta opoia katalhgei kaue g synarthsh*/


  fclose(fp); /*kleinw to arxeio input_file poy periexei to Dataset moy giati to exw diabasei ki exw topouethsei*/
              /*tis eikones poy perieixe to Dataset stis domes anazhthshs, dhladh toys pinakes katakermatismoy*/

/////////////////////----------
  /*Se ayto to shmeio uelw na diabazw kaue eikona apo to image_table*/
	/*kai gia kaue pinaka katakermatismoy na ypologizw thn antistoixh synarthsh g*/
	/*p.x. ypologizw gia mia sygkekrimenh eikona ton unsigned int poy epistrefei h*/
  /*g2 vste na thn topouethsv sto katallhlo bucket ston 2o pinaka katakermatismoy*/

  int m = power(2,27) - 5;  /*m = 2^27 - 5*/
	int exponent = 32/K;
	int M = power(2,exponent);  /*M = 2^(32/K)*/

  int *m_modM;/*o pinakas aytos ua exei ola ta (m)modM poy ua xreiastw kata ton ypologismo twn h synarthsevn*/

	m_modM = create_mmodM(m, M, distances);


  bucket_ptr_table = Hash_Table_Creation(image_table, L, number_of_images, K, m_modM, distances, s_L_tables, w, M, &table_size);
  /*kataskeyasame toys L ejwterikoys pinakes katakermatismoy*/
////////////////////////////////////////////////////////////
  printf("Give the input_file_labels:");
  char* input_file_labels = malloc(MAX_LENGTH_WORD*sizeof(char));
  scanf("%s",input_file_labels);

	printf("Give the query_file_labels:");
	char* query_file_labels = malloc(MAX_LENGTH_WORD*sizeof(char));
	scanf("%s",query_file_labels);

	printf("Give the output_file from C exercise:");
  char* output_C = malloc(MAX_LENGTH_WORD*sizeof(char));
  scanf("%s",output_C);

	FILE *fp_in_labels;
	fp_in_labels = fopen(input_file_labels,"r");
	int* input_labels =  labels_creation(fp_in_labels);
	fclose(fp_in_labels);

	FILE *fp_qr_labels;
	fp_qr_labels = fopen(query_file_labels,"r");
	int* query_labels =  labels_creation(fp_qr_labels);
	fclose(fp_qr_labels);
///////////////////////////////////////////////////////////

	FILE* fnew_input = fopen(inputfilenewspace,"rb");
  FILE* fnew_query = fopen(queryfilenewspace,"rb");

  int img_numberof_inputfilenewspace, img_numberof_queryfilenewspace;
  int new_distances, new_distances_diff;

  input_info(fnew_input,&img_numberof_inputfilenewspace,&new_distances);
	input_info(fnew_query,&img_numberof_queryfilenewspace,&new_distances_diff);

  image_node* new_image_table = image_creation_new(fnew_input,img_numberof_inputfilenewspace,new_distances);

	char command[MAX_LENGTH_WORD];
	char command2[MAX_LENGTH_WORD];
	//operation
while(1) {

	FILE *fp_qr = fopen(query_file,"r");
  FILE* out = fopen(output_file,"a");
	FILE* outC = fopen(output_C,"a");

	int qr_number_of_im;
	int qr_dist;

	image_node newquery;

  input_info(fp_qr, &qr_number_of_im, &qr_dist);
	image_node* query_image_table = image_creation(fp_qr, qr_number_of_im, qr_dist); //-

  //printf("qr_number_of_im=%d qr_dist=%d\n", qr_number_of_im, qr_dist);//to query_file exei 10000 eikones me diastaseis 784
	newquery.pixels = malloc(new_distances*sizeof(int));
	unsigned int *g_valuesofQuery = malloc(L*sizeof(unsigned int));

  int *appr_NN = malloc(N*sizeof(int)); //periexei ta image_numbers twn plhsiesterwn geitonwn
	int *dist_NN = malloc(N*sizeof(int)); //periexei ta distances twn plhsiesterwn geitonwn apo to kaue query

	int *exct_NN = malloc(N*sizeof(int));
	int *exdist_NN = malloc(N*sizeof(int));
	int* newexct_NN = malloc(N*sizeof(int));
	int* newexdist_NN = malloc(N*sizeof(int));
	double sum1=0.0;
	double sum2=0.0;
	double sum3=0.0;
	double apprfacLSH = 0.0;
  double apprfacNeurNet = 0.0;
	int correct = 0;

	for(int i = 0; i < qr_number_of_im; i++) { /*gia kaue eikona apo to query_file*///anti gia 100 eixa qr_number_of_im
    //printf("query = %d\n",i);
		fprintf(out,"Query: %d\n",i);

		for(int j = 0; j < new_distances; j++) {

			unsigned char buf[2];
			fread(buf,2*sizeof(unsigned char), 1,fnew_query);
			int number = (buf[0]<<8)+buf[1];
			int pixel = number;
			newquery.pixels[j] = pixel;

		}
		/*gia to query i na brw ta g toy gia toys L pinakes katakermatismoy kai na ta apouhkeysw se enan pinaka*/
		for (int j = 0; j < L; j++)  /*ston g_valuesofQuery apouhkeysa tis g times toy query i*/
		{	g_valuesofQuery[j] = compute_g(&query_image_table[i], j, K, m_modM, distances, s_L_tables, w, M);
		    //if(i == 0) printf("%u\n", g_valuesofQuery[j]);
	   	}

		for (int j = 0; j < N; j++) {
			appr_NN[j] = -1;
			dist_NN[j] = -1;
			exct_NN[j] = -1;
			exdist_NN[j] = -1;
			newexct_NN[j] = -1;
			newexdist_NN[j] = -1;
		}

		clock_t t2;
		t2 = clock();
    exact_NN(newquery,new_image_table,img_numberof_inputfilenewspace,new_distances,newexct_NN,newexdist_NN,N);
		t2 = clock() - t2;
		double time_taken2 = ((double)t2)/CLOCKS_PER_SEC;
		sum1+=time_taken2;

		clock_t t;
		t = clock();
    approximateNN(query_image_table[i], g_valuesofQuery, N, L, bucket_ptr_table, table_size, distances, appr_NN, dist_NN);
		t = clock() - t;
		double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
		sum2+=time_taken;


		clock_t t1;
		t1 = clock();
    exact_NN(query_image_table[i], image_table, number_of_images, distances, exct_NN, exdist_NN, N);
		t1 = clock() - t1;
		double time_taken1 = ((double)t1)/CLOCKS_PER_SEC;
		sum3+=time_taken1;
		if(query_labels[i] == input_labels[exct_NN[0]-1]) correct++;

		double apprtemp = ((double)dist_NN[0])/exdist_NN[0];
		apprfacLSH = apprfacLSH + apprtemp;

		int temp1 =  manhattan_dist(&image_table[newexct_NN[0]-1], &query_image_table[i], distances);
		apprtemp = (double)temp1/exdist_NN[0];
		apprfacNeurNet = apprfacNeurNet + apprtemp;

		fprintf(out,"Nearest neighbor Reduced:%d\n",newexct_NN[0]);
		fprintf(out,"Nearest neighbor LSH:%d\n",appr_NN[0]);
		fprintf(out,"Nearest neighbor True:%d\n",exct_NN[0]);
		fprintf(out,"distanceReduced:%d\n",newexdist_NN[0]);
		fprintf(out,"distanceLSH: %d\n",dist_NN[0]);
		fprintf(out,"distanceTrue: %d\n\n",exdist_NN[0]);
	}

	fprintf(out,"\ntReduced:%f\n",sum1);
	fprintf(out,"tLSH: %f\n",sum2);
	fprintf(out,"tTrue: %f\n",sum3);
	apprfacLSH = apprfacLSH/qr_number_of_im;  //anti gia 100 eixa qr_number_of_im
	apprfacNeurNet = apprfacNeurNet/qr_number_of_im; //anti gia 100 eixa qr_number_of_im
	fprintf(out,"Approximation Factor LSH: %f\n",apprfacLSH);
	fprintf(out,"Approximation Factor Reduced: %f\n",apprfacNeurNet);

  fprintf(outC,"Average Correct Search Results MANHATTAN: %f\n",(double)correct/qr_number_of_im); //anti gia 100 eixa qr_number_of_im

  free(g_valuesofQuery);
	free(appr_NN);
	free(dist_NN);
	free(exct_NN);
	free(exdist_NN);
	free(newexdist_NN);
	free(newexct_NN);
	free(newquery.pixels);

	for(int i = 0; i < qr_number_of_im; i++)
		free(query_image_table[i].pixels);
	free(query_image_table);

	fclose(fp_qr);
	fclose(out);
	fclose(outC);
	fclose(fnew_query);
	fclose(fnew_input);
	printf("%s\n","Type the name of new query file and new output file or type NO if you want to terminate the program");
	scanf("%s",command);

	if(!strcmp(command,"NO"))
		break;
	else {
		memset(query_file, 0, strlen(query_file));
		strcpy(query_file, command);   /*ua asxolhuw me neo query_file an moy zhthuei apo ton xrhsth*/
    	scanf("%s",command2);
		memset(output_file, 0, strlen(output_file));
		strcpy(output_file, command2);
	}

}


		free(input_file_labels);
		free(query_file_labels);
		free(output_C);
		free(input_labels);
		free(query_labels);
  	free(m_modM);  //PREPEI NA APELEYUERWSW TON m_MODM!!

  	exit_memory(query_file, output_file, input_file, number_of_images, image_table, L, K, s_L_tables, bucket_ptr_table, table_size,inputfilenewspace,queryfilenewspace,new_image_table,img_numberof_inputfilenewspace);
	/*apeleyuerwnw thn mnhmh*/


	return 0;

}
