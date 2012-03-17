#include "binary-tree.h"

int comp(void *p1, void *p2) {
	int i1, i2;
	i1 = *(int *)p1;
	i2 = *(int *)p2;
	
	if (i1 > i2) {
		return 1;
	} else if (i1 == i2) {
		return 0;
	} else {
		return -1;
	}

}

int traverse(Node *node) {
	int count = 0;
	
	printf("Node data: %ld\n", *(long *)node->data);
	
	if (node->left != NULL) {
		printf("Left: ");
		count += traverse(node->left);
	}
	
	if (node->right != NULL) {
		printf("Right: ");
		count += traverse(node->right);
	}
	
	printf("Back\n");

	return 1+count;
}

int main(int argc, char **argv) {
	BTree *tree = create_tree_with_cmp_func(comp);
	Node *node;
	int i;
	long l;
	long nums[] = {
		7, 2, 13, 1, 4, 9, 22, 3, 5, 8, 10, 17, 6, 11
	};
	
	if (tree == NULL) {
		fprintf(stderr, "Houston, we have a problem\n");
		exit(1);
	}
	
	for (i = 0; i < 14; i++) {
		l = nums[i];
		if (add_object_to_tree((void *)&l, sizeof(l), tree) == NULL) {
			fprintf(stderr, "Unable to add node\n");
		}
	}
	
	if (find_node((void *)&l, tree) == NULL) {
		fprintf(stderr, "Couldn't find node\n");
		exit(1);
	}
	
	printf("traversed %d nodes\n\n", traverse(tree->head));
	
	printf("-------------------------------\n");
	
	l = 9;
	
	if ((node = find_node(&l, tree)) == NULL) {
		fprintf(stderr, "Couldn't find node\n");
		exit(1);
	}
	
	if (delete_node(node, tree) != 1) {
		fprintf(stderr, "Unable to delete node\n");
		exit(1);
	}
	
	printf("traversed %d nodes\n\n", traverse(tree->head));
	
	printf("-------------------------------\n");
	
	l = 10;
	
	if ((node = find_node(&l, tree)) == NULL) {
		fprintf(stderr, "Couldn't find node\n");
		exit(1);
	}
	
	if (delete_node(node, tree) != 1) {
		fprintf(stderr, "Unable to delete node\n");
		exit(1);
	}
	
	printf("traversed %d nodes\n\n", traverse(tree->head));
	
	return 0;
}
