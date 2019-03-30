#ifndef TER_MD5_UTILS_H
#define TER_MD5_UTILS_H

#include <openssl/md5.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
	int nbMots;
	char* fichierMots;
	int isHash;
	int isList;
	unsigned char** hashList;
	char** brutHashList;
	char** wordList;
} parametre_t;

parametre_t* init_parametre(char*);

#endif //TER_MD5_UTILS_H
