#include "mynet.h"
#include <sys/wait.h>

#define PRCS_LIMIT 10 /* プロセス数制限 */
#define BUFSIZE 50   /* バッファサイズ */

int main(int argc, char *argv[]){
    int i;
    int port_number, num_fork;
    int sock_listen, sock_accepted;
    pid_t child;
    char buf[BUFSIZE];
    int strsize;

    /* 引数のチェックと使用法の表示 */
    if(argc != 3){
        fprintf(stderr,"check Port_number Number_of_Fork\n");
        exit(EXIT_FAILURE);
    }

    port_number = atoi(argv[1]);
        num_fork = atoi(argv[2]);

    /* サーバの初期化 */
    sock_listen = init_tcpserver(port_number, 2);

    for(i = 0; i < num_fork-1; i++){
        if((child = fork()) == 0){
            /* Child process */
            break;
        }
        else if(child > 0){
            /* parent's process */
            printf("Client is accepted.[%d]\n", child);
        }
        else{
            /* fork()に失敗 */
            close(sock_listen);
            exit_errmesg("fork()");
        }
    }

    for(;;){
        /* クライアントの接続を受け付ける */
        sock_accepted = accept(sock_listen, NULL, NULL);
          do{
            /* 文字列をクライアントから受信する */
            if((strsize=recv(sock_accepted, buf, BUFSIZE, 0)) == -1){
                  exit_errmesg("recv()");
            }

            /* 文字列をクライアントに送信する */
            if(send(sock_accepted, buf, strsize, 0) == -1 ){
                  exit_errmesg("send()");
            }
        }while( buf[strsize-1] != '\n' ); /* 改行コードを受信するまで繰り返す */

           close(sock_accepted);
    }
    /* never reached */
}
