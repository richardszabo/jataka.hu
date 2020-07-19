/*
  b0 - Simple robocup client

  Connects to the server, sends a command and prints received data
 */

#include <stdio.h>

#include "conn.h"
#include "command.h"

int main(int argc, char *argv[])
{
  char *s;
  if(conn_new("localhost", 6000) < 0) return perror("conn_new"), 1;
  if(init("b0") < 0) return perror("init"), 1;
  s = conn_receive_first();
  if(s==NULL) return perror("conn_receive_first"), 1;
  printf(s);
  if(turn(90) < 0) return perror("turn"), 1;
  s = conn_receive();
  if(s==NULL) return perror("conn_receive"), 1;
  printf(s);
  if(conn_destroy() < 0) return perror("conn_destroy"), 1;
  return 0;
}
