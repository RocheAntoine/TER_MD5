/*****************************************************************************/
/* Michael Krajecki,                                                         */
/* Universite de Reims                                                       */
/*                                                                           */
/* Les fonctions utiles a l'enumeration complete                             */
/*****************************************************************************/


#ifndef BF_H
#define BF_H

#include <stdbool.h>
#include <math.h>
#include <stddef.h>
#include "utils.h"

#define MAXSYMBOLE 100
#define LONGMAXMOT 6
#define TIME_MODE 1

#define REQUEST_TAG 0
#define ANSWER_TAG 1
#define WORD_TAG 2
#define NEW_MD5_TAG 3

#define max(a,b) ((a) < (b) ? (b) : (a))
#define min(a,b) ((a) > (b) ? (b) : (a))


struct bf {
	char tabSymbole[MAXSYMBOLE] ;
	int nbSymbole;
};

typedef struct
{
	int wordSize;
	int prefixeSize;
	long unsigned int prefixe;
	int newMD5;
} request_t;

void destroy_mpi_struct();
void init_mpi_struct();
void initTabSymb(struct bf*);
void decode(struct bf* e, int c, int l, char word[]);
bool bruteForceMPI_maitre(char**, parametre_t*, int);
void bruteForceMPI_esclave();
bool bruteForcePrefixe(struct bf* e, int p, int l, char word[],unsigned char monMD5[]);

#endif
