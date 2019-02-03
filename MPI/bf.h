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


#define MAXSYMBOLE 100
#define LONGMAXMOT 10
#define TIME_MODE 1

struct bf {
	char tabSymbole[MAXSYMBOLE] ;
	int nbSymbole;
};

void initTabSymb(struct bf*);
void decode(struct bf* e, int c, int l, char word[]);
bool bruteForceMPI_maitre(int p, int l,char word[], unsigned char monMD5[], int);
void bruteForceMPI_esclave(int p, int l, unsigned char monMD5[]);
bool bruteForcePrefixe(struct bf* e, int p, int l, char word[],unsigned char monMD5[]);

#endif
