#include <unistd.h>
#include "helper.h"
#include "kmeans.h"

int Q; // number of anchor points
int P; // number of class profiles
DATA data; // initial training data

void loadData(){
	FILE* fp = fopen("initial_training.data","r");
	fscanf(fp,"%d %d %d",&data.size,&data.dimension,&data.noOfClass);
	data.data = (double**) malloc(sizeof(double*) * data.size); 
	int i,j;
	for(i=0;i<data.size;i++){
		data.data[i] = (double*) malloc(sizeof(double) * data.dimension);
	}
	for(i=0;i<data.size;i++){
		for(j=0;j<data.dimension;j++){
			double a; 
			fscanf(fp,"%lf",&a);
			data.data[i][j] = a;
		}
	}
	fclose(fp);
}

int main(int argc,char* argv[]){
	
	srand(time(NULL));
	int i,j,o;

	// taking inputs
	while((o = getopt(argc,argv,"q:p:h")) != -1){
		switch(o){
			case 'q':
			Q = atoi(optarg);
			if(Q < 1)
				showError("Number of anchor points 'q' should be greater than 0");
			break;
			case 'p':
			P = atoi(optarg);
			if(P < 1)
				showError("Number of class profiles should be greater than 0");
			break;
		}
	}

	if(Q < 1 || P < 1) 
		showError("Invalid input parameters.");
	
	// load initial training data
	loadData();
	printf("Data loaded\n");

	// Get the anchor points from k-means
	DATA anchorPoints = { NULL, Q, data.dimension, -1};
	anchorPoints.data = (double**) malloc(sizeof(double*) * Q);
	for(i=0;i<Q;i++)
		anchorPoints.data[i] = (double*) malloc(sizeof(double) * data.dimension); 
	kmeans(Q,data,anchorPoints.data,NULL);

	// Printing anchor points
	for(i=0;i<Q;i++){
		for(j=0;j<data.dimension;j++)
			printf("%lf ",anchorPoints.data[i][j]);
		printf("\n");
	}

	// distribute class profiles among classes

	return 0;
}
