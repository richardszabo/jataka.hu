#include "log.h"

FILE *logfile=0;

FILE *log_open(int unum)
{
  char logf_name[80];

  sprintf(logf_name, "player-%d.log", unum);
  logfile = fopen(logf_name, "w");
  return logfile;
}

int log_close()
{
  return logfile ? fclose(logfile) : 0;
}
