#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "binary-tree.h"
#include <unistd.h>
#include <sys/socket.h>

typedef struct {
    int src; /* fd id */
    int dst;
} Connection;

int add_connection_to_tree(int sock1, int sock2, BTree *tree);
int remove_connection_from_tree(int sock1, int sock2, BTree *tree);
void handle_OOB(int param);

#endif

