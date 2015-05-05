/* File implementing tracker class which primarily handles prediction of the ball's location given current location and time in future
when the position is required*/
#include "driver.h"

Tracker::Tracker(): prev_pos(X_MIN, Y_MIN), prev_velocity(), prediction(X_MIN , Y_MIN){//Constructor to initialize variables correctly
    //previous_time = millis();
    first = true;
}

int Tracker::predict(Point2f curr_pos, Point2d &pixel_pred, int &rod_to_move) {
    unsigned int diff_time;
    diff_time = PERIOD;//since the function is called periodically, the time difference between each invocation can be assumed to be fixed at the period of execution
    Point2f curr_velocity(1.0, 1.0);
    DigitalOut led1(LED1);//Leds to indicate infinite loops in code
    DigitalOut led2(LED2);

    int t;

    if(curr_pos.x < 0.0 || curr_pos.y < 0.0) {//if raspberry Pi didn't find the ball, set current position to the predicted position of the ball
            curr_pos = prediction;
    }
        
    if(first) {//if first time invocation, set the state variables to proper values
        first = false;
        prev_pos = curr_pos;
    }

    curr_velocity = (curr_pos - prev_pos) / diff_time;//find the velocity of the ball, both x and y velocities are computed in this one statement
    
    if(curr_velocity.x > 0) {//if ball moving towards opponent's goal
        t = (ROD_FWD_X - curr_pos.x) / curr_velocity.x;//find the time taken for the ball to reach the rod closest to the opponent's goal
        rod_to_move = 1;
    }
    else if(curr_velocity.x < 0) {//if ball moving towards computer's goal
        t = (ROD_DEF_X - curr_pos.x) / curr_velocity.x;//find time taken for the ball to reach the rod closest to the computer's goal
        rod_to_move = 0;
    }
    else {
        t = INT_MAX;//if ball not moving anywhere, time to go anywhere else is infinite
    }
    
    if(t < 0) {//if negative time returned by any of the cases above, set time to INT_MAX
        t = INT_MAX;
    }
        
    prediction.x = curr_pos.x + (curr_velocity.x * diff_time);//update the predicted position of the ball given current position
    prediction.y = curr_pos.y + (curr_velocity.y * diff_time);
    
    //Handling reflection. 
    int i=0;
    while((i++<10) && ((prediction.x < X_MIN) || (prediction.x > X_MAX) || (prediction.y < Y_MIN) || (prediction.y > Y_MAX))) { //while the ball is out of the table's bounds, keep bouncing the ball off the surface till it is within the table
        led1 = 1;
        if(prediction.x < X_MIN) {
            prediction.x = 2 * X_MIN - prediction.x;
        }
        if (prediction.x > X_MAX) {
            prediction.x = 2 * X_MAX - prediction.x;
        }
        
        if(prediction.y < Y_MIN) {
            prediction.y = 2 * Y_MIN - prediction.y;
        }
        if (prediction.y > Y_MAX) {
            prediction.y = 2 * Y_MAX - prediction.y;
        }
    }
    led1 = 0;
    
    if(t != INT_MAX) {
        //pixel_pred.y = curr_pos.y + (curr_velocity.y * t) + (0.5 * (curr_velocity.y - prev_velocity.y) * t);
        pixel_pred.y = curr_pos.y + (curr_velocity.y * t);
        i=0;
        while((i++<10)&&(pixel_pred.y < Y_MIN || pixel_pred.y > Y_MAX)) {//same as above while loop
            led2 = 1;
            if(pixel_pred.y < Y_MIN) {
                pixel_pred.y = 2 * Y_MIN - pixel_pred.y;
            }
            if (pixel_pred.y > Y_MAX) {
                pixel_pred.y = 2 * Y_MAX - pixel_pred.y;
            } 
        }
        led2 = 0;
    }
    
    prev_velocity = curr_velocity;
    prev_pos = curr_pos;
    return t;
}
