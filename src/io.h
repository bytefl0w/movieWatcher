/* 
 * crud.h file
 *  @author Erik Mortimer
 */
#ifndef IO_H
#define IO_H

//functions
void clearEOL();
char *lowerCaseString(int c, char *input);
char *copyString(char str[]);
void parseFile(FILE *fp, struct tree **root, char **dataEntries, int key, bool isUserData, bool keyTitle);
void writeTreeToFile(const struct tree *root, FILE *fp);
void editUserDataForEntry(struct tree *node, bool isInit);

#endif