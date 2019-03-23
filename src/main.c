/* 
 * main.c file
 *  @author Erik Mortimer
 */

// Standard Library Includes
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Written Includes
#include "tree.h"
#include "io.h"


int main(int argc, char* argv[]){
    printf("***movieWatcher Program***\n****Welcome****\n");

    FILE *fp = fopen(argv[1],"r+");
    struct tree *root = TREE_EMPTY;
    parseFile(fp, &root, 2);
    treePrint(root);
    //printf("Print\n");
    //searchTree(&root, "tt6172666");
    //searchTree(&root, "tt0000000");
    //searchTree(&root, "tt2395427");
    treeDestroy(&root);
    fclose(fp);
    return 0;
}