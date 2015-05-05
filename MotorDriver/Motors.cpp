/*Code to implement Motors class which handles driving the motors to the desired position, keeping the interface clean from low level details*/
#include "driver.h"
#define START 0
#define FWD 1
#define RWD 2  
#define Kp 0.003
#define Ki 0
#define Kd (Kp * 1000)

Motors::Motors(PinName MotL_en, PinName MotL_fw, PinName MotL_re, PinName MotL_Qa, PinName MotL_Qb, 
                PinName MotR_en, PinName MotR_fw, PinName MotR_re, PinName MotR_Qa, PinName MotR_Qb):

    MotorLateral_en(MotL_en),
    MotorLateral_fwd(MotL_fw),
    MotorLateral_rev(MotL_re),
    MotorLateral_qei(MotL_Qa, MotL_Qb, NC, ENCODER_MAX, QEI::X4_ENCODING),//Initialize the QEI class with the pins required for encoder reading
    
    MotorRotate_en(MotR_en),
    MotorRotate_fwd(MotR_fw),
    MotorRotate_rev(MotR_re),
    MotorRotate_qei(MotR_Qa, MotR_Qb, NC, ENCODER_MAX, QEI::X4_ENCODING)
    {
        kick=false;
        //setting both the forward and reverse pins to high keeps the motors held in their current position, it is like holding the brakes
        MotorRotate_fwd = 1;
        MotorRotate_rev = 1;

        //setting both the forward and reverse pins to high keeps the motors held in their current position, it is like holding the brakes
        MotorLateral_fwd = 1;
        MotorLateral_rev = 1;

        //Set the duty cycles for the PWM to 20ms or 50Hz
        MotorLateral_en.period(0.02);
        MotorRotate_en.period(0.02);

        MotorLateral_en.write(DUTY_CYCLE);
        MotorRotate_en.write(DUTY_CYCLE);
        
        state = START;//variable keeping tracking of the Motor orientation
        //Initialize control law variables to zero
        prev_disp = 0;
        err_sum = 0;
    }

void Motors::move(int d) {
        
    int count = LAT_ONE_UNIT_COUNT * (d - Y_MIN);//Compute the desired encoder counts by mapping the pixel space distance to encoder space distance
    int displacement = count - MotorLateral_qei.getScaledPulses(SCALE);//Current error in encode counts
    float duty_cycle = (Kp * displacement) + (Ki * err_sum) + (Kd * (displacement - prev_disp));//Control law
    
    //Update previous displacement and sum of errors so far
    prev_disp  = displacement;
    err_sum   += displacement;
    
    if(duty_cycle>0){//If duty cycle is positive, set the motor to turn in forward direction
        MotorLateral_fwd=1;
        MotorLateral_rev=0;
        MotorLateral_en.write(duty_cycle);
    }else{//If duty cycle is negative, set the motor to turn in reverse direction
        MotorLateral_fwd=0;
        MotorLateral_rev=1;
        MotorLateral_en.write(-duty_cycle);//negate the negative duty cycle to get a positive duty cycle
    }
}

void Motors::act(){
   int encoder_counts=MotorRotate_qei.getScaledPulses(SCALE);//Get the current encoder readings of the rotational motor           
    switch(state){
        case START:
            MotorRotate_fwd=1;//Put brakes on the motor to prevent external forces from turning the motor
            MotorRotate_rev=1;
            if(kick)//If kick command issued, start forward rotation of the rod
            {
                state=FWD;
                kick=false;
            }
            break;
        case FWD:
            MotorRotate_fwd=1;
            MotorRotate_rev=0;
            if(encoder_counts>=FINAL)//If the players have finished a kick, return to the initial position
                state=RWD;
            break;
        case RWD:
            MotorRotate_fwd=0;
            MotorRotate_rev=1;
            if(encoder_counts<=INIT)//Once past the initial position, go to START state to hold the current orientation
                state=START;
            break;                
    }
} 

void Motors::hit() {
    kick=true;//Set kick to true, this indicates intention to kick to the act() function
}
