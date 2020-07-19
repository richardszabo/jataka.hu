#ifndef GAME_DATA_H
#define GAME_DATA_H

void game_init(const char *my_team_name);

typedef struct {
  char side;
  int unum;
  const char *my_team_name;
} game_data;

game_data game;

#endif
