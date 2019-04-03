#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <openssl/md5.h>
#include <mpi.h>
#include "bf.h"
#include <unistd.h>
#include "utils.h"

int main (int argc, char **argv) {
	unsigned char monMD5[MD5_DIGEST_LENGTH];
	int i;
	char word[64];

	int taillePrefixe;
	// pour le temps
	struct timeb tav, tap ;
	double te;
	int nbThreads;
	int rank;
	char nomFichier[64];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nbThreads);

	/*if(nbThreads == 1)
	{
		printf("Erreur : Impossible d'utiliser qu'un thread avec MPI\n");
		MPI_Finalize();
		exit(1);
	}*/

	/* create a type for struct request */
	init_mpi_struct();

	if (argc > 1)
	{
		strcpy(nomFichier, argv[1]);
	}
	else
		strcpy(nomFichier, "conf.conf");


	if(!rank)
	{
	//	printf("Init parametre\n");fflush(stdout);
		//printf("Taille d'un hash : %d", MD5_DIGEST_LENGTH);

#pragma omp parallel sections
		{
#pragma omp section
			{
				parametre_t* param;
				param = init_parametre(nomFichier);
				if(param == NULL)
				{
					printf("Erreur init parametre");
					MPI_Finalize();
					exit(EXIT_FAILURE);
				}
				if(!param->isHash)
				{
					//	printf("alloc hash\n");fflush(stdout);
					param->hashList=(unsigned char**)malloc(param->nbMots * sizeof(unsigned char*));
					for(i=0;i<param->nbMots;i++)
					{
						//memset(monMD5, 0, sizeof(monMD5));
						//memset(word, 0, sizeof (word));
						//	printf("Hash de %s\n", param->wordList[i]);
						param->hashList[i] = (unsigned char*)malloc(MD5_DIGEST_LENGTH * sizeof(unsigned char));
						memset(param->hashList[i], 0, sizeof(unsigned char) * MD5_DIGEST_LENGTH);
						strcpy(word, param->wordList[i]);
						MD5(param->wordList[i], strlen(param->wordList[i]),param->hashList[i]);
						//strcpy(param->hashList[i], param->hashList[i]);
					}
				}
				printf("Init terminee\n");fflush(stdout);
				bruteForceMPI_maitre(param->wordList, param, nbThreads);
			}
#pragma omp section
			{
				bruteForceMPI_esclave();
			}

		};
		/*
		printf("Init terminee\n");fflush(stdout);
		//printf("Maitre : Mot a trouver : %s %s\n", param->wordList[0], param->hashList[0]);fflush(stdout);

		printf("\n");
		for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
			printf("%02x", (unsigned int) param->hashList[0][i]);
		}
		printf("\n");
*/
	}

	else
	{
		bruteForceMPI_esclave();
	}

/*
	ftime(&tav);

	if(!TIME_MODE)
		if(rank == 0)
			printf("Nb Threads MPI : %d\n", nbThreads);

	/*memset(monMD5, 0, sizeof(monMD5));
	memset(word, 0, sizeof (word));

	sprintf(word,"%s",argv[1]);
	taillePrefixe=atoi(argv[2]);
				 
	// on hash le code
	MD5(word, strlen(word),monMD5);
	//printf("%s %s\n", word, monMD5);
	if(!rank)
	{
		for (i = 0; i < MD5_DIGEST_LENGTH; i++)
		{
			printf("%d ", (int) monMD5[i]);
		}
		printf("\n");
		for (i = 0; i < MD5_DIGEST_LENGTH; i++)
		{
			printf("%d ", (int) param->hashList[0][i]);
		}
		printf("\n");
	}*/
	/*if(!rank)
	{
		printf("YOUR %d\n", (int) monMD5[0]);
		printf("MINE %d\n", (int) param->hashList[0][0]);
	}*/
	//sleep(1000);

/*
	if(rank == 0)
	{
		printf("Maitre : debut\n");fflush(stdout);
		if (bruteForceMPI_maitre(param->wordList, param, nbThreads))
		{
			if(!TIME_MODE)
			{
				printf("Gagne : %s\n", word);
				for (i = 0; i < 16; i++) {
					printf("%02x", (unsigned int) monMD5[i]);
				}
				printf("\n");
			}
		}
		else
			sprintf(word, "NON_TROUVE");
	}
	*/


	/*MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0)
	{
		ftime(&tap);

		te = (double) ((tap.time * 1000 + tap.millitm) - (tav.time * 1000 + tav.millitm)) / 1000;
		printf("%f \t%s", te,word);
	}
*/
	MPI_Barrier(MPI_COMM_WORLD);
	destroy_mpi_struct();
	printf("%d : fini\n", rank);
	MPI_Finalize();
	return 0;
}
