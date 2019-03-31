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
void  treeDestroy(struct tree **root, bool isUserData){
    int i;
    if(*root) {
        for(i = 0; i < TREE_NUM_CHILDREN; i++) {
            treeDestroy(&(*root)->child[i], isUserData);
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
        if(isUserData){
            free((*root)->mediaType);
            free((*root)->dateAcquired);
        }
        free(*root);
        *root = TREE_EMPTY;
    }
}

struct tree *treeUserInsert(struct tree **user, struct tree *node){
    struct tree *e;
    if(*user == 0) {
        e = malloc(sizeof(*e));
        assert(e);
        
        e->tconst = (char*)malloc(sizeof(char)*strlen(node->tconst)+2);
        e->titleType = (char*)malloc(sizeof(char)*strlen(node->titleType)+2);
        e->primaryTitle = (char*)malloc(sizeof(char)*strlen(node->primaryTitle)+2);
        e->originalTitle = (char*)malloc(sizeof(char)*strlen(node->originalTitle)+2);
        e->isAdult = (char*)malloc(sizeof(char)*strlen(node->isAdult)+2);
        e->startYear = (char*)malloc(sizeof(char)*strlen(node->startYear)+2);
        e->endYear = (char*)malloc(sizeof(char)*strlen(node->endYear)+2);
        e->runtimeMinutes = (char*)malloc(sizeof(char)*strlen(node->runtimeMinutes)+2);
        e->genres = (char*)malloc(sizeof(char)*strlen(node->genres)+2);
        e->mediaType = (char*)malloc(sizeof(char)*13);
        e->dateAcquired = malloc(sizeof(*e->dateAcquired));

        strncpy(e->tconst, node->tconst, strlen(node->tconst)+1);
        strncpy(e->titleType, node->titleType, strlen(node->titleType)+1);
        strncpy(e->primaryTitle, node->primaryTitle, strlen(node->primaryTitle)+1);
        strncpy(e->originalTitle, node->originalTitle, strlen(node->originalTitle)+1);
        strncpy(e->isAdult, node->isAdult, strlen(node->isAdult)+1);
        strncpy(e->startYear, node->startYear, strlen(node->startYear)+1);
        strncpy(e->endYear, node->endYear, strlen(node->endYear)+1);
        strncpy(e->runtimeMinutes, node->runtimeMinutes, strlen(node->runtimeMinutes)+1);
        strncpy(e->genres, node->genres, strlen(node->genres)+1);

        e->key = node->tconst;
        e->child[LEFT] = e->child[RIGHT] = 0;

        *user = e;
        return (*user);
    } else if((*user)->key == node->key) {
        /* already there, do nothing */
        return NULL;
    } else {
        /* do this recursively so we can fix data on the way back out */
        if(strncmp(node->key,(*user)->key,strlen(node->key)) < 0){
            treeUserInsert(&(*user)->child[0], node);
        }else if(strncmp(node->key,(*user)->key,strlen(node->key)) > 0){
            treeUserInsert(&(*user)->child[1], node);
        }
    }

    /* fix the aggregate data */
    treeAggregateFix(*user);
    treeRebalance(user);
}

// Specific tree insert for trees with no user data e.i movie_records and 
void treeInitInsert(struct tree **root, char **newElement, int key, bool isUserData, bool keyTitle){
    struct tree *e;
    char *delim = "/";
    char *token;
    int j =0;
    if(*root == 0) {
        //Dynamically allocate memory
        e = malloc(sizeof(*e));
        assert(e);
        e->tconst = malloc(sizeof(e->tconst)*strlen(newElement[0]));
        e->titleType = malloc(sizeof(e->titleType)*strlen(newElement[1]));
        e->primaryTitle = malloc(sizeof(e->primaryTitle)*strlen(newElement[2]));
        e->originalTitle = malloc(sizeof(e->originalTitle)*strlen(newElement[3]));
        e->isAdult = malloc(sizeof(e->isAdult)*strlen(newElement[4]));
        e->startYear = malloc(sizeof(e->startYear)*strlen(newElement[5]));
        e->endYear = malloc(sizeof(e->endYear)*strlen(newElement[6]));
        e->runtimeMinutes = malloc(sizeof(e->runtimeMinutes)*strlen(newElement[7]));
        e->genres = malloc(sizeof(e->genres)*strlen(newElement[8]));

        //Copy data into allocated memory
        strncpy(e->tconst, newElement[0], strlen(newElement[0])+1);
        strncpy(e->titleType,newElement[1], strlen(newElement[1])+1);
        strncpy(e->primaryTitle, newElement[2], strlen(newElement[2])+1);
        strncpy(e->originalTitle, newElement[3], strlen(newElement[3])+1);
        strncpy(e->isAdult, newElement[4], strlen(newElement[4])+1);
        strncpy(e->startYear, newElement[5], strlen(newElement[5])+1);
        strncpy(e->endYear, newElement[6], strlen(newElement[6])+1);
        strncpy(e->runtimeMinutes, newElement[7], strlen(newElement[7])+1);
        strncpy(e->genres, newElement[8], strlen(newElement[8])+1);

        //If this is a user log, allocate memory and copy data into variables
        if(isUserData){
            e->mediaType = malloc(sizeof(char)*strlen(newElement[9])+2);
            e->dateAcquired = malloc(sizeof(*e->dateAcquired));
            strncpy(e->mediaType, newElement[9], strlen(newElement[9])+1);
            token = strtok(newElement[10], delim);
            e->dateAcquired->month = atoi(token);
            token = strtok(NULL, delim);
            e->dateAcquired->day = atoi(token);
            token = strtok(NULL, delim);
            e->dateAcquired->year = atoi(token);
        }

        //If ordered by title, set key to title
        if(keyTitle){
            e->key = e->primaryTitle;
        }
        //else will be sorted by tconst
        else{
            e->key = e->tconst;
        }
        
        e->child[LEFT] = e->child[RIGHT] = 0;

        *root = e;
    } 
    else {
        /* do this recursively so we can fix data on the way back out */
        if(strncmp(newElement[key],(*root)->key,strlen(newElement[key])) < 0){
            treeInitInsert(&(*root)->child[0], newElement, key, isUserData, keyTitle);
        }else if(strncmp(newElement[key],(*root)->key, strlen(newElement[key])) > 0){
            treeInitInsert(&(*root)->child[1], newElement, key, isUserData, keyTitle);
        }
    }

    /* fix the aggregate data */
    treeAggregateFix(*root);
    treeRebalance(root);
}

/* delete minimum element from the tree and return its key */
/* do not call this on an empty tree */
struct tree *treeDeleteMin(struct tree **root){
    struct tree *toFree;
    struct tree *retval;

    assert(*root);  /* can't delete mscanf("%d", &key) == 1in from empty tree */

    if((*root)->child[LEFT]) {
        /* recurse on left subtree */
        retval = treeDeleteMin(&(*root)->child[LEFT]);
    } else {
        /* delete the root */
        toFree = *root;
        //strcpy(retval,toFree->key);
        retval = malloc(sizeof(*retval));
        retval->tconst = malloc(sizeof(char)*strlen(toFree->tconst)+2);
        retval->titleType = malloc(sizeof(char)*strlen(toFree->titleType)+2);
        retval->primaryTitle = malloc(sizeof(char)*strlen(toFree->primaryTitle)+2);
        retval->originalTitle = malloc(sizeof(char)*strlen(toFree->originalTitle)+2);
        retval->isAdult = malloc(sizeof(char)*strlen(toFree->isAdult)+2);
        retval->startYear = malloc(sizeof(char)*strlen(toFree->startYear)+2);
        retval->endYear = malloc(sizeof(char)*strlen(toFree->endYear)+2);
        retval->runtimeMinutes = malloc(sizeof(char)*strlen(toFree->runtimeMinutes)+2);
        retval->genres = malloc(sizeof(char)*strlen(toFree->genres)+2);
        retval->mediaType = malloc(sizeof(char)*strlen(toFree->mediaType)+2);
        retval->dateAcquired = malloc(sizeof(*retval->dateAcquired));

        strncpy(retval->tconst, toFree->tconst, strlen(toFree->tconst)+1);
        strncpy(retval->titleType, toFree->titleType, strlen(toFree->titleType)+1);
        strncpy(retval->primaryTitle, toFree->primaryTitle, strlen(toFree->primaryTitle)+1);
        strncpy(retval->originalTitle, toFree->originalTitle, strlen(toFree->originalTitle)+1);
        strncpy(retval->isAdult, toFree->isAdult, strlen(toFree->isAdult)+1);
        strncpy(retval->startYear, toFree->startYear, strlen(toFree->startYear)+1);
        strncpy(retval->endYear, toFree->endYear, strlen(toFree->endYear)+1);
        strncpy(retval->runtimeMinutes, toFree->runtimeMinutes, strlen(toFree->runtimeMinutes)+1);
        strncpy(retval->genres, toFree->genres, strlen(toFree->genres)+1);
        strncpy(retval->mediaType, toFree->mediaType, strlen(toFree->mediaType)+1);

        retval->dateAcquired->day = toFree->dateAcquired->day;
        retval->dateAcquired->month = toFree->dateAcquired->month;
        retval->dateAcquired->year = toFree->dateAcquired->year;
        *root = toFree->child[RIGHT];

        free(toFree->tconst);
        free(toFree->titleType);
        free(toFree->primaryTitle);
        free(toFree->originalTitle);
        free(toFree->isAdult);
        free(toFree->startYear);
        free(toFree->endYear);
        free(toFree->runtimeMinutes);
        free(toFree->genres);
        free(toFree->dateAcquired);
        free(toFree->mediaType);
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
    struct tree *temp;
    /* do nothing if target not in tree */
    if(*root) {
        if(strncmp((*root)->tconst,target,9) == 0) {
            if((*root)->child[RIGHT]) {
                /* replace root with min value in right subtree */
                temp = treeDeleteMin(&(*root)->child[RIGHT]);
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
                (*root)->tconst = malloc(sizeof((*root)->tconst)*strlen(temp->tconst)+2);
                (*root)->titleType = malloc(sizeof((*root)->titleType)*strlen(temp->titleType)+2);
                (*root)->primaryTitle = malloc(sizeof((*root)->primaryTitle)*strlen(temp->primaryTitle)+2);
                (*root)->originalTitle = malloc(sizeof((*root)->originalTitle)*strlen(temp->originalTitle)+2);
                (*root)->isAdult = malloc(sizeof((*root)->isAdult)*strlen(temp->isAdult)+2);
                (*root)->startYear = malloc(sizeof((*root)->startYear)*strlen(temp->startYear)+2);
                (*root)->endYear = malloc(sizeof((*root)->endYear)*strlen(temp->endYear)+2);
                (*root)->runtimeMinutes = malloc(sizeof((*root)->runtimeMinutes)*strlen(temp->runtimeMinutes)+2);
                (*root)->genres = malloc(sizeof((*root)->genres)*strlen(temp->genres)+2);
                (*root)->mediaType = malloc(sizeof((*root)->mediaType)*strlen(temp->mediaType)+2);
                strncpy((*root)->tconst, temp->tconst, strlen(temp->tconst)+1);
                strncpy((*root)->titleType, temp->titleType, strlen(temp->titleType)+1);
                strncpy((*root)->primaryTitle, temp->primaryTitle, strlen(temp->primaryTitle)+1);
                strncpy((*root)->originalTitle, temp->originalTitle, strlen(temp->originalTitle)+1);
                strncpy((*root)->isAdult, temp->isAdult, strlen(temp->isAdult)+1);
                strncpy((*root)->startYear, temp->startYear, strlen(temp->startYear)+1);
                strncpy((*root)->endYear, temp->endYear, strlen(temp->endYear)+1);
                strncpy((*root)->runtimeMinutes, temp->runtimeMinutes, strlen(temp->runtimeMinutes)+1);
                strncpy((*root)->genres, temp->genres, strlen(temp->genres)+1);
                strncpy((*root)->mediaType, temp->mediaType, strlen(temp->mediaType)+1);
                (*root)->dateAcquired->day = temp->dateAcquired->day;
                (*root)->dateAcquired->month = temp->dateAcquired->month;
                (*root)->dateAcquired->year = temp->dateAcquired->year;
                free(temp->tconst);
                free(temp->titleType);
                free(temp->primaryTitle);
                free(temp->originalTitle);
                free(temp->isAdult);
                free(temp->startYear);
                free(temp->endYear);
                free(temp->runtimeMinutes);
                free(temp->genres);
                free(temp->dateAcquired);
                free(temp->mediaType);
                free(temp);
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
                free((*root)->dateAcquired);
                free((*root)->mediaType);
                toFree = *root;
                *root = toFree->child[LEFT];
                free(toFree);
            }
        } else {
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

void lookForSimilar(const struct tree *root, char *term){
    if(root != 0) {
        lookForSimilar(root->child[LEFT], term);
        if(strstr(lowerCaseString(strlen(root->key),root->key), term) != NULL)
            treeSinglePrint(root, false);
        lookForSimilar(root->child[RIGHT], term);
    }
}

/* a general search the tree to see if the string "term" matches any of the nodes in the tree */
struct tree *searchTree(struct tree *root, char *term){
    int i = 0;
    if(root == 0) {
        printf("Entry could not be found\n\n");
        return NULL;
    } else if(strstr(lowerCaseString(strlen(root->key),root->key), term) != NULL) {
        lookForSimilar(root, term);
    } else if(strcmp(lowerCaseString(strlen(root->key),root->key), term) == 0){
        return root;
    } else{
        if(strcmp(term,lowerCaseString(strlen(root->key),root->key)) < 0){
            searchTree(root->child[LEFT], term);
        }else if(strcmp(term,lowerCaseString(strlen(root->key),root->key)) > 0){
            searchTree(root->child[RIGHT], term);
        }
        return root;
    }
}

// A more specific search only looking for one node with a exact match
struct tree *treeSpecificSearch(struct tree *root, char *term){
    int i =0;
    if(root == 0) {
        printf("Entry could not be found\n\n");
        return NULL;
    } else if(strcmp(root->key, term) == 0){
        return root;
    }
    else{
        if(strcmp(term,root->key) < 0){
            treeSpecificSearch(root->child[LEFT], term);
        }else if(strcmp(term,root->key) > 0){
            treeSpecificSearch(root->child[RIGHT], term);
        }
    }
}

// Print a single entry from the tree in a nice format
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