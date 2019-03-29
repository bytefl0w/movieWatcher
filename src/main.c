/* 
 * main.c file
 *  @author Erik Mortimer
 */

// Standard Library Includes
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Written Includes
#include "tree.h"
#include "io.h"

void clearEOL();

void clearEOL(){
    char ch;
    ch = getchar();
    while(ch != '\n')
        ch = getchar();
    return;
}

int main(int argc, char* argv[]){
    FILE *user, *movies = fopen("movie_records", "r+");
    struct tree *movie_tree = TREE_EMPTY;
    struct tree *tt_movie_tree = TREE_EMPTY;
    struct tree *user_tree = TREE_EMPTY;
    struct tree *tempNode;

    bool exit_flag = false, valid_choice = false;

    char fileLocation[33] = "./usr/";
    char input[25], ch, search[200], *p = NULL;

    int i=0, char_count=0, row=11, col[11]={20, 20, 400, 400, 20, 20, 20, 20, 200, 20, 20};

    char **dataEntries;
    dataEntries = (char**)malloc(row * sizeof(char*));
    for(i=0;i<row;i++)
        dataEntries[i] = (char*)malloc(col[i] * sizeof(char));

    printf("***movieWatcher Program***\n****Welcome****\n");
    if(movies == NULL){
        printf("No movie_records file found. Exiting...\n");
        exit(EXIT_FAILURE);
    }else{
        printf("Loading...\n");
        parseFile(movies, &movie_tree, dataEntries, 2, false, true);
        fclose(movies);
        movies = fopen("movie_records", "r+");
        parseFile(movies, &tt_movie_tree, dataEntries, 0, false, false);
        fclose(movies);
    }
    system("ls usr/");
    while(!exit_flag){
        valid_choice = false;
        printf("Enter in your first name: ");
        ch = getchar();
        while((char_count < 25) && (ch != '\n')){
            input[char_count++] = ch;
            if(!isalpha(ch))
                break;
            ch = getchar();
        }
        input[char_count] = '\0';
        if(!isalpha(input[char_count-1])){
            printf("Error: Input is not alphabetical. Try again.\n");
            memset(input, 0, sizeof(input));
            char_count=0;
            clearEOL();

        }else{
            input[char_count] = '\0';
            strcat(fileLocation, input);
            strcat(fileLocation, ".log");
            user = fopen(fileLocation, "r+");
            if(user == NULL)
                user = fopen(fileLocation, "w+");
            parseFile(user,  &user_tree, dataEntries, 0, true, false);
            exit_flag = true;
        }
    }
    exit_flag = false;
    for(i=0;i<row;i++)
        free(dataEntries[i]);
    free(dataEntries);
    while(!exit_flag){
        printf("Welcome %s!\n", input);
        printf("[1] Add Movie to Log\n");
        printf("[2] Remove Movie from Log\n");
        printf("[3] List Movie Log\n");
        printf("[4] Update Movie Entry\n");
        printf("[5] Save and Quit\n> ");
        scanf("%c", &ch);
        switch(ch){
            case '1':
                clearEOL();
                printf("Add Movie to Log\n");
                printf("Enter in title of movie: ");
                fgets(search, 200, stdin);
                if((p = strchr(search, '\n'))) *p = 0;
                searchTree(movie_tree, lowerCaseString(200, search));
                printf("Type in index number (ttXXXXXXX): ");
                fgets(search, 10, stdin);
                tempNode = treeSpecificSearch(tt_movie_tree, search);
                if(tempNode == NULL)
                    break;
                treeUserInsert(&user_tree, tempNode);
                tempNode = treeSpecificSearch(user_tree, search);
                editUserDataForEntry(tempNode, true);
                treePrint(user_tree, true);
                clearEOL();
                break;
            case '2': 
                printf("Remove Movie from Log\n");
                clearEOL();
                treePrint(user_tree, true);
                printf("Enter index number to delete (ttXXXXXX): ");
                fgets(search, 10, stdin);
                if((p = strchr(search, '\n'))) *p = 0;
                treeDelete(&user_tree, search);
                clearEOL();
                break;
            case '3':
                printf("List Movie Log\n");
                treePrint(user_tree, true);
                getchar();
                clearEOL();
                break;
            case '4':
                clearEOL();
                printf("Update Movie Entry\n");
                treePrint(user_tree, true);
                printf("Type in index number (ttXXXXXX): ");
                fgets(search, 10, stdin);
                if((p = strchr(search, '\n'))) *p = 0;
                struct tree *tempNode = treeSpecificSearch(user_tree, search);
                if(tempNode == NULL)
                    break;
                editUserDataForEntry(tempNode, false);
                treePrint(user_tree, true);
                clearEOL();
                break;
            case '5': //WORKS
                printf("Save and Quit\n");
                fclose(user);
                user = fopen(fileLocation, "w");
                writeTreeToFile(user_tree, user);
                exit_flag = true;
                fclose(user);
                clearEOL();
                break;
            default: //WORKS
                printf("Not a vaild option. Try again.\n");
                clearEOL();
                break;
        }
    }
    //Freeing Dynamically Allocated memory
    treeDestroy(&tt_movie_tree, false);
    treeDestroy(&movie_tree, false);
    treeDestroy(&user_tree, true);
    return 0;
}