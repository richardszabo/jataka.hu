#include "imageprocessing.h"

#define	H_PIXEL(i,j,image) ((uchar*)((image)->imageData))[((i) * (image)->widthStep) + ((j) * (image)->nChannels)]
#define	S_PIXEL(i,j,image) ((uchar*)((image)->imageData))[(((i) * (image)->widthStep) + ((j) * (image)->nChannels)) + 1]
#define	V_PIXEL(i,j,image) ((uchar*)((image)->imageData))[(((i) * (image)->widthStep) + ((j) * (image)->nChannels)) + 2]
#define SET_PIXEL_V1(i,j,image,x) ((uchar*)((image)->imageData))[((i) * (image)->widthStep) + ((j) * (image)->nChannels)] = (x)

#define MAXHUE 360

#include <curses.h>

// taken from unixrobot

int findColor(IplImage* image, IplImage* resultImage, Configuration *configuration) {
    int i, j;

    // hue is between 0 and 360
    int hlower = configuration->getIntParam("hue-lower-limit");
    int hupper = configuration->getIntParam("hue-upper-limit");
    int slower = configuration->getIntParam("saturation-lower-limit");
    int supper = configuration->getIntParam("saturation-upper-limit");
    int vlower = configuration->getIntParam("value-lower-limit");
    int vupper = configuration->getIntParam("value-upper-limit");

    //cout << image->height << " " << image->width << " " << image->widthStep << " " << image->depth << endl;
    int height = image->height;
    int width = image->width;
    int step = image->widthStep / (image->depth / 8);
    int channels = image->nChannels;

    uchar *resultData = (uchar *) resultImage ->imageData;
    int stepmono = resultImage->widthStep;
    int channelsmono = resultImage->nChannels;

    IplImage *imageHSV = cvCloneImage(image);

    cvCvtColor(imageHSV, imageHSV, CV_BGR2HSV); // Converting the color space
    uchar *data = (uchar *) imageHSV->imageData;

    // clearing the result image
    cvZero(resultImage);

    assert(imageHSV->height == resultImage->height);
    assert(imageHSV->width == resultImage->width);

    //cout << hlower << " " << hupper << " " << slower << " " << supper << " " << vlower << " " << vupper << endl;
    for (i = 0; i < image->height; i++) {
        for (j = 0; j < image->width; j++) {
            if ((2 * H_PIXEL(i, j, imageHSV) - hlower + MAXHUE) % MAXHUE <= (hupper - hlower + MAXHUE) % MAXHUE
                    && S_PIXEL(i, j, imageHSV) >= slower && S_PIXEL(i, j, imageHSV) <= supper
                    && V_PIXEL(i, j, imageHSV) >= vlower && V_PIXEL(i, j, imageHSV) <= vupper) {
                SET_PIXEL_V1(i, j, resultImage, 255);
            }
        }
    }
    cvReleaseImage(&imageHSV);

    for (i = 0; i < configuration->getIntParam("dilate_erode_number"); ++i) {
        cvDilate(resultImage, resultImage);
        cvErode(resultImage, resultImage);
    }

    return 1;
}

// calculates the distance of the robot from the edge of the tracked color
// the input image is a binary: nonzero values mean traversable places
// this image can be generated from the original with findColor
int calculateDistance(IplImage* image) {
    int i;
    int sum = 0;

    for (i = image->height - 1; i >= image->height / 2; --i) {
        if( H_PIXEL(i,image->width/2,image) != 0 ) {
            ++sum;
        }
    }
    return sum;
}

int thresh = 50;

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2

double angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0) {
    double dx1 = pt1->x - pt0->x;
    double dy1 = pt1->y - pt0->y;
    double dx2 = pt2->x - pt0->x;
    double dy2 = pt2->y - pt0->y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1)*(dx2 * dx2 + dy2 * dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage

CvSeq* findSquares4(IplImage* img, CvMemStorage* storage) {
    CvSeq* contours;
    int i, c, l, N = 11;
    CvSize sz = cvSize(img->width & -2, img->height & -2);
    IplImage* timg = cvCloneImage(img); // make a copy of input image
    IplImage* gray = cvCreateImage(sz, 8, 1);
    IplImage* pyr = cvCreateImage(cvSize(sz.width / 2, sz.height / 2), 8, 3);
    IplImage* tgray;
    CvSeq* result;
    double s, t;
    // create empty sequence that will contain points -
    // 4 points per square (the square's vertices)
    CvSeq* squares = cvCreateSeq(0, sizeof (CvSeq), sizeof (CvPoint), storage);

    // select the maximum ROI in the image
    // with the width and height divisible by 2
    cvSetImageROI(timg, cvRect(0, 0, sz.width, sz.height));

    // down-scale and upscale the image to filter out the noise
    cvPyrDown(timg, pyr, 7);
    cvPyrUp(pyr, timg, 7);
    tgray = cvCreateImage(sz, 8, 1);

    // find squares in every color plane of the image
    for (c = 0; c < 3; c++) {
        // extract the c-th color plane
        cvSetImageCOI(timg, c + 1);
        cvCopy(timg, tgray, 0);

        // try several threshold levels
        for (l = 0; l < N; l++) {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if (l == 0) {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                cvCanny(tgray, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                cvDilate(gray, gray, 0, 1);
            } else {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                cvThreshold(tgray, gray, (l + 1)*255 / N, 255, CV_THRESH_BINARY);
            }

            // find contours and store them all as a list
            cvFindContours(gray, storage, &contours, sizeof (CvContour),
                    CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

            // test each contour
            while (contours) {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                result = cvApproxPoly(contours, sizeof (CvContour), storage,
                        CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);
                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if (result->total == 4 &&
                        fabs(cvContourArea(result, CV_WHOLE_SEQ)) > 1000 &&
                        cvCheckContourConvexity(result)) {
                    s = 0;

                    for (i = 0; i < 5; i++) {
                        // find minimum angle between joint
                        // edges (maximum of cosine)
                        if (i >= 2) {
                            t = fabs(angle(
                                    (CvPoint*) cvGetSeqElem(result, i),
                                    (CvPoint*) cvGetSeqElem(result, i - 2),
                                    (CvPoint*) cvGetSeqElem(result, i - 1)));
                            s = s > t ? s : t;
                        }
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if (s < 0.3)
                        for (i = 0; i < 4; i++)
                            cvSeqPush(squares,
                                (CvPoint*) cvGetSeqElem(result, i));
                }

                // take the next contour
                contours = contours->h_next;
            }
        }
    }

    // release all the temporary images
    cvReleaseImage(&gray);
    cvReleaseImage(&pyr);
    cvReleaseImage(&tgray);
    cvReleaseImage(&timg);

    return squares;
}

const char* wndname = "Square Detection Demo";

// the function draws all the squares in the image

void drawSquares(IplImage* img, CvSeq* squares) {
    CvSeqReader reader;
    IplImage* cpy = cvCloneImage(img);
    int i;

    // initialize reader of the sequence
    cvStartReadSeq(squares, &reader, 0);

    // read 4 sequence elements at a time (all vertices of a square)
    for (i = 0; i < squares->total; i += 4) {
        CvPoint pt[4], *rect = pt;
        int count = 4;

        // read 4 vertices
        CV_READ_SEQ_ELEM(pt[0], reader);
        CV_READ_SEQ_ELEM(pt[1], reader);
        CV_READ_SEQ_ELEM(pt[2], reader);
        CV_READ_SEQ_ELEM(pt[3], reader);

        // draw the square as a closed polyline
        cvPolyLine(cpy, &rect, &count, 1, 1, CV_RGB(0, 255, 0), 3, CV_AA, 0);
    }

    // show the resultant image
    cvShowImage(wndname, cpy);
    cvReleaseImage(&cpy);
}
