#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK 1

/* Decodeaza litera <l> cu cheia <key> */
char decodeLetter(char l, int key);

/* Returneaza o copie a cuvantului <word> ce contine numai minuscule */
char* toLower(char *word);

/* Citeste caracter cu caracter din <stdin> si decodeaza literele cu
 * functia <decode> si cheia <key> date ca parametru */
char* readLine(char (*decode) (char, int), int key);

int main(void)
{
	char *text;				// Sir de caractere in care este retinut textul original
	char **prefixes;		// Vector de siruri de caractere in care se retin prefixele
	char *separators;		// Sir de caractere in care se retin semnele de punctuatie
	int key;				// Cheia de decodare
	int prefixesNr;			// Numarul de prefixe

	// Setare separatori de cuvinte(semnele de punctuatie)
	separators = " ,.;!?-:";

	/* =============== Citire & Subpunct A ================ */

	char *prefixesTmp; 		// Retine temporar sirul prefixelor

	// Citire cheie
	scanf("%d", &key);
	getc(stdin);

	// Citire text si decodare text + citire prefixe
    text = readLine(decodeLetter, key);
 	prefixesTmp = readLine(NULL, 0);

 	/* ==================================================== */


	/* ============ Trecere prefixe in matrice ============ */

	// Variabile auxiliare
	char *p;
	int i;

	// Calculeaza numarul de prefixe
	prefixesNr = 0;
	p = strchr(prefixesTmp, ' ');
	while (p)
	{
		prefixesNr++;
		p = strchr(p + 1, ' ');
	}
	prefixesNr++;

	// Aloca vectorul de siruri de caractere
	prefixes = (char**) calloc(prefixesNr, sizeof(char*));

	// Imparte sirul prefixelor in functie de spatii si pune prefixele
	// in vectorul de prefixe
	i = 0;
	p = strtok(prefixesTmp, " ");
	while (p)
	{
		prefixes[i] = (char*) calloc(strlen(p) + 1, sizeof(char));
		strcpy(prefixes[i], p);
		p = strtok(NULL, " ");
		i++;
	}

	// Curata memoria ocupata de sirul temporar de prefixe
	free(prefixesTmp);
	prefixesTmp = NULL;

	/* ==================================================== */

	/* =============== Subpunct A - Afisare =============== */

	printf("%s\n", text);

	/* ==================================================== */

	/* ==================== Subpunct B ==================== */

	int j, wordLength, prefixLength, textLength, wordCount;
	int hasPrefix, hasValidLength, isLongestPrefix;
	char *wordLow, *word;

	char *newText;			// Retine textul rezultat dupa eliminarea prefixelor
	char **newWords;		// Vector ce retine cuvintele din text dupa eliminarea prefixelor
	int newWordsSize;		// Dimensiunea vectorului de cuvinte

	// Alocarea dinamica a memoriei
	newWords = (char**) calloc(1, sizeof(char*));
	newText = (char*) calloc(strlen(text) + 1, sizeof(char));
	word = (char*) calloc(1, sizeof(char));
	wordLow = NULL;

	// Initializarea variabilelor
	wordCount = 0;
	newWordsSize = 0;
	textLength = strlen(text);
	wordLength = 0;
	prefixLength = 0;

	// Parcurge textul caracter cu caracter
	for (i = 0; i <= textLength; i++)
	{
		// Daca este litera atunci creste lungimea cuvantului
		if (strchr(separators, text[i]) == NULL)
		{
			wordLength++;
		}
		else
		{
			// Caracterul este separator
			// Verifica daca exista separatori consecutivi
			if (wordLength > 0)
			{
				// Realoca memorie daca lungimea cuvantului depaseste memoria alocata
				if (wordLength > strlen(word))
				{
					word = realloc(word, wordLength + 1);
				}

				// Curata memoria alocata pentru cuvant
				memset(word, 0, wordLength + 1);

				// Copiaza cuvantul gasit in variabila <word>
				strncpy(word, text + i - wordLength, wordLength);

				// Cauta prefixul care sa corespunda cerintelor
				wordLow = toLower(word);
				prefixLength = 0;
				for (j = 0; j < prefixesNr; j++)
				{
					hasPrefix = strncmp(wordLow, prefixes[j], strlen(prefixes[j])) == 0;
					hasValidLength = strlen(wordLow) - strlen(prefixes[j]) > 1;
					isLongestPrefix = strlen(prefixes[j]) > prefixLength;
					if (hasPrefix && hasValidLength && isLongestPrefix)
					{
						prefixLength = strlen(prefixes[j]);
					}
				}
				free(wordLow);

				// Copiaza cuvantul fara prefix
				strcpy(newText + strlen(newText), word + prefixLength);

				// Realoca memorie in vectorul de cuvinte daca numarul de cuvinte
				// depaseste dimensiunea alocata
				if (wordCount >= newWordsSize)
				{
					newWordsSize += CHUNK;
					newWords = (char**) realloc(newWords, sizeof(char*) * newWordsSize);
				}

				// Aloca memorie pentru un nou cuvant
				newWords[wordCount] = (char*) calloc(strlen(word + prefixLength) + 1, sizeof(char));

				// Introduce cuvantul fara prefix in vectorul de cuvinte
				strcpy(newWords[wordCount], word + prefixLength);
				wordCount++;
			}

			// Adauga caracterul in textul nou si reseteaza lungimea cuvantului
			newText[strlen(newText)] = text[i];
			wordLength = 0;
		}
	}
	newText[strlen(newText)] = 0;

	// Afiseaza textul obtinut dupa eliminarea prefixelor
	printf("%s\n", newText);

	// Curata memoria ocupata de prefixe, de textul initial si de cuvant
	for (i = 0; i < prefixesNr; i++)
	{
		free(prefixes[i]);
	}
	free(prefixes);
	prefixes = NULL;

	free(text);
	text = NULL;

	free(word);
	word = NULL;

	/* ==================================================== */

	/* ==================== Subpunct C ==================== */
	char *frequentWord, *wordA, *wordB;
	int nr, nrMax;

	nr = 0;
	nrMax = 0;

	frequentWord = (char*) calloc(1, sizeof(char));
	frequentWord[0] = 0;

	// Parcurge vectorul de cuvinte
	for (i = 0; i < wordCount; i++)
	{
		nr = 0;
		wordA = toLower(newWords[i]);

		// Calculeaza numarul de aparitii ale fiecarui cuvant
		for (j = 0; j < wordCount; j++)
		{
			wordB = toLower(newWords[j]);
			if (strcmp(wordA, wordB) == 0)
			{
				nr++;
			}
			free(wordB);
		}

		// Actualizeaza cuvantul daca numarul de aparitii este mai mare
		if (nr > nrMax)
		{
			nrMax = nr;
			if (strlen(frequentWord) < strlen(wordA))
			{
				frequentWord = realloc(frequentWord, 1 + strlen(wordA));
			}
			strcpy(frequentWord, wordA);
		}
		else if (nr == nrMax && strcmp(wordA, frequentWord) < 0)
		{
			// Daca numarul de aparitii este egal cu numarul maxim atunci
			// compara cuvintele
			if (strlen(frequentWord) < strlen(wordA))
			{
				frequentWord = realloc(frequentWord, 1 + strlen(wordA));
			}
			strcpy(frequentWord, wordA);
		}

		free(wordA);
	}

	// Afisare
	printf("%s %d\n", frequentWord, nrMax);

	// Curata memoria
	for (i = 0; i < wordCount; i++)
	{
		free(newWords[i]);
	}
	free(newWords);
	newWords = NULL;

	free(newText);
	newText = NULL;

	free(frequentWord);
	frequentWord = NULL;

	/* ==================================================== */

	return 0;
}

char decodeLetter(char l, int key)
{
	char x = l;
	int diff = 0;

	key = key % 26;

	if (l >= 'A' && l <= 'Z')
	{
		x = l - key;
		diff = x - 'A';
		if (diff < 0)
		{
			x = 'Z' + diff + 1;
		}
	}
	else if (l >= 'a' && l <= 'z')
	{
		x = l - key;
		diff = x - 'a';
		if (diff < 0)
		{
			x = 'z' + diff + 1;
		}
	}

	return x;
}

char* toLower(char *word)
{
	int i, len;

	char *low = (char*) calloc(strlen(word) + 1, sizeof(char));
	strcpy(low, word);

	len = strlen(low);
	for (i = 0; i < len; i++)
	{
		if (low[i] >= 'A' && low[i] <= 'Z')
		{
			low[i] = low[i] + ('a' - 'A');
		}
	}

	return low;
}

char* readLine(char (*decode) (char, int), int key)
{
	char *line = NULL, *tmp = NULL;
    size_t size = 0, index = 0;
    int ch = EOF;

    while (ch)
    {
        ch = getc(stdin);

        /* Check if we need to stop */
        if (ch == EOF || ch == '\n')
    	{
            ch = 0;
    	}

        /* Check if we need to expand */
        if (size <= index)
        {
            size += CHUNK;
            tmp = realloc(line, size);
            if (!tmp)
            {
                free(line);
                line = NULL;
                break;
            }
            line = tmp;
        }

        /* Actually store the thing */
        line[index++] = decode ? (*decode)(ch, key) : ch;
    }

    return line;
}