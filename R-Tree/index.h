#ifndef _INDEX_
#define _INDEX_

/* PGSIZE is normally the natural page size of the machine */
#define PGSIZE	512
#define NUMDIMS	20	/* number of dimensions */
#define NDEBUG

#include "../helper.h"

typedef double RectReal;


/*-----------------------------------------------------------------------------
| Global definitions.
-----------------------------------------------------------------------------*/

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define NUMSIDES 2*NUMDIMS

struct Rect
{
	RectReal boundary[NUMSIDES]; /* xmin,ymin,...,xmax,ymax,... */
};

struct RNode;

struct Branch
{
	struct Rect rect;
	struct RNode *child;
	// custom : 1 lines
	struct RNode *outer;
};

/* max branching factor of a node */
//#define MAXCARD (int)((PGSIZE-(2*sizeof(int))) / sizeof(struct Branch))
#define MAXCARD 8

struct RNode
{
	int count;
	int level; /* 0 is leaf, others positive */
	struct Branch branch[MAXCARD];
	// custom : 3 lines
	struct Branch *parent;
	ENTITY agg;
	ENTITY lazy;
};

struct ListNode
{
	struct ListNode *next;
	struct RNode *node;
};

/*
 * If passed to a tree search, this callback function will be called
 * with the ID of each data rect that overlaps the search rect
 * plus whatever user specific pointer was passed to the search.
 * It can terminate the search early by returning 0 in which case
 * the search will return the number of hits found up to that point.
 */
typedef int (*SearchHitCallback)(int id, void* arg);


extern int RTreeSearch(struct RNode*, struct Rect*, SearchHitCallback, void*);
extern int RTreeInsertRect(struct Rect*, int, struct RNode**, int depth);
extern int RTreeDeleteRect(struct Rect*, int, struct RNode**);
extern struct RNode * RTreeNewIndex();
extern struct RNode * RTreeNewNode();
extern void RTreeInitNode(struct RNode*);
extern void RTreeFreeNode(struct RNode *);
extern void RTreePrintNode(struct RNode *, int);
extern void RTreeTabIn(int);
extern struct Rect RTreeNodeCover(struct RNode *);
extern void RTreeInitRect(struct Rect*);
extern struct Rect RTreeNullRect();
extern RectReal RTreeRectArea(struct Rect*);
extern RectReal RTreeRectSphericalVolume(struct Rect *R);
extern RectReal RTreeRectVolume(struct Rect *R);
extern struct Rect RTreeCombineRect(struct Rect*, struct Rect*);
extern int RTreeOverlap(struct Rect*, struct Rect*);
extern void RTreePrintRect(struct Rect*, int);
extern int RTreeAddBranch(struct Branch *, struct RNode *, struct RNode **);
extern int RTreePickBranch(struct Rect *, struct RNode *);
extern void RTreeDisconnectBranch(struct RNode *, int);
extern void RTreeSplitNode(struct RNode*, struct Branch*, struct RNode**);

extern int RTreeSetNodeMax(int);
extern int RTreeSetLeafMax(int);
extern int RTreeGetNodeMax();
extern int RTreeGetLeafMax();

// custom : 1 lines
extern void RTreeUpdateLazy(struct RNode* n);

#endif /* _INDEX_ */
