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


struct tree;

void clearEOL(){
    char ch;
    ch = getchar();
    while(ch != '\n')
        ch = getchar();
    return;
}

//Utility function to lower case a string of characters
char *lowerCaseString(int c, char input[c]){
    int i=0;
    for(i=0; input[i]; i++) 
        input[i] = tolower(input[i]);
    return input;
}

//function that parses user log and movie database files
void parseFile(FILE *fp, struct tree **root, char **dataEntries, int key, bool isUserData, bool keyTitle){
    char lineBuf[500]; //line buffer of the file
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
}
// ultility for saving AVL tree to user loclearEOL()g file after selecting "Save and Quit"
void writeTreeToFile(const struct tree *root, FILE *fp){
    if(root != 0){
        writeTreeToFile(root->child[0], fp);
        fprintf(fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%d/%d/%d\n", root->tconst, root->titleType, root->primaryTitle, root->originalTitle, 
                                                                        root->isAdult, root->startYear, root->endYear, root->runtimeMinutes, root->genres, 
                                                                        root->mediaType, root->dateAcquired->month, root->dateAcquired->day, root->dateAcquired->year);
        writeTreeToFile(root->child[1], fp);
    }
}

//Edit the user data for any movie entry
void editUserDataForEntry(struct tree *node, bool isInit){
    char choice[4];
    int items=0;
    bool isValid = false;
    char buffer[12];
    while(!isValid){
        printf("Media Type (1=dvd, 2=bluray, 3=digital): ");
        if(!fgets(choice, sizeof choice, stdin))
            printf("Error. Invalid input, try again.\n");
        else{
            if(!strchr(choice, '\n')){
                printf("Error. Invalid input, try again.\n");
                while(fgets(choice, sizeof choice, stdin) && !strchr(choice, '\n'));
            }
            else{
                char *chk;
                int temp = (int)strtol(choice, &chk, 10);
                if(isspace(*chk) || *chk == 0){
                    switch(temp){
                        case 1:
                            free(node->mediaType);
                            node->mediaType = malloc(sizeof(char)*5);
                            strcpy(node->mediaType,"dvd");
                            isValid = true;
                            break;
                        case 2:
                            free(node->mediaType);
                            node->mediaType = malloc(sizeof(char)*8);
                            strcpy(node->mediaType,"bluray");
                            isValid = true;
                            break;
                        case 3:
                            free(node->mediaType);
                            node->mediaType = malloc(sizeof(char)*9);
                            strcpy(node->mediaType,"digital");
                            isValid = true;
                            break;
                        default:
                            printf("Error. Invalid input, try again.\n");
                            break;
                    }
                }
                else
                    printf("Error. Invalid input, try again.\n");
            }
        }
    }
    isValid = false;
    while(!isValid){
        printf("Date Acquired (MM/DD/YYYY): ");
        //items = scanf("%02d/%02d/%04d", &node->dateAcquired->month, &node->dateAcquired->day, &node->dateAcquired->year);
        fgets(buffer, 12, stdin);
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