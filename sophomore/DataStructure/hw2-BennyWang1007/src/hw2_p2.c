#pragma GCC diagnostic ignored "-Wunused-result"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLACK 0
#define RED 1


typedef struct RBTNode {
    int key;
    int color;  // 0 for black, 1 for red
    struct RBTNode *left;
    struct RBTNode *right;
    struct RBTNode *parent;
} RBTNode;

typedef struct RBT {
    RBTNode *root;
    RBTNode *nil;
} RBT;

// initialize a red-black tree node
void RBTNodeInit(RBT *T, RBTNode *n, int key)
{
    RBTNode *nil = T->nil;
    n->key = key;
    n->color = BLACK;
    n->left = nil;
    n->right = nil;
    n->parent = nil;
}

// initialize a red-black tree
void RBTInit(RBT *T)
{
    T->nil = (RBTNode *) malloc(sizeof(RBTNode));
    RBTNodeInit(T, T->nil, -1);
    T->root = T->nil;
}

// search the node with key, return the node if found, otherwise return NULL
RBTNode *RBTSearch(RBT *T, int key)
{
    RBTNode *nil = T->nil;
    RBTNode *x = T->root;
    while (x != nil && key != x->key) {
        if (key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    if (x == nil)
        return NULL;
    return x;
}

// a left rotation on node x
void RBT_RRRot(RBT *T, RBTNode *x)
{
    RBTNode *nil = T->nil;
    RBTNode *y = x->right;
    x->right = y->left;
    if (y->left != nil) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nil) {
        T->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

// a right rotation on node x
void RBT_LLRot(RBT *T, RBTNode *x)
{
    RBTNode *nil = T->nil;
    RBTNode *y = x->left;
    x->left = y->right;
    if (y->right != nil) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nil) {
        T->root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

void RBT_LRRot(RBT *T, RBTNode *x)
{
    RBT_RRRot(T, x->left);
    RBT_LLRot(T, x);
}

void RBT_RLRot(RBT *T, RBTNode *x)
{
    RBT_LLRot(T, x->right);
    RBT_RRRot(T, x);
}

// fixup the red-black tree after insertion
void RBTInsertFixup(RBT *T, RBTNode *z)
{
    if (z == T->root) {
        z->color = 0;
        return;
    }
    if (z->parent == T->root)
        return;
    if (z->parent->color == 0)
        return;
    // RBTNode *nil = T->nil;
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBTNode *y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    RBT_RRRot(T, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                RBT_LLRot(T, z->parent->parent);
            }
        } else {
            RBTNode *y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    RBT_LLRot(T, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                RBT_RRRot(T, z->parent->parent);
            }
        }
        T->root->color = BLACK;
    }
}

void RBTInsert_node(RBT *T, RBTNode *z)
{
    RBTNode *nil = T->nil;
    RBTNode *y = nil;
    RBTNode *x = T->root;
    // find y = the parent of z
    while (x != nil) {
        y = x;
        if (z->key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    if (y == nil) {
        T->root = z;
        T->root->color = BLACK;
    } else if (z->key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }
    z->parent = y;
    z->color = RED;
    RBTInsertFixup(T, z);
}

void RBTInsert(RBT *T, int key)
{
    RBTNode *z = (RBTNode *) malloc(sizeof(RBTNode));
    RBTNodeInit(T, z, key);
    RBTInsert_node(T, z);
}

// transplant the subtree rooted at node v to node u
void RBTTransplant(RBT *T, RBTNode *u, RBTNode *v)
{
    if (u->parent == T->nil) {
        T->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v != T->nil) {
        v->parent = u->parent;
    }
}

void RBTSwap(RBTNode *x, RBTNode *y)
{
    int temp = x->key;
    x->key = y->key;
    y->key = temp;
}


RBTNode *RBTMaximum(RBT *T, RBTNode *x)
{
    while (x->right != T->nil) {
        x = x->right;
    }
    return x;
}

void RBTDeleteFixup(RBT *T, RBTNode *x)
{
    while (x != T->root && x->color == BLACK) {
        if (x == x->parent->left) {
            RBTNode *w = x->parent->right;  // sibling of x
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                RBT_RRRot(T, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x->parent->color = BLACK;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    RBT_LLRot(T, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                RBT_RRRot(T, x->parent);
                x = T->root;
            }
        } else {
            RBTNode *w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                RBT_LLRot(T, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x->parent->color = BLACK;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    RBT_RRRot(T, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                RBT_LLRot(T, x->parent);
                x = T->root;
            }
        }
    }
    x->color = BLACK;
}

RBTNode *Successor(RBT *T, RBTNode *x)
{
    RBTNode *nil = T->nil;
    if (x->left != nil)
        return RBTMaximum(T, x->left);

    RBTNode *y = x->parent;
    while (y != nil && x == y->left) {
        x = y;
        y = y->parent;
    }
    return y;
}

void RBTDelete_node(RBT *T, RBTNode *z)
{
    RBTNode *nil = T->nil;
    RBTNode *y;  // the node to be deleted
    RBTNode *x;  // the child of y
    if (z->left == nil || z->right == nil)
        y = z;
    else
        y = Successor(T, z);

    if (y->left != nil)
        x = y->left;
    else
        x = y->right;

    x->parent = y->parent;
    if (y->parent == nil)
        T->root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;

    if (y != z) {
        z->key = y->key;
    }
    if (y->color == BLACK)
        RBTDeleteFixup(T, x);
    free(y);
}



void print_RBTNode(RBT *T, RBTNode *x)
{
    if (x == T->nil)
        printf("()");
    else
        printf("(%d,%s)", x->key, x->color == 0 ? "B" : "R");
}

void print_RBT(RBT *T, RBTNode *x)
{
    RBTNode *nil = T->nil;
    if (x == nil)
        return;
    if (x->left == nil && x->right == nil)
        return;
    print_RBTNode(T, x);
    printf("-> ");
    print_RBTNode(T, x->left);
    printf(" / ");
    print_RBTNode(T, x->right);
    printf("\n");
    print_RBT(T, x->left);
    print_RBT(T, x->right);
}

void RBT_free(RBT *T, RBTNode *x)
{
    RBTNode *nil = T->nil;
    if (x == nil)
        return;
    RBT_free(T, x->left);
    RBT_free(T, x->right);
    free(x);
}

int main(int argc, char *argv[])
{
    RBT *T = (RBT *) malloc(sizeof(RBT));
    RBTInit(T);
    char command[20];
    int key;
    char line[100];
    while (fgets(line, 100, stdin) != NULL) {
        if (strncmp(line, "search ", 7) == 0) {
            sscanf(line, "%s %d", command, &key);
            RBTNode *x = RBTSearch(T, key);
            if (x == NULL)
                printf("Not found\n");
            else
                printf("%s\n", x->color == 0 ? "black" : "red");
        } else if (strncmp(line, "insert ", 7) == 0) {
            sscanf(line, "%s %d", command, &key);
            RBTNode *x = RBTSearch(T, key);
            if (x == NULL)
                RBTInsert(T, key);
        } else if (strncmp(line, "delete ", 7) == 0) {
            sscanf(line, "%s %d", command, &key);
            RBTNode *x = RBTSearch(T, key);
            if (x != NULL)
                RBTDelete_node(T, x);
        } else if (strncmp(line, "quit", 4) == 0) {
            break;
        }
    }

    return 0;
}
