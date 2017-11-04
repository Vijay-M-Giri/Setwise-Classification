#include "helper.h"



void freeDATA(DATA data){
	int i,j;
	for(i=0;i<data.size;i++)
		free(data.data[i]);
	free(data.data);
}

void showError(char* str){
	fprintf(stderr,"ERROR: %s.",str);
	exit(0);
}

double eucledianDistance(double* A,double* B,int dimension){
	double dis = 0.0;
	int i;
	for(i=0;i<dimension;i++)
		dis += (A[i] - B[i]) * (A[i] - B[i]);
	dis = sqrt(dis);
	return dis;
}

void shuffle(int n,int* array){
	// Fisher-Yates shuffle
	int i;
	for(i=n-1;i;i--){
		int j = rand() % (i+1);
		int t = array[i];
		array[i] = array[j];
		array[j] = t;
	}
}

void chooseRFromN(int n,int r,int* choose){
	int i,numbers[n];
	for(i=0;i<n;i++)
		numbers[i] = i;
	shuffle(n,numbers);
	for(i=0;i<r;i++)
		choose[i] = numbers[i];
}

int closestPoint(double* point, double** points, int dimension, int K){
	int id = -1;
	double mn = DBL_MAX;
	int i,j;
	for(i=0;i<K;i++){
		double dis = eucledianDistance(point,points[i],dimension);
		if(dis < mn){
			mn = dis; 
			id = i;
		}
	}
	return id;
}
