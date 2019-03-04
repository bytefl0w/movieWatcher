/* main.c file
 * @author Erik Mortimer <egmortimer.crimson.ua.edu>
 */
// Standard Library Includes
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Written Includes
#include "crud.h"

int main(int argc, char* argv[]){
    printf("movieWatcher Program\n*Welcome*\n");
    crudTest(argv[1]);
    return 0;
}