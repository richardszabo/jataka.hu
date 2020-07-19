/*

  b3 - nobunch

  The first signs of team awareness. If the robot finds a
  teammate who is closer to the ball than itself, then does
  not move too close to the ball.
  So, instead of being in a bunch at the ball, there is only
  one or two robots having contact with the ball, the rest
  following them from a certain distance.

  Note: b3 did not turn out to be better than b2.

 */

#include <stdio.h>
#include <math.h>

#include "conn.h"
#include "command.h"
#include "parse.h"
#include "game_data.h"
#include "see_data.h"
#include "defs.h"

int main(int argc, char *argv[])
{
  int st;
  char *s;
  int succ=1;

  st = conn_new("localhost", 6000);
  game_init(argc>1?argv[1]:"b3");
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
	  } else if(see.ball.pos.distance > 10) {
	    dash(50);
	  } else {
	    float pdist_2;
	    int leave_it = 0;
	    int i;
	    for(i=0; i<see.player_length; ++i) {
	      if(see.player[i].ps>=PL_TEAM && see.player[i].is_my_player) {
		pdist_2 = see.ball.pos.distance*see.ball.pos.distance
		  + see.player[i].pos.distance*see.player[i].pos.distance
		  - 2*see.ball.pos.distance*see.player[i].pos.distance*cos(DEG2RAD(see.ball.pos.direction-see.player[i].pos.direction));
		if(pdist_2 < see.ball.pos.distance*see.ball.pos.distance) leave_it = 1;
	      }
	    }
	    if(!leave_it) {
	      dash(50);
	    }
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
