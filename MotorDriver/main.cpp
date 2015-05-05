#include "driver.h"

 /*
    The motor driver controls two motors.
    The motor driver gets the current position on serial from the raspberry pi and decides to rotate motors appropriately
 */

Serial pc(USBTX, USBRX);//define the serial interface

int get_x_and_y(int &x, int &y){//function that decodes characters sent on serial to x and y coordinates of the ball
    if(pc.readable()){//if any character present in the buffer, enter and decode x and y
        char buf[10];
        memset(buf,0,10);//initialize the buffer to null characters
        int i=0;
        while(1){//infinite loop
            char temp=pc.getc();//get the next character in the buffer
            if(temp==','){//comma seperates x and y coordinates
                x=atoi(buf);//convert the characters in the buffer array to number
                memset(buf,0,10);//reset the buffer
                i=0;
            }else if(temp==';'){//semi colon seperates one ball position reading from another
                y=atoi(buf);//convert the characters stored in the buffer to number
                memset(buf,0,10);//reset the buffer
                i=0;
                break;//exit while(1) since both x and y are obtained
            }else if(temp>='0' && temp<= '9'){//copy only numbers to buffer
                buf[i++]=temp;
            }
        }
        return 1;//if successfully decoded, return 1
    }
    else{
        return 0;//return 0 indicating no characters available
    }
}

main() {
    pc.baud(115200);    //set baud rate to 115200
    Point2d temp;    
    Point2f curr;
    
    bool fresh = false;
    // predicted position of the ball
    Point2d pixel_pred;
    
    int time_to_fwd = INT_MAX;    
    int time_to_def = INT_MAX;
    
    /*PinName MotL_en, PinName MotL_fw, PinName MotL_re, PinName MotL_Qa, PinName MotL_Qb, 
    PinName MotR_en, PinName MotR_fw, PinName MotR_re, PinName MotR_Qa, PinName MotR_Qb*/
    
    Rod rod_fwd(p22, p27, p28, p29, p30, 
                p23, p15, p16, p17, p18);
                
    Rod rod_def(p24, p5, p6, p7, p8, 
                p25, p9, p10, p11, p12); // initialise the two rods
    
    Tracker t; 
    DigitalOut led3(LED3);
    int rod_to_move;
    Timer timer;
    timer.start();
    led3 = 0;
    Point2d prev_temp;
    
    while (1) {//infinite while loop
        if(timer.read_ms()>=PERIOD){
            timer.reset();
            led3 = !led3;
            curr.x = curr.y = -1;
            if(get_x_and_y(temp.x,temp.y)) {//simple logic to determine which rod to activate, time_to_def and time_to_fwd determine urgency
                curr.x = temp.x;
                curr.y = temp.y;
                pixel_pred.y = temp.y;
            }
           if(curr.x >= ROD_FWD_X) {
               time_to_fwd = 20; 
               time_to_def = INT_MAX;
            }
           else {
               time_to_def = 20;
               time_to_fwd = INT_MAX;
            }
           
               
           fresh = true;//set fresh to true to indicate new ball position reading
        }                        
        rod_fwd.take_action(pixel_pred.y, time_to_fwd, fresh);//call both rods to run control loops and determine position to move
        rod_def.take_action(pixel_pred.y, time_to_def, fresh);
        fresh = false;//set fresh to false to indicate stale readings from now on until new updates
    }
}
