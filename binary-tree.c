#include "binary-tree.h"

/***********************************************************************
 * Function: create_tree_with_cmp_func
 *
 * Date: March 14, 2012
 *
 * Revisions: (Date and Description)
 *
 * Designer: David Young
 *
 * Programmer: David Young
 *
 * Interface: BTree * create_tree_with_cmp_func (int (*compare_func)(void *, void *))
 *        param: int (*compare_func)(void *, void *) - comparison function for BTree
 *
 * Returns: (BTree *) - Pointer to newly allocated BTree struct. NULL on error
 *
 * Notes: 
 **********************************************************************/
BTree *create_tree_with_cmp_func(int (*compare_func)(void *, void *)) {
	BTree *btree;
	
	if (compare_func == NULL) {
		return NULL;
	}
	
	if ((btree = (BTree *)malloc(sizeof(BTree))) != NULL) {
		btree->head = NULL;
		btree->n_nodes = 0;
		btree->__compare_func = compare_func;
	}
	
	return btree;
}

/***********************************************************************
 * Function: create_node_with_data
 *
 * Date: March 14, 2012
 *
 * Revisions: (Date and Description)
 *
 * Designer: David Young
 *
 * Programmer: David Young
 *
 * Interface: Node * create_node_with_data (void *data, uint32_t data_len)
 *        param: void *data - void pointer to data to store
 *        param: uint32_t data_len - byte size of data to store
 *
 * Returns: (Node *) - Pointer to newly created node. NULL on error
 *
 * Notes: 
 **********************************************************************/
Node *create_node_with_data(void *data, uint32_t data_len) {
	Node *new_node;
	
	/* malloc for new node */
	if ((new_node = (Node *)malloc(sizeof(Node))) == NULL) {
		return NULL;
	}
	
	/* malloc for data */
	if ((new_node->data = malloc(data_len)) == NULL) {
		return NULL;
	}
	
	memcpy(new_node->data, data, data_len);
	new_node->parent = NULL;
	new_node->left = NULL;
	new_node->right = NULL;

	return new_node;
}

/***********************************************************************
 * Function: add_object_to_tree
 *
 * Date: March 14, 2012
 *
 * Revisions: (Date and Description)
 *
 * Designer: David Young
 *
 * Programmer: David Young
 *
 * Interface: Node * add_object_to_tree (void *object, uint32_t obj_size, BTree *tree)
 *        param: void *object - void pointer to object to add
 *        param: uint32_t obj_size - size of object
 *        param: BTree *tree - pointer to BTree struct
 *
 * Returns: (Node *) - pointer to new node holding the added object.
 *		  NULL on error.
 *
 * Notes: 
 **********************************************************************/
Node *add_object_to_tree(void *object, uint32_t obj_size, BTree *tree) {
	Node *new_node;
	Node *current_node;
	int not_found_yet = 1;
	
	if ((new_node = create_node_with_data(object, obj_size)) == NULL) { /* couldn't create */
		return NULL;
	}
	
	if (tree == NULL) { /* Null tree = big problem */
		return NULL;
	} else if (tree->head == NULL) { /* empty tree */
		tree->head = new_node;
	} else { /* find where this node goes */
		current_node = tree->head;
		while (not_found_yet) {
			not_found_yet = 0;
			
			switch (tree->__compare_func(new_node->data, current_node->data)) {
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
	
	tree->n_nodes++;
	
	return new_node;
}

/***********************************************************************
 * Function: find_node
 *
 * Date: March 14, 2012
 *
 * Revisions: (Date and Description)
 *
 * Designer: David Young
 *
 * Programmer: David Young
 *
 * Interface: Node * find_node (void *object, BTree *tree)
 *        param: void *object - void pointer to object to find
 *        param: BTree *tree - pointer to binary tree to find node in
 *
 * Returns: (Node *) - pointer to node that was found. NULL if not found
 *
 * Notes: 
 **********************************************************************/
Node *find_node(void *object, BTree *tree) {
	Node *current_node = tree->head;
	
	while (1) {
		switch (tree->__compare_func(object, current_node->data)) {
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

/***********************************************************************
 * Function: delete_node
 *
 * Date: March 14, 2012
 *
 * Revisions: (Date and Description)
 *
 * Designer: David Young
 *
 * Programmer: David Young
 *
 * Interface: int  delete_node (Node *node_to_delete, BTree *tree)
 *        param: Node *node_to_delete - pointer to node to be deleted
 *        param: BTree *tree - tree to delete the node from
 *
 * Returns: (int ) - (-1) on error, 0 if nothing to do, or 1 if successful
 *
 * Notes: 
 **********************************************************************/
int delete_node(Node *node_to_delete, BTree *tree) {
	Node *current_node;
	int skip = 0, n_children;
	
	if (tree == NULL) { /* error */
		return -1;
	}
	
	if (node_to_delete == NULL) {
		return 0; /* nothing to do */
	} else if (node_to_delete->left == NULL && node_to_delete->right == NULL) {
		/* no children makes it a lot easier */
		current_node = node_to_delete->parent;
		if (node_to_delete == current_node->left) {
			current_node->left = NULL;
		} else if (node_to_delete == current_node->right) {
			current_node->right = NULL;
		} else { /* we have a corrupted tree */
			return -1;
		}
		skip = 1;
	} else if ((n_children = n_side_children(node_to_delete->left, RIGHT)) == 0 ||
			   n_side_children(node_to_delete->right, LEFT) > n_children) {
		/* node_to_delete->right will replace node_to_delete */
		current_node = node_to_delete->right;
	} else {
		/* node_to_delete->left will replace node_to_delete */
		current_node = node_to_delete->left;
	}
	
	/* adjust parent pointers to new anchor node */
	if (!skip && node_to_delete->parent != NULL) { /* if not head node */
		current_node->parent = node_to_delete->parent;
		
		if (node_to_delete == node_to_delete->parent->left) {
			node_to_delete->parent->left = current_node;
		} else if (node_to_delete == node_to_delete->parent->right) {
			node_to_delete->parent->right = current_node;
		}
	}
	
	/* current_node->left->left... = node_to_delete->left ||
	 current_node->right->right... = node_to_delete->right */
	
	if (current_node == node_to_delete->left) {
		while (current_node->right != NULL) {
			current_node = current_node->right;
		}
		
		current_node->right = node_to_delete->right;
	} else if (current_node == node_to_delete->right) {
		while (current_node->left != NULL) {
			current_node = current_node->left;
		}
		
		current_node->left = node_to_delete->left;
	}

	tree->n_nodes--;
	free(node_to_delete->data);
	free(node_to_delete);
	
	return 1;
}

/***********************************************************************
 * Function: n_side_children
 *
 * Date: March 14, 2012
 *
 * Revisions: (Date and Description)
 *
 * Designer: David Young
 *
 * Programmer: David Young
 *
 * Interface: int  n_side_children (Node *node, int side)
 *        param: Node *node - pointer to node to delete
 *        param: int side - LEFT or RIGHT
 *
 * Returns: (int ) - how many child nodes are down the specified side
 *
 * Notes: node->left = 1, node->left->left = 2, etc...
 **********************************************************************/
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
	}

	return count;
}
