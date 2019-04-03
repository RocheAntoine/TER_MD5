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

#define DATA_SIZE 64

static MPI_Datatype mpi_request_type;

void destroy_mpi_struct()
{
	MPI_Type_free(&mpi_request_type);
}

void init_mpi_struct()
{
	const int nitems=4;
	int blocklengths[4] = {1,1,1,1};
	MPI_Datatype types[4] = {MPI_INT, MPI_INT, MPI_UNSIGNED_LONG, MPI_INT};
	MPI_Aint     offsets[4];

	offsets[0] = offsetof(request_t, wordSize);
	offsets[1] = offsetof(request_t, prefixeSize);
	offsets[2] = offsetof(request_t, prefixe);
	offsets[3] = offsetof(request_t, newMD5);

	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_request_type);
	MPI_Type_commit(&mpi_request_type);

}

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

void bruteForceMPI_esclave()
{
	struct bf env;
	char match = 1;
	char noMatch = 0;
	long unsigned int prefixe;
	int p,l;
	char word[64];
	unsigned char monMD5[MD5_DIGEST_LENGTH];
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	request_t request;
	initTabSymb(&env);
	//init_mpi_struct();
	printf("Esclave %d : Reception hash\n", rank);fflush(stdout);
	//MPI_Barrier(MPI_COMM_WORLD);
	do
	{
		MPI_Recv(monMD5, MD5_DIGEST_LENGTH, MPI_UNSIGNED_CHAR, 0, NEW_MD5_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//printf("Esclave %d : Hash recu\n", rank);fflush(stdout);
		MPI_Send(&noMatch, 1, MPI_SIGNED_CHAR, 0, ANSWER_TAG, MPI_COMM_WORLD);
		//printf("Esclave %d : Premiere reponse envoyee\n", rank);fflush(stdout);
		MPI_Recv(&request, 1, mpi_request_type, 0, REQUEST_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	while(request.newMD5);
	printf("Esclave %d : Requete recue\n", rank);fflush(stdout);
	p = request.prefixeSize;
	l = request.wordSize;
	prefixe = request.prefixe;
	//printf("Esclave : Requete recue\n");
	while(prefixe != -1)
	{
		decode(&env, prefixe, p, word);
		if(bruteForcePrefixe(&env, p, l, word, monMD5))
		{
			//printf("Esclave %d : match true\n", rank);fflush(stdout);
		//	printf("Esclave %d : Envoi mot : %s\n", rank, word);fflush(stdout);
				//printf("Esclave %d : Envoi match\n", rank);fflush(stdout);
			MPI_Send(&match, 1, MPI_SIGNED_CHAR, 0, ANSWER_TAG, MPI_COMM_WORLD);
		//	printf("Esclave %d : Envoi mot : %s\n", rank, word);
			MPI_Send(word, 64, MPI_CHAR, 0, WORD_TAG, MPI_COMM_WORLD);
				//printf("Esclave %d : mot envoye\n", rank);fflush(stdout);
		}
		//printf("Esclave : Envoi match\n");
	//	else
	//	{
		MPI_Send(&noMatch, 1, MPI_SIGNED_CHAR, 0, ANSWER_TAG, MPI_COMM_WORLD);
	//	}
		//printf("Esclave %d : Reception requete\n", rank);
		MPI_Recv(&request, 1, mpi_request_type, 0, REQUEST_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		while(request.newMD5)
		{
			//printf("Esclave %d : En attente du Hash\n", rank);fflush(stdout);
			MPI_Recv(monMD5, MD5_DIGEST_LENGTH, MPI_UNSIGNED_CHAR, 0, NEW_MD5_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//printf("Esclave %d : Nouveau hash recu\n", rank);fflush(stdout);
			MPI_Send(&noMatch, 1, MPI_SIGNED_CHAR, 0, ANSWER_TAG, MPI_COMM_WORLD);
			//printf("Esclave %d : Demande requete envoyee\n", rank);fflush(stdout);
			MPI_Recv(&request, 1, mpi_request_type, 0, REQUEST_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//printf("Esclave %d : Requete recue\n", rank);fflush(stdout);
		}

		//printf("Esclave %d : Nouvelle requete recue\n", rank);
		p = request.prefixeSize;
		l = request.wordSize;
		prefixe = request.prefixe;
		/*if(prefixe == -1)
		{
			//printf("Esclave %d : Message stop recu\n", rank);
			fflush(stdout);
		}*/
		//printf("Esclave : Requete recue\n");
	}
	//printf("Esclave %d : Sortie de boucle\n", rank);fflush(stdout);
}

bool bruteForceMPI_maitre(char** motGagnant, parametre_t* param, int nbThreads) {
	bool match;
	char tmpMatch;
	int i, j, resteThreads, stop = -1, p , l;
	long unsigned int nbPrefixe, prefixe;
	unsigned char monMD5[MD5_DIGEST_LENGTH];
	char output[DATA_SIZE];
	char winnerWord[64];
	struct bf env;
	request_t request;
	MPI_Status status;
	// l'initialisation de la table des symboles
	initTabSymb(&env);
	FILE* fp;
	fp = fopen("res5616.txt", "w");
	char word[64]; // le mot local sur lequel travailler

	if(!TIME_MODE) {
		printf("longueur du mot : %d, longueur du prefixe %d\n", l, p);
		printf("Ensemble des symboles (%d) :\n", env.nbSymbole);
		for (j = 0; j < env.nbSymbole; j++) printf("%c", env.tabSymbole[j]);
		printf("\n");
	}
/*
	request.wordSize = 1;
	request.prefixeSize = 1;
	nbPrefixe = env.nbSymbole;

	request.prefixe = 0;
	for (i = 1; i < nbThreads; i++)
	{
		MPI_Send(&request, 1, mpi_request_type, i, 0, MPI_COMM_WORLD);
	}
*/

	printf("Maitre : nbMots : %d\n", param->nbMots);

	request.newMD5 = 0;

	// on commence par calculer le nombre de prefixe
	for(i = 0; i < param->nbMots; i ++)
	{
		match = false;
		resteThreads = nbThreads;
		printf("Maitre : Envoi des hash\n");fflush(stdout);
		request.newMD5=0;
		//MPI_Barrier(MPI_COMM_WORLD);
		for(j = 0; j < nbThreads; j++)
		{
		//	printf("Envoi à %d\n",j);fflush(stdout);
			MPI_Send(param->hashList[i], MD5_DIGEST_LENGTH, MPI_UNSIGNED_CHAR, j, NEW_MD5_TAG, MPI_COMM_WORLD);
		//	printf("Envoye à %d\n",j);fflush(stdout);
		}
		printf("Maitre : Hash envoyes\n");fflush(stdout);

		for (l = 1; l <= LONGMAXMOT; l++)
		{
			request.wordSize = l;
			p = min(2, l);
			request.prefixeSize = p;
			nbPrefixe = pow(env.nbSymbole, p);
			prefixe = 0;
			//printf("Taille Mot : %d, taille prefixe : %d, nb prefixe : %d\n", l,p,nbPrefixe);
//	printf("\nLe nombre de prefixe : \t %d\n", nbPrefixe);

			//printf("Definition des tailles : %d\n", l);



			while (!match && prefixe < nbPrefixe)
			{
				//	printf("Maitre : match : %d, prefixe : %d, nbPrefixe : %d, resteThreads : %d\n", match, prefixe, nbPrefixe, resteThreads);
				MPI_Recv(&tmpMatch, 1, MPI_SIGNED_CHAR, MPI_ANY_SOURCE, ANSWER_TAG, MPI_COMM_WORLD, &status);
		//printf("Maitre : Match recu\n");
				if (tmpMatch)
				{
					//request.newMD5 = 1;
					match = true;
					printf("Maitre : Match true\n");fflush(stdout);
					MPI_Recv(winnerWord, 64, MPI_SIGNED_CHAR, status.MPI_SOURCE, WORD_TAG, MPI_COMM_WORLD, &status);
				//	MPI_Send(&request, 1, mpi_request_type, status.MPI_SOURCE, REQUEST_TAG, MPI_COMM_WORLD);
					printf("Maitre : Mot trouve : %s\n", winnerWord);fflush(stdout);
					//request.newMD5 = 0;
					if(param->isHash)
						fprintf(fp, "%s -> %s\n", param->brutHashList[i], winnerWord);
					else
						fprintf(fp, "%s\n", winnerWord);
					//memset(winnerWord,0, strlen(winnerWord));
				} else
				{
					//printf("source : %d\n", status.MPI_SOURCE); fflush(stdout);
					//printf("Maitre : Envoi requete\n");
					request.prefixe = ++prefixe;
					MPI_Send(&request, 1, mpi_request_type, status.MPI_SOURCE, REQUEST_TAG, MPI_COMM_WORLD);
					//printf("Maitre : Requete envoyee\n");
				}

			}

			if (match)
			{
			//	printf("Maitre : Debut envoi des stop\n");fflush(stdout);
				request.newMD5 = 1;
				if(i >= param->nbMots - 1)
				{
					printf("Maitre : Travail termine\n");
					request.prefixe = -1;
					request.newMD5 = 0;
				}

				while (resteThreads > 0)
				{
					MPI_Recv(&tmpMatch, 1, MPI_SIGNED_CHAR, MPI_ANY_SOURCE, ANSWER_TAG, MPI_COMM_WORLD, &status);
					if(tmpMatch)
					{
						printf("Maitre : Match en trop !\n");
						MPI_Recv(word, 64, MPI_SIGNED_CHAR, status.MPI_SOURCE, WORD_TAG, MPI_COMM_WORLD, &status);
						MPI_Recv(&tmpMatch, 1, MPI_SIGNED_CHAR, status.MPI_SOURCE, ANSWER_TAG, MPI_COMM_WORLD, &status);

					}
					MPI_Send(&request, 1, mpi_request_type, status.MPI_SOURCE, REQUEST_TAG, MPI_COMM_WORLD);
					resteThreads--;
			//		printf("Maitre : Stop envoye, encore %d\n", resteThreads);fflush(stdout);
				}
			//	printf("Maitre : Fin envoi des stop\n");fflush(stdout);
				//printf("Maitre : Sortie while\n");fflush(stdout);
				request.newMD5 = 0;
				break;
			}
		}

		if(i>=param->nbMots-1)
		{
			request.prefixe = -1;
			request.newMD5 = 0;
		}
		else
			request.newMD5 = 1;

		while (resteThreads > 0)
		{
			printf("Nop nop nop nop !\n");
			MPI_Recv(&tmpMatch, 1, MPI_SIGNED_CHAR, MPI_ANY_SOURCE, ANSWER_TAG, MPI_COMM_WORLD, &status);
			if(tmpMatch)
			{
				MPI_Recv(word, 64, MPI_SIGNED_CHAR, status.MPI_SOURCE, WORD_TAG, MPI_COMM_WORLD, &status);
				MPI_Recv(&tmpMatch, 1, MPI_SIGNED_CHAR, status.MPI_SOURCE, ANSWER_TAG, MPI_COMM_WORLD, &status);
				if(!match)
				{
					printf("Maitre : Mot trouve : %s\n", winnerWord);fflush(stdout);
					if(param->isHash)
						fprintf(fp, "%s -> %s\n", param->brutHashList[i], winnerWord);
					else
						fprintf(fp, "%s\n", winnerWord);
					//memset(winnerWord,0, strlen(winnerWord));
				}

			}
			MPI_Send(&request, 1, mpi_request_type, status.MPI_SOURCE, REQUEST_TAG, MPI_COMM_WORLD);
			resteThreads--;

			//printf("Maitre : Stop envoye, encore %d\n", resteThreads);fflush(stdout);
		}
		request.newMD5 = 0;
		//printf("Maitre : Sortie while\n");fflush(stdout);
	}
	fclose(fp);
	printf("Maitre : sortie\n");fflush(stdout);
	return match;
}

bool bruteForcePrefixe(struct bf* e, int p, int l, char sharedWord[], unsigned char monMD5[]) {
	// p designe la position de depart dans le mot
	// word[0..p-1] contient le prefixe
	// l designe la longueur totale du mot a construire
	// monMD5 contient le hachage a trouver
	bool match = false, sharedMatch;
	char word[16];
	int pos[LONGMAXMOT], lmin, lc, i, tour = 0, nbTest;
	//printf("Debut prefixe\n");
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
#pragma omp parallel for shared(sharedMatch, sharedWord) private(match, pos, word, lc, i)
	for (tour = 0; tour < nbTest; ++tour) {
		if (!sharedMatch) {
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
			else
			{
				printf("Trouve");
				sharedMatch = 1;
				sprintf(sharedWord,"%s",word);
				//sharedWord = word;
			}
		}
	}
	 //printf("Sortie de bf\n");
	// fflush(stdout);
	return sharedMatch;
}

