#include <ctime>
#include "cv_headers.h"
#include <vector>
#include <iostream>
#include <sys/time.h>

using namespace std;
using namespace cv;

struct rect{
	int startX;
	int startY;
	int cols;
	int rows;
	rect():startX(0),startY(0),cols(IMAGE_WIDTH),rows(IMAGE_HEIGHT){}
};

class smartDetect{
private:
	Point2f prev_pos;
	Point2f prev_velocity;
	timeval previous_time;
	bool first;

public:
// constructor
	smartDetect();
// function to determine a subimage for blob detection	
	void determine_subimage(rect& s, KeyPoint& prev_point);	
};

