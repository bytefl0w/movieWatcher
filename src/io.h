/* 
 * crud.h file
 *  @author Erik Mortimer
 */
#ifndef IO_H
#define IO_H

//functions
char *copyString(char str[]);
void parseFile(FILE *fp, struct tree **root, int key);
void writeTreeToFile();
void updateEntry();

#endif