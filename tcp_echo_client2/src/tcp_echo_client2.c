#include "mynet.h"

#define S_BUFSIZE 100   /* 送信用バッファサイズ */
#define R_BUFSIZE 100   /* 受信用バッファサイズ */

int main(int argc, char *argv[])
{
  int port,sock;
  char servername[] = "localhost";
  char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
  int strsize;

  if(argc == 2){
	  port = atoi(argv[1]);
  }else{
	  fprintf(stderr, "Invalid number of argument (%d). \nUsage: %s URI [ProxyServer ProxyPort]\n", argc, argv[0]);
	  return EXIT_FAILURE;
  }

  /* サーバに接続する */
  sock = init_tcpclient(servername,port); /* ←ライブラリの関数を使った */

  /* キーボードから文字列を入力する */
  fgets(s_buf, S_BUFSIZE, stdin);
  strsize = strlen(s_buf);

  /* 文字列をサーバに送信する */
  if( send(sock, s_buf, strsize, 0) == -1 ){
    exit_errmesg("send()");
  }

  /* サーバから文字列を受信する */
  do{
	  if((strsize=recv(sock, r_buf, R_BUFSIZE-1, 0)) == -1){
      exit_errmesg("recv()");
    }
    r_buf[strsize] = '\0';
    printf("%s",r_buf);
  }while( r_buf[strsize-1] != '\n' );

  close(sock);             /* ソケットを閉じる */

  exit(EXIT_SUCCESS);
}
