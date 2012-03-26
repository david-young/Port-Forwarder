#include "helpers.h"

int add_connection_to_tree(int sock1, int sock2, BTree *tree) {
	Connection conn;

	conn.src = sock1;
	conn.dst = sock2;

	if (add_object_to_tree((void *)&conn, sizeof(conn), tree) == NULL) {
		fprintf(stderr, "unable to add node\n");
		return 0;
	}

	if (find_node(&conn, tree) == NULL)
		fprintf(stderr, "Node was not saved successfully.\n");

	conn.src = sock2;
	conn.dst = sock1;

	if (add_object_to_tree((void *)&conn, sizeof(conn), tree) == NULL) {
		fprintf(stderr, "unable to add node\n");
		return 0;
	}
	
	if (find_node(&conn, tree) == NULL)
		fprintf(stderr, "Node was not saved successfully.\n");

/*	printf("n children: %d\n", tree->n_children);*//*DEBUG*/

	return 1;
}

int remove_connection_from_tree(int sock1, int sock2, BTree *tree) {
	Connection conn;
	Node *node;

/*	printf("src %d dst %d\n", sock1, sock2);*//*DEBUG*/
	
	conn.src = sock2;
	if ((node = find_node(&conn, tree)) == NULL) {
		fprintf(stderr, "Couldn't find node with src fd %d line %d\n", conn.src, __LINE__);
		return 0;
	}

	if (delete_node(node, tree) <= 0) {
		fprintf(stderr, "Unable to delete node.\n");
		return 0;
	}

	conn.src = sock1;
	if ((node = find_node(&conn, tree)) == NULL) {
		fprintf(stderr, "Couldn't find node with src fd %d line %d\n", conn.src, __LINE__);
		return 0;
	}

	if (delete_node(node, tree) <= 0) {
		fprintf(stderr, "Unable to delete node.\n");
		return 0;
	}

	shutdown(sock1, SHUT_RDWR);
	if (close(sock1) != 0) {
		perror("close failed");
	}
	shutdown(sock2, SHUT_RDWR);
	if (close(sock2) != 0) {
		perror("close failed");
	}

	return 1;
}

void handle_OOB(int param) {
	printf("Received OOB data! We don't care!\n");
}
