#include <ServoTimer2.h>
#define left_sensor A4
#define right_sensor A2
#define ECHOPIN A0 
#define TRIGPIN A1 
#define lift_motor1 9
#define lift_motor2 10
#define right_motor1 5
#define right_motor2 6
#define hand_port         3
#define turn_R     100
#define turn_L     140
#define motor_speed_R     160
#define motor_speed_L     200
ServoTimer2 myservo; 
int hand_ang=80;
int a;
char line[500] = ""; 
int ret = 0;
String line2="";
int DisMeasure(int value)
{
  return ((2914/(value+5))-1);
}
void echo_init()
{
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  }
float echo_work()
{
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  float distance = pulseIn(ECHOPIN, HIGH);
  distance= distance/58;
  Serial.println(distance);
  //delay(200);
  return distance;
  }
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
  delay(400);
  motor_stop();
  
  }
void back_1s()
  {
    analogWrite(right_motor1,motor_speed_R);
    analogWrite(right_motor2,0);
    analogWrite(lift_motor1,motor_speed_L);
    analogWrite(lift_motor2,0);
    delay(400);
    motor_stop();
  }
  void turn_right()
  {
    analogWrite(right_motor1,0);
    analogWrite(right_motor2,turn_R);
    analogWrite(lift_motor1,turn_L);
    analogWrite(lift_motor2,0);
    delay(200);
    motor_stop(); 
    
    }
    void turn_lift()
  {
    analogWrite(right_motor1,turn_R+15);
    analogWrite(right_motor2,0);
    analogWrite(lift_motor1,0);
    analogWrite(lift_motor2,turn_L+5);
    delay(200);
    motor_stop();
    
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
  echo_init();
  Serial.begin(9600);
}
int data=3;
 int data_his=3;
 float distance=1000;
 int flag=0;
 int f=0;
 int left_wall=0;
 int right_wall=0;

 
void loop() {
  flag++;
  
  //data=Serial.parseInt();
  if (Serial.available() > 0) 
{  
ret = Serial.readBytesUntil('\n', line, 500);
line2=line;
data=line2.toInt();
}
if(flag==3)
{
distance=echo_work();
flag=0;
}
if(distance>8)
{

//Serial.println(data);
  //data = data.decode('utf-8');
  if(data>0)
  {
    select(data);
    //data=data_his;
    //select(data);
    
    
      
    }
    //Serial.println(data);
    data_his=data;
    data=0;
    delay(50);
}
else if(distance<8&&f==0)
{
  a=map(130,20,160,750,2250);
   myservo.write(a);
   f=1;
   delay(400);
   back_1s();
  }
  if (f==1)
  {
    left_wall=DisMeasure(analogRead(left_sensor));
    right_wall=DisMeasure(analogRead(right_sensor));
    if(left_wall<200&&right_wall<200)
    {
    turn_lift();
    turn_lift();
    go_1s();
    }
    if(left_wall>200&&right_wall<200)
    {
    turn_right();
    turn_right();
    }
    if(left_wall<200&&right_wall>200)
    {
    turn_lift();
    turn_lift();
    }
    if(left_wall>200&&right_wall>200)
    {
    back_1s();
    back_1s();
    }
    
    
    
   }
  



}
