#ifndef LOG_H
#define LOG_H

#include  <stdio.h>

extern FILE *logfile;

FILE *log_open(int unum);
int log_close();

#define RCLOG(s) if(logfile) fputs(s, logfile)

#endif
