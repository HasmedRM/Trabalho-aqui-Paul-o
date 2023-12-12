#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#define DIRETORIO_IMAGENS "images/"

struct pgm{
	int tipo;
	int c;
	int r;
	int mv;
	unsigned char *pData;
};

void geraCSV(unsigned int, char *);
void preencheCSV(int  **, unsigned int, char *, unsigned int);
void readPGMImage(struct pgm *, char *);
void writeSCMImage(int **, char *,int);
void quantizeImage(struct pgm *, int);
int valorVizinho(struct pgm *, int, int, int, int);
void aplicaFiltro(int *,int, int, int,unsigned char *);
void filtraMatriz(struct pgm *, int, unsigned char *);
void criaCoocorrencias(struct pgm *, struct pgm *,int **,int);
void filterMatriz(struct pgm *img, int numfilter);
void viewPGMImage(struct pgm *pio);
