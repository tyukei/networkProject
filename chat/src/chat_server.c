#include "chat.h"
#include "mynet.h"
#include <stdlib.h>
#include <unistd.h>

struct user {
	int sock;
	char name[NAMELENGTH];
	int len;
	int status;
}typedef User;

int Sock_accepted;
fd_set Mask;
User *Client;
static int Max_sock = 0;



void chat_init_server(int n_client) {
	char prompt[] = "Input your name: ";
	char loginname[NAMELENGTH];
	int namesize;
	int i, j;
	int sock_accepted;

	/* クライアント情報の保存用構造体の初期化 */
	// User Client[num_client]の宣言
	if ((Client = (User*) malloc(n_client * sizeof(User))) == NULL) {
		exit_errmesg("malloc()");
	}

	/* クライアントのログイン処理 */
	for (i = 0; i < n_client; i++) {
		/* クライアントの接続を受け付ける */
		sock_accepted = accept(Sock_accepted, NULL, NULL);
		printf("Client[%d] connected.\n", i);

		/* ログインプロンプトを送信 */
		send(sock_accepted, prompt, strlen(prompt), 0);
		/* ログイン名を受信 */
		namesize = recv(sock_accepted, loginname, NAMELENGTH - 1, 0);
		loginname[namesize - 1] = '\0';
		/* ユーザ情報を保存 */
		Client[i].status = 1;
		Client[i].sock = sock_accepted;
		strncpy(Client[i].name, loginname, namesize);
		Client[i].len = strlen(Client[i].name);
		printf("%s connected.\n", Client[i].name);
		send(Client[i].sock, "you are ", strlen("you are "), 0);
		send(Client[i].sock, Client[i].name, Client[i].len, 0);
		send(Client[i].sock, "\n", 1, 0);

		for (j = 0; j <= i; j++) {
			send(Client[j].sock, Client[i].name, Client[i].len, 0);
			send(Client[j].sock, " is coming now\n", strlen(" is coming now\n"),
					0);
		}
	}
	Max_sock = sock_accepted; // select の監視範囲の最大値

	for (j = 0; j < n_client; j++) {
		send(Client[j].sock, "Let's start\n", strlen("Let's start\n"), 0);
		send(Client[j].sock, "You:", strlen("You:"), 0);
	}

	FD_ZERO(&Mask);
	for (i = 0; i < n_client; i++) {
		FD_SET(Client[i].sock, &Mask);
	}
}

void chat_server_loop(int n_client) {
	char red[] = "\x1b[31m";
	char black[] = "\x1b[0m";
	fd_set readfds;
	char buf[BUFSIZE];
	int strsize;
	int i, j;

	while (1) {
		readfds = Mask;
		select(Max_sock + 1, &readfds, NULL, NULL, NULL);
		for (i = 0; i < n_client; i++) {
			if (Client[i].status == 0) {
				continue;
			}
			if (FD_ISSET(Client[i].sock, &readfds)) {
				/* クライアントから文字列を受信したとき */
				strsize = recv(Client[i].sock, buf, BUFSIZE - 1, 0);
				if (strsize <= 0) {
					printf("here");
					Client[i].status = 0;
					for (j = 0; j < n_client; j++) {
						if (j != i) {
							send(Client[j].sock, red, strlen(red), 0);
							send(Client[j].sock, Client[i].name, Client[i].len,
									0);
							send(Client[j].sock, " left now\n",
									strlen(" left now\n"), 0);
							send(Client[j].sock, black, strlen(black), 0);
							send(Client[j].sock, "You:", strlen("You:"), 0);
						}
					}
					continue;
				}
				buf[strsize] = '\0';
				//printf("%s: %s",Client[i].name,buf);

				for (j = 0; j < n_client; j++) {
					if (j != i) {
						send(Client[j].sock, red, strlen(red), 0);
						send(Client[j].sock, Client[i].name, Client[i].len, 0);
						send(Client[j].sock, ": ", 2, 0);
						send(Client[j].sock, buf, strsize, 0);
						send(Client[j].sock, black, strlen(black), 0);
					}
					send(Client[j].sock, "You:", strlen("You:"), 0);
				}
			}
		}
	}
	free(Client);
}

void chat_server(int port_number, int n_client) {
	printf("calling chat_server");
	/* サーバの初期化 */
	Sock_accepted = init_tcpserver(port_number, n_client);

	chat_init_server(n_client);

	chat_server_loop(n_client);
}

//void chat_server(int port_number, int n_client)
//{
//	int Sock_accepted;, sock_accepted;
//	fd_set Mask, readfds;
//	User *Client;
//	char buf[BUFSIZE];
//	char loginname[NAMELENGTH];
//	char prompt[] = "Input your name: ";
//	char red[] = "\x1b[31m";
//	char black[] = "\x1b[0m";
//	int Max_sock = 0;
//	int namesize, strsize;
//	int i, j;
//
//  printf("calling chat_server");
//  /* サーバの初期化 */
//	Sock_accepted; = init_tcpserver(port_number, n_client);
//
//	/* クライアント情報の保存用構造体の初期化 */
//	// User Client[num_client]の宣言
//	if ((Client = (User*) malloc(n_client * sizeof(User))) == NULL) {
//		exit_errmesg("malloc()");
//	}
//
//	/* クライアントのログイン処理 */
//	for (i = 0; i < n_client; i++) {
//		/* クライアントの接続を受け付ける */
//		sock_accepted = accept(Sock_accepted;, NULL, NULL);
//		printf("Client[%d] connected.\n", i);
//
//		/* ログインプロンプトを送信 */
//		send(sock_accepted, prompt, strlen(prompt), 0);
//		/* ログイン名を受信 */
//		namesize = recv(sock_accepted, loginname, NAMELENGTH - 1, 0);
//		loginname[namesize - 1] = '\0';
//		/* ユーザ情報を保存 */
//		Client[i].status = 1;
//		Client[i].sock = sock_accepted;
//		strncpy(Client[i].name, loginname, namesize);
//		Client[i].len = strlen(Client[i].name);
//		printf("%s connected.\n", Client[i].name);
//		send(Client[i].sock, "you are ", strlen("you are "), 0);
//		send(Client[i].sock, Client[i].name, Client[i].len, 0);
//		send(Client[i].sock, "\n", 1, 0);
//
//		for (j = 0; j <= i; j++) {
//			send(Client[j].sock, Client[i].name, Client[i].len, 0);
//			send(Client[j].sock, " is coming now\n", strlen(" is coming now\n"),0);
//		}
//	}
//	Max_sock = sock_accepted; // select の監視範囲の最大値
//
//	for (j = 0; j < n_client; j++) {
//		send(Client[j].sock, "Let's start\n", strlen("Let's start\n"), 0);
//		send(Client[j].sock, "You:", strlen("You:"), 0);
//	}
//
//	FD_ZERO(&Mask);
//	for (i = 0; i < n_client; i++) {
//		FD_SET(Client[i].sock, &Mask);
//	}
//
//	while (1) {
//		readfds = Mask;
//		select(Max_sock + 1, &readfds, NULL, NULL, NULL);
//		for (i = 0; i < n_client; i++) {
//			if(Client[i].status == 0){
//				continue;
//			}
//			if (FD_ISSET(Client[i].sock, &readfds)) {
//				/* クライアントから文字列を受信したとき */
//				strsize = recv(Client[i].sock, buf, BUFSIZE - 1, 0);
//				if(strsize <= 0){
//					printf("here");
//					Client[i].status = 0;
//					for (j = 0; j < n_client; j++) {
//						if (j != i) {
//							send(Client[j].sock, red, strlen(red), 0);
//							send(Client[j].sock, Client[i].name, Client[i].len, 0);
//							send(Client[j].sock, " left now\n",strlen(" left now\n"), 0);
//							send(Client[j].sock, "You:", strlen("You:"), 0);
//						}
//					}
//					continue;
//				}
//				buf[strsize] = '\0';
//				//printf("%s: %s",Client[i].name,buf);
//
//				for (j = 0; j < n_client; j++) {
//					if (j != i) {
//						send(Client[j].sock, red, strlen(red), 0);
//						send(Client[j].sock, Client[i].name, Client[i].len, 0);
//						send(Client[j].sock, ": ", 2, 0);
//						send(Client[j].sock, buf, strsize, 0);
//						send(Client[j].sock, black, strlen(black), 0);
//					}
//					send(Client[j].sock, "You:", strlen("You:"), 0);
//				}
//			}
//		}
//	}
//	free(Client);
//}
//
//
