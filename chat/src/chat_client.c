#include "chat.h"
#include "mynet.h"
#include <stdlib.h>
#include <sys/select.h>

int Sock;
fd_set Mask;


void chat_client_init(char *servername, int port_number) {
	Sock = init_tcpclient(servername, port_number);
	FD_ZERO(&Mask); //maskの全ビットを0に初期化
	FD_SET(0, &Mask); //maskの第0ビットを1
	FD_SET(Sock, &Mask); //maskの第sockビットを1
}
void chat_client_loop() {
	fd_set readfds;
	char r_buf[BUFSIZE];
	char w_buf[BUFSIZE];
	int strsize;
	//キーボードからの入力があるか、サーバからの入力があるか、を selectにより検査
	while (1) {
		readfds = Mask;
		select(Sock + 1, &readfds, NULL, NULL, NULL);
		if (FD_ISSET(0, &readfds)) {
			/* キーボードから文字列を入力する */
			fgets(w_buf, BUFSIZE, stdin);
			strsize = strlen(w_buf);
			send(Sock, w_buf, strsize, 0);
		}
		//readfdsの第sockビットが 1かどうか(真であればサーバから入力あり)
		if (FD_ISSET(Sock, &readfds)) {
			/* サーバから文字列を受信する */
			strsize = recv(Sock, r_buf, BUFSIZE - 1, 0);
			if (strsize == 0) {
				printf("\n server down");
				exit(0);
			}
			r_buf[strsize] = '\0';
			printf("%s", r_buf);
			fflush(stdout);
		}
	}
}
void chat_client(char *servername, int port_number) {

	chat_client_init(servername,port_number);
	chat_client_loop();
}
