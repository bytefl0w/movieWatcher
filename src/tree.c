#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tree.h"
#include "io.h"

char *lowerCaseString(char *input){
    char *output = copyString(input);
    int i;
    for(i=0; input[i]; i++) 
        output[i] = tolower(input[i]);
    return output;
}

int treeHeight(const struct tree *root){
    if(root == 0) {
        return TREE_EMPTY_HEIGHT;
    } else {
        return root->height;
    }
}

/* recompute height from height of kids */
static int treeComputeHeight(const struct tree *root){
    int childHeight;
    int maxChildHeight;
    int i;

    if(root == 0) {
        return TREE_EMPTY_HEIGHT;
    } else {
        maxChildHeight = TREE_EMPTY_HEIGHT;

        for(i = 0; i < TREE_NUM_CHILDREN; i++) {
            childHeight = treeHeight(root->child[i]);
            if(childHeight > maxChildHeight) {
                maxChildHeight = childHeight;
            }
        }

        return maxChildHeight + 1;
    }
}

size_t treeSize(const struct tree *root){
    if(root == 0) {
        return 0;
    } else {
        return root->size;
    }
} 

/* recompute size from size of kids */
static int treeComputeSize(const struct tree *root){
    int size;
    int i;

    if(root == 0) {
        return 0;
    } else {
        size = 1;

        for(i = 0; i < TREE_NUM_CHILDREN; i++) {
            size += treeSize(root->child[i]);
        }

        return size;
    }
}

/* fix aggregate data in root */
/* assumes children are correct */
static void treeAggregateFix(struct tree *root){
    if(root) {
        root->height = treeComputeHeight(root);
        root->size = treeComputeSize(root);
    }
}

/* rotate child in given direction to root */
static void treeRotate(struct tree **root, int direction){
    struct tree *x;
    struct tree *y;
    struct tree *b;

    /*scanf("%d", &key) == 1
     *      y           x 
     *     / \         / \
     *    x   C  <=>  A   y
     *   / \             / \
     *  A   B           B   C
     */

    y = *root;                  assert(y);
    x = y->child[direction];    assert(x);
    b = x->child[!direction];

    /* do the rotation */
    *root = x;
    x->child[!direction] = y;
    y->child[direction] = b;

    /* fix aggregate data for y then x */
    treeAggregateFix(y);
    treeAggregateFix(x);
}

/* restore AVL property at *root after an insertion or deletion */
/* assumes subtrees already have AVL property */
static void treeRebalance(struct tree **root){
    int tallerChild;

    if(*root) {
        for(tallerChild = 0; tallerChild < TREE_NUM_CHILDREN; tallerChild++) {
            if(treeHeight((*root)->child[tallerChild]) >= treeHeight((*root)->child[!tallerChild]) + 2) {

                /* which grandchild is the problem? */
                if(treeHeight((*root)->child[tallerChild]->child[!tallerChild]) 
                            > treeHeight((*root)->child[tallerChild]->child[tallerChild])) {
                    /* opposite-direction grandchild is too tall */
                    /* rotation at root will just change its parent but not change height */
                    /* so we rotate it up first */
                    treeRotate(&(*root)->child[tallerChild], !tallerChild);
                }

                /* now rotate up the taller child */
                treeRotate(root, tallerChild);

                /* don't bother with other child */
                break;
            }
        }

        /* test that we actually fixed it */
        assert(abs(treeHeight((*root)->child[LEFT]) - treeHeight((*root)->child[RIGHT])) <= 1);

#ifdef PARANOID_REBALANCE
        treeSanityCheck(*root);
#endif
    }
}


/* free all elements of a tree, replacing it with TREE_EMPTY */
void  treeDestroy(struct tree **root){
    int i;
    if(*root) {
        for(i = 0; i < TREE_NUM_CHILDREN; i++) {
            treeDestroy(&(*root)->child[i]);
        }
        free(*root);
        *root = TREE_EMPTY;
    }
}

void treeUserInsert(struct tree **user, struct tree *node){
    struct tree *e;
    if(*user == 0) {
        e = malloc(sizeof(*e));
        assert(e);
        //*e->tconst = malloc();
        strcpy(e->tconst,node->tconst);
        strcpy(e->titleType,node->titleType);
        strcpy(e->primaryTitle, node->primaryTitle);
        strcpy(e->originalTitle, node->originalTitle);
        strcpy(e->genres, node->genres);
        strcpy(e->isAdult, node->isAdult);
        strcpy(e->startYear, node->startYear);
        strcpy(e->endYear, node->endYear);
        strcpy(e->runtimeMinutes, node->runtimeMinutes);
        e->key = node->primaryTitle;
        e->child[LEFT] = e->child[RIGHT] = 0;

        *user = e;
    } else if((*user)->key == node->key) {
        /* already there, do nothing */
        return;
    } else {
        /* do this recursively so we can fix data on the way back out */
        if(strcmp(node->key,(*user)->key) < 0){
            treeUserInsert(&(*user)->child[0], node);
        }else if(strcmp(node->key,(*user)->key) > 0){
            treeUserInsert(&(*user)->child[1], node);
        }
    }

    /* fix the aggregate data */
    treeAggregateFix(*user);
    treeRebalance(user);
}

/* insert an element into a tree pointed to by root */
void treeInitInsert(struct tree **root, char **newElement, int key){
    struct tree *e;

    if(*root == 0) {
        e = malloc(sizeof(*e));
        assert(e);
        //*e->tconst = malloc();
        strcpy(e->tconst,newElement[0]);
        strcpy(e->titleType,newElement[1]);
        strcpy(e->primaryTitle, newElement[2]);
        strcpy(e->originalTitle, newElement[3]);
        strcpy(e->genres, newElement[4]);
        strcpy(e->isAdult, newElement[5]);
        strcpy(e->startYear, newElement[6]);
        strcpy(e->endYear, newElement[7]);
        strcpy(e->runtimeMinutes, newElement[8]);
        e->key = newElement[key];
        e->child[LEFT] = e->child[RIGHT] = 0;

        *root = e;
    } else if((*root)->key == newElement[key]) {
        /* already there, do nothing */
        return;
    } else {
        /* do this recursively so we can fix data on the way back out */
        //treeInsert(&(*root)->child[(*root)->key < newElement], newElement);
        if(strcmp(newElement[key],(*root)->key) < 0){
            treeInitInsert(&(*root)->child[0], newElement, key);
        }else if(strcmp(newElement[key],(*root)->key) > 0){
            treeInitInsert(&(*root)->child[1], newElement, key);
        }
    }

    /* fix the aggregate data */
    treeAggregateFix(*root);
    treeRebalance(root);
}

/* delete minimum element from the tree and return its key */
/* do not call this on an empty tree */
char *treeDeleteMin(struct tree **root){
    struct tree *toFree;
    char *retval;

    assert(*root);  /* can't delete mscanf("%d", &key) == 1in from empty tree */

    if((*root)->child[LEFT]) {
        /* recurse on left subtree */
        retval = treeDeleteMin(&(*root)->child[LEFT]);
    } else {
        /* delete the root */
        toFree = *root;
        strcpy(retval,toFree->key);
        *root = toFree->child[RIGHT];
        free(toFree);
    }

    /* fix the aggregate data */
    treeAggregateFix(*root);
    treeRebalance(root);

    return retval;
}

/* delete target from the tree */
/* has no effect if target is not in tree */
void treeDelete(struct tree **root, char *target){
    struct tree *toFree;

    /* do nothing if target not in tree */
    if(*root) {
        //(*root)->key == target
        if(strcmp((*root)->key,target) == 0) {
            if((*root)->child[RIGHT]) {
                /* replace root with min value in right subtree */
                strcpy((*root)->key,treeDeleteMin(&(*root)->child[RIGHT]));
            } else {
                /* patch out root */
                toFree = *root;
                *root = toFree->child[LEFT];
                free(toFree);
            }
        } else {
            //treeDelete(&(*root)->child[(*root)->key < target], target);
            if(strcmp(target,(*root)->key) < 0){
                treeDelete(&(*root)->child[0], target);
            }else if(strcmp(target,(*root)->key) > 0){
                treeDelete(&(*root)->child[1], target);
            }
        }

        /* fix the aggregate data */
        treeAggregateFix(*root);
        treeRebalance(root);
    }
}

/* print the contents of a tree */
void treePrint(const struct tree *root){
    if(root != 0) {
        treePrint(root->child[LEFT]);
        
        treePrint(root->child[RIGHT]);
    }
}

/* check that aggregate data is correct throughout the tree */
void treeSanityCheck(const struct tree *root){
    int i;

    if(root) {
        assert(root->height == treeComputeHeight(root));
        assert(root->size == treeComputeSize(root));

        assert(abs(treeHeight(root->child[LEFT]) - treeHeight(root->child[RIGHT])) < 2);

        for(i = 0; i < TREE_NUM_CHILDREN; i++) {
            treeSanityCheck(root->child[i]);
        }
    }
}

void lookForSimilar(const struct tree *root, char *term){
    if(root != 0) {
        //if(strstr(root->key, term)){
            lookForSimilar(root->child[LEFT], term);
            treeSinglePrint(root);
            lookForSimilar(root->child[RIGHT], term);
        //}
    }
}

/* a general search the tree to see if the string "term" matches any of the nodes in the tree */
struct tree *searchTree(struct tree *root, char *term){
    int i =0;
    if(root == 0) {
        printf("Entry could not be found\n");
        return NULL;
    } else if(strstr(root->key, term) != NULL) {
        lookForSimilar(root, term);
    } else if(strcmp(root->key, term) == 0){
        return root;
    }
    else{
        //treeInsert(&(*root)->child[(*root)->key < newElement], newElement);
        printf("%s\n", root->key);
        if(strcmp(term,root->key) < 0){
            searchTree(root->child[LEFT], term);
        }else if(strcmp(term,root->key) > 0){
            searchTree(root->child[RIGHT], term);
        }
    }
    return NULL;
}

// A more specific search only looking for one node with a exact match
struct tree *treeSpecificSearch(struct tree *root, char *term){
    int i =0;
    if(root == 0) {
        printf("Entry could not be found\n");
        return NULL;
    } else if(strcmp(root->key, term) == 0){
        return root;
    }
    else{
        printf("%s\n", root->key);
        printf("LEFT: %s RIGHT: %s\n", root->child[LEFT]->key, root->child[RIGHT]->key);
        if(strcmp(term,root->key) < 0){
            treeSpecificSearch(root->child[LEFT], term);
        }else if(strcmp(term,root->key) > 0){
            treeSpecificSearch(root->child[RIGHT], term);
        }
    }
    return NULL;
}

void treeSinglePrint(const struct tree *root){
    printf(" - Index: %.9s Title:%s %s %s %s %s %s %s\n", root->tconst, root->primaryTitle, root->originalTitle, root->genres, root->isAdult, root->startYear, root->endYear, root->runtimeMinutes);
}