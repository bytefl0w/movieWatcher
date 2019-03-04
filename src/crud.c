#include "crud.h"

void crudCreate(FILE *fp){
    struct Movie *mov;
    if((mov = malloc(sizeof mov) != NULL)){
        printf("test\n");
    }
    return;
}
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
                crudCreate(&fp);
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