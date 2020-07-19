#include "parse.h"
#include "game_data.h"
#include "see_data.h"
#include "defs.h"

void see_new(int time)
{
  see.time = time;
  see.player_length = 0;
}

bool_type is_my_name(const char *s) {
  return strcmp(game.my_team_name, s)==0;
}

void see_ball(bool_type really_see, position_spec poss, ball_position *posp)
{
  see.ball.time = see.time;
  see.ball.poss = poss;
  see.ball.pos = *posp;
}

void see_goal(char side, position_spec poss, goal_position *posp)
{
  int dir = side!=game.side ? DIR_FD : DIR_BK ;
  see.goal[dir].time = see.time;
  see.goal[dir].poss = poss;
  see.goal[dir].pos = *posp;
}

void see_player(player_spec ps, bool_type really_see, bool_type is_my_player,
		position_spec poss, player_position *posp) {
  int player_idx = see.player_length++;
  see.player[player_idx].ps = ps;
  see.player[player_idx].is_my_player = is_my_player;
  see.player[player_idx].poss = poss;
  see.player[player_idx].pos = *posp;
}

parse_see_cb pscb = {
  see_new,
  is_my_name,
  see_ball,
  see_goal,
  see_player
};
