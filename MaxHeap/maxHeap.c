#include "maxHeap.h"

MaxHeap* createHeap(int cap)
{
    MaxHeap *mp=(MaxHeap *)malloc(sizeof(MaxHeap));
    mp->heap_size = 0;
    mp->capacity = cap;
    mp->heap = (HNode **)calloc(cap,sizeof(HNode *));
    return mp;
}

HNode* getMax(MaxHeap *mp) { return mp->heap[0]; }

void insertKey(MaxHeap *mp,HNode *k)
{
    if (mp->heap_size == mp->capacity)
    {
         printf("\nOverflow: Could not insertKey\n");
    }
 
    // First insert the new key at the end
    mp->heap_size++;
    int i = mp->heap_size - 1;
    //mp->heap[i]=(HNode *)malloc(sizeof(HNode));
    //memcpy(mp->heap[i],k,sizeof(k));
    mp->heap[i] = k;
 
    // Fix the min heap property if it is violated
    while (i != 0 && mp->heap[parent(i)]->key < mp->heap[i]->key)
    {
       swap(mp->heap[i], mp->heap[parent(i)]);
       i = parent(i);
    }
}
 
// Decreases value of key at index 'i' to new_val.  It is assumed that
// new_val is smaller than harr[i].
void decreaseKey(MaxHeap *mp,int i, HNode *new_val)
{
    mp->heap[i] = new_val;
    while (i != 0 && mp->heap[parent(i)]->key < mp->heap[i]->key)
    {
       swap(mp->heap[i], mp->heap[parent(i)]);
       i = parent(i);
    }
}
 
// Method to remove minimum element (or root) from min heap
HNode* extractMax(MaxHeap *mp)
{
    if (mp->heap_size <= 0){
        printf("heap empty while extractMax\n");
        return 0;
    }
    if (mp->heap_size == 1)
    {
        mp->heap_size--;
        return mp->heap[0];
    }
 
    // Store the minimum vakue, and remove it from heap
    HNode* root = mp->heap[0];
   // free(mp->heap[0]);
    mp->heap[0] = mp->heap[mp->heap_size-1];
    mp->heap_size--;
    MaxHeapify(mp,0);
 
    return root;
}
 
 
// This function deletes key at index i. It first reduced value to minus
// infinite, then calls extractMin()
void deleteKey(MaxHeap *mp,int i)
{   
    HNode *temp;
    temp->key=INT_MAX;
    decreaseKey(mp,i,temp);
    extractMax(mp);
}
 
// A recursive method to heapify a subtree with root at given index
// This method assumes that the subtrees are already heapified
void MaxHeapify(MaxHeap *mp,int i)
{
    int l = left(i);
    int r = right(i);
    int smallest = i;
    if (l < mp->heap_size && mp->heap[l]->key > mp->heap[i]->key)
        smallest = l;
    if (r < mp->heap_size && mp->heap[r]->key > mp->heap[smallest]->key)
        smallest = r;
    if (smallest != i)
    {
        swap(mp->heap[i], mp->heap[smallest]);
        MaxHeapify(mp,smallest);
    }
}
 
// A utility function to swap two elements
void swap(HNode *x, HNode *y)
{
    HNode temp = *x;
    *x = *y;
    *y = temp;
}

int parent(int i) { return (i-1)/2; }
 
    // to get index of left child of node at index i
int left(int i) { return (2*i + 1); }
 
    // to get index of right child of node at index i
int right(int i) { return (2*i + 2); }

//HNode* getMax() { return mcInHeap[0]; }

//  free the heap
void freeHeap(MaxHeap* mp){
	while(mp->heap_size){
		HNode* nextMax = extractMax(mp);
		free(nextMax);
	}
	free(mp->heap);
}
