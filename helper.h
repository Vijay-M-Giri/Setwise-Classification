#ifndef __HELPER_H
#define __HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <time.h>


typedef struct{
	double** data;
	int size;
	int dimension;
	int noOfClass;
	int noOfEntity;
}DATA;

typedef struct{
	double* fingerprint;
	int size;
	int dimension;
	int label;
	int profile;
}ENTITY;

int closestPoint(double* point, double** points, int dimension, int K);
void freeDATA(DATA data);
void showError(char* str);
double eucledianDistance(double* A,double* B,int dimension);
void chooseRFromN(int r,int n,int* choose);

#endif
