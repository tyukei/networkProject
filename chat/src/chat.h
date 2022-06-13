#ifndef CHAT_H_
#define CHAT_H_
/*
  chat.h
*/
#include "mynet.h"
#define NAMELENGTH 20   /* 名前サイズ */
#define BUFSIZE 1024


/* サーバメインルーチン */
void chat_server(int port_number, int n_client);

/* クライアントメインルーチン */
void chat_client(char* servername, int port_number);

void chat_server(int port_number, int n_client);
void chat_init_server(int n_client);
void chat_server_loop(int n_client);

void chat_client(char *servername, int port_number);
void chat_client_init(char *servername, int port_number);
void chat_client_loop();

#endif /* CHAT_H_ */
