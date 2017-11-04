#include <unistd.h>
#include "helper.h"
#include "kmeans.h"

int Q; // number of anchor points
int P; // number of class profiles
int MIN_STAT; // minimum number of data points that must be present in entity before including it to the profile
DATA data; // initial training data
ENTITY* entities; // array of training entities
DATA anchorPoints; // all anchor points
int* profileCount; // profile count of each class
ENTITY** profiles; // stores profiles of each class
int* cntEntities; // stores the number of entities in initial training data for each class

void loadData(){
	FILE* fp = fopen("initial_training.data","r");
	fscanf(fp,"%d %d %d %d",&data.size,&data.dimension,&data.noOfClass,&data.noOfEntity);
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

void getAnchorPoints(){
	int i,j;
	anchorPoints = (DATA){NULL, Q, data.dimension, -1, -1};
	anchorPoints.data = (double**) malloc(sizeof(double*) * Q);
	for(i=0;i<Q;i++)
		anchorPoints.data[i] = (double*) malloc(sizeof(double) * data.dimension); 
	kmeans(Q,data,data.dimension-2,anchorPoints.data,NULL);

	// Printing anchor points
	/*for(i=0;i<Q;i++){
		for(j=0;j<data.dimension;j++)
			printf("%lf ",anchorPoints.data[i][j]);
		printf("\n");
	}*/
}

void entitiesToFingerprints(){
	int i,j;

	// Initializing the entity array
	entities = (ENTITY*) malloc(sizeof(ENTITY) * (data.noOfEntity+1));
	for(i=1;i<=data.noOfEntity;i++){
		entities[i].dimension = Q;
		entities[i].fingerprint = (double*) calloc(entities[i].dimension,sizeof(double));
		entities[i].size = 0;
		entities[i].label = entities[i].profile = 0;
	}

	// Iterate through all the initial data points
	for(i=0;i<data.size;i++){
		int entity = data.data[i][data.dimension-1];
		entities[entity].size++;
		entities[entity].label = data.data[i][data.dimension-2];
		int id = closestPoint(data.data[i],anchorPoints.data,data.dimension-2,Q);
		entities[entity].fingerprint[id]++; 
	}
}

void distributeClassProfiles(){
	int i,j;

	// Initializing
	profileCount = (int*) calloc(data.noOfClass+1,sizeof(int));
	cntEntities = (int*) calloc(data.noOfClass+1,sizeof(int));
	for(i=1;i<=data.noOfEntity;i++)
		cntEntities[entities[i].label]++;
	int totAssigned = 0;
	for(i=1;i<=data.noOfClass;i++){
		profileCount[i] = (double) P * cntEntities[i] / data.noOfEntity + 0.5;
		totAssigned += profileCount[i];
	}

	// each class is assigned at least one profile
	for(i=1;i<=data.noOfClass;i++) 
		if(profileCount[i] == 0) 
			profileCount[i] = 1 , totAssigned++;

	/* In the event that the number of profiles (assigned to the
	different classes) is more than P, we remove one profile allocation 
	from each of the classes with the largest number of
	assigned profiles in decreasing order, until the total number is P */
	while(totAssigned > P){
		int id = -1 , mx = -1;
		for(i=1;i<=data.noOfClass;i++) 
			if(profileCount[i] > mx)
				mx = profileCount[i] , id = i;
		profileCount[id]--;
		totAssigned--;
	}

	/* In the event that the number of profiles is less than p, we add one 
	profile allocation to each of the classes with the least number of 
	assigned profiles in increasing order, until the total number is p */
	while(totAssigned < P){
		int id = -1, mn = P+1;
		for(i=1;i<=data.noOfClass;i++)
			if(profileCount[i] < mn) 
				mn = profileCount[i] , id = i;
		profileCount[i]++;
		totAssigned++;
	}

}

void generateProfiles(){
	int i,j;
	// prepare data for k-means of profiles
	DATA* profileData = (DATA*) calloc(data.noOfClass+1,sizeof(DATA));
	int** entityNo = (int**) calloc(data.noOfClass+1,sizeof(int*));
	for(i=1;i<=data.noOfClass;i++){
		profileData[i].data = (double**) calloc(cntEntities[i],sizeof(double*));
		profileData[i].dimension = Q;
		entityNo[i] = (int*) malloc(cntEntities[i]*sizeof(int));
	}
	for(i=1;i<=data.noOfEntity;i++){
		if(entities[i].size >= MIN_STAT){
			int label = entities[i].label;
			int size = profileData[label].size;
			profileData[label].data[size] = entities[i].fingerprint;
			profileData[label].size++;
			entityNo[label][size] = i;
		}
	}
	printf("Data prepared.\n");

	// generate profiles from k-means
	for(i=1;i<=data.noOfClass;i++){
		double** tempCentroids = (double**) malloc(sizeof(double*)*profileCount[i]);
		for(j=0;j<profileCount[i];j++)
			tempCentroids[j] = (double*) malloc(sizeof(double)*Q);
		int* cluster = (int*) malloc(sizeof(int)*profileData[i].size);
		kmeans(profileCount[i],profileData[i],Q,tempCentroids,cluster);
		for(j=0;j<profileData[i].size;j++){
			entities[entityNo[i][j]].profile = cluster[j] + 1;
		}
		// free
		free(cluster);
		for(j=0;j<profileCount[i];j++)
			free(tempCentroids[j]);
		free(tempCentroids);
	}
	printf("Profiles generated from k-means.\n");

	// populate profiles of each class
	profiles = (ENTITY**) malloc(sizeof(ENTITY*) * (data.noOfClass+1));
	for(i=1;i<=data.noOfClass;i++){
		profiles[i] = (ENTITY*) malloc(sizeof(ENTITY)*(profileCount[i]+1));
		for(j=1;j<=profileCount[i];j++){
			profiles[i][j] = (ENTITY){NULL, 0, Q, i+1, -1};
			profiles[i][j].fingerprint = (double*) calloc(Q,sizeof(double));
		}
	}
	for(i=1;i<=data.noOfEntity;i++){
		int label = entities[i].label;
		int profile = entities[i].profile;
		if(profile == 0) continue;
		profiles[label][profile].size++;
		for(j=0;j<entities[i].dimension;j++){
			profiles[label][profile].fingerprint[j] += entities[i].fingerprint[j];
		}
	}
	printf("Profiles populated.\n");

	// free
	for(i=1;i<=data.noOfClass;i++){
		free(entityNo[i]);
		free(profileData[i].data);
	}
	free(entityNo);
	free(profileData);
}

int main(int argc,char* argv[]){
	
	srand(time(NULL));
	int i,j,o;

	// taking inputs
	while((o = getopt(argc,argv,"q:p:m:h")) != -1){
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
			case 'm':
			MIN_STAT = atoi(optarg);
			break;
		}
	}

	if(Q < 1 || P < 1) 
		showError("Invalid input parameters.");
	
	// load initial training data
	loadData();
	printf("Data loaded.\n");

	// Get the anchor points from k-means
	getAnchorPoints();
	printf("Anchor points obtained.\n");
	
	// Represent entities into fingerprints
	entitiesToFingerprints();
	printf("Entities are reperesented as fingerprints.\n");

	// distribute class profiles among classes
	distributeClassProfiles();
	printf("Class Profiles Distributed.\n");

	// Free initial data as it is not required further
	freeDATA(data);
	
	// Generate class profiles for each class
	generateProfiles();
	printf("Class profiles generated.\n");

	return 0;
}
