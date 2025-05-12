#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_CHILDREN 2
#define MAX_THREADS 3  // Limitando o número de threads

typedef struct {
    double value;
    int num_children;
    int* children;  // Alocação dinâmica para o número variável de filhos
} Node;

typedef struct {
    double sum;
    int* path;
    int pathLength;
} Result;

typedef struct {
    Node* tree;
    int idx;
    Result* result;
} ThreadArgs;

Result computePaths(Node* tree, int idx); // Forward declaration

void* threadFunc(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    *(args->result) = computePaths(args->tree, args->idx);
    return NULL;
}

Result computePaths(Node* tree, int idx) {
    Result res = {0, NULL, 0};

    if (idx == -1) return res;

    // Se for nó folha
    if (tree[idx].num_children == 0) {
        res.sum = tree[idx].value;
        res.path = (int*)malloc(sizeof(int));
        if (res.path == NULL) {
            fprintf(stderr, "Falha na alocação de memória para path.\n");
            exit(1);
        }
        res.path[0] = idx;
        res.pathLength = 1;
        return res;
    }

    double maxSum = -1;
    Result maxChildRes = {0};
    maxChildRes.path = NULL;

    pthread_t threads[MAX_THREADS];
    ThreadArgs args[MAX_THREADS];
    int numThreads = 0;

    // Limita a criação de threads com base no número de filhos
    for (int i = 0; i < tree[idx].num_children && numThreads < MAX_THREADS; ++i) {
        args[numThreads].tree = tree;
        args[numThreads].idx = tree[idx].children[i];
        args[numThreads].result = (Result*)malloc(sizeof(Result));  // Aloca memória para o resultado da thread
        if (args[numThreads].result == NULL) {
            fprintf(stderr, "Falha na alocação de memória para result.\n");
            exit(1);
        }
        pthread_create(&threads[numThreads], NULL, threadFunc, &args[numThreads]);
        numThreads++;
    }

    // Espera o término e coleta os resultados
    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
        if (args[i].result->sum > maxSum) {
            maxSum = args[i].result->sum;
            maxChildRes = *(args[i].result);  // Armazena o melhor resultado
        }
        free(args[i].result);  // Libera a memória alocada para o resultado da thread
    }

    // Processa os filhos restantes sequencialmente
    for (int i = numThreads; i < tree[idx].num_children; ++i) {
        Result childRes = computePaths(tree, tree[idx].children[i]);
        if (childRes.sum > maxSum) {
            maxSum = childRes.sum;
            maxChildRes = childRes;
        }
    }

    // Calcula o resultado final
    res.sum = tree[idx].value + maxSum;
    res.path = (int*)malloc((maxChildRes.pathLength + 1) * sizeof(int));
    if (res.path == NULL) {
        fprintf(stderr, "Falha na alocação de memória para path final.\n");
        exit(1);
    }
    res.path[0] = idx;
    for (int i = 0; i < maxChildRes.pathLength; ++i) {
        res.path[i + 1] = maxChildRes.path[i];
    }
    res.pathLength = maxChildRes.pathLength + 1;

    return res;
}

int main() {
    int N;
    scanf("%d", &N);

    Node* tree = (Node*)malloc(N * sizeof(Node));
    for (int i = 0; i < N; ++i) {
        scanf("%lf %d", &tree[i].value, &tree[i].num_children);
        tree[i].children = (int*)malloc(tree[i].num_children * sizeof(int));  // Aloca memória para filhos
        for (int j = 0; j < tree[i].num_children; ++j) {
            scanf("%d", &tree[i].children[j]);
            tree[i].children[j]--; // Ajusta para índice 0-based
        }
    }

    Result finalRes = computePaths(tree, 0);
    printf("Max Sum: %.2lf\n", finalRes.sum);
    printf("Path: ");
    for (int i = 0; i < finalRes.pathLength; ++i) {
        printf("%d ", finalRes.path[i] + 1); // Ajusta de volta para índice 1-based
    }
    printf("\n");

    // Libera memória
    for (int i = 0; i < N; ++i) {
        free(tree[i].children);
    }
    free(tree);
    free(finalRes.path);

    return 0;
}
