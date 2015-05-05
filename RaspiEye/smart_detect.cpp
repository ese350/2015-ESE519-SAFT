#include "smart_detect.h"

/*
	The class smartDetect is used to increase the frame rate.
	When blob detection is done on the entire 160x120 frame, the throughput was around 17-20 FPS.
	With smart detection the throughput increased to 25 FPS.
	The basic idea is, based on previous position of the ball, we determine a smaller subimage of 30x40 where the ball could be present and the do blob detetion on the smaller subimage.
*/

// Constructor of the class
smartDetect::smartDetect():
	prev_pos(0, 0), // previous position of the ball
	prev_velocity(0, 0) // previous velocity of the ball
	{ 
		gettimeofday(&previous_time, NULL); // store time to compute velocity on successive invocations of smart detect.
		first = true; // set to flase in first invocation of smart_detect.
	}

void smartDetect::determine_subimage(rect& s, KeyPoint& prev_keypoint) {
/*
parameters:
	s -> determines the coordinates of the smaller rectangular region
	prev_key_points -> previous position of the ball as detected in the main program.
*/
	timeval current_time;
	timeval result_time;
	double diff_time;
	Point2f curr_velocity, predicted_center;
	int c_x, c_y, lim;
	
	// Calculate the amount of time passed since last invocation of this function.
	gettimeofday(&current_time, NULL);
	timersub(&current_time, &previous_time, &result_time);
	diff_time = ((double)result_time.tv_usec)/1000;
	previous_time = current_time;

	if(first) {
		first = false;
		prev_pos = prev_keypoint.pt;
	}

	// Calculate current velocity based on previous velocity.
	curr_velocity.x = (prev_keypoint.pt.x - prev_pos.x) / diff_time;
	curr_velocity.y = (prev_keypoint.pt.y - prev_pos.y)/ diff_time;
	
	// predict ball position based on previous and current velocity
	predicted_center = prev_keypoint.pt + (curr_velocity * diff_time) + (0.5 * (curr_velocity - prev_velocity) * diff_time);
	c_x = (int)predicted_center.x;
	c_y = (int)predicted_center.y;

	// updated previous position and velocity to current values
	prev_velocity = curr_velocity;		
	prev_pos = prev_keypoint.pt;
	cout << predicted_center << "\t";
			
	// determine a sub frame based on predicted center.		
	s.startX = c_x - X_LIMIT;
	s.startY = c_y - Y_LIMIT;		

	if(c_x >= IMAGE_WIDTH) {
		s.startX = IMAGE_WIDTH - 2*X_LIMIT;
	}
	else if(s.startX < 0) {
		s.startX = 0;
	}
	
	if(c_y >= IMAGE_HEIGHT) {
		s.startY = IMAGE_HEIGHT - 2*Y_LIMIT;
	}
	else if(s.startY < 0) {
		s.startY = 0;
	}

	lim = X_LIMIT*2;
	
	if((lim + s.startX) > IMAGE_WIDTH) {
		s.cols = IMAGE_WIDTH - s.startX;
	}else { 
		s.cols = lim;
	}
	lim = Y_LIMIT*2;
	if((lim + s.startY) > IMAGE_HEIGHT) {
		s.rows = IMAGE_HEIGHT - s.startY;
	}else {
		s.rows = lim;
	}
}
