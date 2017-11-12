#include <unistd.h>
#include "helper.h"
#include "K-Means/kmeans.h"
#include "R-Tree/index.h"
#include "MaxHeap/maxHeap.h"

int Q; // number of anchor points
int MIN_STAT; // minimum number of data points that must be present in entity before including it to the profile
double UPD_FRAC; // the fraction of updates below which the class profiles of the entity is not changed
DATA data; // initial training data
ENTITY* entities; // array of training entities
DATA anchorPoints; // all anchor points
int* cntEntities; // stores the number of entities in initial training data for each class
struct RNode** roots; // array of roots of the tree of each class
struct Branch** leafPointers; // array of pointers which point to the leaf of the tree. 
							  //leafPointers[i] points to the leaf of the tree which corresponds to ith entity

void loadData(){
	FILE* fp = fopen("Data Generation/initial_training.data","r");
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
		initEntity(&entities[i],Q);
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

void constructRTrees(){
	int i,j;
	
	// initialize
	roots = (struct RNode**) calloc(data.noOfClass+1,sizeof(struct RNode*));
	for(i=1;i<=data.noOfClass;i++)
		roots[i] = RTreeNewIndex();
	leafPointers = (struct Branch**) calloc(data.noOfEntity+1,sizeof(struct Branch*));
	
	// insert entities to its respective class
	for(i=1;i<=data.noOfEntity;i++){
		if(entities[i].size >= MIN_STAT && entities[i].label > 0){ 
			// size should be greater than MIN_STAT to go into tree
			struct Rect rect;
			for(j=0;j<NUMDIMS;j++){
				rect.boundary[j] = rect.boundary[NUMDIMS+j] = entities[i].fingerprint[j];
			}
			// i is rect ID. Note: root can change
			RTreeInsertRect(&rect, i, &roots[entities[i].label], 0); 
		}
	}
}


int isInterrupted(clock_t tstart,double assignedTime){
	clock_t tend = clock();
	double diff = (double) (tend - tstart) / CLOCKS_PER_SEC * 1000.0; 
	if(diff >= assignedTime)
		return 1;
	return 0;
}

int classifyEntity(int entity,clock_t tstart,double assignedTime){
	int i,j;
	int classLabel = -1;
	double mx = -DBL_MAX; // finding max because the keys are stored as negatives
	int maxHeapSize = 10000;
	MaxHeap *frontiers[data.noOfClass+1];
	
	// initialize frontiers and inserting the root of each tree into the heap
	for(i=1;i<=data.noOfClass;i++){
		frontiers[i] = createHeap(maxHeapSize);
		HNode* temp = (HNode*) malloc(sizeof(HNode));
		temp->nodeInHeap = roots[i];
		// multiplied with -1 to make the MaxHeap into a MinHeap
		temp->key = -1.0 * cosineDistance(temp->nodeInHeap->agg.fingerprint,
										entities[entity].fingerprint,Q);
		insertKey(frontiers[i],temp);
	}

	int robin = 1;
	int interrupted = 0;

	while(robin){ // round robin among the classes

		robin = 0;
		for(i=1;i<=data.noOfClass;i++){
			if(frontiers[i]->heap_size <= 0) continue;
			if(isInterrupted(tstart,assignedTime)){
				interrupted = 1;
				break;
			}
			HNode* nextMax = extractMax(frontiers[i]);
			struct RNode* rnode = nextMax->nodeInHeap;
			if(rnode->level == 0){ // reached the leaf
				for(j=0;j<MAXCARD;j++){
					if(rnode->branch[j].child != NULL){
						int id = (int) rnode->branch[j].child;
						double dis = -1.0 * cosineDistance(entities[id].fingerprint,
													entities[entity].fingerprint,Q);
						if(dis >= mx){
							mx = dis;
							classLabel = i;
						}
					}
				}
			}
			else{
				for(j=0;j<MAXCARD;j++){
					if(rnode->branch[j].child != NULL){
						HNode* temp = (HNode*) malloc(sizeof(HNode));
						temp->nodeInHeap = rnode->branch[j].child;
						temp->key = -1.0 * cosineDistance(temp->nodeInHeap->agg.fingerprint,
														entities[entity].fingerprint,Q);
						insertKey(frontiers[i],temp);
					}
				}
			}
			free(nextMax);
			if(frontiers[i]->heap_size > 0) robin = 1;
		}
		if(interrupted)
			break;
	}

	for(i=1;i<=data.noOfClass;i++){
		if(frontiers[i]->heap_size <= 0) continue;
		HNode* nextMax = extractMax(frontiers[i]);
		if(nextMax->key >= mx){
			mx = nextMax->key;
			classLabel = i;
		}
		// free the heap
		free(nextMax);
		freeHeap(frontiers[i]);
	}

	return classLabel;
}


void processStream(){
	int i,j;
	FILE* fp = fopen("Data Generation/stream.data","r");
	FILE* ftime = fopen("Data Generation/inter_arrival_time.data","r");
	int flag;
	double temp , point[data.dimension], assignedTime, mu;
	mu = 0.010; // assign the mean of the exponential distribution
	clock_t tstart;
	int totProcessed = 0;

	while((flag = fscanf(fp,"%lf",&temp)) != EOF){ // stream starts
		// reading data point
		totProcessed++;
		point[0] = temp;
		for(i=1;i<data.dimension;i++){
			fscanf(fp,"%lf",&point[i]);
		}
		fscanf(ftime,"%lf",&assignedTime);
		assignedTime *= mu;
		tstart = clock(); // time starts for processing the current point
		int entity = point[data.dimension-1];
		int label = point[data.dimension-2];

		if(label != -1){ // training point

			entities[entity].cntUpdate++;
			entities[entity].size++;
			int id = closestPoint(point,anchorPoints.data,data.dimension-2,Q);
			entities[entity].fingerprint[id]++;

			if(entities[entity].size == MIN_STAT){ // Remove from wait list
				struct Rect rect;
				for(j=0;j<NUMDIMS;j++){
					rect.boundary[j] = rect.boundary[NUMDIMS+j] = entities[entity].fingerprint[j];
				}
				RTreeInsertRect(&rect, entity, &roots[label], 0); 
				entities[entity].cntUpdate = 0;
			}
			else if( entities[entity].cntUpdate >= UPD_FRAC * entities[entity].size){

				struct Branch *b = leafPointers[entity];
				b->rect.boundary[id]++; b->rect.boundary[NUMDIMS+id]++;
				b->outer->lazy.fingerprint[id]++;
				b->outer->lazy.size++;
				// propagate up
				while(b && !isInterrupted(tstart,assignedTime)){ // b has not reached root
					RTreeUpdateLazy(b->outer);
					b = b->outer->parent;
				}
				if(b) printf("Interrupted\n");
				else printf("Completed\n");
				entities[entity].cntUpdate = 0;
			}
		}
		else{ // test data point
			entities[entity].size++;
			int id = closestPoint(point,anchorPoints.data,data.dimension-2,Q);
			entities[entity].fingerprint[id]++;
			// Finding the class label of the entity
			int classLabel = classifyEntity(entity,tstart,assignedTime);
			printf("Entity %d belongs to class %d.\n",entity,classLabel);
		}
	}
	fclose(fp);
}

void printFinalResult(){
	int i,j;
	FILE* fp = fopen("Data Generation/result.data","w");
	for(i=1;i<=data.noOfEntity;i++){
		if(entities[i].label == 0){
			int classLabel = classifyEntity(i,clock(),0.1);
			fprintf(fp,"%d %d\n",i,classLabel);
		}
	}
	fclose(fp);
}

int main(int argc,char* argv[]){
	
	srand(time(NULL));
	int i,j,o;

	// taking inputs
	while((o = getopt(argc,argv,"q:m:f:h")) != -1){
		switch(o){
			case 'q':
			Q = atoi(optarg);
			if(Q < 1)
				showError("Number of anchor points 'q' should be greater than 0");
			break;
			case 'm':
			MIN_STAT = atoi(optarg);
			break;
			case 'f':
			UPD_FRAC = atof(optarg);
			break;
		}
	}

	if(Q < 1) 
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

	// Construct R-trees for every class
	constructRTrees();
	printf("Trees constructed for each class.\n");

	// Free initial data as it is not required further
	freeDATA(data);
	
	// stream processing begins
	processStream();
	printf("Stream processed.\n");
	
	printFinalResult();
	return 0;
}
