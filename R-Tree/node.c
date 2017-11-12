
#include <stdio.h>
#include <malloc.h>
#include "assert.h"
#include "index.h"
#include "card.h"


// Initialize one branch cell in a node.
//
static void RTreeInitBranch(struct Branch *b)
{
	RTreeInitRect(&(b->rect));
	b->child = NULL;
	// custom : 1 lines
	b->outer = NULL;
}



// Initialize a Node structure.
//
void RTreeInitNode(struct RNode *N)
{
	register struct RNode *n = N;
	register int i;
	n->count = 0;
	n->level = -1;
	for (i = 0; i < MAXCARD; i++)
		RTreeInitBranch(&(n->branch[i]));
	// custom : 3 lines
	n->parent = NULL;
	initEntity(&n->agg, NUMDIMS);
	initEntity(&n->lazy, NUMDIMS);
}



// Make a new node and initialize to have all branch cells empty.
//
struct RNode * RTreeNewNode()
{
	register struct RNode *n;

	//n = new Node;
	n = (struct RNode*)malloc(sizeof(struct RNode));
	assert(n);
	RTreeInitNode(n);
	return n;
}


void RTreeFreeNode(struct RNode *p)
{
	assert(p);
	// custom : 2 lines
	freeEntity(p->agg);
	freeEntity(p->lazy);
	//delete p;
	free(p);
}



static void RTreePrintBranch(struct Branch *b, int depth)
{
	RTreePrintRect(&(b->rect), depth);
	RTreePrintNode(b->child, depth);
}


extern void RTreeTabIn(int depth)
{
	int i;
	for(i=0; i<depth; i++)
		putchar('\t');
}


// Print out the data in a node.
//
void RTreePrintNode(struct RNode *n, int depth)
{
	int i;
	assert(n);

	RTreeTabIn(depth);
	printf("node");
	if (n->level == 0)
		printf(" LEAF");
	else if (n->level > 0)
		printf(" NONLEAF");
	else
		printf(" TYPE=?");
	printf("  level=%d  count=%d  address=%p\n", n->level, n->count, n);

	for (i=0; i<n->count; i++)
	{
		if(n->level == 0) {
			RTreeTabIn(depth);
			printf("%d: data = %d\n", i, (int)n->branch[i].child);
		}
		else {
			RTreeTabIn(depth);
			printf("branch %d\n", i);
			RTreePrintBranch(&n->branch[i], depth+1);
		}
	}
}



// Find the smallest rectangle that includes all rectangles in
// branches of a node.
//
struct Rect RTreeNodeCover(struct RNode *N)
{
	register struct RNode *n = N;
	register int i, first_time=1;
	struct Rect r;
	assert(n);

	RTreeInitRect(&r);
	for (i = 0; i < MAXKIDS(n); i++)
		if (n->branch[i].child)
		{
			if (first_time)
			{
				r = n->branch[i].rect;
				first_time = 0;
			}
			else
				r = RTreeCombineRect(&r, &(n->branch[i].rect));
		}
	return r;
}



// Pick a branch.  Pick the one that will need the smallest increase
// in area to accomodate the new rectangle.  This will result in the
// least total area for the covering rectangles in the current node.
// In case of a tie, pick the one which was smaller before, to get
// the best resolution when searching.
//
int RTreePickBranch(struct Rect *R, struct RNode *N)
{
	register struct Rect *r = R;
	register struct RNode *n = N;
	register struct Rect *rr;
	register int i, first_time=1;
	RectReal increase, bestIncr=(RectReal)-1, area, bestArea;
	int best;
	struct Rect tmp_rect;
	assert(r && n);

	for (i=0; i<MAXKIDS(n); i++)
	{
		if (n->branch[i].child)
		{
			rr = &n->branch[i].rect;
			area = RTreeRectVolume(rr);
			tmp_rect = RTreeCombineRect(r, rr);
			increase = RTreeRectVolume(&tmp_rect) - area;
			if (increase < bestIncr || first_time)
			{
				best = i;
				bestArea = area;
				bestIncr = increase;
				first_time = 0;
			}
			else if (increase == bestIncr && area < bestArea)
			{
				best = i;
				bestArea = area;
				bestIncr = increase;
			}
		}
	}
	return best;
}



// Add a branch to a node.  Split the node if necessary.
// Returns 0 if node not split.  Old node updated.
// Returns 1 if node split, sets *new_node to address of new node.
// Old node updated, becomes one of two.
//
int RTreeAddBranch(struct Branch *B, struct RNode *N, struct RNode **New_node)
{
	register struct Branch *b = B;
	register struct RNode *n = N;
	register struct RNode **new_node = New_node;
	register int i;

	assert(b);
	assert(n);

	if (n->count < MAXKIDS(n))  /* split won't be necessary */
	{
		// custom : 11 lines
		b->outer = n; 
		if(n->level > 0 && b->child != NULL){ // branch has a child
			for(i=0;i<NUMDIMS;i++)
				n->agg.fingerprint[i] += b->child->agg.fingerprint[i];
			n->agg.size += b->child->agg.size;
		}
		else{ // if leaf
			for(i=0;i<NUMDIMS;i++)
				n->agg.fingerprint[i] += b->rect.boundary[i],
				n->agg.size += b->rect.boundary[i];
		}
		for (i = 0; i < MAXKIDS(n); i++)  /* find empty branch */
		{
			if (n->branch[i].child == NULL)
			{
				n->branch[i] = *b;
				n->count++;
				// custom : 7 lines
				if(n->level == 0){ // if n is a leaf
					int id = (int) b->child;
					leafPointers[id] = &n->branch[i];
				}
				else if(b->child){
					n->branch[i].child->parent = &n->branch[i];
				}
				break;
			}
		}
		return 0;
	}
	else
	{
		assert(new_node);
		RTreeUpdateLazy(n); // custom
		RTreeSplitNode(n, b, new_node);
		return 1;
	}
}



// Disconnect a dependent node.
//
void RTreeDisconnectBranch(struct RNode *n, int i)
{
	assert(n && i>=0 && i<MAXKIDS(n));
	assert(n->branch[i].child);

	RTreeInitBranch(&(n->branch[i]));
	n->count--;
}

// Custom function for lazy update. Here the progation is from bottom-up.
//
void RTreeUpdateLazy(struct RNode* n){
	if(n == NULL || n->lazy.size == 0) // nothing to update
		return ;
	int i;
	for(i=0;i<NUMDIMS;i++){
		if(n->parent)
			n->parent->outer->lazy.fingerprint[i] += n->lazy.fingerprint[i];
		n->agg.fingerprint[i] += n->lazy.fingerprint[i];
		n->lazy.fingerprint[i] = 0;
	}
	if(n->parent)
		n->parent->outer->lazy.size += n->lazy.size;
	n->agg.size += n->lazy.size;
	n->lazy.size = 0;
}
