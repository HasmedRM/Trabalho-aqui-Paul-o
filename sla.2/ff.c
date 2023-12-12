#include "ff.h"

void geraCSV(unsigned int tam, char *filename) {
    printf("Chamando geraCSV com tam=%u, filename=%s\n", tam, filename);
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        puts("ERRO AO ABRIR O ARQUIVO");
        exit(EXIT_FAILURE);  // Remova o "return 1;"
    }

    for (int i = 1; i <= tam * tam; i++) {
        fprintf(fp, "%u, ", i);
    }
    fprintf(fp, "%u \n", tam * tam + 1);
    fclose(fp);
}

void preencheCSV(int **scm, unsigned int tam, char *filename, unsigned int rotulo) {
    FILE *fp = fopen(filename, "a");
    if (!fp) {
        puts("ERRO AO ABRIR O ARQUIVO");
        exit(EXIT_FAILURE);  // Remova o "return 1;"
    }

    for (int i = 0; i < tam * tam; i++) {
        fprintf(fp, "%d", *(scm + i));
    }
    fprintf(fp, "%s\n", rotulo ? "stroma" : "epithelium");
    fclose(fp);
}

void readPGMImage(struct pgm *pio, char *filename){

	FILE *fp;
	char ch;
	FILE *fo;

	fo = fopen("ReadPGM.txt","a+");

	if (!(fp = fopen(filename,"r"))){
		perror("Erro.");
		exit(1);
	}

	if ( (ch = getc(fp))!='P'){
		puts("A imagem fornecida não está no formato pgm");
		exit(2);
	}
	
	pio->tipo = getc(fp)-48;
	
	fseek(fp,1, SEEK_CUR);

	while((ch=getc(fp))=='#'){
		while( (ch=getc(fp))!='\n');
	}

	fseek(fp,-1, SEEK_CUR);

	fscanf(fp, "%d %d",&pio->c,&pio->r);
	if (ferror(fp)){ 
		perror(NULL);
		exit(3);
	}	
	fscanf(fp, "%d",&pio->mv);
	fseek(fp,1, SEEK_CUR);

	pio->pData = (unsigned char*) malloc(pio->r * pio->c * sizeof(unsigned char));

	switch(pio->tipo){
		case 2:
			puts("Lendo imagem PGM (dados em texto)");
			for (int k=0; k < (pio->r * pio->c); k++){
				fscanf(fp, "%hhu", pio->pData+k);
			}
		break;	
		case 5:
			puts("Lendo imagem PGM (dados em binário)");
			fread(pio->pData,sizeof(unsigned char),pio->r * pio->c, fp);
		break;
		default:
			puts("Não está implementado");
	}
	fclose(fp);
	for(int i = 0;i < pio->c*pio->r;i++){
		fprintf(fo, "%hhu ", pio->pData[i]);
	}
	fclose(fo);
}

void writeSCMImage(int **scm, char *filename, int niveis) {
    printf("Chamando writeSCMImage com niveis=%d, filename=%s\n", niveis, filename);

    FILE *fp;

    if (!(fp = fopen(filename, "a+"))) {
        perror("Erro ao abrir o arquivo para escrita");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < niveis; i++) {
        for (int j = 0; j < niveis; j++) {
            fprintf(fp, "%d ", scm[i][j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

void viewPGMImage(struct pgm *pio) {
    printf("Tipo: %d\n", pio->tipo);
    printf("Dimensoes: [%d %d]\n", (pio->c), pio->r);
    printf("Max: %d\n", pio->mv);

}

void quantizeImage(struct pgm *pio, int levels) {

    if (pio == NULL || pio->pData == NULL) {
        fprintf(stderr, "Erro: A imagem não está inicializada corretamente.\n");
        exit(EXIT_FAILURE);
    }

    if (levels != 8  && levels != 16) {
        fprintf(stderr, "Erro: O número de níveis deve ser 8 ou 16.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < pio->r * pio->c; i++) {
        // Calcula o novo valor quantizado
        int quantizedValue = (pio->pData[i] * levels) / 255;

        // Atualiza o valor na imagem
        pio->pData[i] = (unsigned char)quantizedValue;
    }
}


int valorVizinho(struct pgm *pio, int linhas, int colunas, int linha, int coluna) {
    return (linha >= 0 && linha < linhas && coluna >= 0 && coluna < colunas) ? pio->pData[linha * colunas + coluna] : 0;
}

void filterMatriz(struct pgm *img, int numfilter) {
    if (numfilter % 2 == 0) {
        fprintf(stderr, "Erro: O número de filtro deve ser ímpar para garantir um pixel central.\n");
        exit(EXIT_FAILURE);
    }

    int sum = 0;
    int average = 0;
    unsigned char *filteredData = malloc(img->r * img->c * sizeof(unsigned char));

    if (filteredData == NULL) {
        // Tratar erro de alocação de memória, se necessário
        exit(1);
    }

    for (int i = 0; i < img->r; i++) {
        for (int j = 0; j < img->c; j++) {
            // Aplicar o filtro apropriado, dependendo do valor de numfilter
            // Por exemplo, se numfilter for 1, pode ser uma média simples dos pixels ao redor
            // Se numfilter for 2, pode ser outro tipo de filtro, etc.

            // Aqui, por exemplo, apenas uma média simples dos pixels vizinhos
            sum = 0;
            for (int x = i - numfilter/2; x <= i + numfilter/2; x++) {
                for (int y = j - numfilter/2; y <= j + numfilter/2; y++) {
                    if (x >= 0 && x < img->r && y >= 0 && y < img->c) {
                        sum += img->pData[x * img->c + y];
                    }
                }
            }
            average = sum / (numfilter * numfilter);// Para um filtro 3x3, onde 9 é o número total de pixels na vizinhança

            // Armazenar o resultado na matriz filtrada
            filteredData[i * img->c + j] = average;
        }
    }

    // Atualizar os dados da imagem com os dados filtrados
    free(img->pData);
    img->pData = filteredData;
}





void criaCoocorrencias(struct pgm *pio, struct pgm *pio2, int **scm, int niveis) {
    // Alocar memória para a matriz scm
    for (int i = 0; i < niveis; i++) {
        
        scm[i] = (int *)malloc(niveis * sizeof(int));
        
        if (scm[i] == NULL) {
            // Tratar erro de alocação de memória, se necessário
            exit(1);
        }
        
        // Inicializar a matriz com zeros
        for (int j = 0; j < niveis; j++) {
            scm[i][j] = 0;
        }
        
    }

   for (int i = 0; i < pio->r; i++) {
    if (pio->pData[i] < 0 || pio->pData[i] >= niveis) {
        continue;
    }
    for (int j = 0; j < pio2->c; j++) {
        if (pio2->pData[j] < 0 || pio2->pData[j] >= niveis) {
            continue;
        }

        scm[pio->pData[i]][pio2->pData[j]]++;
    }
}
}
