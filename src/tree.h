/*
 * Basic binary search tree data structure without balancing info.
 *
 * Convention: 
 *
 * Operations that update a tree are passed a struct tree **,
 * so they can replace the argument with the return value.
 *
 * Operations that do not update the tree get a const struct tree *.
 */

#define LEFT (0)
#define RIGHT (1)
#define TREE_NUM_CHILDREN (2)
#define MAX_CHAR 200
#define bool _Bool
static int count = 0;

struct time {
    int day, month, year;
};

struct tree {
    /* we'll make this an array so that we can make some operations symmetric */
    struct tree *child[TREE_NUM_CHILDREN];

    //User Information
    char *mediaType;
    struct time *dateAcquired;

    //IMDB Database Information
    char tconst[10];
    char titleType[30];
    char primaryTitle[MAX_CHAR];
    char originalTitle[MAX_CHAR];
    char isAdult[4];
    char startYear[5];
    char endYear[5];
    char runtimeMinutes[5];
    char genres[MAX_CHAR];

    char *key;
    int height;    /* height of this node */
    size_t size;   /* size of subtree rooted at this node */
};

#define TREE_EMPTY (0)
#define TREE_EMPTY_HEIGHT (-1)

/* free all elements of a tree, replacing it with TREE_EMPTY */
void treeDestroy(struct tree **root);

/* insert an element into a tree pointed to by root */
void treeInitInsert(struct tree **root, char **newElement, int key);

/* return 1 if target is in tree, 0 otherwise */
/* we allow root to be modified to allow for self-balancing trees */
int treeContains(const struct tree *root, int target);

/* delete minimum element from the tree and return its key */
/* do not call this on an empty tree */
char *treeDeleteMin(struct tree **root);

/* delete target from the tree */
/* has no effect if target is not in tree */
void treeDelete(struct tree **root, char *target);

/* return height of tree */
int treeHeight(const struct tree *root);

void treeSinglePrint(const struct tree *root, bool isUserData);

/* return size of tree */
size_t treeSize(const struct tree *root);

/* pretty-print the contents of a tree */
void treePrint(const struct tree *root, bool isUserData);

/* return the number of elements in tree less than target */
size_t treeRank(const struct tree *root, int target);

/* return an element with the given rank */
/* rank must be less than treeSize(root) */
int treeUnrank(const struct tree *root, size_t rank);

/* check that aggregate data is correct throughout the tree */
void treeSanityCheck(const struct tree *root);

struct tree *searchTree(struct tree *root, char *term);

void resetCount();

char *lowerCaseString(char *input);

void treeUserInsert(struct tree **user, struct tree *node);

struct tree *treeSpecificSearch(struct tree *root, char *term);