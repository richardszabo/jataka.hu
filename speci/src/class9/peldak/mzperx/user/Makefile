#
# Makefile for a webots controller
#
# Author: Olivier.Michel@cyberbotics.com
# Date:   April 30th, 1998.
#

NAME = ecal1

LIBRARY  = -L$(WEBOTS_HOME)/lib -lController -lKhepera -L/usr/X11R6/lib -lX11 -lpng -lm -lz -lstdc++ 
CC_OPTS  = -c -I$(WEBOTS_HOME)/include -DSIMULATION

$(NAME):	$(NAME).o
		$(CC) -o $(NAME) $(NAME).o $(LIBRARY)

$(NAME).o:	$(NAME).c 
		$(CC) $(CC_OPTS) $(NAME).c

clean:
	-rm -f *.o *~ $(NAME)
