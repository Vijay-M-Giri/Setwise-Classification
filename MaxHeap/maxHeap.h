#ifndef __MAXHEAP_H
#define __MAXHEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

	typedef struct HNode
	{
		struct RNode *nodeInHeap;
		double key;  
	}HNode;
	// A struct for Max Heap

	typedef struct MaxHeap
	{
		HNode **heap; // pointer to array of nodes in heap
		int capacity; // maximum possible size of min heap
		int heap_size; // Current number of elements in min heap
	}MaxHeap;

   // Constructor
    MaxHeap* createHeap(int);

    void swap(HNode* , HNode* );
 
    // to heapify a subtree with root at given index
    void MaxHeapify(MaxHeap *,int );
 
    int parent(int ); //{ return (i-1)/2; }
 
    // to get index of left child of node at index i
    int left(int ); //{ return (2*i + 1); }
 
    // to get index of right child of node at index i
    int right(int );//{ return (2*i + 2); }
 
    // to extract the root which is the minimum element
    HNode* extractMax(MaxHeap *);
 
    // Decreases key value of key at index i to new_val
    void decreaseKey(MaxHeap *,int , HNode* );
 
    // Returns the minimum key (key at root) from min heap
    HNode* getMax(MaxHeap *); 
    // Deletes a key stored at index i
    void deleteKey(MaxHeap *,int );
 
    // Inserts a new key 'k'
    void insertKey(MaxHeap *,HNode*);

	// free
	void freeHeap(MaxHeap* mp);

#endif
