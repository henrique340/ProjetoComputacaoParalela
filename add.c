#include <stdio.h>

int main() {
    FILE *fp = fopen("sequencia.txt", "a"); // Abre para adicionar no final
    if (fp == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    for (int i = 20001; i <= 40000; i++) {
        fprintf(fp, "%d 0\n", i);
    }

    fclose(fp);
    printf("Sequência de 5001 a 10000 adicionada com sucesso ao arquivo.\n");
    return 0;
}
