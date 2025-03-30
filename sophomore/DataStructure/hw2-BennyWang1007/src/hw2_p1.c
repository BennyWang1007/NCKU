#pragma GCC diagnostic ignored "-Wunused-result"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define DEBUG

typedef struct FHeapNode {
    int key;
    int value;
    int degree;
    bool childCut;
    struct FHeapNode *left;
    struct FHeapNode *right;
    struct FHeapNode *child;
    struct FHeapNode *parent;
} FHeapNode;

typedef struct MinFHeap {
    FHeapNode *min;
    int size;
} MinFHeap;


FHeapNode *FHeapNode_new(int key, int value);
void FHeap_insert(MinFHeap *heap, FHeapNode *node);
bool FHeap_extractMin(MinFHeap *heap, int *key, int *value);
void FHeap_delete(MinFHeap *heap, int key, int value);
void FHeap_delete_node(MinFHeap *heap, FHeapNode *node);
void FHeap_decreaseKey(MinFHeap *heap, int key, int value, int delta);
void FHeap_cut(MinFHeap *heap, FHeapNode *node, FHeapNode *parent);
void FHeap_cutOut(MinFHeap *heap, FHeapNode *node, FHeapNode *parent);
void FHeap_cascadingCut(MinFHeap *heap, FHeapNode *node);
void FHeap_addChildrenToTop(MinFHeap *heap, FHeapNode *node);
void FHeapConsolidate(MinFHeap *heap);
int FHeapGetTopSize(MinFHeap *heap, int *max_degree);
FHeapNode *FHeap_find(MinFHeap *heap, int key, int value);
void FHeapMeld(FHeapNode *node1, FHeapNode *node2);
void FHeap_insert_right(FHeapNode *node1, FHeapNode *node2);
void print_fheap(FHeapNode *root, int layer);

// insert node2 to the right of node1, don't care node2's left and right
void FHeap_insert_right(FHeapNode *node1, FHeapNode *node2)
{
    node2->left = node1;
    node2->right = node1->right;
    node1->right->left = node2;
    node1->right = node2;
}

typedef struct HashNode {
    FHeapNode *FNode;
    struct HashNode *next;
} HashNode;


typedef struct HashTable {
    int size;
    int capacity;
    HashNode **table;
} HashTable;


int hash(int key, int capacity)
{
    return key % capacity;
}

FHeapNode *findFHeapNode(int key, int value, HashTable *hashTable)
{
    int index = hash(key, hashTable->capacity);
    HashNode *node = hashTable->table[index];
    while (node != NULL) {
        FHeapNode *FNode = node->FNode;
        if (FNode->key == key && FNode->value == value) {
            return FNode;
        }
        node = node->next;
    }
    return NULL;
}

void initHashTable(HashTable *hashTable, int capacity)
{
    hashTable->size = 0;
    hashTable->capacity = capacity;
    hashTable->table = (HashNode **) malloc(sizeof(HashNode *) * capacity);
    for (int i = 0; i < capacity; i++) {
        hashTable->table[i] = NULL;
    }
}

void insertHashTable(HashTable *hashTable, FHeapNode *FNode)
{
    int index = hash(FNode->key, hashTable->capacity);
    HashNode *node = (HashNode *) malloc(sizeof(HashNode));
    node->FNode = FNode;
    node->next = hashTable->table[index];
    hashTable->table[index] = node;
    hashTable->size++;
}

void removeHashTable(HashTable *hashTable, FHeapNode *FNode)
{
    int index = hash(FNode->key, hashTable->capacity);
    HashNode *node = hashTable->table[index];
    HashNode *prev = NULL;
    while (node != NULL) {
        if (node->FNode == FNode) {
            if (prev == NULL) {
                hashTable->table[index] = node->next;
            } else {
                prev->next = node->next;
            }
            hashTable->size--;
            free(node);
            return;
        }
        prev = node;
        node = node->next;
    }
}

int FHeapNode_compare(FHeapNode *node1, FHeapNode *node2)
{
    if (node1->key < node2->key)
        return -1;
    if (node1->key > node2->key)
        return 1;

    if (node1->value < node2->value)
        return -1;
    if (node1->value > node2->value)
        return 1;

    return 0;
}

FHeapNode *FHeapNode_new(int key, int value)
{
    FHeapNode *node = (FHeapNode *) malloc(sizeof(FHeapNode));
    node->key = key;
    node->value = value;
    node->left = node;
    node->right = node;
    node->child = NULL;
    node->parent = NULL;
    node->childCut = false;
    node->degree = 0;
    return node;
}

MinFHeap *MinFHeap_new()
{
    MinFHeap *heap = (MinFHeap *) malloc(sizeof(MinFHeap));
    heap->min = NULL;
    heap->size = 0;
    return heap;
}

void FHeap_insert(MinFHeap *heap, FHeapNode *node)
{
    if (heap->min == NULL) {
        heap->min = node;
    } else {
        FHeapNode *min = heap->min;
        FHeap_insert_right(min, node);
        if (FHeapNode_compare(node, heap->min) < 0)
            heap->min = node;
    }
    heap->size++;
}

bool FHeap_extractMin(MinFHeap *heap, int *key, int *value)
{
    if (heap->min == NULL)
        return false;

    FHeapNode *min = heap->min;
    *key = min->key;
    *value = min->value;
    FHeap_addChildrenToTop(heap, min);

    FHeapNode *nextMin = min->right;
    FHeap_delete_node(heap, min);
    if (heap->size == 0) {
        heap->min = NULL;
        return true;
    }
    heap->min = nextMin;
    min = nextMin;
    nextMin = nextMin->right;
    while (nextMin != min) {
        if (FHeapNode_compare(nextMin, heap->min) < 0)
            heap->min = nextMin;
        nextMin = nextMin->right;
    }
    return true;
}

void FHeap_delete(MinFHeap *heap, int key, int value)
{
    FHeapNode *node = FHeap_find(heap, key, value);
    if (node != NULL)
        FHeap_delete_node(heap, node);
}

void FHeap_delete_node(MinFHeap *heap, FHeapNode *node)
{
    FHeap_cutOut(heap, node, node->parent);
    heap->size--;
    if (node->parent != NULL) {
        if (node->parent->childCut)
            FHeap_cascadingCut(heap, node->parent);
        else
            node->parent->childCut = true;
    }
    FHeap_addChildrenToTop(heap, node);

    free(node);
    if (heap->size == 0) {
        heap->min = NULL;
    }
    FHeapConsolidate(heap);
}

void FHeap_decreaseKey(MinFHeap *heap, int key, int value, int delta)
{
    FHeapNode *node = FHeap_find(heap, key, value);
    if (node == NULL)
        return;

    node->key -= delta;
    FHeapNode *parent = node->parent;
    if (parent != NULL && node->key < parent->key) {
        FHeap_cut(heap, node, parent);
        FHeap_cascadingCut(heap, parent);
    }
    if (FHeapNode_compare(node, heap->min) < 0) {
        heap->min = node;
    }
}

void FHeap_addChildrenToTop(MinFHeap *heap, FHeapNode *node)
{
    if (node->child == NULL)
        return;
    FHeapNode *child = node->child;
    FHeapNode *child_right = child->right;
    FHeapNode *min = heap->min;
    if (min == NULL) {
        heap->min = child;
    } else {
        FHeap_cutOut(heap, child, child->parent);
        FHeap_insert_right(min, child);
        if (FHeapNode_compare(child, heap->min) < 0)
            heap->min = child;
    }
    child->parent = NULL;
    FHeapNode *cur = child_right;
    while (cur != child) {
        FHeapNode *next = cur->right;
        FHeap_cutOut(heap, cur, cur->parent);
        FHeap_insert_right(min, cur);
        if (FHeapNode_compare(cur, heap->min) < 0)
            heap->min = cur;
        cur->parent = NULL;
        cur = next;
    }
}

// cut without to top
void FHeap_cutOut(MinFHeap *heap, FHeapNode *node, FHeapNode *parent)
{
    if (parent == NULL) {
        heap->min = node->right;
        if (node->right == node) {
            heap->min = NULL;
        } else {
            node->right->left = node->left;
            node->left->right = node->right;
        }
        node->parent = NULL;
        node->left = node;
        node->right = node;
        node->childCut = false;
        return;
    }


    if (node->right == node) {
        parent->child = NULL;
    } else {
        parent->child = node->right;
        node->right->left = node->left;
        node->left->right = node->right;
    }
    parent->childCut = true;
    parent->degree -= 1;
    node->parent = NULL;
    node->left = node;
    node->right = node;
    node->childCut = false;
}

void FHeap_cut(MinFHeap *heap, FHeapNode *node, FHeapNode *parent)
{
    FHeap_cutOut(heap, node, parent);
    heap->size--;
    FHeap_insert(heap, node);
}

void FHeap_cascadingCut(MinFHeap *heap, FHeapNode *node)
{
    FHeapNode *parent = node->parent;
    if (parent != NULL) {
        if (node->childCut == false) {
            node->childCut = true;
        } else {
            FHeap_cut(heap, node, parent);
            FHeap_cascadingCut(heap, parent);
        }
    }
}

int FHeapGetTopSize(MinFHeap *heap, int *max_degree)
{
    int top_size = 1;
    FHeapNode *node = heap->min;
    if (node == NULL) {
        return 0;
    }
    *max_degree = node->degree;
    node = node->right;
    while (node != heap->min) {
        top_size++;
        if (node->degree > *max_degree) {
            *max_degree = node->degree;
        }
        node = node->right;
    }
    return top_size;
}



void FHeapMeld(FHeapNode *node1, FHeapNode *node2)
{
    if (node1->degree != node2->degree) {
        return;
    }
    if (FHeapNode_compare(node1, node2) >= 0) {
        FHeapNode *tmp = node1;
        node1 = node2;
        node2 = tmp;
    }
    if (node1->child == NULL) {
        node1->child = node2;
        node2->left = node2;
        node2->right = node2;
    } else {
        FHeap_insert_right(node1->child, node2);
    }
    node2->parent = node1;
    node1->degree += 1;
    node2->childCut = false;
}

void FHeapConsolidate(MinFHeap *heap)
{
    if (heap->min == NULL) {
        return;
    }
    int max_degree = 18;
    int top_size = FHeapGetTopSize(heap, &max_degree);

    FHeapNode **top_nodes =
        (FHeapNode **) malloc(sizeof(FHeapNode *) * top_size);
    FHeapNode *node = heap->min;
    for (int i = 0; i < top_size; i++) {
        top_nodes[i] = node;
        node = node->right;
    }
    FHeapNode **heap_degree =
        (FHeapNode **) malloc(sizeof(FHeapNode *) * (max_degree + 1));
    for (int i = 0; i < max_degree; i++) {
        heap_degree[i] = NULL;
    }

    for (int i = 0; i < top_size; i++) {
        node = top_nodes[i];
        if (heap_degree[node->degree] == NULL) {
            heap_degree[node->degree] = node;
        } else {
            if (FHeapNode_compare(node, heap_degree[node->degree]) >= 0) {
                FHeapNode *tmp = heap_degree[node->degree];
                heap_degree[node->degree] = node;
                node = tmp;
            }
            FHeap_cutOut(heap, heap_degree[node->degree],
                         heap_degree[node->degree]->parent);
            FHeapMeld(node, heap_degree[node->degree]);
        }
    }
}

HashTable *hashTable = NULL;

FHeapNode *FHeap_find(MinFHeap *heap, int key, int value)
{
    return findFHeapNode(key, value, hashTable);
}

void print_hash_table(HashTable *hashTable)
{
    for (int i = 0; i < hashTable->capacity; i++) {
        HashNode *node = hashTable->table[i];
        printf("%d -> ", i);
        while (node != NULL) {
            printf("(%d %d), ", node->FNode->key, node->FNode->value);
            node = node->next;
        }
        printf("\n");
    }
}

void print_fheap(FHeapNode *root, int layer)
{
    if (layer > 3)
        return;
    printf("Layer %d: ", layer);
    if (root == NULL) {
        printf("(X,X)");
        return;
    }
    FHeapNode *curr = root;
    do {
        printf("(%d,%d)(%d) ", curr->key, curr->value, curr->degree);
        curr = curr->right;
    } while (curr != root);
    printf("\n");

    do {
        if (curr->child) {
            printf("Layer %d Parent (%d,%d) | Child -> ", layer, curr->key,
                   curr->value);
            print_fheap(curr->child, layer + 1);
        }
        curr = curr->right;
    } while (curr != root);
}

int main(int argc, char *argv[])
{
    hashTable = (HashTable *) malloc(sizeof(HashTable));
    initHashTable(hashTable, 10);
    MinFHeap *heap = MinFHeap_new();
    char command[100];
    while (scanf("%s ", command)) {
        // print_heap(heap);

        if (strncmp(command, "insert", 6) == 0) {
            int key, value;
            scanf("%d %d", &key, &value);
#ifdef DEBUG
            printf("---------after %s (%d, %d)\n", command, key, value);
#endif
            FHeapNode *node = FHeapNode_new(key, value);
            // printf("insert %d %d\n", key, value);
            FHeap_insert(heap, node);
            insertHashTable(hashTable, node);
            // print_hash_table(hashTable);
        } else if (strncmp(command, "delete", 6) == 0) {
            int key, value;
            scanf("%d %d\n", &key, &value);
#ifdef DEBUG
            printf("---------after %s (%d, %d)\n", command, key, value);
#endif
            FHeapNode *node = FHeap_find(heap, key, value);
            FHeap_delete_node(heap, node);
            removeHashTable(hashTable, node);
        } else if (strncmp(command, "decrease", 8) == 0) {
            int key, value, delta;
            scanf("%d %d %d\n", &key, &value, &delta);
#ifdef DEBUG
            printf("---------after %s (%d, %d) %d\n", command, key, value,
                   delta);
#endif
            FHeap_decreaseKey(heap, key, value, delta);
        } else if (strncmp(command, "extract", 7) == 0) {
#ifdef DEBUG
            printf("---------after %s\n", command);
#endif
            int key, value;
            if (FHeap_extractMin(heap, &key, &value)) {
                printf("(%d)%d\n", key, value);
            }
        } else if (strncmp(command, "quit", 3) == 0) {
            break;
        }
#ifdef DEBUG
        printf("Size: %d\n", heap->size);
        print_fheap(heap->min, 0);
#endif
    }
    return 0;
}
