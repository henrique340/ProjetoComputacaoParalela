#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_CHILDREN 2
#define MAX_VALUE 100
#define MAX_NODES 10000

typedef struct {
    double value;
    int num_children;
    int children[MAX_CHILDREN];
} Node;

typedef struct {
    double sum;
    int path[MAX_NODES];
    int pathLength;
} Result;

typedef struct {
    Node* tree;
    int idx;
    Result result;
} ThreadArgs;

Result computePaths(Node* tree, int idx); // Forward declaration

void* threadFunc(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    args->result = computePaths(args->tree, args->idx);
    return NULL;
}

Result computePaths(Node* tree, int idx) {
    Result res = {0, {0}, 0};

    if (idx == -1) return res;

    if (tree[idx].num_children == 0) {
        res.sum = tree[idx].value;
        res.path[res.pathLength++] = idx;
        return res;
    }

    double maxSum = -1;
    Result maxChildRes = {0};

    pthread_t threads[MAX_CHILDREN];
    ThreadArgs args[MAX_CHILDREN];

    // Criação das threads
    for (int i = 0; i < tree[idx].num_children; ++i) {
        args[i].tree = tree;
        args[i].idx = tree[idx].children[i];
        pthread_create(&threads[i], NULL, threadFunc, &args[i]);
    }

    // Espera o término e coleta os resultados
    for (int i = 0; i < tree[idx].num_children; ++i) {
        pthread_join(threads[i], NULL);
        if (args[i].result.sum > maxSum) {
            maxSum = args[i].result.sum;
            maxChildRes = args[i].result;
        }
    }

    res.sum = tree[idx].value + maxSum;
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

    free(tree);
    return 0;
}
