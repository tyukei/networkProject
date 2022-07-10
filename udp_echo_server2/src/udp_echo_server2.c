/*
  echo_server.c (UDP版)
*/
#include "mynet.h"
#include <arpa/inet.h>

#define BUFSIZE 5   /* バッファサイズ */
int main(int argc, char *argv[])
{
  struct sockaddr_in from_adrs;

  in_port_t port_number;
  int udp_sock_listen, tcp_sock_listen, sock_accepted;
  socklen_t from_len;

  char buf[BUFSIZE];
  int strsize;

  /* 引数のチェックと使用法の表示 */
  if( argc != 2 ){
    fprintf(stderr,"Usage: %s Port_number\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  port_number = atoi(argv[1]);
  /* UDPサーバの初期化 */
	udp_sock_listen = init_udpserver((in_port_t) port_number);
	tcp_sock_listen = init_tcpserver(port_number, 5);
	sock_accepted = tcp_sock_listen;


  for(;;){
    /* 文字列をクライアントから受信する */
    from_len = sizeof(from_adrs);
    if((strsize=recvfrom(sock, buf, BUFSIZE, 0,
			 (struct sockaddr *)&from_adrs, &from_len)) == -1){
      exit_errmesg("recvfrom()");
    }

    /* 文字列をクライアントに送信する */
    if(sendto(sock, buf, strsize, 0,
	      (struct sockaddr *)&from_adrs, sizeof(from_adrs)) == -1 ){
      exit_errmesg("sendto()");
    }
  }

  close(sock);

  exit(EXIT_SUCCESS);
}

