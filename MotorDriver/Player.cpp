/*Code implementing player class, Player objects correspond to the foosmen mounted on the rods, these men are abstracted away and queried
for distance between them and the ball to collectively identify the player who can be moved the shortest distance to hit the ball*/
#include "driver.h"
    
Player::Player(int minimum, int maximum, int init) {
    //Set the minimum and maximum pixel limits that can be reached by this player. The limits are calibrated manually
    min = minimum;
    max = maximum;
    curr = init;
}
void Player::update(int m) {
    curr += m;//set the new position to reflect reality on the board
}

int Player::inRange(int ball_pos){//Function to return the distance from ball position
    if(ball_pos >= min && ball_pos <= max){
        return ball_pos - curr;//If within physical range, return the distance
    }
    else
        return LIMIT;//return some max value to indicate unreachability
}
