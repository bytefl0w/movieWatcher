#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "tree.h"
#include "io.h"

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
        free((*root)->tconst);
        free((*root)->titleType);
        free((*root)->primaryTitle);
        free((*root)->originalTitle);
        free((*root)->isAdult);
        free((*root)->startYear);
        free((*root)->endYear);
        free((*root)->runtimeMinutes);
        free((*root)->genres);
        free((*root)->mediaType);
        free((*root)->dateAcquired);
        free(*root);
        *root = TREE_EMPTY;
    }
}

struct tree *treeUserInsert(struct tree **user, struct tree *node){
    struct tree *e;
    if(*user == 0) {
        e = malloc(sizeof(*e));
        assert(e);
        
        e->tconst = (char*)malloc(sizeof(char)*strlen(node->tconst));
        e->titleType = (char*)malloc(sizeof(char)*strlen(node->titleType));
        e->primaryTitle = (char*)malloc(sizeof(char)*strlen(node->primaryTitle));
        e->originalTitle = (char*)malloc(sizeof(char)*strlen(node->originalTitle));
        e->isAdult = (char*)malloc(sizeof(char)*strlen(node->isAdult));
        e->startYear = (char*)malloc(sizeof(char)*strlen(node->startYear));
        e->endYear = (char*)malloc(sizeof(char)*strlen(node->endYear));
        e->runtimeMinutes = (char*)malloc(sizeof(char)*strlen(node->runtimeMinutes));
        e->genres = (char*)malloc(sizeof(char)*strlen(node->genres));
        e->mediaType = (char*)malloc(sizeof(char)*11);
        e->dateAcquired = malloc(sizeof(*e->dateAcquired));

        strcpy(e->tconst, node->tconst);
        strcpy(e->titleType, node->titleType);
        strcpy(e->primaryTitle, node->primaryTitle);
        strcpy(e->originalTitle, node->originalTitle);
        strcpy(e->isAdult, node->isAdult);
        strcpy(e->startYear, node->startYear);
        strcpy(e->endYear, node->endYear);
        strcpy(e->runtimeMinutes, node->runtimeMinutes);
        strcpy(e->genres, node->genres);

        e->key = node->tconst;
        e->child[LEFT] = e->child[RIGHT] = 0;

        *user = e;
        return (*user);
    } else if((*user)->key == node->key) {
        /* already there, do nothing */
        return NULL;
    } else {
        /* do this recursively so we can fix data on the way back out */
        //printf("hi\n");
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
void treeInitInsert(struct tree **root, char **newElement, int key, bool isUserData, bool keyTitle){
    struct tree *e;
    char *delim = "/";
    char *token;
    int j =0;
    //puts(newElement[0]);
    if(*root == 0) {
        //puts("Made it\n");
        e = malloc(sizeof(*e));
        assert(e);
        e->tconst = malloc(sizeof(e->tconst)*strlen(newElement[0])+1);
        e->titleType = malloc(sizeof(e->titleType)*strlen(newElement[1])+1);
        e->primaryTitle = malloc(sizeof(e->primaryTitle)*strlen(newElement[2])+1);
        e->originalTitle = malloc(sizeof(e->originalTitle)*strlen(newElement[3])+1);
        e->isAdult = malloc(sizeof(e->isAdult)*strlen(newElement[4])+1);
        e->startYear = malloc(sizeof(e->startYear)*strlen(newElement[5])+1);
        e->endYear = malloc(sizeof(e->endYear)*strlen(newElement[6])+1);
        e->runtimeMinutes = malloc(sizeof(e->runtimeMinutes)*strlen(newElement[7])+1);
        e->genres = malloc(sizeof(e->genres)*strlen(newElement[8])+1);

        //strncpy(e->tconst, newElement[0], strlen(newElement[0]));
        //strncpy(e->titleType,newElement[1], strlen(newElement[1]));
        //strncpy(e->primaryTitle, newElement[2], strlen(newElement[2]));
        //strncpy(e->originalTitle, newElement[3], strlen(newElement[3]));
        //strncpy(e->isAdult, newElement[4], strlen(newElement[4]));
        //strncpy(e->startYear, newElement[5], strlen(newElement[5]));
        //strncpy(e->endYear, newElement[6], strlen(newElement[6]));
        //strncpy(e->runtimeMinutes, newElement[7], strlen(newElement[7]));
        //strncpy(e->genres, newElement[8], strlen(newElement[8]));

        strcpy(e->tconst, newElement[0]);
        strcpy(e->titleType,newElement[1]);
        strcpy(e->primaryTitle, newElement[2]);
        strcpy(e->originalTitle, newElement[3]);
        strcpy(e->isAdult, newElement[4]);
        strcpy(e->startYear, newElement[5]);
        strcpy(e->endYear, newElement[6]);
        strcpy(e->runtimeMinutes, newElement[7]);
        strcpy(e->genres, newElement[8]);

        if(isUserData){
            e->mediaType = malloc(sizeof(char)*11);
            e->dateAcquired = malloc(sizeof(*e->dateAcquired));
            strncpy(e->mediaType, newElement[9], strlen(newElement[9]));
            token = strtok(newElement[10], delim);
            e->dateAcquired->month = atoi(token);
            token = strtok(NULL, delim);
            e->dateAcquired->day = atoi(token);
            token = strtok(NULL, delim);
            e->dateAcquired->year = atoi(token);
        }
        if(keyTitle){
            e->key = e->primaryTitle;
        }
        else{
            e->key = e->tconst;
        }
        
        e->child[LEFT] = e->child[RIGHT] = 0;

        *root = e;
    } 
    //else if((*root)->key == newElement[key]) {
    //    /* already there, do nothing */
    //    puts("Same");
    //    return;
    //} 
    else {
        /* do this recursively so we can fix data on the way back out */
        //treeInsert(&(*root)->child[(*root)->key < newElement], newElement);
        //puts("In here\n");
        if(strncmp(newElement[key],(*root)->key,strlen(newElement[key])) < 0){
            //puts("Left");
            treeInitInsert(&(*root)->child[0], newElement, key, isUserData, keyTitle);
        }else if(strncmp(newElement[key],(*root)->key, strlen(newElement[key])) > 0){
            //puts("right");
            treeInitInsert(&(*root)->child[1], newElement, key, isUserData, keyTitle);
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
        if(strncmp((*root)->tconst,target,9) == 0) {
            if((*root)->child[RIGHT]) {
                /* replace root with min value in right subtree */
                strcpy((*root)->tconst,treeDeleteMin(&(*root)->child[RIGHT]));
            } else {
                /* patch out root */
                free((*root)->tconst);
                free((*root)->titleType);
                free((*root)->primaryTitle);
                free((*root)->originalTitle);
                free((*root)->isAdult);
                free((*root)->startYear);
                free((*root)->endYear);
                free((*root)->runtimeMinutes);
                free((*root)->genres);
                toFree = *root;
                *root = toFree->child[LEFT];
                free(toFree);
            }
        } else {
            //treeDelete(&(*root)->child[(*root)->key < target], target);
            if(strncmp(target,(*root)->tconst,9) < 0){
                treeDelete(&(*root)->child[0], target);
            }else if(strncmp(target,(*root)->tconst,9) > 0){
                treeDelete(&(*root)->child[1], target);
            }
        }

        /* fix the aggregate data */
        treeAggregateFix(*root);
        treeRebalance(root);
    }
}

/* print the contents of a tree */
void treePrint(const struct tree *root, bool isUserData){
    if(root != 0) {
        treePrint(root->child[LEFT], isUserData);
        treeSinglePrint(root, isUserData);
        treePrint(root->child[RIGHT], isUserData);
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
        //if(strstr(root->key, term) != NULL){
            lookForSimilar(root->child[LEFT], term);
            if(strstr(lowerCaseString(strlen(root->key),root->key), term) != NULL)
                treeSinglePrint(root, false);
            lookForSimilar(root->child[RIGHT], term);
        //}
    }
}

/* a general search the tree to see if the string "term" matches any of the nodes in the tree */
struct tree *searchTree(struct tree *root, char *term){
    int i = 0;
    if(root == 0) {
        printf("Entry could not be found\n");
        return NULL;
    } else if(strstr(lowerCaseString(strlen(root->key),root->key), term) != NULL) {
        lookForSimilar(root, term);
    } else if(strcmp(lowerCaseString(strlen(root->key),root->key), term) == 0){
        return root;
    } else{
        //printf("%s\n",root->key);
        if(strcmp(term,lowerCaseString(strlen(root->key),root->key)) < 0){
            searchTree(root->child[LEFT], term);
        }else if(strcmp(term,lowerCaseString(strlen(root->key),root->key)) > 0){
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
        //printf("%s\n", root->key);
        //printf("LEFT: %s RIGHT: %s\n", root->child[LEFT]->key, root->child[RIGHT]->key);
        if(strcmp(term,root->key) < 0){
            treeSpecificSearch(root->child[LEFT], term);
        }else if(strcmp(term,root->key) > 0){
            treeSpecificSearch(root->child[RIGHT], term);
        }
    }
}

void treeSinglePrint(const struct tree *root, bool isUserData){
    printf("\n - Index: %.9s | Title:%s | Adult: ", root->tconst, root->primaryTitle);
    if(strchr(root->isAdult, '0')) // Making the isAdult data user readable
        printf("No | ");
    else
        printf("Yes | ");
    printf("Year: %s | Runtime: %s | Genres: %s\n", root->startYear, root->runtimeMinutes, root->genres);
    if(isUserData){
        char buf[50];
        sprintf(buf, "%d/%d/%d", root->dateAcquired->month, root->dateAcquired->day, root->dateAcquired->year);
        printf("USER DATA - Media Type: %s | Date Acquired: %s\n\n", root->mediaType, buf);
    }
}