#include "fileio.h"

int countLines(FILE *fp){
    char c, result;
    if(fp == NULL){
        printf("ERROR: Could not open file\n");
        return 0;
    }
    for(c = getc(fp); c != EOF; c = getc(fp))
        if(c == 'n') result++;
    return result;
}