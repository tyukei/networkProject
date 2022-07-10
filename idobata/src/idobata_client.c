#include "idobata.h"
char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE], p_buf[S_BUFSIZE];
struct sockaddr_in broadcast_adrs;
struct sockaddr_in from_adrs;
int broadcast_sw = 1;
socklen_t from_len;
int strsize;
int sock, sock2;
fd_set mask, readfds, readfds2;
struct idobata *packet;
#define TIMEOUT_SEC 2

void idobata_client(char *username, int port_number) {
	idobata_client_init(username, port_number); // client側の初期化を行う
	idobata_client_loop(username, port_number); // client側のループ処理を行う
}

void idobata_client_init(char *username, int port_number) {
	int count;
	struct timeval timeout;

	/* ブロードキャストアドレスの情報をsockaddr_in構造体に格納する */
	set_sockaddr_in_broadcast(&broadcast_adrs, (in_port_t) port_number);

	/* ソケットをDGRAMモードで作成する */
	sock = init_udpclient();

	/* ソケットをブロードキャスト可能にする */
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void*) &broadcast_sw,
			sizeof(broadcast_sw)) == -1) {
		exit_errmesg("setsockopt()");
	}

	FD_ZERO(&mask);
	FD_SET(sock, &mask);

	//Send HELO
	//Get HERE
	for (count = 1; count <= 3; count++) {
		readfds = mask;
		timeout.tv_sec = TIMEOUT_SEC;
		timeout.tv_usec = 0;

		//Send HELO
		create_packet(s_buf, HELO, NULL);
		Sendto(sock, s_buf, strlen(s_buf), 0,
				(struct sockaddr*) &broadcast_adrs, sizeof(broadcast_adrs));
		printf("***S: %s[%s (%d)]\n", s_buf, inet_ntoa(broadcast_adrs.sin_addr),
				port_number);

		if (select(sock + 1, &readfds, NULL, NULL, &timeout) != 0) {
			from_len = sizeof(from_adrs);
			strsize = Recvfrom(sock, r_buf, R_BUFSIZE - 1, 0,
					(struct sockaddr*) &from_adrs, &from_len);
			r_buf[strsize] = '\0';
			fflush(stdout);
			packet = (struct idobata*) r_buf;
			//check HERE
			if (analyze_header(packet->header) == HERE) {
				printf("***R: %s\n", r_buf);
				printf("Now I am a client.[%s(%d)]\n",
						inet_ntoa(from_adrs.sin_addr), port_number);
				break;
			}
		}
		if (count >= 3) {
			printf("Now I am a server.\n");
			idobata_server(username, port_number);
		}
	}
}

void idobata_client_loop(char *username, int port_number) {
	char str_join[20] = "JOIN ";
	char str_quit[5] = "QUIT\n";
	char str_post[4] = "POST";
	/* TCPサーバに接続する */
	sock2 = init_tcpclient(inet_ntoa(from_adrs.sin_addr), port_number); /* ←ライブラリの関数を使った */
	/* ビットマスクの準備 */
	FD_ZERO(&mask); //maskの全ビットを0に初期化
	FD_SET(0, &mask); //maskの第0ビットを1
	FD_SET(sock2, &mask);

	readfds2 = mask;
	select(sock2 + 1, &readfds2, NULL, NULL, 0);
	if (FD_ISSET(sock2, &readfds2)) {
		strsize = Recv(sock2, r_buf, R_BUFSIZE - 1, 0);
		r_buf[strsize] = '\0';
		printf("***R: %s", r_buf);
		//Send JOIN username
		strcat(str_join, username);
		strsize = strlen(username) + 5;
		Send(sock2, str_join, strsize, 0);
		printf("***S: %s\n", str_join);
	}
//	//Send JOIN username
//	strcat(str_join, username);
//	strsize = strlen(username) + 5;
//	Send(sock2, str_join, strsize, 0);
//	printf("***S: %s\n", str_join);

	for (;;) {
		//printf("================================\n");
		/* 受信データの有無をチェック */
		readfds2 = mask;
		select(sock2 + 1, &readfds2, NULL, NULL, 0);

		//server
		if (FD_ISSET(sock2, &readfds2)) {
			printf("server2\n");
			strsize = Recv(sock2, r_buf, R_BUFSIZE - 1, 0);
			if (strsize == 0) {
				printf("\n server down");
				exit(0);
			}
			r_buf[strsize] = '\0';
			printf("***R: %s", r_buf);
			fflush(stdout);
		}

		//readfdsの第0ビットが 1かどうか(真であればkeyboardから入力あり)
		if (FD_ISSET(0, &readfds2)) {
			printf("keyboard\n");
			fgets(s_buf, S_BUFSIZE, stdin);
			// QUITかどうか確認する
			if (strcmp(s_buf, str_quit) == 0) {
				Sendto(sock2, s_buf, strsize, 0,
						(struct sockaddr*) &broadcast_adrs,
						sizeof(broadcast_adrs));
				close(sock);
				exit(EXIT_SUCCESS);
			}
			sprintf(p_buf, "%s %s", str_post, s_buf);
			strsize = strlen(p_buf);
			p_buf[strsize - 1] = '\0';
			Sendto(sock, p_buf, strsize, 0, (struct sockaddr*) &broadcast_adrs,
					sizeof(broadcast_adrs));

			if (send(sock2, p_buf, strsize, 0) == -1) {
				exit_errmesg("send()");
			}
			printf("%s\n", p_buf);
		}

	}
	close(sock); /* ソケットを閉じる */
	close(sock2);
}

