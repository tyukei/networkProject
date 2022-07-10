#include "idobata.h"

int main(int argc, char *argv[]) {
	char *username;
	int port_number = DEFAULT_PORT;

	/* 引数のチェックと使用法の表示 */
	if (argc == 3) {
		username = argv[1];
		port_number = atoi(argv[2]);
	} else if (argc == 2) {
		username = argv[1];
	} else {
		fprintf(stderr, "Usage: %s username\n", argv[0]);
		exit(1);
	}

	idobata_client(username,port_number);
	return 0;
}
