#include "SVR.h"
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

#include "cv.h"
#include "highgui.h"

#include "imageprocessing.h"
#include "configurationhandler.h"

#define MOVE_SPEED    50 // [-128 - 127]
#define MOVE_DURATION 0 // [   0 - 255], 0 is infinite
#define TURN_DURATION 30 // [   0 - 255], 0 is infinite

#define MOVE_LIMIT 60

int main(int argc, char *argv[]) {
    Surveyor robot(ADDRESS);
    char buf[256];
    int key;
    int finish = 0;
    IplImage *img = 0;
    IplImage *res = 0;
    int storeImages = 1;
    char imagename[100];
    char resimagename[100];
    double turn;
    int count = 0;

    robot.setDebuging(2);
    robot.getVersion(buf);
    initscr();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    cvInitSystem(argc, argv);
    Configuration *configuration = new Configuration("config.xml");

    do {
        key = getch();

        if (storeImages) {
            sprintf(imagename, "test%04d.jpg", ++count);
            sprintf(resimagename, "test%04d.res.jpg", count);
        } else {
            sprintf(imagename, "test.jpg");
            sprintf(resimagename, "test.res.jpg");
        }
        robot.takePhoto();
        robot.savePhoto(imagename);
        img = cvLoadImage(imagename);
        cvNamedWindow("camera", CV_WINDOW_AUTOSIZE);
        cvShowImage("camera", img);
        res = cvCloneImage(img);
        findColor(img, res, configuration);
        cvNamedWindow("color", CV_WINDOW_AUTOSIZE);
        cvShowImage("color", res);
        cvSaveImage(resimagename,res);
        int limit_from_move_edge = calculateDistance(res);
        if (limit_from_move_edge < MOVE_LIMIT) {
            turn = 1;
        } else {
            turn = 0;
        }
        cvWaitKey(10);
        if (img) {
            cvReleaseImage(&img);
        }
        if (res) {
            cvReleaseImage(&res);
        }

        if (++count % 24 == 0) {
            clear();
        }
        printw("return: %d %lf \n", limit_from_move_edge, turn);
        if (turn) {
            robot.drive(
                    -1 * MOVE_SPEED,
                    MOVE_SPEED,
                    TURN_DURATION);
        } else {
            robot.drive(
                    MOVE_SPEED,
                    MOVE_SPEED,
                    MOVE_DURATION);
        }
        switch (key) {
            case 'p':
                storeImages = !storeImages;
                break;
            case 'x':
                finish = 1;
                robot.drive(
                        0,
                        0,
                        MOVE_DURATION);
                break;
        }
    } while (!finish);

    delete configuration;

    endwin();

    return 0;
}
