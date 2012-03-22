#include "helpers.h"

int add_connection_to_tree(int sock1, int sock2, BTree *tree) {
	Connection conn;

	conn.src = sock1;
	conn.dst = sock2;

	if (add_object_to_tree((void *)&conn, sizeof(conn), tree) == NULL) {
		fprintf(stderr, "unable to add node\n");
		return 0;
	}

	conn.src = sock2;
	conn.dst = sock1;
	if (add_object_to_tree((void *)&conn, sizeof(conn), tree) == NULL) {
		fprintf(stderr, "unable to add node\n");
		return 0;
	}

	return 1;
}
