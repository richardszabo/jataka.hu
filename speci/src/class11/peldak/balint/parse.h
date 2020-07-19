#ifndef PARSE_H
#define PARSE_H

#include "defs.h"

#define CMD_ERR 0
#define CMD_UNK 1
#define CMD_UNK_ERR -1
#define CMD_INIT 2
#define CMD_INIT_ERR -2
#define CMD_SEE 3
#define CMD_SEE_ERR -3

typedef struct {
  void (*cmd_init)(char side, int unum);
} parse_init_cb;

typedef enum{PL_NONE, PL_TEAM} player_spec;

typedef struct {
  float distance;
  float direction;
} obj_position;

typedef obj_position goal_position;
typedef obj_position flag_position;

typedef struct {
  float distance;
  float direction;
  float dist_change;
  float dir_change;
} ball_position;

typedef struct {
  float distance;
  float direction;
  float dist_change;
  float dir_change;
  float body_facing_dir;
  float head_facing_dir;
} position;

typedef position player_position;

typedef enum {POS_NONE, POS_DIR, POS_DISTDIR, POS_DIST_CHANGE, POS_DIR_CHANGE,
	      POS_BODY_FACING, POS_HEAD_FACING}  position_spec;

typedef struct {
  void (*see_new)(int time);
  bool_type (*is_my_name)(const char *s);
  void (*see_ball)(bool_type really_see, position_spec poss, ball_position *posp);
  void (*see_goal)(char side, position_spec poss, goal_position *posp);
  void (*see_player)(player_spec pls, bool_type really_see, bool_type is_my_player,
		     position_spec poss, player_position *posp);
} parse_see_cb;

extern parse_init_cb picb;	/* needs to be defined elsewhere! */
extern parse_see_cb pscb;	/* needs to be defined elsewhere! */

const char *parse_errstr;

int parse(char *s);

#endif
