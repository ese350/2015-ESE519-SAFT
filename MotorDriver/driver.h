/* system header file containing all class declerations and #defines required throughout code*/
#ifndef DRIVER_H
#define DRIVER_H

#include "mbed.h"
#include "QEI.h"
#include "millis.h"
#include "Point.hpp"
#include <stdlib.h>
#include <cmath>

#define OFFSET           5

#define ENCODER_MAX      1920
#define PLAYER1_INIT     9 // Initial position of player 12
#define PLAYER2_INIT     42 // Initial position of player 45
#define PLAYER3_INIT     73 // Initial position of player 76

#define PLAYER1_MIN      12
#define PLAYER1_MAX      45

#define PLAYER2_MIN      45
#define PLAYER2_MAX      76

#define PLAYER3_MIN      76
#define PLAYER3_MAX      104

#define LIMIT            105

#define ROD_FWD_X       98 
#define ROD_DEF_X       25

#define DUTY_CYCLE 1

#define TIME_TO_HIT     (43/(DUTY_CYCLE))

#define X_MIN 8
#define X_MAX 150

#define Y_MIN 10
#define Y_MAX 105

#define PERIOD 33

#define SCALE 1

#define INIT 0
#define FINAL (960/SCALE)

#define LAT_TOTAL_COUNT     (1200.0/SCALE)
#define LAT_ONE_UNIT_COUNT  LAT_TOTAL_COUNT/(PLAYER1_MAX - PLAYER1_MIN) // how many counts of the encoder to move 1 pixel.

extern Serial pc;

//Declaration of tracker class
class Tracker {
private:
    Point2f prev_pos;
    Point2f prediction;
    Point2f prev_velocity;
    bool first;

public:
    Tracker();
    int predict(Point2f curr, Point2d &pred, int &rod_to_move);
};


//Declaration of player class
class Player {
    /*
     * Each player has three attributes.
     * Current postion, left-most and right-most positions it can reach
     * */
private:
    int min; // left-most
    int max; // right-most
    int curr; // current position of the player

public:
    Player(int minimum, int maximum, int init); // constructor
    int inRange(int ball_pos); // If ball psoition is in range of this player
    void update(int m);
};


//Declaration of Motors class
class Motors {
private:
    // Motor 1
    PwmOut MotorLateral_en;
    DigitalOut MotorLateral_fwd;
    DigitalOut MotorLateral_rev;
    QEI MotorLateral_qei;
    
    // Motor 2
    PwmOut MotorRotate_en;
    DigitalOut MotorRotate_fwd;
    DigitalOut MotorRotate_rev;
    QEI MotorRotate_qei;
    
    bool kick;
    int state;
    float prev_disp;
    float err_sum;    

public:
    Motors(PinName MotL_en, PinName MotL_fw, PinName MotL_re, PinName MotL_Qa, PinName MotL_Qb, 
            PinName MotR_en, PinName MotR_fw, PinName MotR_re, PinName MotR_Qa, PinName MotR_Qb);
    void move(int d);
    void hit();
    void act();
};

class Rod{
    /*
     * A rod has three players, and is controlled by two motors
     */
private:
    Player player1;
    Player player2;
    Player player3;
    int previous_pos;
    Motors motor_set;
    Timer timer;
    
    int min(int p1, int p2, int p3);
    

public:
    Rod(PinName MotL_en, PinName MotL_fw, PinName MotL_re, PinName MotL_Qa, PinName MotL_Qb, 
        PinName MotR_en, PinName MotR_fw, PinName MotR_re, PinName MotR_Qa, PinName MotR_Qb); // constructor

    // Finds where to move, command motors and update position
    void take_action(int pred, int t, bool flag);
};
#endif
