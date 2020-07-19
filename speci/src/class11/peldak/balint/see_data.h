#ifndef SEE_DATA_H
#define SEE_DATA_H

#include "defs.h"

/* do not change these two constants */
#define DIR_BK 0
#define DIR_FD 1

typedef struct {
  int time;
  position_spec poss;
  ball_position pos;
} ball_data;

typedef struct {
  int time;
  position_spec poss;
  goal_position pos;
} goal_data;

typedef struct {
  player_spec ps;
  bool_type is_my_player;
  position_spec poss;
  player_position pos;
} player_data;

typedef struct {
  int time;
  ball_data ball;
  goal_data goal[2];
  player_data player[2*PLAYER_NUM];
  int player_length;
} see_data;

see_data see;

#endif
