#include "SVR.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

Surveyor robot(ADDRESS);

#define MOVE_SPEED    100 // [-128 - 127]
#define MOVE_DURATION 0 // [   0 - 255], 0 is infinite
#define TURN_DURATION 20 // [   0 - 255], 0 is infinite

int main(int argc, char *argv[]) {
    char buf[256];
    int arg = 0;
    int key;
    int left_speed = 0, right_speed = 0;
    int laser_mode = 0;
    int finish = 0;

    robot.setDebuging(2);
    robot.getVersion(buf);
    //printf("surveyor_remote: SVR-1 version %s\n", buf);
    initscr();
    keypad(stdscr,TRUE);

    do {
        key = getch();
        //left_speed = 0;
        //right_speed = 0;
        switch (key) {
            case 'q':
                left_speed = 0;
                right_speed = 0;
                break;
            case 'w':
            case KEY_UP:
                left_speed = 1;
                right_speed = 1;
                break;
            case 'z':
            case KEY_DOWN:
                left_speed = -1;
                right_speed = -1;
                break;
            case 'a':
            case KEY_LEFT:
                left_speed = -1;
                right_speed = 1;
                break;
            case 's':
            case KEY_RIGHT:
                left_speed = 1;
                right_speed = -1;
                break;
            case 'r':
                printw("\ndistance: %d\n",robot.getDistanceFromLasers());
                break;
            case 'l':
                robot.setLasers(++laser_mode % 2);
                break;
            case 'p':
                robot.takePhoto();
                robot.savePhoto("test.jpg");
                break;
            case 'x':
                finish = 1;
                break;
        }
        robot.drive(
                left_speed * MOVE_SPEED,
                right_speed * MOVE_SPEED,
                left_speed == right_speed ? MOVE_DURATION : TURN_DURATION);
    } while (!finish);

    endwin();

    return 0;
}
