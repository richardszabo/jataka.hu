#ifndef CONN_H
#define CONN_H

int conn_new(const char* host, int port);
int conn_send_first(const char *msg);
int conn_send(const char *msg);
char *conn_receive_first();
char *conn_receive();
int conn_destroy();

#endif
