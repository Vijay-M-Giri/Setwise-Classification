#include "kmeans.h"

int closestCentroid(double* point, double** centroids, int dimension, int K){
	int id = -1;
	double mn = DBL_MAX;
	int i,j;
	for(i=0;i<K;i++){
		double dis = eucledianDistance(point,centroids[i],dimension);
		if(dis < mn){
			mn = dis; 
			id = i;
		}
	}
	return id;
}

void kmeans(int K,DATA data,double** centroids,int* cluster){
	if(K > data.size){
		showError("K is greater than total points");
	}
	data.dimension -= 2; // The class label column and entity column is not required here
	int choose[K+1];
	chooseRFromN(data.size,K,choose);
	int i,j;
	for(i=0;i<K;i++){
		for(j=0;j<data.dimension;j++){
			centroids[i][j] = data.data[choose[i]][j];
		}
	}
	int flag = 0;
	if(cluster == NULL){
		cluster = (int*) calloc(data.size,sizeof(int));
		flag = 1;
	}
	else{
		for(i=0;i<data.size;i++) cluster[i] = 0;
	}
	int maxIter = 100000000.0 / data.size / data.dimension;
	int done = 0 , iter = 0;
	int* clusterSize = (int*) calloc(K,sizeof(int));
	clusterSize[0] = data.size;

	// k-means loop
	while(!done && iter < maxIter){
		done = 1;
		// re-assigning the nearest centroids of all the data points
		for(i=0;i<data.size;i++){
			int new = closestCentroid(data.data[i],centroids,data.dimension,K);
			if(new != cluster[i]){
				done = 0;
				clusterSize[cluster[i]]--;
				clusterSize[new]++;
				cluster[i] = new;
			}
		}
		// re-calculating the centroids
		for(i=0;i<K;i++)
			for(j=0;j<data.dimension;j++)
				centroids[i][j] = 0.0;
		for(i=0;i<data.size;i++){
			for(j=0;j<data.dimension;j++){
				centroids[cluster[i]][j] += data.data[i][j];
			}
		}
		for(i=0;i<K;i++){
			for(j=0;j<data.dimension;j++)
				centroids[i][j] /= clusterSize[i];
		}
		iter++;
	}
	// freeing the pointers
	free(clusterSize);
	if(flag)
		free(cluster);
}
