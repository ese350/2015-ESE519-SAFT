/* File implementing functionality of the Rod class*/
#include "driver.h"

Rod::Rod(PinName MotL_en, PinName MotL_fw, PinName MotL_re, PinName MotL_Qa, PinName MotL_Qb, 
        PinName MotR_en, PinName MotR_fw, PinName MotR_re, PinName MotR_Qa, PinName MotR_Qb):
    player1(PLAYER1_MIN, PLAYER1_MAX, PLAYER1_INIT),//Initialize three players with their maximum and minimum positions
    player2(PLAYER2_MIN, PLAYER2_MAX, PLAYER2_INIT),
    player3(PLAYER3_MIN, PLAYER3_MAX, PLAYER3_INIT),
    previous_pos(10),
    motor_set(MotL_en, MotL_fw, MotL_re, MotL_Qa, MotL_Qb, //Initialize the two motors controlling rotation and translation of the rod
                MotR_en, MotR_fw, MotR_re, MotR_Qa, MotR_Qb)
    {   }

int Rod::min(int p1, int p2, int p3) {//function to find out the minimum of three numbers irrespective of sign and returning the signed number
    if(abs(p1) < abs(p2)) {
        if(abs(p1) < abs(p3))
            return p1;
        else
            return p3;
    }
    else {
        if(abs(p2) < abs(p3))
            return p2;
        else
            return p3;
    }
}

void Rod::take_action(int pos, int t, bool flag) {
    int dist1 = player1.inRange(pos);//Get the distance of each of the players from the position pos
    int dist2 = player2.inRange(pos);
    int dist3 = player3.inRange(pos);
    
    int move = min(dist1, dist2, dist3);//find the minimum of each player's distance
    if(move == LIMIT)//If none can reach it, don't do anything
        move = 0;
    motor_set.move(previous_pos + move);//command the motors to move to the absolute new position
    player1.update(move);//update each of the players to reflect reality
    player2.update(move);
    player3.update(move);
    previous_pos += move;//update previous_pos to new position of the rod
    if(flag){    //if first time this function is called with new value, start timer enable preplanned kicking
        timer.reset();//reset and start timer
        timer.start();
    }
    int eta = t - TIME_TO_HIT;    //calculate time till hit commencement
    if(timer.read_ms() > eta){//if time is right for hitting, call hit function of the motor to start rotational motion
        motor_set.hit();
        timer.stop();//stop and reset timer after one invocation
        timer.reset();
    }
    motor_set.act();//server like function which runs the rotational motor state machine to ensure proper hitting action
        
}
