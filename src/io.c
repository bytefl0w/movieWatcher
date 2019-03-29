/* 
 * io.c file
 *  @author Erik Mortimer
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "tree.h"
#include "io.h"

/* utility function to parse the "movie_records" file */
struct tree;

char *lowerCaseString(int c, char input[c]){
    int i=0;
    for(i=0; input[i]; i++) 
        input[i] = tolower(input[i]);
    return input;
}

//char *copyString(char str[]){
//    size_t len = strlen(str), i;
//    char *copy[len+1];
//    for(i=0;i<len;i++)
//        copy[i] = str[i];
//    copy[i] = '\0';
//    return copy;
//}

void parseFile(FILE *fp, struct tree **root, char **dataEntries, int key, bool isUserData, bool keyTitle){
    char lineBuf[500]; //line buffer of the file
    //char dataEntries[11][200]; //data parsed from the file that will be put into a node of the AVL tree
    char *delim = "\t\n"; //delimiter
    char *token;
    int i = 0,j=0;
    while(fgets(lineBuf, sizeof(lineBuf), fp)){
        token = strtok(lineBuf, delim);
        while(token != NULL){
            strcpy(dataEntries[i], token);
            token = strtok(NULL, delim);
            i++;
        }
        treeInitInsert(&(*root), dataEntries, key, isUserData, keyTitle);
        i=0;
    }
    //CLEARBUF()
}
// ultility for saving AVL tree to user log file after selecting "Save and Quit"
void writeTreeToFile(const struct tree *root, FILE *fp){
    if(root != 0){
        writeTreeToFile(root->child[0], fp);
        fprintf(fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%d/%d/%d\n", root->tconst, root->titleType, root->primaryTitle, root->originalTitle, 
                                                                        root->isAdult, root->startYear, root->endYear, root->runtimeMinutes, root->genres, 
                                                                        root->mediaType, root->dateAcquired->month, root->dateAcquired->day, root->dateAcquired->year);
        writeTreeToFile(root->child[1], fp);
    }
}

void editUserDataForEntry(struct tree *node, bool isInit){
    int choice=0, items=0;
    bool isValid = false;
    char buffer[12];
    while(!isValid){
        printf("Media Type (1=dvd, 2=bluray, 3=digital): ");
        fgets(buffer, 1, stdin);
        items = sscanf(buffer, "%1c", &node->dateAcquired->month, &node->dateAcquired->day, &node->dateAcquired->year);
        switch(choice){
            case 1:
                strcpy(node->mediaType,"dvd");
                isValid = true;
                break;
            case 2:
                strcpy(node->mediaType,"bluray");
                isValid = true;
                break;
            case 3:
                strcpy(node->mediaType,"digital");
                isValid = true;
                break;
            default:
                printf("Error. Invalid input, try again.\n");
                break;
        }
    }
    isValid = false;
    while(!isValid){
        printf("Date Acquired (MM/DD/YYYY): ");
        //items = scanf("%02d/%02d/%04d", &node->dateAcquired->month, &node->dateAcquired->day, &node->dateAcquired->year);
        fgets(buffer, 10, stdin);
        items = sscanf(buffer, "%02d/%02d/%04d", &node->dateAcquired->month, &node->dateAcquired->day, &node->dateAcquired->year);
        if(items == 3){
            //node->dateAcquired = dateAcquired;
            isValid = true;
        } else if(items == 0){
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            node->dateAcquired->day = timeinfo->tm_mday+1;
            node->dateAcquired->month = timeinfo->tm_mon+1;
            node->dateAcquired->year = timeinfo->tm_year+1900;
        } else{
            printf("Error. Invalid input, try again.\n");
        }
    }

}