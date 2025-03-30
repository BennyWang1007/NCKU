#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool first_line = true;


int op_find(int a2, int *parent)
{
    while (parent[a2] >= 0) {
        a2 = parent[a2];
    }
    return a2;
}

void op_union(int a, int b, int *parent)
{
    int height1, height2;
    while (parent[a] >= 0) {
        a = parent[a];
    }
    while (parent[b] >= 0) {
        b = parent[b];
    }
    if (a == b) {
        return;
    }
    height1 = -parent[a];
    height2 = -parent[b];
    if (height1 > height2) {
        parent[b] = a;
    } else if (height1 == height2) {
        parent[b] = a;
        parent[a] -= 1;
    } else {
        parent[a] = b;
    }

    return;
}


bool op_same(int a, int b, int *parent)
{
    int a2 = op_find(a, parent);
    int b2 = op_find(b, parent);
    return (a2 == b2);
}

void testcase()
{
    int n, ops;
    scanf("%d %d", &n, &ops);
    int *parent = (int *) malloc(n * sizeof(int));
    for (int i = 0; i < n; ++i) {
        parent[i] = -1;
    }

    char op[10];
    int a, b;

    for (int i = 0; i < ops; ++i) {
        scanf("%s", op);
        if (strcmp(op, "union") == 0) {
            scanf("%d %d", &a, &b);
            op_union(a, b, parent);
        } else if (strcmp(op, "find") == 0) {
            scanf("%d", &a);
            if (!first_line)
                printf("\n");
            else
                first_line = false;
            printf("%d", op_find(a, parent));
        } else if (strcmp(op, "same") == 0) {
            scanf("%d %d", &a, &b);
            if (!first_line)
                printf("\n");
            else
                first_line = false;
            printf("%s", op_same(a, b, parent) ? "true" : "false");
        }
    }
    free(parent);
}

int main()
{
    int test_case_count;
    scanf("%d", &test_case_count);
    for (int t = 0; t < test_case_count; ++t) {
        testcase();
    }

    return 0;
}
