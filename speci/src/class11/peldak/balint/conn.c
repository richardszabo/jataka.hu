#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#include "conn.h"

#define DGRAM_BUF_SIZE 1500

char buf[DGRAM_BUF_SIZE+1];
int sock;
struct sockaddr_in server_addr;

int conn_new(const char* host, int port)
{
  struct hostent *hp;

  /* create datagram socket */
  sock = socket(PF_INET, SOCK_DGRAM, 0);
  if(sock<0) return -1;

  /* determine host IP address by its name */
  hp = gethostbyname(host);
  if(hp==NULL) return -1;

  /* set up server address structure */
  memcpy(hp->h_addr, &server_addr.sin_addr, hp->h_length);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  return 0;
}

int conn_send_first(const char *msg)
{
  return sendto(sock, msg, strlen(msg), 0,
		&server_addr, sizeof(server_addr));
}

int conn_send(const char *msg)
{
  /*fprintf(stderr, msg);*/
  return send(sock, msg, strlen(msg), 0);
}

char *conn_receive_first()
{
  socklen_t len = sizeof(server_addr);
  int ret = recvfrom(sock, buf, DGRAM_BUF_SIZE, 0, &server_addr, &len);
  if(ret<0) return NULL;
  buf[ret] = '\0';
  if(connect(sock, &server_addr, sizeof(server_addr)) < 0) return NULL;
  return buf;
}

char *conn_receive()
{
  int ret = recv(sock, buf, DGRAM_BUF_SIZE, 0);
  if(ret<0) return NULL;
  buf[ret] = '\0';
  return buf;
}

int conn_destroy()
{
  return close(sock);
}
