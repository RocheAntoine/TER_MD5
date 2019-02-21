#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <openssl/md5.h>
#include "bf.h"
#include <mpi.h>
#include <omp.h>

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
	struct timeb tav, tap ;
	double te;
	ftime(&tav);
	MPI_Init(&argc, &argv);


	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int taillePrefixe;
	// pour le temps


	memset(monMD5, 0, sizeof(monMD5));
	memset(word, 0, sizeof (word));

	if (argc != 3)
	{
		fprintf (stderr, "usage: %s string-to-hash taille-du-prefixe o/s\n", argv[0]);
		exit(1);
	}
	sprintf(word,"%s",argv[1]);
	taillePrefixe=atoi(argv[2]);
				 
	// on hash le code
	MD5(word, strlen(word),monMD5);
	if(!TIME_MODE)
		printf("Début_bruteforce\n");
	if (bruteForceOMP(taillePrefixe, word, monMD5, &nbThreads))
	{
		gagnant = 1;
		if(!TIME_MODE)
		{
			printf("Gagne : %s\n", word)
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
	MPI_Finalize();
	return 0;
}
