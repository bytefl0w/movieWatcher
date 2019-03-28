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
    bool exit_flag = false, valid_choice = false;
    char fileLocation[33] = "./usr/";
    char input[25], ch, search[200], *p = NULL;
    int i=0, char_count=0;
    printf("***movieWatcher Program***\n****Welcome****\n");
    if(movies == NULL){
        printf("No movie_records file found. Exiting...\n");
        exit(EXIT_FAILURE);
    }else{
        printf("Loading...\n");
        parseFile(movies, &movie_tree, 2);
        fclose(movies);
        movies = fopen("movie_records", "r+");
        parseFile(movies, &tt_movie_tree, 0);
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
            parseFile(user, &user_tree, 2);
            exit_flag = true;
        }
    }
    exit_flag = false;
    while(!exit_flag){
        //system("clear");
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
                fgets(search, 200, stdin);
                if((p = strchr(search, '\n'))) *p = 0;
                searchTree(movie_tree, search);
                printf("Type in index number (ttXXXXXX): ");
                fgets(search, 10, stdin);
                struct tree *tempNode = treeSpecificSearch(tt_movie_tree, search);
                if(tempNode == NULL)
                    break;
                editUserDataForEntry(tempNode);
                treeUserInsert(&user_tree, tempNode);
                treePrint(user_tree, true);
                clearEOL();
                //Ask for date or just use current date
                //Digital, bluray, or dvd
                break;
            case '2': //WORKS
                printf("Remove Movie from Log\n");
                clearEOL();
                treePrint(user_tree, true);
                printf("Enter index number to delete (ttXXXXXX): ");
                fgets(search, 10, stdin);
                if((p = strchr(search, '\n'))) *p = 0;
                puts(search);
                treeDelete(&user_tree, search);
                //list user movie log
                //ask which movie they want to remove
                break;
            case '3':
                printf("List Movie Log\n"); //WORKS
                treePrint(user_tree, true);
                getchar();
                clearEOL();
                break;
            case '4':
                printf("Update Movie Entry\n");
                //List contents of movies in log
                //Ask which one they would like to update
                clearEOL();
                break;
            case '5': //WORKS
                printf("Save and Quit\n");
                fclose(user);
                fopen(fileLocation, "w");
                writeTreeToFile(user_tree, user);
                exit_flag = true;
                clearEOL();
                break;
            default: //WORKS
                printf("Not a vaild option. Try again.\n");
                clearEOL();
                break;
        }
    }

    //FILE *fp = fopen(argv[1],"r+");
    //struct tree *root = TREE_EMPTY;
    //parseFile(fp, &root, 2);
    //treePrint(root);
    //printf("Print\n");
    //searchTree(&root, "tt6172666");
    //searchTree(&root, "tt0000000");
    //searchTree(&root, "tt2395427");
    //treeDestroy(&root);
    //fclose(fp);
    fclose(user);
    fclose(movies);
    return 0;
}