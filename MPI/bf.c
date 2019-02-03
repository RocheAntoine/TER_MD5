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
#include <mpi.h>
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

void bruteForceMPI_esclave(int p, int l, unsigned char monMD5[])
{
	struct bf env;
	char match = 0;
	int prefixe;
	char word[64];
	initTabSymb(&env);
//	printf("Esclave : Reception requete\n");
	MPI_Recv(&prefixe, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//	printf("Esclave : Requete recue\n");
	while(prefixe != -1)
	{
		decode(&env, prefixe, p, word);
		if(bruteForcePrefixe(&env, p, l, word, monMD5))
		{
//			printf("Esclave : match true\n");
			match = 1;
//			printf("Esclave : Envoi match\n");
			MPI_Send(&match, 1, MPI_SIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
//			printf("Esclave : Envoi mot\n");
			MPI_Send(&word, 64, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
//			printf("Esclave : mot envoye\n");
			break;
		}
//		printf("Esclave : Envoi match\n");
		MPI_Send(&match, 1, MPI_SIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
//		printf("Esclave : Reception requete\n");
		MPI_Recv(&prefixe, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//		printf("Esclave : Requete recue\n");
	}
//	printf("Esclave : Sortie de boucle\n");
}

bool bruteForceMPI_maitre(int p, int l, char motGagnant[], unsigned char monMD5[], int nbThreads) {
	bool match;
	char tmpMatch;
	int i, j, nbPrefixe, prefixe, resteThreads, stop = -1;
	struct bf env;
	MPI_Status status;
	// l'initialisation de la table des symboles
	initTabSymb(&env);

	if(!TIME_MODE) {
		printf("longueur du mot : %d, longueur du prefixe %d\n", l, p);
		printf("Ensemble des symboles (%d) :\n", env.nbSymbole);
		for (j = 0; j < env.nbSymbole; j++) printf("%c", env.tabSymbole[j]);
		printf("\n");
	}

	// on commence par calculer le nombre de prefixe
	nbPrefixe = pow(env.nbSymbole, p);
//	printf("\nLe nombre de prefixe : \t %d\n", nbPrefixe);
	match = false;

	char word[64]; // le mot local sur lequel travailler

	for(prefixe = 0; prefixe < nbThreads - 1 && prefixe < nbPrefixe; prefixe++)
	{
		MPI_Send(&prefixe, 1, MPI_INT, prefixe + 1, 0, MPI_COMM_WORLD);
	}

	resteThreads = nbThreads - 2;
	while((!match || resteThreads > 0) && prefixe < nbPrefixe + nbThreads - 1)
	{
	//	printf("Maitre : match : %d, prefixe : %d, nbPrefixe : %d, resteThreads : %d\n", match, prefixe, nbPrefixe, resteThreads);
		MPI_Recv(&tmpMatch, 1, MPI_SIGNED_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//		printf("Maitre : Match recu\n");
		if(match)
		{
//			printf("Maitre : Match true\n");
			MPI_Send(&stop, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			resteThreads--;
		}
		else if(tmpMatch)
		{
			match = true;
			MPI_Recv(motGagnant, 64, MPI_SIGNED_CHAR, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		}
		else if(prefixe < nbPrefixe)
		{
			//printf("source : %d\n", status.MPI_SOURCE); fflush(stdout);
//			printf("Maitre : Envoi requete\n");
			MPI_Send(&prefixe, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
//			printf("Maitre : Requete envoyee\n");
		}

		prefixe++;
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

