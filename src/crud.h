//crud.h
#ifndef CRUD_H
#define CRUD_H
//#include <string.h>

typedef struct{
  char* tconst, titleType, primaryTitle;
  char* originalTitle, isAdult, genres;
  int startYear, endYear, runtimeMinutes;
} Movie;

Movie* crudCreate();
Movie crudRetrieve();
Movie crudUpdate();
Movie crudDelete();

#endif