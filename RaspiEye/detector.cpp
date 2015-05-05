#include <ctime>
#include <stdio.h>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include "cv_headers.h"
#include "smart_detect.h"
#include <wiringSerial.h>
#include <stdlib.h>
#include <string.h>


/*
  This code is responsible for detecting the position of the ball and then communicating the current position of the ball to mbed through COM port.
*/

using namespace std; 
using namespace cv; 

int main (int argc,char **argv) {
    // handle for serial communication to mbed
    int handle;    
    
    // These three timeval variables are used to caculate the amount of time elapsed between successive itearions of the while loop
    timeval start; 
    timeval end;
    timeval total;
    
    // To control the camera
    raspicam::RaspiCam_Cv Camera; 
    
    // To hold pixel values of one frame of video
    cv::Mat image;
    
    unsigned int index;
    float max;
    Point2f offset(0, 0);
    
    // Set camera params
    Camera.set(CV_CAP_PROP_FORMAT, 			CV_8UC1 );
    Camera.set(CV_CAP_PROP_FRAME_WIDTH,		IMAGE_WIDTH);
    Camera.set(CV_CAP_PROP_FRAME_HEIGHT,	IMAGE_HEIGHT);
    
    // Open camera
    cout << "Opening Camera..." << endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    
    // Start capture
    SimpleBlobDetector::Params params;
    
    // Adjust various parameters for blob detection
    params.minArea 			=	MIN_BLOB_SIZE;
    params.minDistBetweenBlobs		=	MIN_BLOB_DIST;
    params.maxThreshold			=	MAX_THRES;
    params.minThreshold			=	MIN_THRES;
    params.thresholdStep		=	THRES_STEP;
    params.filterByCircularity		=	false; 
    SimpleBlobDetector detector(params);
    
    vector<KeyPoint> keypoints(0);
    gettimeofday(&start, NULL);

// LVL_2 is used to introduce optimization for better frame rates.
// If this constant is set during compilation, then the a smaller subimage around the previous ball position is used for blob detection.
#ifdef LVL_2
    cout << "Level 2" << endl;
    smartDetect smartDetector;
#endif
    
    // Open serial port for communication with mbed.     
    handle = serialOpen("/dev/ttyACM0", 115200);
    if(handle<=0){
	cout<<"Serial Port already open!!!"<<endl;
    }
    
    while (1){
        Camera.grab();
        Camera.retrieve(image); // Get one frame of video into the image Mat structure

#ifdef LVL_2
// If LVL_2 optimization is set then work on a smaller subimage.

	Mat smaller_image;
	rect s;
    
    // If previous ball position is known then try to detect a smaller subimage. Woring on lesser number of pixels increases the FPS throughput.
	if(keypoints.size()) {
		smartDetector.determine_subimage(s, keypoints[index]);
	}
	else {
		s.startX	=	0;
		s.startY	=	0;
		s.cols		=	IMAGE_WIDTH;
		s.rows		=	IMAGE_HEIGHT;
	}
	offset.x = s.startX;
	offset.y = s.startY;
		
	image(Rect(s.startX, s. startY, s.cols, s.rows)).copyTo(smaller_image);
	detector.detect(smaller_image, keypoints);
	if(!keypoints.size()) {
		detector.detect(image, keypoints);
	}
#else
// If no LVL_2 optimation, the do blob detection on the entire 160x120 image.
	detector.detect(image, keypoints);
#endif

	if(keypoints.size()) {
    // If many blobs are detected in the frame, then find the position of the largest frame.
		index 	= 0;
		max 	= keypoints[index].size;
		for(int i=1; i<keypoints.size(); i++) {
			if(keypoints[i].size > max) {
				index 	= i;
				max 	= keypoints[index].size;
			}
		}
		keypoints[index].pt += offset;
        // Send the co-ordinates to mbed.
		serialPrintf(handle, "%d,%d;", (int)keypoints[index].pt.x, (int)keypoints[index].pt.y); 
	}

// The following code pops up a new window and dispalys the working of blob detection.
// This was sued just for demo.

#ifdef LVL_2
	Mat im_with_keypoints;
    drawKeypoints(image, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
    imshow("points",smaller_image); 
	imshow("points", im_with_keypoints);
#else
	Mat im_with_keypoints;
	drawKeypoints(image, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
	imshow("points", image);
	imshow("points", im_with_keypoints);
#endif
	waitKey(1);
    }
    cout<<"Stop camera..."<<endl;
    Camera.release();
    serialClose(handle);
}
