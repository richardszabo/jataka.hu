/*

  b5 - airy

  The first version that acts like a simple team and -unlike b3-
  actually does it in a beneficial way. Takes ideas from both b4
  and b5. The main concepts of this robot:
  - If it sees the ball and seems to be in the best position from
  the team, it goes for it.
  - If it does not want to go for the ball, then turns in the
  direction where there are fewer team mates.
  - If it can kick the ball then it tries in a direction where
  there are no opponents.

 */

#include <stdio.h>
#include <math.h>

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
  game_init(argc>1?argv[1]:"b5");
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
	if(see.ball.pos.distance<=0.5) {
	  int i;
	  int enemies[18];
	  int danger=0;
	  RCLOG("The ball is here; ");
	  for(i=0; i<18; ++i) enemies[i]=0;
	  for(i=0; i<see.player_length; ++i) {
	    if(see.player[i].ps>=PL_TEAM && !see.player[i].is_my_player) {
	      int idx = see.player[i].pos.direction/10+9;
	      if(see.player[i].pos.distance<20 && idx>=0 && idx<18) ++enemies[idx];
	      if(see.player[i].pos.distance<5) danger = 1;
	    }
	  }
	  if(see.goal[DIR_FD].time==see.time) {
	    float kpow = see.goal[DIR_FD].pos.distance > 20 ? 50 : 80;
	    int hotidx = see.goal[DIR_FD].pos.direction/10+9;
	    int bestidx = 0;
	    int rarest = 12;
	    for(i=0; i<18; ++i) {
	      if(enemies[i]<rarest) {
		rarest = enemies[i];
		bestidx = i;
	      } else if(enemies[i]==rarest && abs(hotidx-i)<abs(hotidx-bestidx)) {
		bestidx = i;
	      }
	    }
	    RCLOG("I see the goal so I kick there\n");
	    kick(kpow, 10*bestidx-90);
	  } else {
	    if(danger) {
	      float kpow = see.goal[DIR_FD].pos.distance > 20 ? 50 : 80;
	      RCLOG("Danger, I kick forward\n");
	      kick(kpow, 0);
	    } else {
	      RCLOG("I cannot see the goal so I turn\n");
	      turn(59);
	    }
	  }
	} else {
	  float pdist_2;
	  int leave_it = 0;
	  float pleft = 0, pright = 0;
	  int i;
	  RCLOG("The ball needs to be approached; ");
	  for(i=0; i<see.player_length; ++i) {
	    if(see.player[i].ps>=PL_TEAM && see.player[i].is_my_player) {
	      pdist_2 = see.ball.pos.distance*see.ball.pos.distance
		+ see.player[i].pos.distance*see.player[i].pos.distance
		- 2*see.ball.pos.distance*see.player[i].pos.distance*cos(DEG2RAD(see.ball.pos.direction-see.player[i].pos.direction));
	      if(pdist_2 < see.ball.pos.distance*see.ball.pos.distance) leave_it = 1;
	      if(see.player[i].pos.distance<15) {
		if(see.player[i].pos.direction > 0) {
		  pright += see.player[i].pos.direction;
		} else {
		  pleft -= see.player[i].pos.direction;
		}
	      }
	    }
	  }
	  if(!leave_it) {
	    RCLOG("I am selected to have the ball; ");
	    if(see.goal[DIR_FD].time==see.time) {
	      float wdir = see.ball.pos.direction
		- (see.goal[DIR_FD].pos.direction-see.ball.pos.direction)/4;
	      RCLOG("I see the goal; ");
	      if(wdir>10 || wdir<-10) {
		RCLOG("I adjust myself to go behind the ball\n");
		turn(wdir);
	      } else {
		RCLOG("I approach the ball\n");
		dash(DASH_POWER);
	      }
	    } else if(see.ball.pos.direction>10 || see.ball.pos.direction<-10) {
	      RCLOG("The ball is sideways\n");
	      turn(see.ball.pos.direction);
	    } else {
	      RCLOG("I approach the ball\n");
	      dash(DASH_POWER);
	    }
	  } else {
	    RCLOG("Others could go to the ball; ");
	    if(pleft>pright) {
	      RCLOG("Right looks free\n");
	      turn(60);
	    } else {
	      RCLOG("Left looks free\n");
	      turn(-60);
	    }
	  }
	}
      } else {
	int i;
	float pleft = 0, pright = 0;
	RCLOG("I do not see the ball; ");
	for(i=0; i<see.player_length; ++i) {
	  if(see.player[i].pos.distance<15) {
	    if(see.player[i].pos.direction > 0) {
	      pright += see.player[i].pos.direction;
	    } else {
	      pleft -= see.player[i].pos.direction;
	    }
	  }
	}
	if(pleft>pright) {
	  RCLOG("Right looks free\n");
	  turn(60);
	} else {
	  RCLOG("Left looks free\n");
	  turn(-60);
	}
      }
    }
  }
 finalize:
  log_close();
    
  conn_destroy();
  return succ;
}
