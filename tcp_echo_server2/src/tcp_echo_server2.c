#include "mynet.h"


#define BUFSIZE 50   /* バッファサイズ */
const char command[3][10] = {"list","type","exit"};
const char command_call[2][11] = {"ls ~/work","cat ~/work/"};

int main(int argc, char *argv[])
{
  int sock_listen, sock_accepted;
  FILE * fp;
  char buf[BUFSIZE];
  char com_buf[BUFSIZE];
  char *p;
  int strsize,port;

  //コマンドラインの処理
  if(argc == 2){
	  port = atoi(argv[1]);
  }else{
	  fprintf(stderr, "Invalid number of argument (%d). \nUsage: %s URI [ProxyServer ProxyPort]\n", argc, argv[0]);
	  return EXIT_FAILURE;
  }

  //クライアントからの接続を準備する
  sock_listen = init_tcpserver(port, 5); //ライブラリの使用
  sock_accepted = accept(sock_listen, NULL, NULL);
  close(sock_listen);

  // password処理
  if((send(sock_accepted, "\npassword:", strlen("\npassword:"), 0)) == -1){
      exit_errmesg("send()");
  }
  if((strsize=recv(sock_accepted, buf, BUFSIZE, 0)) == -1){
      exit_errmesg("recv()");
  }
  if(strncmp(buf, "0708", 4) != 0){
	  fprintf(stderr, "%s is worng password \n", buf);
	  return EXIT_FAILURE;
  }

  do{

	  // ">"を送信する
      if((send(sock_accepted, "\n>", strlen("\n>"), 0)) == -1){
          exit_errmesg("send()");
      }
      /* 文字列をクライアントから受信する */
      if((strsize=recv(sock_accepted, buf, BUFSIZE, 0)) == -1){
          exit_errmesg("recv()");
      }
     // command == list の処理
      if(strncmp(buf, command[0], 4) == 0){
    	  if(strstr(buf,"../") == NULL){
    		  fp = popen(command_call[0], "r");
    		            while(fgets(buf, BUFSIZE, fp) != NULL) {
    		                printf("%s", buf);
    		                send(sock_accepted, buf, strlen(buf),0);
    		            }
    		            strcpy(buf, "\0");
    	  }else{
    		  send(sock_accepted, "cannot access there.\n", strlen("cannot access there.\n"),0);
    		  continue;
    	  }

      // command == type の処理
      }else if(strncmp(buf, command[1], 4) == 0){
    	  if(strstr(buf,"../") != NULL){
    		  send(sock_accepted, "cannot access there.\n", strlen("cannot access there.\n"),0);
    		  continue;
    	  }
          strcpy(com_buf, command_call[1]); // combuf = "cat ~/work/"
          strcat(com_buf, &buf[5]); // "cat ~/work/" + filename
          p = strstr(com_buf, "\r"); // find "\r" in com_buf
          *p = '\0';
          printf("%s\n", com_buf);
          if((fp = popen(com_buf, "r")) != NULL){
              while(fgets(buf, BUFSIZE, fp) != NULL){
                  send(sock_accepted, buf, strlen(buf),0);
              };
              strcpy(buf, "\0");
          }else{
              send(sock_accepted, "no such dir.\n", strlen("no such dir.\n"),0);
          }
      }else if(strncmp(buf, command[2], 4) != 0){
          send(sock_accepted, "no such command.\n", strlen("no such command.\n"),0);
      }
  }while(strncmp(buf, "exit", 4) != 0);
  pclose(fp);
  close(sock_accepted);

  exit(EXIT_SUCCESS);
}
