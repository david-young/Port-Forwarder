#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "binary-tree.h"

typedef struct {
    int src; /* fd id */
    int dst;
} Connection;

int add_connection_to_tree(int sock1, int sock2, BTree *tree);

#endif

