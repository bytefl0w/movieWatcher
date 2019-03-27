/* 
 * io.c file
 *  @author Erik Mortimer
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
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
    char *delim = "\t\n"; //delimiter
    char *token;
    int i = 0;
    while(fgets(lineBuf, sizeof(lineBuf), fp)){
        token = strtok(lineBuf, delim);
        while(token != NULL){
            //printf("yo\n");
            //dataEntries[i] = malloc(strlen(token)* sizeof(char));
            dataEntries[i] = copyString(token);
            token = strtok(NULL, delim);
            i++;
        }
        treeInitInsert(&(*root), dataEntries, key);
        i=0;
    }
    //CLEARBUF()
}
// ultility for saving AVL tree to user log file after selecting "Save and Quit"
void writeTreeToFile(const struct tree *root, FILE *fp){
    if(root != 0){
        writeTreeToFile(root->child[0], fp);
        fprintf(fp, "%.9s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", root->tconst, root->titleType, root->primaryTitle, root->originalTitle, root->genres, root->isAdult, root->startYear, root->endYear, root->runtimeMinutes);
        writeTreeToFile(root->child[1], fp);
    }
}

void updateMovieEntry(struct tree *node){
    int choice=0, isvalid = 0, items=0;
    char buffer[10];
    struct time *dateAcquired = malloc(sizeof(*dateAcquired));
    while(isvalid == 0){
        printf("Media Type (1=dvd, 2=bluray, 3=digital): ");
        scanf("%d", choice);
        switch(choice){
            case 1:
                *node->mediaType = copyString("dvd");
                isvalid = 1;
                break;
            case 2:
                *node->mediaType = copyString("bluray");
                isvalid = 1;
                break;
            case 3:
                *node->mediaType = copyString("digital");
                isvalid = 1;
                break;
            default:
                printf("Error. Invalid input, try again.\n");
                break;
        }
    }
    isvalid = 0;
    while(isvalid == 0){
        printf("Date Acquired (MM/DD/YYYY): ");
        items = scanf("%02d/%02d/%04d", &dateAcquired->month, &dateAcquired->day, &dateAcquired->year);
        if(items == 3){
            node->dateAcquired = dateAcquired;
            isvalid = 1;
        } else{
            printf("Error. Invalid input, try again.\n");
        }
    }

}