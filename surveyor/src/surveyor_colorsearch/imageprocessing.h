/* 
 * File:   imageprocessing.h
 * Author: rics
 *
 * Created on January 18, 2010, 7:56 AM
 */

#ifndef _IMAGEPROCESSING_H
#define	_IMAGEPROCESSING_H

#include "cv.h"
#include "highgui.h"

#include "configurationhandler.h"

int findColor(IplImage* image, IplImage* resultImage,Configuration *configuration);
int calculateDistance(IplImage* image);
CvSeq* findSquares4( IplImage* img, CvMemStorage* storage );
void drawSquares( IplImage* img, CvSeq* squares );

#endif	/* _IMAGEPROCESSING_H */

