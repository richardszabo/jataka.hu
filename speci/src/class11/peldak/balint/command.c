#include <stdio.h>

#include "command.h"
#include "conn.h"
#include "log.h"

#define SBUF_SIZE 1000

char s[SBUF_SIZE];

int init(const char *team_name)
{
  int ret = snprintf(s, SBUF_SIZE, "(init %s)\n", team_name);
  if(ret<0 || ret>=SBUF_SIZE) return -1;	/* some implementations return -1, some return the too long size */
  RCLOG(s);
  return conn_send_first(s);
}

int turn(float moment)
{
  int ret = snprintf(s, SBUF_SIZE, "(turn %f)\n", moment);
  if(ret<0 || ret>=SBUF_SIZE) return -1;	/* some implementations return -1, some return the too long size */
  RCLOG(s);
  return conn_send(s);
}

int dash(float power)
{
  int ret = snprintf(s, SBUF_SIZE, "(dash %f)\n", power);
  if(ret<0 || ret>=SBUF_SIZE) return -1;	/* some implementations return -1, some return the too long size */
  RCLOG(s);
  return conn_send(s);
}

int kick(float power, float direction)
{
  int ret = snprintf(s, SBUF_SIZE, "(kick %f %f)\n", power, direction);
  if(ret<0 || ret>=SBUF_SIZE) return -1;	/* some implementations return -1, some return the too long size */
  RCLOG(s);
  return conn_send(s);
}
