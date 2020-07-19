#ifndef COMMAND_H
#define COMMAND_H

int init(const char *team_name);

int turn(float moment);

int dash(float power);

int kick(float power, float direction);

#endif
