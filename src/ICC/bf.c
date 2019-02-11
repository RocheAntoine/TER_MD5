/*****************************************************************************/
/* Michael Krajecki,                                                         */
/* Universite de Reims                                                       */
/*                                                                           */
/* Les fonctions utiles a l'enumeration complete                             */
/*****************************************************************************/

#include <stdio.h>
#include <omp.h>
#include <openssl/md5.h>
#include "bf.h"
#include <string.h>
#include <math.h>

void initTabSymb(struct bf* e) {
	int i, index;
	e->nbSymbole = 62;
	index = 0;
	// les miniscules
	for (i = 0; i < 26; i++)
		e->tabSymbole[index + i] = (char) ('a' + i);

	index = 26;
	// les majuscules
	for (i = 0; i < 26; i++)
		e->tabSymbole[index + i] = (char) ('A' + i);
	index = 52;
	// les chiffres
	for (i = 0; i < 10; i++)
		e->tabSymbole[index + i] = (char) ('0' + i);

	e->tabSymbole[e->nbSymbole++] = '+';
	e->tabSymbole[e->nbSymbole++] = '-';
	e->tabSymbole[e->nbSymbole++] = '*';
	e->tabSymbole[e->nbSymbole++] = '/';
	e->tabSymbole[e->nbSymbole++] = '=';
	e->tabSymbole[e->nbSymbole++] = '(';
	e->tabSymbole[e->nbSymbole++] = ')';
	e->tabSymbole[e->nbSymbole++] = '<';
	e->tabSymbole[e->nbSymbole++] = '>';
	e->tabSymbole[e->nbSymbole++] = '%';
	e->tabSymbole[e->nbSymbole++] = '$';
	e->tabSymbole[e->nbSymbole++] = '@';
	e->tabSymbole[e->nbSymbole++] = '#';
	e->tabSymbole[e->nbSymbole++] = '!';
	e->tabSymbole[e->nbSymbole++] = '&';
	e->tabSymbole[e->nbSymbole++] = '.';
	e->tabSymbole[e->nbSymbole++] = ';';
	e->tabSymbole[e->nbSymbole++] = '?';
	e->tabSymbole[e->nbSymbole++] = ',';
	e->tabSymbole[e->nbSymbole++] = '_';
}

void decode(struct bf* e, int c, int l, char word[]) {
	int i, val;
	val = c;
	for (i = 0; i < l; i++) {
		word[i] = e->tabSymbole[val % e->nbSymbole];
		val /= e->nbSymbole;
	}
	word[l] = '\0';
	//printf("%s\n",word);
}

bool bruteForceSeq(int p, int l, char motGagnant[], unsigned char monMD5[]) {
	bool match;
	int j, nbPrefixe, prefixe;
	struct bf env;

	// l'initialisation de la table des symboles
	initTabSymb(&env);
	printf("longueur du mot : %d, longueur du prefixe %d\n", l, p);
	printf("Ensemble des symboles (%d) :\n", env.nbSymbole);
	for (j = 0; j < env.nbSymbole; j++) printf("%c", env.tabSymbole[j]);
	printf("\n");

	// on commence par calculer le nombre de prefixe
	nbPrefixe = pow(env.nbSymbole, p);
	printf("\nLe nombre de prefixe : \t %d\n", nbPrefixe);
	match = false;

	char word[64]; // le mot local sur lequel travailler
	for (prefixe = 0; prefixe < nbPrefixe; prefixe++) {
		decode(&env, prefixe, p, word);
		if (!match) {
			if (bruteForcePrefixe(&env, p, l, word, monMD5)) {
				match = true;
				// sprintf(motGagnant, "%s",word);
			}
		}
	}

	return match;
}

bool bruteForceOMP(int p, int l, char motGagnant[], unsigned char monMD5[], int* nbThreads) {
	bool match;
	int j, nbPrefixe, prefixe;
	struct bf env;

	// l'initialisation de la table des symboles
	initTabSymb(&env);
	printf("longueur du mot : %d, longueur du prefixe %d\n", l, p);
	printf("Ensemble des symboles (%d) :\n", env.nbSymbole);
	for (j = 0; j < env.nbSymbole; j++) printf("%c", env.tabSymbole[j]);
	printf("\n");

	// on commence par calculer le nombre de prefixe
	nbPrefixe = pow(env.nbSymbole, p);
	printf("\nLe nombre de prefixe : \t %d\n", nbPrefixe);
	match = false;
	//omp_set_num_threads(nbThreads);
#pragma omp parallel shared(env, match, motGagnant)
	{
		char word[64]; // le mot local sur lequel travailler
#pragma omp single
		{
			*nbThreads = omp_get_num_threads();
			printf("Nombre de threads : \t %d\n", *nbThreads);
		}
#pragma omp for schedule(dynamic)
		for (prefixe = 0; prefixe < nbPrefixe; prefixe++) {
			decode(&env, prefixe, p, word);
			if (!match) {
				if (bruteForcePrefixe(&env, p, l, word, monMD5)) {
					match = true;
					// sprintf(motGagnant, "%s",word);
				}
			}
		}
	}
	return match;
}

bool bruteForcePrefixe(struct bf* e, int p, int l, char word[], unsigned char monMD5[]) {
	// p designe la position de depart dans le mot
	// word[0..p-1] contient le prefixe
	// l designe la longueur totale du mot a construire
	// monMD5 contient le hachage a trouver
	bool match = false;
	int pos[LONGMAXMOT], lmin, lc, i, tour = 0, nbTest;

	// pour le hachage
	unsigned char courantMD5[MD5_DIGEST_LENGTH];

	lc = l - 1; // la longueur courante du mot est egale a p
	lmin = l - 1; // on a pas ete plus loin que

	// position dans la la table des symboles pour chaque symbole du mot
	pos[p - 1] = 0; // cas particulier du dernier tour
	for (i = p; i < l; ++i) {
		pos[i] = 0;
		word[i] = e->tabSymbole[0];
	}
	word[l] = '\0';
	// il y aura nbTest a realiser sauf pour un prefixe qui conduit a la solution
	nbTest = (int) pow(e->nbSymbole, l - p);
	// printf("bf : premier mot = %s, nbTest=%d\n",word,nbTest);
	for (tour = 0; tour < nbTest; ++tour) {
		if (!match) {
			// on teste le mot courant
			// on hash le code
			MD5(word, strlen(word), courantMD5);
			match = true;
			i = 0;
			while (match && i < MD5_DIGEST_LENGTH) match = (monMD5[i] == courantMD5[i++]);
			if (!match) {
				// on passe au mot suivant
				if (++pos[lc] == e->nbSymbole) {
					// on change de position dans le mot
					word[lc] = e->tabSymbole[0];
					pos[lc--] = 0;
					// de combien de lettres on decale a gauche ?
					while (++pos[lc] == e->nbSymbole) {
						word[lc] = e->tabSymbole[0];
						pos[lc--] = 0;
					}
					if (lc < lmin) lmin = lc;
					word[lc] = e->tabSymbole[pos[lc]];
					lc = l - 1;
				} else word[lc] = e->tabSymbole[pos[lc]];
			}
		}
	}
	// printf("Sortie de bf\n");
	// fflush(stdout);
	return match;
}

