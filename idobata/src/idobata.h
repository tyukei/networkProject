#ifndef IDOBATA_H_
#define IDOBATA_H_

#include <stdio.h>
#include <stdlib.h>
#include "mynet.h"
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define S_BUFSIZE 512   /* 送信用バッファサイズ */
#define R_BUFSIZE 512   /* 受信用バッファサイズ */
#define DEFAULT_PORT 50001 /* ポート番号既定値 */
#define L_USERNAME 15
#define MSGBUF_SIZE 512
#define HELO    1
#define HERE    2
#define JOIN    3
#define POST    4
#define MESSAGE 5
#define QUIT    6
#define USER_LEN 15

struct idobata {
	char header[4]; /* パケットのヘッダ部(4バイト) */
	char sep; /* セパレータ(空白、またはゼロ) */
	char data[]; /* データ部分(メッセージ本体) */
};

typedef struct _imember {
	char username[L_USERNAME]; /* ユーザ名 */
	int sock; /* ソケット番号 */
	struct _imember *next; /* 次のユーザ */
} imember;


int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int Send(int s, void *buf, size_t len, int flags);
int Recv(int s, void *buf, size_t len, int flags);


char* create_packet(char *buffer, u_int32_t type, char *message);
u_int32_t analyze_header( char *header );

void idobata_client_init(char *username, int port_number);
void idobata_client(char *username, int port_number);
void idobata_client_loop(char *username, int port_number);

void idobata_server_init(int port_number, char *servername,int n_client);
void idobata_server_loop();
void idobata_server(char *servername,int port_number);
int connect_client(int sock_listen, char *username);
char* chop_nl(char *s);
int create_member(int I_tcp_lsock);
void set_username(char username[]);

#endif /* IDOBATA_H_ */
