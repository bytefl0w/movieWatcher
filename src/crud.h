//crud.h
#ifndef CRUD_H
#define CRUD_H
#include <stdio.h>
#include <stdlib.h>
#include "database.h"

void crudCreate(FILE *fp);
void crudRetrieve();
void crudUpdate();
void crudDelete();
void crudTest(char* fname);

#endif