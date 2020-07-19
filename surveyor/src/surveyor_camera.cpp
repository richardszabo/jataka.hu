#include "SVR.h"
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

#include "cv.h"
#include "highgui.h"

#define MOVE_SPEED    100 // [-128 - 127]
#define MOVE_DURATION 0 // [   0 - 255], 0 is infinite
#define TURN_DURATION 20 // [   0 - 255], 0 is infinite

void showPhoto(char *filename) {
    IplImage *img = 0;

    img = cvLoadImage(filename);
    cvNamedWindow("camera", CV_WINDOW_AUTOSIZE);
    cvShowImage("camera", img);
    cvWaitKey(10);
    cvReleaseImage(&img);
}

int main(int argc, char *argv[]) {
    Surveyor robot(ADDRESS);
    char buf[256];
    int key;
    int left_speed = 0, right_speed = 0;
    int laser_mode = 0;
    int finish = 0;
    char imagename[100];
    int storeImages = 0;
    int count = 0;

    robot.setDebuging(2);
    robot.getVersion(buf);
    robot.setVideoMode(MODE160x128);
    initscr();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    cvInitSystem(argc, argv);

    do {
        key = getch();

        if (storeImages) {
            sprintf(imagename, "test%04d.jpg", ++count);
        } else {
            sprintf(imagename, "test.jpg");
        }
        robot.takePhoto();
        robot.savePhoto(imagename);
        showPhoto(imagename);

        switch (key) {
            case 'q':
            case ' ':
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
                printw("distance: %d\n", robot.getDistanceFromLasers());
                break;
            case 'l':
                robot.setLasers(++laser_mode % 2);
                break;
            case 'p':
                storeImages = !storeImages;
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
