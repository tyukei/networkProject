/*
  echo_server.c (UDP版)
*/
#include "mynet.h"
#include <arpa/inet.h>

#define BUFSIZE 5   /* バッファサイズ */
void show_adrsinfo(struct sockaddr_in *adrs_in);
int main(int argc, char *argv[])
{
  //struct sockaddr_in my_adrs;
  struct sockaddr_in from_adrs;

  in_port_t port_number;
  int sock;
  socklen_t from_len;

  char buf[BUFSIZE];
  int strsize;

  /* 引数のチェックと使用法の表示 */
  if( argc != 2 ){
    fprintf(stderr,"Usage: %s Port_number\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  port_number = (in_port_t)atoi(argv[1]);
//
//  /* サーバ(自分自身)の情報をsockaddr_in構造体に格納する */
//  memset(&my_adrs, 0, sizeof(my_adrs));
//  my_adrs.sin_family = AF_INET;
//  my_adrs.sin_port = htons(port_number);
//  my_adrs.sin_addr.s_addr = htonl(INADDR_ANY);
//
//  /* ソケットをDGRAMモードで作成する */
//  if((sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
//    exit_errmesg("socket()");
//  }
//
//  /* ソケットに自分自身のアドレス情報を結びつける */
//  if(bind(sock, (struct sockaddr *)&my_adrs, sizeof(my_adrs)) == -1 ){
//    exit_errmesg("bind()");
//  }

  /* UDPサーバの初期化 */
    sock = init_udpserver(port_number);

  for(;;){
    /* 文字列をクライアントから受信する */
    from_len = sizeof(from_adrs);
    if((strsize=recvfrom(sock, buf, BUFSIZE, 0,
			 (struct sockaddr *)&from_adrs, &from_len)) == -1){
      exit_errmesg("recvfrom()");
    }
    show_adrsinfo(&from_adrs);

    /* 文字列をクライアントに送信する */
    if(sendto(sock, buf, strsize, 0,
	      (struct sockaddr *)&from_adrs, sizeof(from_adrs)) == -1 ){
      exit_errmesg("sendto()");
    }
  }

  close(sock);

  exit(EXIT_SUCCESS);
}

void show_adrsinfo(struct sockaddr_in *adrs_in)
{
  int  port_number;
  char ip_adrs[20];

  strncpy(ip_adrs, inet_ntoa(adrs_in->sin_addr), 20);
  port_number = ntohs(adrs_in->sin_port);

  printf("%s[%d]\n",ip_adrs,port_number);
}
