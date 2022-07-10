#include "idobata.h"
int I_udp_lsock, I_tcp_lsock, I_tcp_asock;
fd_set Mask;  //セットされたmask
imember *Client; //client情報の構造体
imember Head;
int I_nclient = 0; //client数
int I_maxsock = 0; // select の監視範囲の最大値

//TCPサーバでクライアントからの接続を受け付けるための処理
int create_member(int I_tcp_lsock) {
	I_nclient += 1;
	//printf("number of clinet [%d]\n", I_nclient);
	// imember *Client[I_nclient]の宣言
//	if ((Client = (imember*) malloc(I_nclient * sizeof(imember))) == NULL) {
//		exit_errmesg("malloc()");
//	}
	I_tcp_asock = Accept(I_tcp_lsock, NULL, NULL);
	if (I_nclient > 1) {
		imember *new, *p, *prev;
		new = (imember*) malloc(sizeof(imember));
		if (new == NULL) {
			return -1;
		}
		new->sock = I_tcp_asock;
		new->next = NULL;
		prev = &Head;
		for (p = Head.next; p != NULL; p = p->next) {
			prev = p;
		}
		prev->next = new;
	} else if (I_nclient == 1) {
		Head.sock = I_tcp_asock;
		Head.next = NULL;
	}
	I_maxsock = I_tcp_asock;
	printf("sock accept [%d]\n", I_tcp_asock);
	FD_SET(I_tcp_asock, &Mask);
	return I_tcp_asock;
}

void idobata_server_init(int port_number, char *servername, int I_nclient) {
	/* サーバの初期化 */
	I_udp_lsock = init_udpserver((in_port_t) port_number); //udpの初期化
	I_tcp_lsock = init_tcpserver(port_number, 5); //tcpの初期化
	I_tcp_asock = I_tcp_lsock; //sock_acceptの初期化
	I_maxsock = I_tcp_lsock; //selctの最大範囲の初期化
	printf("set server sock\n");

	FD_ZERO(&Mask); //Maskを0にセットする
	FD_SET(0, &Mask); //0番目を1にセットする
	FD_SET(I_udp_lsock, &Mask); //udp_sock番目を1にセットする
	FD_SET(I_tcp_lsock, &Mask); //tcp_sock番目を1にセットする
	FD_SET(I_tcp_asock, &Mask); //tcp_sock番目を1にセットする
	printf("set mask\n");
}

void idobata_server_loop() {
	socklen_t from_len;
	struct sockaddr_in from_adrs;
	char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE], k_buf[S_BUFSIZE];
	int r_strsize;
	int i, k;
	struct idobata *packet;
	for (;;) {
		fd_set readfds = Mask; //selectのmaskのセット
		select(I_maxsock + 1, &readfds, NULL, NULL, NULL);

		//keyboardから文字列を受信したとき
		if (FD_ISSET(0, &readfds)) {
			fgets(k_buf, S_BUFSIZE, stdin);
			create_packet(s_buf, MESSAGE, k_buf);
			//packet = (struct idobata *)k_buf;
			Send(I_tcp_lsock, s_buf, strlen(s_buf), 0);
			continue;
		}

		//クライアントからブロードキャスト(udp)で文字列を受信したとき
		if (FD_ISSET(I_udp_lsock, &readfds)) {
			from_len = sizeof(from_adrs);
			r_strsize = Recvfrom(I_udp_lsock, r_buf, R_BUFSIZE - 1, 0,
					(struct sockaddr*) &from_adrs, &from_len);
			r_buf[r_strsize] = '\0';
			packet = (struct idobata*) r_buf; //受信内容をidobata型にキャストする

			//受信内容をアルファベットと情報とで分けて解析する
			switch (analyze_header(packet->header)) {
			case HELO:
				printf("***R: HELO\n");
				fflush(stdout);
				create_packet(s_buf, HERE, NULL); //HEREパケットを生成する
				fflush(stdout);
				Sendto(I_udp_lsock, s_buf, strlen(s_buf), 0,
						(struct sockaddr*) &from_adrs, sizeof(from_adrs)); //HEREを送信する
				printf("***S: HERE\n");
				//I_tcp_asock = connect_client(I_tcp_lsock, packet->data);
				//printf("connect sock[%d] data[%s]\n",I_tcp_lsock,packet->data);
				break;
			default:
				break;
			}
		}
		//tcp
		if (FD_ISSET(I_tcp_lsock, &readfds)) {
			I_tcp_asock = create_member(I_tcp_lsock);
			Send(I_tcp_asock, "accept ok\n", strlen("accept ok\n"), 0);
		}
//		if (FD_ISSET(I_tcp_asock, &readfds)) { //受け取った文字列を表示
//			r_strsize = Recv(I_tcp_asock, r_buf, R_BUFSIZE - 1, 0);
//			r_buf[r_strsize] = '\0';
//			packet = (struct idobata*) r_buf;
//			printf("***R: %s\n", r_buf);
//			fflush(stdout);
//			switch (analyze_header(packet->header)) {
//			case JOIN:
//				fflush(stdout);
////				client_login(Client, I_nclient, packet->data);
//				set_username(packet->data);
//				break;
//			default:
//				printf("TCPSOCKOK\n");
//				fflush(stdout);
//				break;
//			}
//		}
		for (i = 0; i < I_nclient; i++) {
//			if (FD_ISSET(Client[i].sock, &readfds)) {
//				r_strsize = Recv(Client[i].sock, r_buf, R_BUFSIZE - 1, 0);
			imember *p = &Head;
			int j;
			for (j = 0; j < i; j++) {
				p = p->next;
			}
			if (FD_ISSET(p->sock, &readfds)) {
				r_strsize = Recv(p->sock, r_buf, R_BUFSIZE - 1, 0);
				r_buf[r_strsize] = '\0';
				printf("***R: %s\n", r_buf);
				packet = (struct idobata*) r_buf; //受信内容をidobata型にキャストする

				//受信内容をアルファベットと情報とで分けて解析する
				switch (analyze_header(packet->header)) {
				case JOIN:
					fflush(stdout);
					set_username(packet->data);
					break;
				case POST:
					for (k = 0; k < I_nclient; k++) {
						imember *q = &Head;
						if (k != i) {
//							Send(Client[k].sock, r_buf, strlen(r_buf), 0);
							Send(q->sock,r_buf,strlen(r_buf),0);
						}
						q=q->next;
					}
					printf("***S: POST");
					break;
				case QUIT:
					printf("***R: QUIT");
					break;
				default:
					break;
				}
			}
		}
	}
}

//void client_login(imember *user, int I_nclient, char *username) {
//	strncpy(user[I_nclient - 1].username, username, USER_LEN);
//}
void set_username(char username[]) {
	imember *next, *prev;
	prev = &Head;
	for (next = Head.next; next != NULL; next = next->next) {
		prev = next;
	}
	strncpy(prev->username, username, USER_LEN);
}

void idobata_server(char *servername, int port_number) {
	idobata_server_init(port_number, servername, I_nclient);
	idobata_server_loop();
}
