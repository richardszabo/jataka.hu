#include "SVR.h"
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

#include "cv.h"
#include "highgui.h"

#include "imageprocessing.h"
#include "configurationhandler.h"

#define MOVE_SPEED    40 // [-128 - 127]
#define MOVE_DURATION 0 // [   0 - 255], 0 is infinite

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
        res = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
        Colorpixels cp;
        findColor(img, res, configuration, &cp);
        cvNamedWindow("color", CV_WINDOW_AUTOSIZE);
        cvShowImage("color", res);
        cvSaveImage(resimagename,res);
        cvWaitKey(10);
        if (img) {
            cvReleaseImage(&img);
        }
        if (res) {
            cvReleaseImage(&res);
        }

        if (cp.left + cp.right > configuration->getIntParam("light-lower-limit") &&
                cp.left + cp.right < configuration->getIntParam("light-upper-limit")) {
            double turn = (double) (cp.left - cp.right) / max(cp.left, cp.right);
            robot.drive(
                    max(min(MOVE_SPEED - 0.5 * turn * MOVE_SPEED,100.0),-100.0),
                    max(min(MOVE_SPEED + 0.5 * turn * MOVE_SPEED,100.0),-100.0),
                    MOVE_DURATION);
        } else {
            robot.drive(
                    0,
                    0,
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
