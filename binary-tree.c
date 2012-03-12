#include "binary-tree.h"

int config_tree(int (*compare_func)(void *, void *)) {
	if (compare_func == NULL) {
		return 0;
	}
	
	__compare_func = compare_func;
	return 1;
}

Node *create_node_with_data(void *data) {
	Node *new_node;
	
	if ((new_node = (Node *)malloc(sizeof(Node))) == NULL) {
		return NULL;
	}
	
	new_node->data = data;
	new_node->parent = NULL;
	new_node->left = NULL;
	new_node->right = NULL;

	return new_node;
}

Node *add_object_to_tree(void *object, Node *tree_head) {
	Node *new_node;
	Node *current_node;
	int not_found_yet = 1;
	
	if ((new_node = create_node_with_data(object)) == NULL) { /* couldn't create */
		return NULL;
	}
	
	if (tree_head == NULL) { /* empty tree */
		tree_head = new_node;
	} else { /* find where this node goes */
		current_node = tree_head;
		while (not_found_yet) {
			not_found_yet = 0;
			
			switch (__compare_func(new_node->data, current_node->data)) {
				case -1: /* new_node < current_node: go left */
					if (current_node->left == NULL) {
						current_node->left = new_node;
					} else {
						current_node = current_node->left;
						not_found_yet = 1;
					}
					
					break;
					
				case 0: /* not handling this right now */
					break;
					
				case 1: /* new_node > current_node: go right */
					if (current_node->right == NULL) {
						current_node->right = new_node;
					} else {
						current_node = current_node->right;
						not_found_yet = 1;
					}

					break;

				default:
					break;
			}
		}
		
		new_node->parent = current_node;
	}
	
	return new_node;
}

Node *find_node(void *object, Node *tree_head) {
	Node *current_node;
	
	while (1) {
		switch (__compare_func(object, current_node->data)) {
			case -1: /* object should be to the left */
				if (current_node->left != NULL) {
					current_node = current_node->left;
				} else { /* nothing where this object would be. not found */
					return NULL;
				}

				break;
			
			case 0:
				return current_node;
				
			case 1: /* object should be to the right */
				if (current_node->right != NULL) {
					current_node = current_node->right;
				} else { /* nothing where this object would be. not found */
					return NULL;
				}

				break;

			default:
				break;
		}
	}
}

void *delete_node(Node *node_to_delete) {
	Node *current_node;
	void *data;
	
	current_node = node_to_delete->parent;
	
	if (node_to_delete->left == NULL && node_to_delete->right == NULL) {
		/* no children makes it a lot easier */
		if (node_to_delete == current_node->left) {
			current_node->left = NULL;
		} else if (node_to_delete == current_node->right) {
			current_node->right = NULL;
		}
	} else if (0) {
		/* do something */
	}
	
	data = node_to_delete->data;
	free(node_to_delete);
}

int n_side_children(Node *node, int side) {
	Node *current_node = node;
	int count = 0;
	
	if (side == LEFT) {
		while (current_node->left != NULL) {
			current_node = current_node->left;
			count++;
		}
	} else if (side == RIGHT) {
		while (current_node->right != NULL) {
			current_node = current_node->right;
			count++;
		}
	} else {
		/* this should never happen */
	}

	
	return count;
}