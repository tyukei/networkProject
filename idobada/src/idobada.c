#include <stdio.h>
#include <stdlib.h>
#include "mynet.h"
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#define S_BUFSIZE 512   /* 送信用バッファサイズ */
#define R_BUFSIZE 512   /* 受信用バッファサイズ */
#define DEFAULT_PORT 50001 /* ポート番号既定値 */

/*
 *これはidobata会議の client側を実装したプログラムです
 * >$ ./idobada [username] [portnumber]
 * で実行ができます。
 *
 * UDPサーバにHELOを送りHEREが返ってくると
 * send HELO
 * HERE
 * This is client[10.0.2.15]
 * ================================
 * と表示されます。
 *
 * 一方UDPサーバにHELOを送るが、HEREが返ってこないと
 * send HELO
 * send HELO
 * send HELO
 * Cannot find server
 * と表示され修了します。（今後はここでサーバモードに切り替わります）
 *
 * 他の人がメッセージを送信したとき、
 * ================================
 * MESG [aaa] yes
 * ================================
 * とユーザ名と メッセージ内容が表示されます。
 *
 * 苦労した点
 *＊テキストのまま写すと上手く行かず、 UDPサーバとTCPサーバのやり取りするタイミングや使う変数の違いを意識しなければならなかった点
 *（ブロードキャストはUDPしか使えないから始めだけUDP接続するということがわかれば当然なのですが、、、）
 *＊送ることはできるが、受信ができず時間がかかった点（複数のクライアントが繋ることで上手く動きました）
 *＊まず、このような全体の流れを書いたファイルをつくり、そのあと、変数やクラスに分る部分に手間がかかった点
 *
 */
int main(int argc, char *argv[]) {
	struct sockaddr_in broadcast_adrs;
	struct sockaddr_in from_adrs;
	socklen_t from_len;
	char str_helo[4] = "HELO";
	char str_join[20] = "JOIN ";
	char str_here[4] = "HERE";
	char str_mesg[4] = "MESG";
	char str_quit[5] = "QUIT\n";
	char str_post[4] = "POST";
	int count;

	int sock, sock2;//sockはUDPによるsock、sock2はTCPによるsock
	int broadcast_sw = 1;
	fd_set mask, readfds2;
	int port_number = DEFAULT_PORT;
	char *username;
	char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE], p_buf[S_BUFSIZE];
	int strsize;

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

	/* ブロードキャストアドレスの情報をsockaddr_in構造体に格納する */
	set_sockaddr_in_broadcast(&broadcast_adrs, (in_port_t) port_number);

	/* ソケットをDGRAMモードで作成する */
	sock = init_udpclient();

	/* ソケットをブロードキャスト可能にする */
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void*) &broadcast_sw,
			sizeof(broadcast_sw)) == -1) {
		exit_errmesg("setsockopt()");
	}

	//UDPでサーバにHELOを送り、HEREが返ってくるか確認する。
	for (count = 0; count < 3; count++) {
		//Send HELOをUDPsocketに送る
		Sendto(sock, str_helo, strlen(str_helo), 0,
				(struct sockaddr*) &broadcast_adrs, sizeof(broadcast_adrs));
		printf("send %s\n", str_helo);
		//HEREがUDPsocketから返ってくるか確認
		from_len = sizeof(from_adrs);
		strsize = Recvfrom(sock, r_buf, R_BUFSIZE - 1, 0,
				(struct sockaddr*) &from_adrs, &from_len);
		r_buf[strsize] = '\0';
		//check HERE
		if (strcmp(r_buf, str_here) == 0) {
			printf("%s\n", r_buf);
			printf("This is client");
			printf("[%s]\n", inet_ntoa(from_adrs.sin_addr));
			break;
		} else if (count >= 2) {
			//今後ここにserverになる処理を書いていく。
			printf("Cannot find server");
			return 0;
		}
	}

	/* TCPサーバに接続する */
	sock2 = init_tcpclient(inet_ntoa(from_adrs.sin_addr), port_number); /* ←ライブラリの関数を使った */
	/* ビットマスクの準備 */
	FD_ZERO(&mask); //maskの全ビットを0に初期化
	FD_SET(0, &mask); //maskの第0ビットを1
	FD_SET(sock2, &mask);//maskの第sockビットを1

	//JOIN usernameをTCPサーバに送信する
	strcat(str_join, username);
	strsize = strlen(username) + 5;
	if (send(sock2, str_join, strsize, 0) == -1) {
		exit_errmesg("send()");
	}

	//TCPサーバーとやり取りを行う処理
	for (;;) {
		printf("================================\n");
		/* 受信データの有無をチェック */
		readfds2 = mask;
		select(sock2 + 1, &readfds2, NULL, NULL, 0);

		//readfdsの第sock2ビットが 1かどうか(真であればserverから入力あり)
		if (FD_ISSET(sock2, &readfds2)) {
			//printf("server2\n");
			strsize = recv(sock2, r_buf, R_BUFSIZE - 1, 0);
			if (strsize == 0) {
				printf("\n server down");
				exit(0);
			}
			r_buf[strsize] = '\0';
			printf("%s",r_buf);
			//MESG と メッセージ を分割する
			char *token;
			token = strtok(r_buf, " ");
			//printf("%s\n", token);
			if (strcmp(r_buf, str_mesg) == 0) {
				printf("%s\n", token);
			}
			fflush(stdout);
		}

		//readfdsの第0ビットが 1かどうか(真であればkeyboardから入力あり)
		if (FD_ISSET(0, &readfds2)) {
			//printf("keyboard\n");
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

			if (send(sock2, p_buf, strsize, 0) == -1) {
				exit_errmesg("send()");
			}
			printf("%s\n", p_buf);
		}
	}
	close(sock); /* ソケットを閉じる */
	close(sock2);
	exit(EXIT_SUCCESS);
}
