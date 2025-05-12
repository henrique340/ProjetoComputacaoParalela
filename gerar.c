#include <stdio.h>

int main() {
    FILE *fp = fopen("sequencia.txt", "w");
    if (fp == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    for (int i = 1; i <= 50000; i++) {
        int y = i * 2;
        int z = y + 1;
        fprintf(fp, "%d 2 %d %d\n", i, y, z);
    }

    fclose(fp);
    printf("Arquivo 'sequencia.txt' gerado com sucesso.\n");
    return 0;
}
