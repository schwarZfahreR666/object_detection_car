#include <ServoTimer2.h>


#define lift_motor1 9
#define lift_motor2 10
#define right_motor1 5
#define right_motor2 6
#define hand_port         3
#define motor_speed_R     160
#define motor_speed_L     200
ServoTimer2 myservo; 
int hand_ang=80;
int a;
void servo_init()
{
  myservo.attach(hand_port);
  a=map(hand_ang,20,160,750,2250);
  myservo.write(a);
  }
  void hand_put()
  {
    if(hand_ang>80)
    {
      hand_ang=hand_ang-5;
      a=map(hand_ang,20,160,750,2250);
      myservo.write(a);
      
      }
    }
    void hand_catch()
  {
    if(hand_ang<130)
    {
      hand_ang=hand_ang+5;
      a=map(hand_ang,20,160,750,2250);
      myservo.write(a);
      
      }
    }
void motor_init()
  {
  pinMode(right_motor1,OUTPUT);
  pinMode(right_motor2,OUTPUT);
  pinMode(lift_motor1,OUTPUT);
  pinMode(lift_motor2,OUTPUT);
  
  
  }
  void motor_stop()
  {
    analogWrite(right_motor1,0);
  analogWrite(right_motor2,0);
  analogWrite(lift_motor1,0);
  analogWrite(lift_motor2,0);
    
    }
void go_1s()
  {
  analogWrite(right_motor1,0);
  analogWrite(right_motor2,motor_speed_R);
  analogWrite(lift_motor1,0);
  analogWrite(lift_motor2,motor_speed_L);
  //delay(1000);
  
  }
void back_1s()
  {
    analogWrite(right_motor1,motor_speed_R);
    analogWrite(right_motor2,0);
    analogWrite(lift_motor1,motor_speed_L);
    analogWrite(lift_motor2,0);
   // delay(1000);
  }
  void turn_right()
  {
    analogWrite(right_motor1,0);
    analogWrite(right_motor2,motor_speed_R);
    analogWrite(lift_motor1,motor_speed_L);
    analogWrite(lift_motor2,0);
    //delay(3000);
    
    }
    void turn_lift()
  {
    analogWrite(right_motor1,motor_speed_R);
    analogWrite(right_motor2,0);
    analogWrite(lift_motor1,0);
    analogWrite(lift_motor2,motor_speed_L);
    //delay(3000);
    
    }
    void select(int data)
    {
      switch(data){
        case 1:go_1s();break;
        case 2:turn_right();break;
        case 3:motor_stop();break;
        case 4:turn_lift();break;
        case 5:back_1s();break;
        case 6:hand_put();break;
        case 7:hand_catch();break;
        
        }
      
      }
void setup() {
  
  motor_init();
  servo_init();
  Serial.begin(9600);
}
int data=3;
 int data_his=3;
 int ret=0;
 
void loop() {
  
  data=Serial.parseInt();
  //data = data.decode('utf-8');
  if(data>0)
  {
    select(data);
    data=data_his;
    select(data);
    
    
      
    }
    data_his=data;
    data=0;
    delay(50);
  



}
