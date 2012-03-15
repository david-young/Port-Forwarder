#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define LEFT 0
#define RIGHT 1

typedef struct node {
	struct node *parent; /* pointer to parent node */
	struct node *left; /* pointer to left child */
	struct node *right; /* pointer to right child */
	void *data; /* pointer to data. void* so it can hold anything */
} Node;

typedef struct {
	Node *head;
	uint32_t n_nodes;
	int (*__compare_func)(void *item1, void *item2);
} BTree;

BTree *create_tree_with_cmp_func(int (*compare_func)(void *, void *));
Node *create_node_with_data(void *data, uint32_t data_len);
Node *add_object_to_tree(void *object, uint32_t obj_size, BTree *tree);
Node *find_node(void *object, BTree *tree);
int delete_node(Node *node_to_delete, BTree *tree);
int update_parent_node_counts(Node *current_node, int difference);
int n_side_children(Node *node, int side);

#endif
