/*

  b4 - power-up

  Adjacent of b2, not b3.
  Tries to go not exactly to the ball but somewhat behind it
  (compared to the goal).
  Increased dash and kick power.

 */

#include <stdio.h>

#include "conn.h"
#include "command.h"
#include "parse.h"
#include "game_data.h"
#include "see_data.h"
#include "log.h"

#define DASH_POWER 80

int main(int argc, char *argv[])
{
  int st;
  char *s;
  int succ=1;
  char logf_name[80];

  st = conn_new("localhost", 6000);
  game_init(argc>1?argv[1]:"b4");
  st = init(game.my_team_name);
  s = conn_receive_first();
  st = parse(s);
  if(!log_open(game.unum)) {
    perror("Logfile open");
    goto finalize;
  }
  RCLOG(s);
  while(1) {
    s = conn_receive();
    RCLOG(s);
    st = parse(s);
    if(st<=0) {
      printf("%s\n", parse_errstr);
      succ = 0;
      break;
    }
    if(st == CMD_SEE) {
      if(see.ball.time==see.time) {
	RCLOG("I see the ball; ");
	if(see.ball.pos.distance>0.5) {
	  if(see.goal[DIR_FD].time==see.time) {
	    float wdir = see.ball.pos.direction
	      - (see.goal[DIR_FD].pos.direction-see.ball.pos.direction)/4;
	    if(wdir>10 || wdir<-10) {
	      turn(wdir);
	    } else {
	      dash(DASH_POWER);
	    }
	  } else if(see.ball.pos.direction>10 || see.ball.pos.direction<-10) {
	    RCLOG("The ball is sideways\n");
	    turn(see.ball.pos.direction);
	  } else {
	    RCLOG("I approakich the ball\n");
	    dash(DASH_POWER);
	  }
	} else {
	  if(see.goal[DIR_FD].time==see.time) {
	    float kpow = see.goal[DIR_FD].pos.distance > 20 ? 50 : 80;
	    kick(kpow, see.goal[DIR_FD].pos.direction);
	  } else {
	    turn(59);
	  }
	}
      } else {
	RCLOG("I cannot see the ball, must turn around!\n");
	turn(60);
      }
    }
  }
 finalize:
  log_close();
    
  conn_destroy();
  return succ;
}
