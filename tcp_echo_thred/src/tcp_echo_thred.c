#include "mynet.h"
#include <pthread.h>

#define BUFSIZE 50   /* バッファサイズ */

void * echo_thread(void *arg);

int main(int argc, char *argv[]){
	int i;
    int port_number,num_thread;
    int sock_listen;
    int *tharg;

    /* 引数のチェックと使用法の表示 */
    if( argc != 3 ){
        fprintf(stderr,"Usage: Port_number Number_of_Thread\n");
        exit(EXIT_FAILURE);
    }

    port_number = atoi(argv[1]);
    num_thread = atoi(argv[2]);

    pthread_t threads[num_thread];

    /* サーバの初期化 */
    sock_listen = init_tcpserver(port_number, 2);

    for(i = 0; i < num_thread; i++){
        /* スレッド関数の引数を用意する */
        if((tharg = (int*)malloc(sizeof(int))) == NULL){
              exit_errmesg("malloc()");
        }
        *tharg = sock_listen;
        if(pthread_create(&threads[i], NULL, echo_thread, (void *)tharg) != 0){
            exit_errmesg("pthread_create()");
        }
     }

    while(1){}

  /* never reached */
}

/* スレッドの本体 */
void * echo_thread(void *arg)
{
    int sock, sock_accept;
    char buf[BUFSIZE];
    int strsize;

    sock = *((int *)arg);
    free(arg);/* 引数用のメモリを開放 */

    pthread_detach(pthread_self()); /* スレッドの分離(終了を待たない) */

    while(1){
      do{
        sock_accept = accept(sock, NULL, NULL);//sock = sock_listen
        /* 文字列をクライアントから受信する */
        if((strsize=recv(sock_accept, buf, BUFSIZE, 0)) == -1){
            exit_errmesg("recv()");
        }

        /* 文字列をクライアントに送信する */
        if(send(sock_accept, buf, strsize, 0) == -1 ){
            exit_errmesg("send()");
        }
      }while(buf[strsize-1] != '\n'); /* 改行コードを受信するまで繰り返す */

      close(sock_accept);   /* ソケットを閉じる */
    }
    return(NULL);
}
