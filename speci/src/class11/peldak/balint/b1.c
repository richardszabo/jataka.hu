/*

  b1 - chaser

  This robot looks for the ball and runs to it.
  If it is already near the ball, it kicks into it.

 */

#include <stdio.h>

#include "conn.h"
#include "command.h"
#include "parse.h"
#include "game_data.h"
#include "see_data.h"

int main(int argc, char *argv[])
{
  int st;
  char *s;
  int succ=1;

  st = conn_new("localhost", 6000);
  game_init(argc>1?argv[1]:"b4");
  st = init(argc>1?argv[1]:"b1");
  st = init(game.my_team_name);
  s = conn_receive_first();
  printf(s);
  while(1) {
    s = conn_receive();
    /*    printf(s);*/
    st = parse(s);
    if(st<=0) {
      printf("%s\n", parse_errstr);
      succ = 0;
      break;
    }
    if(st == CMD_SEE) {
      if(see.ball.time==see.time) {
	if(see.ball.pos.distance>0.5) {
	  dash(50);
	} else {
	  kick(20, 0);
	}
      } else {
	turn(60);
      }
    }
  }
  conn_destroy();
  return succ;
}
