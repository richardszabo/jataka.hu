#include "parse.h"
#include "game_data.h"

static void cmd_init(char side, int unum)
{
  game.side = side;
  game.unum = unum;
}

void game_init(const char *my_team_name) {
  game.my_team_name = my_team_name;
}

parse_init_cb picb = {
  cmd_init
};
