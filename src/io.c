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
// ultility for saving AVL tree to user log file after selecting "Save and Quit"
void treePrintToFile(const struct tree *root, FILE *fp){
    if(root != 0){
        treePrintToFile(root->child[0], fp);
        fprintf(fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", root->tconst, root->titleType, root->primaryTitle, root->originalTitle, root->genres, root->isAdult, root->startYear, root->endYear, root->runtimeMinutes);
        treePrintToFile(root->child[1], fp);
    }
}

// looks up tree node and update part of the contents 
void updateEntry(){

}

// Lists the current user's AVL tree, built from there .log file
void listUserTree(){
    
}