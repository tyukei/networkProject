#include "idobata.h"

/*
 パケットの種類=type のパケットをbufferに作成する
 作成されるパケットのデータを格納するためのメモリ領域は呼び出し前に
 確保しておく必要がある
 */

char*
create_packet(char *buffer, u_int32_t type, char *message) {
	switch (type) {
	case HELO:
		snprintf(buffer, MSGBUF_SIZE, "HELO");
		break;
	case HERE:
		snprintf(buffer, MSGBUF_SIZE, "HERE");
		break;
	case JOIN:
		snprintf(buffer, MSGBUF_SIZE, "JOIN %s", message);
		break;
	case POST:
		snprintf(buffer, MSGBUF_SIZE, "POST %s", message);
		break;
	case MESSAGE:
		snprintf(buffer, MSGBUF_SIZE, "MESG %s", message);
		break;
	case QUIT:
		snprintf(buffer, MSGBUF_SIZE, "QUIT");
		break;
	default:
		/* Undefined packet type */
		return (NULL);
		break;
	}
	return (buffer);
}

// Headerの内容を返す
u_int32_t analyze_header( char *header )
{
  if( strncmp( header, "HELO", 4 )==0 ) return(HELO);
  if( strncmp( header, "HERE", 4 )==0 ) return(HERE);
  if( strncmp( header, "JOIN", 4 )==0 ) return(JOIN);
  if( strncmp( header, "POST", 4 )==0 ) return(POST);
  if( strncmp( header, "MESG", 4 )==0 ) return(MESSAGE);
  if( strncmp( header, "QUIT", 4 )==0 ) return(QUIT);
  return 0;
}
