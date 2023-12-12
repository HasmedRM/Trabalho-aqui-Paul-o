#include "ff.h"

int main(int argc, char *argv[]) {
    clock_t begin, end;
    double tempo_total = 0;
    begin = clock();

    // Verifica o número correto de argumentos
    if (argc != 3) {
        fprintf(stderr, "Formato: %s <Nivel de Filtragem> <Nivel de Quantização>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Nível de Filtragem e Quantização
    int niveis = atoi(argv[1]);
    int quantizacao = atoi(argv[2]);

    // Abre o diretório
    DIR *d;
    struct dirent *dir;
    d = opendir(DIRETORIO_IMAGENS);

    char nomeSaida[256];
    snprintf(nomeSaida, sizeof(nomeSaida), "%s/SCM-Qtz_%d_%d_%s", "./img_out", niveis, quantizacao, ".txt");
    geraCSV(quantizacao, nomeSaida);
    printf("%d",d);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            
            // Ignorar diretórios "." e ".."
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
                continue;
            }

            printf("Processando arquivo: %s\n", dir->d_name);

            // Constrói o caminho completo do arquivo
            char caminhoCompleto[256];
            snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s/%s", DIRETORIO_IMAGENS, dir->d_name);

            // Leitura da Imagem - PGM
            struct pgm img;
            readPGMImage(&img, caminhoCompleto);

            // Copia os dados da struct img em uma nova struct que vai ser filtrada
            struct pgm imgF = img;
            filterMatriz(&imgF, niveis);

            // Quantiza a imagem normal e filtrada
            quantizeImage(&img, quantizacao);
            quantizeImage(&imgF, quantizacao);

            // Exibe as informações da imagem
            viewPGMImage(&img);

            // Grava a imagem filtrada
            int **scm = (int **)malloc(niveis * sizeof(int *));
            for (int i = 0; i < niveis; i++) {
                scm[i] = (int *)malloc(niveis * sizeof(int));
                printf("%d",scm[i]);
            }
            
            criaCoocorrencias(&img, &imgF, scm, niveis);
            
            int **scm_copy = (int **)malloc(niveis * sizeof(int *));
            for (int i = 0; i < niveis; i++) {
                scm_copy[i] = (int *)malloc(niveis * sizeof(int));
                
            }
            
            writeSCMImage(scm_copy, nomeSaida, niveis);
            

            // Libera a memória alocada para a imagem
            //free(img.pData);
            //free(imgF.pData);
            //for (int i = 0; i < niveis; i++) {
            //    free(scm[i]);
            //    free(scm_copy[i]);
           // }
            //free(scm);
            //free(scm_copy);
        }
        closedir(d);
    } else {
        perror("Erro ao abrir o diretório");
        exit(EXIT_FAILURE);
    }

    return 0;
}