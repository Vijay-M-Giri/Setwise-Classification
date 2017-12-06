#include "kmeans.h"

void kmeans(int K,DATA data,int dimension,double** centroids,int* cluster){
	if(K > data.size){
		printf("K is %d and data size is %d\n",K,data.size);
		showError("K is greater than total points");
	}
	if(K <= 0){
		showError("K should be atleast 1");
	}
	int choose[K+1];
	chooseRFromN(data.size,K,choose);
	int i,j;
	for(i=0;i<K;i++){
		for(j=0;j<dimension;j++){
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
	int maxIter = 100000000.0 / data.size / dimension;
	int done = 0 , iter = 0;
	int* clusterSize = (int*) calloc(K,sizeof(int));
	clusterSize[0] = data.size;

	// k-means loop
	while(!done && iter < maxIter){
		done = 1;
		// re-assigning the nearest centroids of all the data points
		for(i=0;i<data.size;i++){
			int new = closestPoint(data.data[i],centroids,dimension,K);
			if(new != cluster[i]){
				done = 0;
				clusterSize[cluster[i]]--;
				clusterSize[new]++;
				cluster[i] = new;
			}
		}
		// re-calculating the centroids
		for(i=0;i<K;i++)
			for(j=0;j<dimension;j++)
				centroids[i][j] = 0.0;
		for(i=0;i<data.size;i++){
			for(j=0;j<dimension;j++){
				centroids[cluster[i]][j] += data.data[i][j];
			}
		}
		for(i=0;i<K;i++){
			for(j=0;j<dimension;j++)
				centroids[i][j] /= clusterSize[i];
		}
		iter++;
	}
	// freeing the pointers
	free(clusterSize);
	if(flag)
		free(cluster);
}
