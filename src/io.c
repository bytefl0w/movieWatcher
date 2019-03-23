/* 
 * io.c file
 *  @author Erik Mortimer
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tree.h"
#include "io.h"


/* utility function to parse the "movie_records" file */
struct tree;

char *copyString(char str[]){
    size_t len = strlen(str), i;
    char *copy=(char*)malloc(len+1);
    for(i=0;i<len;i++)
        copy[i] = str[i];
    copy[i] = '\0';
    return copy;
}

void parseFile(FILE *fp, struct tree **root, int key){
    char lineBuf[450]; //line buffer of the file
    char *dataEntries[9]; //data parsed from the file that will be put into a node of the AVL tree
    char *delim = "\t"; //delimiter
    char *token;
    int i = 0;
    while(fgets(lineBuf, sizeof(lineBuf), fp)){
        token = strtok(lineBuf, delim);
        //printf("Hi\n");
        while(token != NULL){
            //printf("yo\n");
            //dataEntries[i] = malloc(strlen(token)* sizeof(char));
            dataEntries[i] = copyString(token);
            token = strtok(NULL, delim);
            i++;
        }
        //printf("I'm here\n");
        treeInsert(&(*root), dataEntries, key);
        //for(i=0;i<9;i++)
            //free(dataEntries[i]);
        //free(*dataEntries);
        i=0;
    }
}

//creates a crud record
void crudCreate(){
    
}

//
void crudRetrieve(){

}
void crudUpdate(){

}
void crudDelete(){

}

// Testing the CRUD implementation
void crudTest(char* fname){
    int ch = 0;
    FILE *fp;
    printf("[1] Create\n");
    printf("[2] Retrieve\n");
    printf("[3] Update\n");
    printf("[4] Delete\n");
    while(1){
        scanf("%d", &ch);
        switch(ch){
            case 1:
                fp = fopen(fname, "a");
                crudCreate();
                break;
            case 2:
                fp = fopen(fname, "rb");
                crudRetrieve();
                break;
            case 3:
                fp = fopen(fname, "r+");
                crudUpdate();
                break;
            case 4:
                crudDelete();
                break;
            default:
                break;
        }
    }
}