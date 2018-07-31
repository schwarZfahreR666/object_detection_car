void setup()
{
 
  Serial.begin(9600);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
}
int  A0Value=0;
 int A1Value=0;
 int button_catch=1;
 int button_put=1;
void loop()
{
  A0Value=analogRead(A0);
  A1Value=analogRead(A1);
  A0Value=map(analogRead(A0),0,1024,0,3);
  A1Value=map(analogRead(A1),0,1024,0,3);
  button_catch=digitalRead(2);
  button_put=digitalRead(3);
  if(A0Value==0&&A1Value==1)
  {
    Serial.print(1);
    Serial.println();
    }
   else if(A0Value==2&&A1Value==1)
  {
    Serial.print(5);
    Serial.println();
    }
    else if(A0Value==1&&A1Value==0)
  {
    Serial.print(2);
    Serial.println();
    }
    else if(A0Value==1&&A1Value==1)
  {
    Serial.print(3);
    Serial.println();
    }
    else if(A0Value==1&&A1Value==2)
  {
    Serial.print(4);
    Serial.println();
    }
    if(button_catch==0&&button_put==1)
  {
    Serial.print(6);
    Serial.println();
    }
    else if(button_catch==1&&button_put==0)
    {
      Serial.print(7);
    Serial.println();
      }
    delay(100);
}

