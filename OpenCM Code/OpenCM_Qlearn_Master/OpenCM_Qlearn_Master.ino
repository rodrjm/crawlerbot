/* 
  Title: OpenCM_ctrl
  Author: Jordan Miller
  Description: This is the main code for the OpenCM side of the control
   for the crawler bot. It initializes the serial connection, waits for a command and 
   then updates the servo positions and returns success code if servo positions are acheived. 
   If the servo position is not acheived, it returns the servos to neutral and sends the 
   failure code back to the Raspberry Pi.
   
  Actions:
   _______  
  |   1   |
  |4     3|
  |___2___|

  The command protocol between Pi and OpenCM is as follows:  
   Command(CMD) is sent from Pi to OpenCM. 
   Answer(ANS) sent from OpenCM to Raspberry Pi.
   |CMD|_________________________Description____________________________        |ANS|_________________________Description____________________________
   |xC1| move one state up on the state map : move shoulder upward              |xA1| Done
   |xC2| move one state down on the state map : move shoulder downward          |xA2| Action Success
   |xC3| move one state right on the state map : move elbow upward              |xA3| Action Fail: Returned to same state
   |xC4| move one state left on the state map : move elbow downward             |xA4| Action Fail: Servo Shutdown
   |xC5| not yet defined                                                        |xA5| not yet defined
   |xC6| not yet defined                                                        |xA6| not yet defined
   |xC7| not yet defined                                                        |xA7| not yet defined
   |xC8| not yet defined                                                        |xA8| not yet defined  
   |xC9| not yet defined                                                        |xA9| not yet defined  
   |xCA| not yet defined                                                        |xAA| Action Acknowledged  
   |xCB| not yet defined                                                        |xAB| not yet defined  
   |xCC| not yet defined                                                        |xAC| not yet defined  
   |xCD| not yet defined                                                        |xAD| not yet defined  
   |xCE| not yet defined                                                        |xAE| not yet defined  
   |xCF| Request from pi to end transmission                                    |xAF| End Transmission Acknowledged
   
  References:
   Dynamixel SDK API Reference: http://support.robotis.com/en/software/dynamixel_sdk/api_reference.htm
   XL_320 Documentation:        http://support.robotis.com/en/product/actuator/dynamixel_x/xl_series/xl-320.htm
*/

//include Libraries
#include <string.h>

//include Files
#include "XL_320.h"
#include "QuadEncoder.h"

//define IDs for servos 
#define SHOULDER    1
#define ELBOW       2
#define ENC_A       13
#define ENC_B       14
#define POS_REWARD  1
#define NEG_REWARD  -1

//declare objects
Dynamixel Dxl(1); 
QuadEncoder Enc(ENC_A, ENC_B);

//declare GLOBAL variables
int S[2] = {0, 0};
int state; 
char cmdFound = 0;
char temp = 0;
char incoming[100];
int strIndex = 0;
int position = 0;
float Reward = 0;

void setup(){
    //initialize servo bus
    Dxl.begin(2);
    //initialize all variables
    Serial3.begin(9600);//initialize serial to pi
    //initialize serial to USB
    SerialUSB.begin();
    
    //initialize servos
    Dxl.jointMode(SHOULDER); //jointMode() is to use position mode
    Dxl.jointMode(ELBOW); //jointMode() is to use position mode
}

void loop()
{
   if (Serial3.available()){
    temp = Serial3.read();
    SerialUSB.print(temp);
    if(temp == '\n'){
      cmdFound = 1;
      incoming[strIndex] = '\0';
      strIndex = 0;
    }
    else{
      incoming[strIndex] = temp;
      strIndex++;
    } 
   }
   if(cmdFound){
      if(strcmp(incoming,"up")==0){
//          Serial3.print("OpenCM got: ");
//          Serial3.println(incoming);
          if((S[0]+1) <= 3){
            S[0]=S[0]+1; 
            Dxl.writeWord(SHOULDER, GOAL_POSITION, 100*(S[0]));
            while(Dxl.readByte(SHOULDER, MOVING));
            Serial3.print("success");
          }
          // Measure the Reward
          if(position < Enc.pos()) Reward = POS_REWARD;
          else if(position > Enc.pos()) Reward = NEG_REWARD;
          position = Enc.pos();
          Serial3.print("Reward: "); Serial3.print(Reward);
          SerialUSB.print(S[0]); SerialUSB.println(S[1]);
      }
      else if(strcmp(incoming,"down")==0){
//          Serial3.print("OpenCM got: ");
//          Serial3.println(incoming);
          if((S[0]-1) >= 0){
            S[0]=S[0]-1; 
            Dxl.writeWord(SHOULDER, GOAL_POSITION, 100*(S[0]));
            while(Dxl.readByte(SHOULDER, MOVING));
            Serial3.print("success");
          }
          // Measure the Reward
          if(position < Enc.pos()) Reward = POS_REWARD;
          else if(position > Enc.pos()) Reward = NEG_REWARD;
          position = Enc.pos();
          Serial3.print("Reward: "); Serial3.print(Reward);
          SerialUSB.print(S[0]); SerialUSB.println(S[1]);
      }
      else if(strcmp(incoming,"left")==0){
//          Serial3.print("OpenCM got: ");
//          Serial3.println(incoming);
          if((S[1]-1) >= 0){
            S[1]=S[1]-1; 
            Dxl.writeWord(ELBOW, GOAL_POSITION, 100*(S[1]));
            while(Dxl.readByte(ELBOW, MOVING));
            Serial3.print("success");
          }
          // Measure the Reward
          if(position < Enc.pos()) Reward = POS_REWARD;
          else if(position > Enc.pos()) Reward = NEG_REWARD;
          position = Enc.pos();
          Serial3.print("Reward: "); Serial3.print(Reward);
          SerialUSB.print(S[0]); SerialUSB.println(S[1]);
      }
      else if(strcmp(incoming,"right")==0){
//          Serial3.print("OpenCM got: ");
//          Serial3.println(incoming);
          if((S[1]+1) <= 3){
            S[1]=S[1]+1; 
            Dxl.writeWord(ELBOW, GOAL_POSITION, 100*(S[1]));
            while(Dxl.readByte(ELBOW, MOVING));
            Serial3.print("success");
          }
          // Measure the Reward
          if(position < Enc.pos()) Reward = POS_REWARD;
          else if(position > Enc.pos()) Reward = NEG_REWARD;
          position = Enc.pos();
          Serial3.print("Reward: "); Serial3.print(Reward);
          SerialUSB.print(S[0]); SerialUSB.println(S[1]);
      }
      else if(strcmp(incoming,"zero")==0){
          Serial3.print("OpenCM got: ");
          Serial3.println(incoming);
          ZeroServos();
          S={0,0};
          Serial3.print("success");
          SerialUSB.print(S[0]); SerialUSB.println(S[1]);
      }
//       else{
//          Serial3.print("OpenCM got: ");
//          Serial3.println(incoming);
//          Serial3.println("That command does nothing");
//       } 
       

    cmdFound = 0;
   }
   
}

void ZeroServos(){
    Dxl.writeWord(BROADCAST_ID, GOAL_POSITION, 0);           //set all servos to zero position
}
