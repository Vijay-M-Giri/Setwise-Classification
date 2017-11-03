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
}DATA;

void showError(char* str);
double eucledianDistance(double* A,double* B,int dimension);
void chooseRFromN(int r,int n,int* choose);

#endif
