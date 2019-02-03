#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <openssl/md5.h>
#include <mpi.h>
#include "bf.h"

int main (int argc, char **argv) {
	unsigned char monMD5[MD5_DIGEST_LENGTH];
	int i = 0;
	int j = 0;
	int _a = 1;
	char word[64];
	char hex[129]; // note : 128 ne passe pas... ptet a cause du caractere de fin dans l'argv.. i don't know

	int taillePrefixe;
	// pour le temps
	struct timeb tav, tap ;
	double te;
	int nbThreads;
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nbThreads);
	if(!TIME_MODE)
		if(rank == 0)
			printf("Nb Threads MPI : %d\n", nbThreads);

	memset(monMD5, 0, sizeof(monMD5));
	memset(word, 0, sizeof (word));

	if (argc != 3)
	{
		if(rank == 0)
			fprintf (stderr, "usage: %s string-to-hash taille-du-prefixe\n", argv[0]);
		MPI_Finalize();
		exit (1);
	}
	sprintf(word,"%s",argv[1]);
	taillePrefixe=atoi(argv[2]);
				 
	// on hash le code
	MD5(word, strlen(word),monMD5);
	ftime(&tav);

	if(nbThreads == 1)
	{
		printf("Erreur : Impossible d'utiliser qu'un thread avec MPI\n");
		MPI_Finalize();
		exit(1);
	}
	if(rank == 0)
	{
		if (bruteForceMPI_maitre(taillePrefixe, strlen(word), word, monMD5, nbThreads))
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
	}
	else
	{
		bruteForceMPI_esclave(taillePrefixe, strlen(word), monMD5);
	}


	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0)
	{
		ftime(&tap);
		te = (double) ((tap.time * 1000 + tap.millitm) - (tav.time * 1000 + tav.millitm)) / 1000;
		printf("%f \t%s", te,word);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
