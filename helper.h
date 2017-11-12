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
	int cntUpdate;
}ENTITY;


extern struct Branch** leafPointers;

extern void initEntity(ENTITY* entity,int dimension);
extern void freeEntity(ENTITY entity);
extern double cosineDistance(double* A,double* B,int dimension);
extern int closestPoint(double* point, double** points, int dimension, int K);
extern void freeDATA(DATA data);
extern void showError(char* str);
extern double eucledianDistance(double* A,double* B,int dimension);
extern void chooseRFromN(int r,int n,int* choose);

#endif
