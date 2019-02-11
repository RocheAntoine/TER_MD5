#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <openssl/md5.h>
#include "bf.h"
#include <omp.h>

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
	ftime(&tav);

	if (bruteForceOMP(taillePrefixe, strlen(word), word, monMD5, &nbThreads))
	{
		printf("Gagne : %s\n", word);
		for (i = 0; i < 16; i++)
		{
			printf("%02x", (unsigned int) monMD5[i]);
		}
		printf("\n");
	}


	ftime(&tap);
	te = (double) ((tap.time * 1000 + tap.millitm) - (tav.time * 1000 + tav.millitm)) / 1000;
	if(argv[3][0] == 'o')
		printf("Nb Threads : %d\tTemps d'exécution : %f\n", nbThreads, te);
	else
		printf("Temps d'exécution : %f\n", te);

	return 0;
}
