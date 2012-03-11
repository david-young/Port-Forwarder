#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

typedef struct node {
	struct node *parent; /* pointer to parent node */
	struct node *left; /* pointer to left child */
	struct node *right; /* pointer to right child */
	uint32_t n_nodes_left; /* counter of node->left->left->left... */
	uint32_t n_nodes_right; /* counter of node->right->right->right... */
	void *data; /* pointer to data. void* so it can hold anything */
} Node;

int (*__compare_func)(void *item1, void *item2);

int config_tree(int (*compare_func)(void *, void *));
Node *create_node_with_data(void *data);
Node *add_object_to_tree(void *object, Node *tree_head);
Node *find_node(void *object, Node *tree_head);
void *delete_node(Node *node_to_delete);
int update_parent_node_counts(Node *current_node, int difference);

#endif
