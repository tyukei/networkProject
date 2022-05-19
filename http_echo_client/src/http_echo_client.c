#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1024

int main(int argc, const char *argv[])
{
	uint i = 0;
  struct hostent *server_host;
  struct sockaddr_in server_adrs;
  char url[BUFSIZE];
  char s_buf[BUFSIZE], r_buf[BUFSIZE];
  int sock;
  int strsize;
  char *p_servername;
  char servername[BUFSIZE];
  char port[BUFSIZE];
  char rescode[BUFSIZE];
  char *p_rescode;
  char contleng[BUFSIZE];
  char *p_contleng;
  char program[BUFSIZE];
  char *p_program;
  char location[BUFSIZE];
  char *p_location;

  // check argumet number
  if(argc != 2 && argc != 4){
  		fprintf(stderr, "Invalid number of argument (%d). \nUsage: %s URI [ProxyServer ProxyPort]\n", argc, argv[0]);
  		return EXIT_FAILURE;
  	}

  strcpy(url,argv[1]);
  if(argc == 2){
	  p_servername = strstr(url, "://");
	  strcpy(servername,&p_servername[3]);
	  for(i=0;i<strlen(servername);i++){
	    if(servername[i]=='/'){
	      servername[i]='\0';
	      break;
	    }
	  }
	  strcpy(port,"80");
  }else{
	  strcpy(servername,argv[2]);
	  strcpy(port, argv[3]);
  }



  /* サーバ名をアドレス(hostent構造体)に変換する */
  if((server_host = gethostbyname( servername )) == NULL){
    fprintf(stderr,"gethostbyname()");
    exit(EXIT_FAILURE);
  }

  /* サーバの情報をsockaddr_in構造体に格納する */
  memset(&server_adrs, 0, sizeof(server_adrs));
  server_adrs.sin_family = AF_INET;
  server_adrs.sin_port = htons(atoi(port));
  memcpy(&server_adrs.sin_addr, server_host->h_addr_list[0], server_host->h_length);

  /* ソケットをSTREAMモードで作成する */
  if((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1){
    fprintf(stderr,"socket()");
    exit(EXIT_FAILURE);
  }

  /* ソケットにサーバの情報を対応づけてサーバに接続する */
  if(connect(sock, (struct sockaddr *)&server_adrs, sizeof(server_adrs))== -1){
    fprintf(stderr,"connect");
    exit(EXIT_FAILURE);
  }

  /* 文字列をサーバに送信する */
  strcpy(s_buf, "GET ");
  strcat(s_buf, url);
  strcat(s_buf," HTTP/1.1\r\n");
  if(send(sock,s_buf,strlen(s_buf),0) == -1){
	    fprintf(stderr,"send()");
	    exit(EXIT_FAILURE);
  }
	strcpy(s_buf, "Host:");
	strcat(s_buf, servername);
	strcat(s_buf, "\r\n");
  if(send(sock,s_buf,strlen(s_buf),0) == -1){
	    fprintf(stderr,"send()");
	    exit(EXIT_FAILURE);
  }

  send(sock,"\r\n",2,0);

  /* サーバから文字列を受信する */
  if((strsize=recv(sock, r_buf, BUFSIZE-1, 0)) == -1){
    fprintf(stderr,"recv()");
    exit(EXIT_FAILURE);
  }
  r_buf[strsize] = '\0';

  //printf("%s\n",r_buf);      /* 受信した文字列を画面に書く */

  p_rescode = strstr(r_buf, "HTTP/1.1 ");
	  strcpy(rescode,&p_rescode[9]);
 	  i = 0;
 	  while(i<strlen(rescode)){
 		  if(rescode[i] == '\n'){
 			  rescode[i] = '\0';
 			  break;
 		  }
 		  i++;
 	  }

 	  switch(atoi(rescode)){
 	  case 301:
 		  p_location = strstr(r_buf, "Location: ");
 		  strcpy(location,&p_location[10]);
 		 i = 0;
 		 	  while(i<strlen(location)){
 		 		  if(location[i] == '\n'){
 		 			  location[i] = '\0';
 		 			  break;
 		 		  }
 		 		  i++;
 		 	  }
 		  printf("Redirect to %s",location);
 		  break;
 	  case 400 ... 499:
	    fprintf(stderr,"Client Error %d",atoi(rescode));
	    exit(EXIT_FAILURE);
	    break;
 	  case 500 ... 599:
	    fprintf(stderr,"Server Error %d",atoi(rescode));
	    exit(EXIT_FAILURE);
 	  }
 	  printf("%s",rescode);

  p_contleng = strstr(r_buf, "Content-Length:");
	  strcpy(contleng,&p_contleng[16]);
 	  i = 0;
 	  while(i<strlen(contleng)){
 		  if(contleng[i] == '\n'){
 			  contleng[i] = '\0';
 			  break;
 		  }
 		  i++;
 	  }
 	  printf("Content Length : %s", contleng);

 	  p_program = strstr(r_buf, "Server:");
 		  strcpy(program,&p_program[7]);
 	 	   i = 0;
 	 	  while(i<strlen(program)){
 	 		  if(program[i] == '\n'){
 	 			  program[i] = '\0';
 	 			  break;
 	 		  }
 	 		  i++;
 	 	  }
 	 	  printf("Server :%s\n", program);

  close(sock);             /* ソケットを閉じる */

  exit(EXIT_SUCCESS);
}
