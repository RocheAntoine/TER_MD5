#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <openssl/md5.h>
#include "bf.h"
#include <mpi.h>
#include <omp.h>
#include <unistd.h>

int main (int argc, char **argv) {
	unsigned char monMD5[MD5_DIGEST_LENGTH];
	int i = 0;
	int j = 0;
	int _a = 1;
	int rank;
	int size;
	int nbThreads;
	int gagnant = 0;
	char word[64];
	char hex[129];
	int numThread;
	struct timeb tav, tap ;
	double te;
	ftime(&tav);
	MPI_Init(&argc, &argv);


	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int taillePrefixe;
	int maitre = 0;
/*
	if(rank == 0)
	{
#pragma omp sections nowait
		{
			for (i = 0; i < 100000000; i++)
				for (j = 0; j < 1000000000; j++)
					_a += i * j;
			printf("hello\n");
		};
	}
#pragma omp parallel private(numThread)
	{
		numThread = omp_get_thread_num();
		printf("%d, %d\n", rank, numThread);
	};
*/
	if(!rank)
	{
	#pragma omp sections
		{
#pragma omp section
			{
				printf("yo\n");
				//maitre
			}
#pragma omp section
			{
	#pragma omp parallel private(numThread) num_threads(4)
				{
					numThread = omp_get_thread_num();
					printf("%d, %d\n", rank, numThread);
				}
			}
		}
	}
	else
	{
#pragma omp parallel private(numThread)
		{
			numThread = omp_get_thread_num();
			printf("%d, %d\n", rank, numThread);
		}
	}
	





	// pour le temps
/*

	memset(monMD5, 0, sizeof(monMD5));
	memset(word, 0, sizeof (word));

	if (argc != 3)
	{
		fprintf (stderr, "usage: %s string-to-hash taille-du-prefixe o/s\n", argv[0]);
		exit(1);
	}
	sprintf(word,"%s",argv[1]);
	taillePrefixe=atoi(argv[2]);
				 printf("Coucou connard\n");
	// on hash le code
	MD5(word, strlen(word),monMD5);
	if(!TIME_MODE)
		printf("Début_bruteforce\n");
	if (bruteForceOMP(taillePrefixe, word, monMD5, &nbThreads))
	{
		gagnant = 1;
		if(!TIME_MODE)
		{
			printf("Gagne : %s\n", word);
			for (i = 0; i < 16; i++) {
				printf("%02x", (unsigned int) monMD5[i]);
			}
			printf("\n");
		}
	}
	if(!TIME_MODE)
		printf("Fin_bruteforce\n");
	MPI_Barrier(MPI_COMM_WORLD);

	if(gagnant)
	{
		ftime(&tap);
		te = (double) ((tap.time * 1000 + tap.millitm) - (tav.time * 1000 + tav.millitm)) / 1000;
		printf("%lf \t%s", te,word);
	}
	MPI_Barrier(MPI_COMM_WORLD);
 */
	MPI_Finalize();
	return 0;
}
