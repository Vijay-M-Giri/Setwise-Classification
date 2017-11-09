#include <unistd.h>
#include "helper.h"
#include "kmeans.h"

int Q; // number of anchor points
int P; // number of class profiles
int MIN_STAT; // minimum number of data points that must be present in entity before including it to the profile
double UPD_FRAC; // the fraction of updates below which the class profiles of the entity is not changed
DATA data; // initial training data
ENTITY* entities; // array of training entities
DATA anchorPoints; // all anchor points
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

int classifyEntity(int entity){
	int i,j;
	int classLabel = -1;
	double mn = DBL_MAX;
	for(i=1;i<=data.noOfClass;i++){
		for(j=1;j<=profileCount[i];j++){
			double dis = cosineDistance(entities[entity].fingerprint,profiles[i][j].fingerprint,Q);
			if(dis < mn){
				mn = dis;
				classLabel = i;
			}
		}
	}
	return classLabel;
}

void processStream(){
	int i,j;
	FILE* fp = fopen("stream.data","r");
	int flag;
	double temp , point[data.dimension];
	int totProcessed = 0;
	while((flag = fscanf(fp,"%lf",&temp)) != EOF){ // stream starts
		// reading data point
		totProcessed++;
		//printf("Processing point : %d\n",totProcessed);
		point[0] = temp;
		for(i=1;i<data.dimension;i++){
			fscanf(fp,"%lf",&point[i]);
		}
		int entity = point[data.dimension-1];
		int label = point[data.dimension-2];
		if(label != -1){ // training point
			entities[entity].cntUpdate++;
			entities[entity].size++;
			int id = closestPoint(point,anchorPoints.data,data.dimension-2,Q);
			entities[entity].fingerprint[id]++;
			if(entities[entity].size == MIN_STAT){ // Remove from wait list
				id = closestProfile(entities[entity],profiles[label],profileCount[label]);
				entities[entity].cntUpdate = 0;
				entities[entity].profile = id;
				profiles[label][id].size++;
				for(i=0;i<Q;i++)
					profiles[label][id].fingerprint[i] += entities[entity].fingerprint[i];
			}
			else if( entities[entity].cntUpdate >= UPD_FRAC * entities[entity].size){
				// check for change in class profiles
				id = closestProfile(entities[entity],profiles[label],profileCount[label]);
				if(id != entities[entity].profile){ // class profile has changed
					int profile = entities[entity].profile;
					profiles[label][profile].size--;
					for(i=0;i<Q;i++)
						profiles[label][profile].fingerprint[i] -= entities[entity].fingerprint[i];
					profiles[label][id].size++;
					for(i=0;i<Q;i++)
						profiles[label][id].fingerprint[i] += entities[entity].fingerprint[i];
					entities[entity].profile = id;
				}
				entities[entity].cntUpdate = 0;
			}
		}
		else{ // test data point
			entities[entity].size++;
			int id = closestPoint(point,anchorPoints.data,data.dimension-2,Q);
			entities[entity].fingerprint[id]++;
			// Finding the class label of the entity
			int classLabel = classifyEntity(entity);
			printf("Entity %d belongs to class %d.\n",entity,classLabel);
		}
	}
	fclose(fp);
}

void printFinalResult(){
	int i,j;
	FILE* fp = fopen("result.data","w");
	for(i=1;i<=data.noOfEntity;i++){
		if(entities[i].label == 0){
			int classLabel = classifyEntity(i);
			fprintf(fp,"%d %d\n",i,classLabel);
		}
	}
	fclose(fp);
}

int main(int argc,char* argv[]){
	
	srand(time(NULL));
	int i,j,o;

	// taking inputs
	while((o = getopt(argc,argv,"q:p:m:f:h")) != -1){
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
			case 'f':
			UPD_FRAC = atof(optarg);
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

	// Free initial data as it is not required further
	freeDATA(data);
	
	// stream processing begins
	processStream();
	printf("Stream processed.\n");
	
	printFinalResult();
	return 0;
}
