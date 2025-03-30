#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct Edge;

struct Node {
    int element;
    struct Edge **edges;
    int edge_num;
};

void Node_init(struct Node *node, int element)
{
    node->element = element;
}

struct Edge {
    int weight;
    struct Node *start;
    struct Node *end;
};

void Edge_init(struct Edge *edge,
               int weight,
               struct Node *start,
               struct Node *end)
{
    edge->weight = weight;
    edge->start = start;
    edge->end = end;
}

struct DisjointSet {
    struct Node *node;
    struct DisjointSet *parent;
    int height;
};

void DisjointSet_init(struct DisjointSet *set, struct Node *node)
{
    set->node = node;
    set->parent = NULL;
    set->height = 1;
}

struct DisjointSet *DisjointSet_find(struct DisjointSet *set)
{
    while (set->parent != NULL) {
        set = set->parent;
    }
    return set;
}

void DisjointSet_union(struct DisjointSet *set1, struct DisjointSet *set2)
{
    set1 = DisjointSet_find(set1);
    set2 = DisjointSet_find(set2);
    if (set1 == set2) {
        return;
    }
    if (set1->height > set2->height) {
        set2->parent = set1;
    } else if (set1->height < set2->height) {
        set1->parent = set2;
    } else {
        set2->parent = set1;
        set1->height += 1;
    }
}

struct minHeap {
    struct Edge **edges;
    int size;
    int capacity;
};

void minHeap_init(struct minHeap *heap, int capacity)
{
    heap->edges = (struct Edge **) malloc(capacity * sizeof(struct Edge *));
    heap->size = 0;
    heap->capacity = capacity;
}

void minHeap_insert(struct minHeap *heap, struct Edge *edge)
{
    int i = heap->size;
    if (i == heap->capacity) {
        printf("heap is full\n");
        return;
    }
    if (i == 0) {
        heap->edges[i] = edge;
        heap->size += 1;
        return;
    }
    while (i > 0 && heap->edges[(i - 1) / 2]->weight > edge->weight) {
        heap->edges[i] = heap->edges[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    heap->edges[i] = edge;
    heap->size += 1;
}

void minHeap_remove(struct minHeap *heap, int index)
{
    if (index >= heap->size) {
        printf("index out of range\n");
        return;
    }
    if (index == heap->size - 1) {
        heap->size -= 1;
        return;
    }
    heap->edges[index] = heap->edges[heap->size - 1];
    heap->size -= 1;
    int i = index;
    while (i < heap->size) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int min = i;
        if (left < heap->size &&
            heap->edges[left]->weight < heap->edges[min]->weight) {
            min = left;
        }
        if (right < heap->size &&
            heap->edges[right]->weight < heap->edges[min]->weight) {
            min = right;
        }
        if (min == i) {
            break;
        }
        struct Edge *temp = heap->edges[i];
        heap->edges[i] = heap->edges[min];
        heap->edges[min] = temp;
        i = min;
    }
}

void minHeap_popMin(struct minHeap *heap, struct Edge *result)
{
    if (heap->size == 0) {
        printf("heap is empty\n");
        return;
    }
    result->weight = heap->edges[0]->weight;
    result->start = heap->edges[0]->start;
    result->end = heap->edges[0]->end;
    minHeap_remove(heap, 0);
}


int main(int argc, char *argv[])
{
    int V, E;
    scanf("%d %d", &V, &E);

    struct Node *nodes = (struct Node *) malloc(V * sizeof(struct Node));
    struct minHeap *edge_heap =
        (struct minHeap *) malloc(sizeof(struct minHeap));
    struct DisjointSet *traversed_nodes_sets =
        (struct DisjointSet *) malloc(V * sizeof(struct DisjointSet));
    for (int i = 0; i < V; ++i) {
        Node_init(&nodes[i], i);
        DisjointSet_init(&traversed_nodes_sets[i], &nodes[i]);
    }

    minHeap_init(edge_heap, E);

    for (int i = 0; i < E; ++i) {
        int s, t, c;
        scanf("%d %d %d", &s, &t, &c);
        struct Edge *edge = (struct Edge *) malloc(sizeof(struct Edge));
        Edge_init(edge, c, &nodes[s], &nodes[t]);
        minHeap_insert(edge_heap, edge);
    }

    int traversed_node_num = 0;
    long long int total_weight = 0;

    // get the first edge
    struct Edge *buffer_edge = (struct Edge *) malloc(sizeof(struct Edge));
    minHeap_popMin(edge_heap, buffer_edge);
    total_weight += buffer_edge->weight;
    DisjointSet_union(&traversed_nodes_sets[buffer_edge->start->element],
                      &traversed_nodes_sets[buffer_edge->end->element]);

    // kruskal's algorithm
    while (traversed_node_num < V && edge_heap->size > 0) {
        minHeap_popMin(edge_heap, buffer_edge);
        if (DisjointSet_find(
                &traversed_nodes_sets[buffer_edge->start->element]) !=
            DisjointSet_find(
                &traversed_nodes_sets[buffer_edge->end->element])) {
            total_weight += buffer_edge->weight;
            DisjointSet_union(
                &traversed_nodes_sets[buffer_edge->start->element],
                &traversed_nodes_sets[buffer_edge->end->element]);
            traversed_node_num += 1;
        }
    }

    printf("%lld\n", total_weight);

    return 0;
}
