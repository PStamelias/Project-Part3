#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include "structB.h"

image_node* image_creation(FILE* file, int number_of_images, int distances) {

	image_node* my_table;
	unsigned char b;

  	my_table = malloc(number_of_images*sizeof(image_node));

	for(int i = 0; i < number_of_images; i++) {
		my_table[i].pixels = malloc(distances*sizeof(int));
		my_table[i].image_number = i+1;
	}

	for(int i = 0; i < number_of_images; i++)
		for(int j = 0; j < distances; j++) {
			fread(&b,sizeof(unsigned char),1,file); /*diabazw ena pixel(poy einai enas akeraios sto [0,255])*/
                                              /*to pixel to diabazw san unsigned char(ena pixel einai 1 byte)giati diaforetika mporei*/
                                              /*na moy dwsei arnhtiko apotelesma, pragma poy den uelw*/
			my_table[i].pixels[j] = (int)b; /*telos kanw cast to b se integer gia na to apouhkeysw sthn j-osth uesh*/
                                      /*toy pinaka pixels ths sygkekrimenhs eikonas i*/
		}
		//exw brei ta g1 g2 g3 (L = 3)//OXI   SE JEXVRH SYNARTHSH
    //topouethsh///OXI
	return my_table;

}



int* create_hi(int w,int d) { /*Function returns a d-size table with values between [0,w-1](s numbers)*/

	int* my_table=malloc(d*sizeof(int));

	for(int i=0;i<d;++i)
		my_table[i]=rand()%w;

	return my_table;
}



int** create_g(int k,int w,int d) { /*Function that returns k-functions h-create hash_table*/
	int** my_table=malloc(k*sizeof(int*));

	for(int i=0;i<k;++i)
		my_table[i]=create_hi(w,d);


	return my_table;
}



int*** create_s_numbers_ofLtables(int L,int k,int w,int d) { /*Function that create L g hash tables*/
	int*** my_table=malloc(L*sizeof(int**));
                                              /*ayth h synarthsh dinei telika enan trisdiastato pinaka*/
	for(int i=0;i<L;++i)                        /*o opoios gia kaue synarthsh g(ua exoyme L se ariumo synarthseis g)*/
		my_table[i]=create_g(k,w,d);              /*dinei k*d akeraioys ariumoys s sto diasthma [0,w)(giati kaue synarthsh*/
                                              /*g apoteleitai apo k synarthseis h kai kaue synarthsh h xreiazetai d se*/
	return my_table;                            /*ariumo ariumoys s)*/
}


void input_info(FILE* file, int* number_of_im, int* sum) { /*me thn input_info diabazw apo to arxeio eisodoy toys 4 prwtoys akeraioys*/
                                                  /*kai epistrefv tis diastaseis ths kaue eikonas(grammes*sthles) kai*/
                                                         /*to poses eikones exw*/
	int magic_number;
	int number_of_images;
	int number_of_columns;
	int number_of_rows;


	fread(&magic_number,sizeof(int),1,file);
	magic_number = ((magic_number>>24)&0xff)|((magic_number<<8)&0xff0000)|((magic_number>>8)&0xff00)|((magic_number<<24)&0xff000000);
  /*oi 4 prvtoi akeraioi ariumoi toy arxeioy einai se big endian*/
  /*toys diabazw loipon se big endian kai gia na toys metatrecw se little endian*/
  /*kanw thn parapanw diadikasia se epipedo bitwise(kai an ta bytes se */
  /*big endian exoyn thn seira 0 1 2 3, me thn parapanw metatroph ginontai 3 2 1 0)*/

	fread(&number_of_images,sizeof(int),1,file); /*pairnw to plhuos twn eikonwn*/
	number_of_images = ((number_of_images>>24)&0xff)|((number_of_images<<8)&0xff0000)|((number_of_images>>8)&0xff00)|((number_of_images<<24)&0xff000000);
	fread(&number_of_rows,sizeof(int),1,file); /*pairnw ton ariumo twn grammwn poy exei mia eikona*/
	number_of_rows = ((number_of_rows>>24)&0xff)|((number_of_rows<<8)&0xff0000)|((number_of_rows>>8)&0xff00)|((number_of_rows<<24)&0xff000000);
	fread(&number_of_columns,sizeof(int),1,file); /*pairnw ton ariumo twn sthlwn poy exei mia eikona*/
	number_of_columns = ((number_of_columns>>24)&0xff)|((number_of_columns<<8)&0xff0000)|((number_of_columns>>8)&0xff00)|((number_of_columns<<24)&0xff000000);


	*number_of_im = number_of_images;
	*sum = number_of_rows*number_of_columns;

}


int manhattan_dist(image_node *image1, image_node *image2, int distances) {  /*Uelw na brw thn apostash Manhattan metajy*/
	                                                                           /*twn eikonwn image1 kai image2*/
  int distance = 0;
  int diff = 0;

	for (int i = 0; i < distances; i++) {
		/*gia kaue zeygari syntetagmenwn twn image1, image2 briskw thn*/
		/*apolyth timh ths diaforas toy kai thn prosuetw sto distances*/
		/*Apostash_Manhattan(image1,image2) = Auroisma_1<=i<=distances(|image1_xi-image2_yi|)*/

	  diff = (image1 -> pixels[i]) - (image2 -> pixels[i]);

		if(diff < 0) diff = diff*(-1); /*an to diff htan arnhtiko,to pollaplasiazw me -1 gia na parw thn apolyth timh toy*/

		distance = distance + diff;

	}

  return distance;

}


int brute_force(image_node* image_table, int i, int number_of_images, int distances) {  /*se ena plhuos number_of_images eikonwn briskw ton */
                                                                                  /*plhsiestero geitona sthn eikona yp'arithmon i kai */
                                                                                  /*epistrefw thn apostash ths eikonas i apo ton*/
																																				          /*plhsiestero geitona ths*/
	int min_dist = -1;  /*to min_dist ua periexei thn apostash metajy ths eikonas i kai toy plhsiesteroy geitona ths eikonas i*/
  int new_dist = 0;

  for (int j = 0; j < number_of_images; ++j) {

		if (j != i) { /*an ejetazw diaforetikh eikona apo thn i (gia na mhn brw thn apostash ths eikonas i me ton eayto ths)*/

			if (min_dist == -1) /*an einai h prwth eikona poy ejetazw, anagkastika to min_dist ua parei thn Apostash_Manhattan metajy ths i kai ths j*/
				min_dist = manhattan_dist(&image_table[i], &image_table[j], distances);

			else { /*an den einai h prwth eikona poy ejetazw*/
				new_dist = manhattan_dist(&image_table[i], &image_table[j], distances);
				if(new_dist < min_dist)  min_dist = new_dist;
			}

  	}

  }

	return min_dist;

}


int power(int number, int exponent) {  /*ypologizw to number^exponent*/

  if(exponent == 0) return 1;

  int result = 1;

  for (int i = 0; i < exponent; i++)
  	result = result*number;

	return result;
}


int mod(int a, int b) {   /*to b ua einai panta dynamh toy 2(kai uetikos ariumos) kai gia*/
													/*dynameis toy 2 isxyei oti xmod(2^n) == x&((2^n) - 1)*/
	int result;
	int new_b = b - 1;

	result = a&new_b;

	return result;

}


int *create_mmodM(int m, int M, int distances) {

	int *m_modM = malloc(distances*sizeof(int));  /*o pinakas m_modM periexei me thn seira ta ejhs stoixeia:*/
																								/*1,mmodM,(m^2)modM,(m^3)modM,...,(m^(d-1))modM*/
																								/*Kaue stoixeio mporei na ypologisuei etsi: (m^x)modM = (m*m^(x-1))modM =*/
																								/*= ((mmodM)*((m^(x-1))modM))modM*/
  m_modM[0] = 1;

  m_modM[1] = mod(m,M);  //sto m_modM[1] exw to mmodM

	unsigned int product;

	for (int i = 2; i < distances; ++i) {
			product = (unsigned int)(m_modM[1]*m_modM[i-1]);
			m_modM[i] = mod((int)product,M);
	}

	return m_modM;

}



int compute_h(image_node *image, int *m_modM, int distances, int* s_array, int w, int M) { /*briskw thn akeraia timh mias*/
																																																		/*sygkekrimenhs h gia mia sygkekrimenh*/
																																																		/*eikona*//*synarthsh compute_h*/
  int result = 0;                                                                                   /*ousiastika ypologizei to h(x) = */
  int temp;                                                                                         /*= (a(d-1)+m*a(d-2)+...+m^(d-1)*a(0))modM*/
	unsigned int product;                                                               /*s_array = pinakas me ta s mias sygkekrimenhs h*/
  int ai;  /*ai = floor((pi-si)/w)*/
	int xi, si;

	for (int i = 0; i < distances; ++i) {
		xi = image -> pixels[(distances - 1) - i];
		si = s_array[(distances - 1) - i];
		if((xi - si) >= 0) ai = (xi - si)/w;
		else {
			if((si - xi)%w != 0) ai = ((xi - si)/w) - 1; /*an exw (-5)/2 ,(-5)/2 = -2 egw omws uelw to floor dhladh to floor(-2,5) = -3*/
			else ai = (xi - si)/w;  /*an diairoyntai akribws tote kanena problhma*/
		}

		temp = mod(ai, M);

		if(i != 0) {
			product = (unsigned int)(temp*m_modM[i]);
			temp = mod((int)product,M);
	  }
		result = result + temp;
	}

	result = mod(result, M);

	return result;

}



unsigned int compute_g(image_node *image, int table, int K, int *m_modM, int distances, int*** s_L_tables, int w, int M) {
	                                                                                  /*briskw thn akeraia timh mias sygkekrimenhs g*/

  unsigned int result = 0;

  int *h_table_of_g = malloc(K*sizeof(int)); /*o pinakas aytos ua periexei ths h1(x),h2(x),...,hk(x) gia na sxhmatistei*/
																						 /*h synarthsh g(x) (= [h1(x)|h2(x)|...|hk(x)] ) poy antistoixei se enan sygkekrimeno*/
																						 /*pinaka katakermatismoy gia mia sygkekrimenh eikona x (=image)*/
	for (int i = 0; i < K; i++)
	{	h_table_of_g[i] = compute_h(image, m_modM, distances, s_L_tables[table][i], w, M);
		//printf("h%d = %d  ", i+1, h_table_of_g[i]);
  }

  /*twra kanoyme to concatenation gia na dhmioyrghsoyme thn akeraia timh toy g:*/
	int num_bits = 32/K; /*apo kaue h ua paroyme ta num_bits prwta bits kai ayta ua kanoyme*/
											 /*concatenation me tis ypoloipes h*/
  unsigned int keep = (unsigned int)(power(2, num_bits) - 1); /*me to keep ua krataw akribws ta bits poy me endiaferoyn*/
	unsigned int temp = 0;															       /*to keep einai mia seira apo num_bits assoys */

	for (int i = 0; i < K; i++) {
		temp = (unsigned int)h_table_of_g[i];
		temp = temp&keep; //krataw apo thn hi ta prwta num_bits poy me endiaferoyn
		result = (result << num_bits)|temp;
	}

	free(h_table_of_g);

	return result; //epistrefoyme to g

}



bucket*** Hash_Table_Creation(image_node* image_table, int number_of_hash_tables, int number_of_images, int K, int *m_modM, int distances, int*** s_L_tables, int w, int M, int* table_siz) {
/*kataskeyazw toys L pinakes ths LSH kai topouetw sta buckets toys tis eikones apo to Dataset moy*/
	bucket*** bucket_ptrs = malloc(number_of_hash_tables*sizeof(bucket**));    /*ousiastika number_of_hash_tables = L*/
  /*o bucket_ptrs deixnei stoys ejwterikoys pinakes katakermatismoy*/
	//int choice = rand()%2;
	int table_size; /*to megeuos twn ejwterikwn pinakwn katakermatismoy*/

	//if(choice == 0)
	table_size = number_of_images/8;
	//else
	//table_size = number_of_images/16;

	for(int i = 0; i < number_of_hash_tables; i++)
		bucket_ptrs[i] = malloc(table_size*sizeof(bucket*)); /*desmeyw mnhmh gia kaue ejwteriko pinaka katakermatismoy*/

	for(int i = 0; i < number_of_hash_tables; i++)
		for(int j = 0; j < table_size; j++)
			bucket_ptrs[i][j] = NULL;

	for(int i = 0; i < number_of_hash_tables; i++) /*gia kaue ejwteriko pinaka katakermatismoy*/
		for(int j = 0; j < number_of_images; j++) {  /*gia kaue eikona*/

			bucket* p = malloc(sizeof(bucket));
			unsigned int g = compute_g(&image_table[j], i, K, m_modM, distances, s_L_tables, w, M); /*h eikona image_table[j] gia ton yp'arithmon*/
			                                                     /*i pinaka katakermatismoy dinei thn timh g (dhladh gi(x) = g opoy x einai h eikona)*/
			p->g = g;
			p->next = NULL;
			p->image_info = &image_table[j];
			int pos = g%table_size;  /*se poia uesh toy ejwterikoy pinaka kat/smoy yp'arithmon i ua mpei*/

			bucket** node = &bucket_ptrs[i][pos];     /*to bucket poy deixnei sthn eikona image_table[j]*/

			if(*node == NULL)        /*afoy brhka se poia uesh ston ejwteriko pinaka kat/smoy yp'arithmon i*/
				*node = p;             /*antistoixei to neo moy bucket, paw kai sarwnw thn lista apo buckets*/
			else {                   /*poy deixnei ayth h uesh kai topouetw to neo bucket sto telos ths listas*/
				bucket* temp = *node;
				while(1) {
					if(temp->next == NULL) {
						temp->next = p;
						break;
					}
					temp = temp->next;
				}
			}


		}

	*table_siz = table_size;
	return bucket_ptrs;


}




void approximateNN(image_node query, unsigned int *g_valuesofQuery, int N, int L, bucket*** Hash_Tables, int table_size, int distances, int *appr_NN, int *dist_NN) {
//oles oi ueseis twn appr_NN kai dist_NN ua exoyn arxika -1
  unsigned int g;
	int pos;
	unsigned int g_temp; /*ua krataei ta g twn buckets poy diabazw*/
	int new_appr; //neos ypochfios plhsiesteros geitonas
	int new_dist; //apostash toy neoy ypochfioy plhsiesteroy geitona
	int check = 0; //an check == 1 shmainei oti eimai se geitona poy exw hdh balei ston pinaka appr_NN
								 //an check == 0 shmainei oti eimai se geitona poy den exw balei ston appr_NN
///////////
	for (int i = 0; i < L; i++) { //gia kaue pinaka katakermatismoy

		g = g_valuesofQuery[i]; //pairnw thn timh g toy query gia ton i-osto pinaka katakermatismoy
		pos = g%table_size; //bres se poio bucket toy pinaka i paei to sygkekrimeno query

		bucket *temp = Hash_Tables[i][pos]; /*ua diatrejw thn lista apo buckets poy deixnei h sygkekrimenh uesh pos*/

		while(temp != NULL) /*an deixnei se kapoio bucket h uesh pos/diatrexw thn lista apo buckets*/
		{
			g_temp = temp -> g; //pare to g toy bucket
			if(g == g_temp) /*an exei idio g me ayto toy query mas*/
			{
				new_appr = temp -> image_info -> image_number;
				new_dist = manhattan_dist(&query, temp -> image_info, distances);
				check = 0;

				for (int j = 0; j < N; j++) {
					if(appr_NN[j] == -1)
						break;
					if(appr_NN[j] == new_appr)
					{ check = 1;
						break;
					}
				}/////---|

				if(check != 1) //an o geitonas poy ejetazw den exei hdh mpei ston appr_NN
				{
				  int keep = -1; //to keep ua krataei thn uesh sthn opoia brhka enan geitona me megalyterh apostash apo ton neo geitona

					for (int j = 0; j < N; j++) {
						if(dist_NN[j] == -1) /*an den yparxei kapoios geitonas se aythn thn uesh j toy appr_NN*/
						{ dist_NN[j] = new_dist;
							appr_NN[j] = new_appr;
							break;
						}
						if(new_dist < dist_NN[j])
						{	keep = j;
							break;
						}
					}////////----|

					if(keep != -1) /*an brhka oti kapoios geitonas poy exw hdh brei exei megalyterh apostash apo ton neo,tote ton antikauistw me ton neo*/
					{
						for (int j = N-1; j >= keep; j--) {
							if(j == keep)
							{	dist_NN[j] = new_dist;
								appr_NN[j] = new_appr;
								break;
							}
							dist_NN[j] = dist_NN[j-1];
							appr_NN[j] = appr_NN[j-1];
						}
					}///////----|

			 }
			}
			temp = temp -> next;

		}

	}
///////////

}





void exact_NN(image_node query, image_node* image_table, int number_of_images, int distances, int *exact_NN, int *dist_NN, int N) { /*ypologizei akribws toys N plhsiesteroys geitones*/
//oles oi ueseis twn exact_NN kai dist_NN ua exoyn arxika -1
	int new_exNN;
	int new_dist;
	int keep;

	for (int i = 0; i < number_of_images; i++) {
		new_exNN = image_table[i].image_number;
    	new_dist = manhattan_dist(&query, &image_table[i], distances);

		keep = -1;

		for (int j = 0; j < N; j++) {
			if(dist_NN[j] == -1)
			{ dist_NN[j] = new_dist;
				exact_NN[j] = new_exNN;
				break;
			}
			if(new_dist < dist_NN[j])
			{ keep = j;
				break;
			}
		}////---|

		if(keep != -1)
		{
			for (int j = N-1; j >= keep; j--) {
				if(j == keep)
				{	dist_NN[j] = new_dist;
					exact_NN[j] = new_exNN;
					break;
				}
				dist_NN[j] = dist_NN[j-1];
			  exact_NN[j] = exact_NN[j-1];
			}////---|
		}
	}


}






int compare( const void* a, const void* b){
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}



void insert_list(image** start,int img_num){
	image* i=malloc(sizeof(image));
	i->next=NULL;
	i->image=img_num;
	if(*start==NULL)
		*start=i;
	else{
		image* k=*start;
		while(1){
			if(k->next==NULL){
				k->next=i;
				break;
			}
			k=k->next;
		}
	}
}


int search_list(image* start,int img_num) {
	int found=0;
	image* k=start;
	while(1) {

		 if(k != NULL) {
			 if(k->image == img_num) {
				 found=1;
 				 break;
			 }
			 k=k->next;
		 }
		 else break;

	}
	return found;

}



int size_list(image* start){
	int size=0;
	while(1){
		if(start==NULL)
			break;
		size++;
		start=start->next;
	}
	return size;
}



void print_list(image* start,FILE* out){
	int size=0;
	image* k=start;
	image* t=start;
	image* e=start;
	while(1){
		if(t==NULL)
			break;
		size++;
		t=t->next;
	}
	int* my_table=malloc(size*sizeof(int));
	int coun=0;
	while(1){
		if(k==NULL)
			break;
		my_table[coun++]=k->image;
		k=k->next;
	}
	qsort( my_table, size, sizeof(int), compare );
	for(int i=0;i<size;i++)
		fprintf(out,"image_number_%d\n",my_table[i]);
	free(my_table);
	if(e==NULL)
		return;
	image* g=e->next;
	while(1){
		free(e);
		e=g;
		if(e==NULL)
			break;
		g=g->next;
	}
}


void exit_memory(char* query_file, char* output, char* input, int number_of_images, image_node* image_table, int L, int K, int*** L_tables, bucket*** bucket_ptr, int table_size,char* inputfilenewspace,char* queryfilenewspace,image_node* new_image_table,int img_numberof_inputfilenewspace) {

	free(output);
	free(query_file);
	free(input);


	free(inputfilenewspace);
	free(queryfilenewspace);

	for(int i = 0; i < L; i++) {
		for(int k = 0; k < table_size; k++) {
			bucket* p = bucket_ptr[i][k];
			if(p == NULL)
				continue;
			bucket* i = p->next;
			while(1) {
				free(p);
				p = i;
				if(p == NULL)
					break;
				i = i->next;
			}
		}
		free(bucket_ptr[i]);
	}
	free(bucket_ptr);

	for(int i = 0; i < number_of_images; i++)
		free(image_table[i].pixels);
	free(image_table);
	for(int i = 0; i < L; i++) {
		for(int a = 0; a < K; a++)
			free(L_tables[i][a]);
		free(L_tables[i]);
	}
	free(L_tables);

 	for(int i=0;i<img_numberof_inputfilenewspace;i++)
  		free(new_image_table[i].pixels);
  	free(new_image_table);


}



image_node* image_creation_new(FILE* file, int number_of_images, int distances){
	image_node* my_table;

	int num;
	unsigned char buf[2];
	int swapped;
  	my_table = malloc(number_of_images*sizeof(image_node));


	for(int i = 0; i < number_of_images; i++) {
		my_table[i].pixels = malloc(distances*sizeof(int));
		my_table[i].image_number = i+1;
	}

	for(int i = 0; i < number_of_images; i++){
		for(int j = 0; j < distances; j++) {
			fread(buf,2*sizeof(unsigned char), 1,file); //fread(buf,2, 1,file);
			int number = (buf[0]<<8)+buf[1];
			my_table[i].pixels[j] = number;
		}
	}
	return my_table;
}



int* labels_creation(FILE* file) {

	int* array;
  int magic_number, number_of_images;

	fread(&magic_number,sizeof(int),1,file);
	magic_number = ((magic_number>>24)&0xff)|((magic_number<<8)&0xff0000)|((magic_number>>8)&0xff00)|((magic_number<<24)&0xff000000);
	fread(&number_of_images,sizeof(int),1,file); /*pairnw to plhuos twn eikonwn*/
	number_of_images = ((number_of_images>>24)&0xff)|((number_of_images<<8)&0xff0000)|((number_of_images>>8)&0xff00)|((number_of_images<<24)&0xff000000);

	unsigned char b;

  array = malloc(number_of_images*sizeof(int));

	for(int i = 0; i < number_of_images; i++) {
			fread(&b,sizeof(unsigned char),1,file);
			array[i] = (int)b;
	}

	return array;

}
