/*

  b2 - chase-and-attack

  Advancement compared to b1 is that when reaching the ball,
  the robot looks for the goal and kicks the ball into that
  direction.

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
  game_init(argc>1?argv[1]:"b2");
  st = init(game.my_team_name);
  s = conn_receive_first();
  st = parse(s);
  printf(s);
  while(1) {
    s = conn_receive();
    /*printf(s);*/
    st = parse(s);
    if(st<=0) {
      printf("%s\n", parse_errstr);
      succ = 0;
      break;
    }
    if(st == CMD_SEE) {
      if(see.ball.time==see.time) {
	if(see.ball.pos.distance>0.5) {
	  if(see.ball.pos.direction>10 || see.ball.pos.direction<-10) {
	    turn(see.ball.pos.direction);
	  } else {
	    dash(50);
	  }
	} else {
	  if(see.goal[DIR_FD].time==see.time) {
	    kick(20, see.goal[DIR_FD].pos.direction);
	  } else {
	    turn(59);
	  }
	}
      } else {
	turn(60);
      }
    }
  }
  conn_destroy();
  return succ;
}
