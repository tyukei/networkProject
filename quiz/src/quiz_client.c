#include "quiz.h"
#include "mynet.h"
#include <stdlib.h>
#include <sys/select.h>

#define S_BUFSIZE 100   /* 送信用バッファサイズ */
#define R_BUFSIZE 100   /* 受信用バッファサイズ */

void quiz_client(char* servername, int port_number)
{
  int sock;
//  int status;
  char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
//  char e_buf[R_BUFSIZE];
  char error_message[]="server is down";
  int strsize;
  fd_set mask, readfds;

  /* サーバに接続する */
  sock = init_tcpclient(servername,port_number); /* ←ライブラリの関数を使った */
  printf("Connected.\n");

  /* ビットマスクの準備 */
  FD_ZERO(&mask);
  FD_SET(0, &mask);
  FD_SET(sock, &mask);

  for(;;){

    /* 受信データの有無をチェック */
    readfds = mask;
    select( sock+1, &readfds, NULL, NULL, NULL );

    if( FD_ISSET(0, &readfds) ){
      /* キーボードから文字列を入力する */
      fgets(s_buf, S_BUFSIZE, stdin);
      strsize = strlen(s_buf);
      Send(sock, s_buf, strsize, 0);
    }

    if( FD_ISSET(sock, &readfds) ){
      /* サーバから文字列を受信する */
      strsize = Recv(sock, r_buf, R_BUFSIZE-1, 0);
//      printf("strsize = %d\n",strsize);
      if(strsize == 0){
    	  printf("\n%s",error_message);
    	  exit(0);
      }
      r_buf[strsize] = '\0';
      printf("%s",r_buf);
      fflush(stdout);
    }
  }

}
