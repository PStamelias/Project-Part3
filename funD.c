#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "structD.h"
#define MAX_COUNT_WORD 50


double randfrom(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}


int* initialization_kmeans(int K, image_node* image_table, int number_of_images, int distances) {
  //opoy K einai o ariumos twn clusters
	srand(time(NULL));

	int* K_clust = malloc(K*sizeof(int)); //pinakas me ta num_of_clusters se ariumo arxika kentra poy ua epilejw efarmozwntas ton kmeans++
	int first_K = rand()%number_of_images; //epilegw tyxaia to prvto kentro

	int* D = malloc(number_of_images*sizeof(int)); //pinakas me ta D(i)

  for (int i = 0; i < number_of_images; i++)
		D[i] = 0;

	D[first_K] = -1;  //se kaue kentro bazw D(i) = -1
	int t = 1; //to t einai o ariumos twn kentrwn poy exw brei mexri stigmhs

	while(1) {

		if(t == K) //an exw brei ta K kentra teleiwsa
			break;

		int coun_K = 0;
    int* pos_first = NULL; //pinakas poy ua exei ta kentra poy exw brei mexri stigmhs

		for (int j = 0; j < number_of_images; j++) { //tsekarw mia-mia tis ueseis toy D
			if(D[j] == -1) {    //an brhka kentroeides
				pos_first = realloc(pos_first, (coun_K+1)*sizeof(int)); //to prosuetw ston pos_first
				pos_first[coun_K++] = j;
				if(coun_K == t) break;
			}
		}

    int max = -1;

		for (int l = 0; l < number_of_images; l++) { //gia kaue mh kentroeides caxnw na brw thn minimum apostash toy apo kapoio kentro
			if(D[l] == -1) //an eimai se kentroeides synexise
				continue;
			int min = INT_MAX;
			for (int e = 0; e < coun_K; e++) {
				int apostasi = manhattan_dist(&image_table[l], &image_table[pos_first[e]], distances);
				if(apostasi < min)
					min = apostasi;
			}
			D[l] = min;
			if(D[l] > max) max = D[l];
		}

		Pnode* P_table = NULL; //pinakas me ta merika auroismata gia ta mh kentroeidh
		int coun2 = 0;
    //Pnode* P_table = malloc((number_of_images-t)*sizeof(Pnode)); //pinakas me ta merika auroismata gia ta mh kentroeidh

		for (int c = 0; c < number_of_images; c++) {
			if(D[c] == -1) //an eimai se kentroeides proxwraw
				continue;
			if(coun2 == 0) { //an eimai sto prwto mh kentroeides
				P_table = realloc(P_table, (coun2+1)*sizeof(Pnode));
				P_table[coun2].img_num = c;
				P_table[coun2].P = ((double)D[c]/(double)max)*((double)D[c]/(double)max);
				//if(t==4) printf("P_table[%d].P = %f\n", coun2, P_table[coun2].P);
				coun2++;
				if(coun2 == number_of_images - t) break;
			}
			else { //an den eimai sto prwto mh kentroeides
				P_table = realloc(P_table,(coun2+1)*sizeof(Pnode));
				P_table[coun2].img_num = c;
				P_table[coun2].P = P_table[coun2-1].P + ((double)D[c]/(double)max)*((double)D[c]/(double)max);
				//if(t==4) printf("P_table[%d].P = %f\n", coun2, P_table[coun2].P);
				coun2++;
				if(coun2 == number_of_images - t) break;
			}
		}

		float c = randfrom(0, P_table[coun2-1].P);

		for (int y = 0; y < coun2; y++) {
			if(c <= P_table[y].P) {
				D[P_table[y].img_num] = -1;
				break;
			}
		}

		free(P_table);
		free(pos_first);

		t+=1;

	}

	int size = 0;

	for (int y = 0; y < number_of_images; y++)
		if(D[y] == -1)
		{
			K_clust[size] = y;
      size++;
			if(size == K) break;
		}


	free(D);

	return K_clust;


}



void Lloyds(int *assignments, image_node* image_table, image_node *kentroeidh, int number_of_images, int num_of_clusters, int distances) {

  for (int j = 0; j < number_of_images; j++) { //gia kaue eikona poy antistoixei se kaue uesh toy assignments

    if(assignments[j] == -1) { //an den exei anateuei ayth h eikona se kapoio kentro
      int min = INT_MAX;
      int temp;
      for (int k = 0; k < num_of_clusters; k++) { //gia thn eikona image_table[j] caxnw na brw to kontinotero kentroeides se aythn
        temp = manhattan_dist(&image_table[j], &kentroeidh[k], distances);
        if(temp < min) {
          min = temp;
          assignments[j] = k;
        }
      }
    }

  }

}


int up_value_fun(int pos) {  //an pos = 7 epistrefei anwtabani(3,5) = 4
	float val = (double)pos/2.0; //an pos = 4 epistrefei anwtabani(2) = 2
	int g = (int)val;
	float y = (float)g +0.5;

	if(val >= y) return g+1;
	else return g;
}


void update(int *assignments, image_node* kentroeidh, image_node* image_table, int number_of_images, int num_of_clusters, int distances) {

	for (int k = 0; k < num_of_clusters; k++) { //gia kaue cluster
		for (int h = 0; h < distances; h++) { //gia kaue diastash toy

			int coun = 0;
			int* table_dist = NULL;
			for (int i = 0; i < number_of_images; i++) { //paw kai tsekarw kaue eikona
				if(assignments[i] == k) {  //an h eikona ayth anateuhke sto cluster k
					table_dist = realloc(table_dist,(coun+1)*sizeof(int));
					table_dist[coun] = image_table[i].pixels[h]; //bale to pixel ths h diastashs ths eikonas ston table_dist
					coun++;
				}
			}

			qsort( table_dist, coun, sizeof(int), compare ); //sortare ton table_dist kata ayjoysa seira
			int e = 0;
			int prev_val = -1;

			for (int g = 0; g < coun; g++) { //uelw tis diakrites times//to table_dist mporei na exei {0,1,4,4,8}, egv uelw to {0,1,4,8}
				if(table_dist[g] > prev_val) {
					e++;
					prev_val = table_dist[g]; //table_dist[e];<-----APORIA???????????????
				}
			}

      int upper_value = up_value_fun(e);
      e = 0;
			prev_val = -1;

			for (int g = 0; g < coun; g++) { //uelw tis diakrites times//to table_dist mporei na exei {0,1,4,4,8}, egv uelw to {0,1,4,8}
				if(table_dist[g] > prev_val) {
					e++;
					prev_val = table_dist[g]; //table_dist[e];<-----APORIA???????????????
				}
				if(e == upper_value) {
					kentroeidh[k].pixels[h] = prev_val;
					break;
				}
			}
			free(table_dist);

		}
	}

}



double silhouette(int image, int cluster, image_node *kentroeidh, int num_of_clusters, int *assignments, int number_of_images, image_node* image_table, int distances) {
  //h eikona image_table[image] anhkei sto kentroeidh[cluster]
  //gia mia eikona ypologizei to (b(i)-a(i))/max{a(i),b(i)}
  double result;

  double a; //a(i) twn diafaneiwn<-average apostasewn ths eikonas apo tis ypoloipes toy cluster
  double b; //b(i) twn diafaneiwn<-average apostasewn ths eikonas apo tis eikones toy 2oy kontinoteroy cluster

  int min = INT_MAX;
  int second, dist;
  for (int i = 0; i < num_of_clusters; i++) { //sto second ua exw to deytero kontinotero kentroeides sthn eikona image_table[image]
    if(i != cluster) { //an den eimai sto cluster poy exw hdh thn eikona image mesa, ejetazw an einai to 2o kontinotero sthn eikona moy
      dist = manhattan_dist(&image_table[image], &kentroeidh[i], distances);
      if(dist < min) {
        min = dist;
        second = i;
      }
    }
  }

  int dist_a = 0;
  int count_a = 0;
  int dist_b = 0;
  int count_b = 0;

  for (int i = 0; i < number_of_images; i++) {

    if(assignments[i] == cluster && i != image) {
      dist = manhattan_dist(&image_table[image], &image_table[i], distances);
      dist_a = dist_a + dist;
      count_a = count_a + 1;
    } else if(assignments[i] == second) {
      dist = manhattan_dist(&image_table[image], &image_table[i], distances);
      dist_b = dist_b + dist;
      count_b = count_b + 1;
    }

  }
  if(count_a==0)
  	a=0;
  if(count_b==0)
  	b=0;
  a = ((double)dist_a)/((double)count_a);
  b = ((double)dist_b)/((double)count_b);

  double max;
  if(a < b) max = b;
  else max = a;

  if(max==0)
  	return 0;
  result = (b-a)/max;

  return result;

}

void printresults(FILE* outptr,int num_of_clusters,int number_of_images,int *assignments,int distances,image_node *kentroeidh,image_node* image_table,double time_taken) {
  int count;
  for (int i = 0; i < num_of_clusters; i++) {
    fprintf(outptr,"CLUSTER-%d {",i+1);

    count = 0;
    for (int j = 0; j < number_of_images; j++) {
      if(assignments[j] == i) count++;
    }
    fprintf(outptr,"size: %d, ",count);

    fprintf(outptr,"%s","centroid: ");
    for (int j = 0; j < distances; j++) {
      fprintf(outptr,"%d",kentroeidh[i].pixels[j]);
      if(j != distances-1) fprintf(outptr,"%s",", ");
    }

    fprintf(outptr, "%s\n", "}");
  }

  fprintf(outptr,"clustering_time: %f\n",time_taken);



  fprintf(outptr,"%s\n","Silhouette: [");
  //silhouette:
  double s_i;
  double s_total = 0;

  for (int i = 0; i < num_of_clusters; i++) { //gia kaue cluster ua brw to silhouette toy

    s_i = 0;
    count = 0;
    for (int j = 0; j < number_of_images; j++) {   //edw eixa number_of_images anti gia 1000!!!!!!!!!!!6666^^^66666666
      //printf("j=%d\n", j);
      if(assignments[j] == i) {  //gia kaue eikona j poy anhkei sto cluster i
        double tempsi = silhouette(j, i, kentroeidh, num_of_clusters, assignments, number_of_images, image_table, distances);
        s_i = s_i + tempsi;
        count = count + 1;
        s_total = s_total + tempsi;
      }
    }

    s_i = s_i/count; //<--silhouette toy cluster i
    fprintf(outptr,"%f,",s_i);

  }

  s_total = s_total/number_of_images;
  fprintf(outptr," %f]\n",s_total);
}

double objfunction(int number_of_images,int *assignments,int distances,image_node *kentroeidh,image_node* image_table) {
  int dist;
  double sum = 0.0;
  for (int i = 0; i < number_of_images; i++) {
    dist = manhattan_dist(&image_table[i], &kentroeidh[assignments[i]], distances);
    sum = sum+dist;
  }
  return sum;
}


void exit_memory_Cluster(FILE* inptr,FILE* outptr,char* input_file,char* configuration_file,char* output_file,int* K_clusters_num,image_node* image_table,int image_number,int* assignments,image_node* kentroeidh,int num_of_clusters){
	fclose(inptr);
	fclose(outptr);
	free(input_file);
	free(configuration_file);
	free(output_file);
	free(K_clusters_num);
	for (int i = 0; i < num_of_clusters; i++)
		free(kentroeidh[i].pixels);
	free(kentroeidh);
  	free(assignments);
	for(int i = 0; i < image_number; i++)
		free(image_table[i].pixels);
	free(image_table);
}
