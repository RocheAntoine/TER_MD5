#include "utils.h"


parametre_t* init_parametre(char* fichier)
{
	parametre_t* param = malloc(sizeof(parametre_t));
	size_t len;
	char* line = NULL;
	char* nomParam = NULL;
	ssize_t read;
	FILE* fp = fopen(fichier, "r");
	if(fp == NULL)
	{
		printf("Error : config file not found : %s\n", fichier);
		return NULL;
	}
	printf("Debut\n");fflush(stdout);
	while((read = getline(&line, &len, fp)) != -1)
	{
		if(line[0] != '#')
		{
			//printf("No comment\n");fflush(stdout);
			nomParam = strsep(&line, " ");

			if(!strcmp(nomParam, "input_type"))
			{
				printf("Input type\n");fflush(stdout);
				nomParam = strsep(&line, " ");
				if(!strncmp(nomParam, "word",4))
				{
					param->isHash=0;
				}
				else if(!strncmp(nomParam, "hash",4))
				{
					param->isHash=1;
				}
				else
				{
					fprintf(stderr,"Error : 'input_type' parameter not recognized\n");
					exit(EXIT_FAILURE);
				}
			}

			else if(!strcmp(nomParam, "is_list"))
			{
				printf("Is list\n");fflush(stdout);
				param->isList=atoi(strsep(&line, " "));
				if(param->isList != 0 && param->isList != 1)
				{
					fprintf(stderr,"Error : 'is_list' parameter must be 0 or 1\n");
					exit(EXIT_FAILURE);
				}
				if(!param->isList)
					param->nbMots = 1;
			}

			else if(!strcmp(nomParam, "input"))
			{
				printf("Input\n");fflush(stdout);
				if(!param->isList)
				{
					if(param->isHash)
					{
						param->hashList = (unsigned char**)malloc(sizeof(unsigned char*));
						param->hashList[0] = strsep(&line, "\n");
					}
					else
					{
						param->wordList = (char**)malloc(sizeof(char*));
						param->wordList[0] = strsep(&line, "\n");
					}
				}
			}

			else if(!strcmp(nomParam, "word_file"))
			{
				printf("Word file\n");fflush(stdout);
				param->fichierMots = strsep(&line, "\n");
				param->fichierMots = strsep(&param->fichierMots, " ");
			}
		}
	}
	fclose(fp);
	if(param->isList)
	{
		int nMots = 0;
		int i = 0, j;
		char tmp, tmp2;
		printf("Fichier mots : %s\n", param->fichierMots);
		fp = fopen(param->fichierMots, "r");
		line = NULL;
		nomParam = NULL;
		if(fp == NULL)
		{
			printf("Error : word list not found\n");
			exit(EXIT_FAILURE);
		}
		while((read = getline(&line, &len, fp)) != -1)
		{
			nMots++;
			//printf("%s", line);
		}
		param->nbMots = nMots;
		if(!param->isHash)
		{
			param->wordList = (char**) malloc(nMots * sizeof(char*));
		}
		else
		{
			param->hashList = (unsigned char**) malloc(nMots * sizeof(unsigned char*));
			param->brutHashList = (char**) malloc(nMots * sizeof(char*));

		}
		line = NULL;
		fclose(fp);
		fp = fopen(param->fichierMots, "r");
		while((read = getline(&line, &len, fp)) != -1)
		{
			//printf("%s", line);
			if(!param->isHash)
			{
				param->wordList[i] = (char*)malloc(16 * sizeof(char));
				strcpy(param->wordList[i],strtok(line, "\r\n"));
				/*param->wordList[i] = strsep(&line, " ");
				if(param->wordList[i][strlen(param->wordList[i]) - 1] == '\n')
					param->wordList[i][strlen(param->wordList[i]) - 1] = '\0';
				printf("%s\n", param->wordList[i]);*/
			}
			else
			{
				param->hashList[i] = (unsigned char*)malloc(sizeof(unsigned char) * MD5_DIGEST_LENGTH);
				param->brutHashList[i] = (char*)malloc(sizeof(char) * MD5_DIGEST_LENGTH*2);
				sprintf(param->brutHashList[i],"%s",line);
			//	for(j = 0; j < MD5_DIGEST_LENGTH; j++)
			//	{
					//sprintf(param->hashList[i]+j*2,"%02X",line[j]);
					//param->hashList[i][j] = (unsigned int)line[j];
			//	}
			//	printf("\n");
			printf("%s", line);
				for(j = 0; j < MD5_DIGEST_LENGTH; j++)
				{
					//sprintf(param->hashList[i]+j*2,"%02X",line[j]);
					//param->hashList[i][j] = (line[j]<='9') ? (line[j]-'0') : (line[j]-'A'+10);
					tmp = line[j*2];
					tmp2 = line[j*2+1];

					printf("%c%c", tmp, tmp2);
					if(tmp <= '9')
						tmp = tmp - '0';
					else if (tmp <= 'z')
						tmp = tmp - 'a' + 10;
					else
						tmp = tmp - 'A' + 10;

					if(tmp2 <= '9')
						tmp2 = tmp2 - '0';
					else if (tmp2 <= 'z')
						tmp2 = tmp2 - 'a' + 10;
					else
						tmp2 = tmp2 - 'A' + 10;
					param->hashList[i][j] = tmp *16 + tmp2;
				}
				param->hashList[i][33] = '\0';
				printf("\nhash : %s\n", param->hashList[i]);
			}
			i++;
		}
	}
	return param;
}