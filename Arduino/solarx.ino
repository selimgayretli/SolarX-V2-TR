#include <Servo.h>

#define STEP_DELAY 15

//Pin Definations of Battery Measure LEDs
#define led1 4
#define led2 5
#define led3 6
#define led4 7

int BUTTON_PIN = A2;

int buttonState = 0; 
int solar_mode = 1; //Default SolarX Mode

int TOLERANCE = 20; //Tolerance Of Between Two LDR's values

Servo servohori;
int servoh = 0;
int servohLimitHigh = 170;  //Maximum Limit Of Horizontal Servo
int servohLimitLow = 20;    //Minimum Limit Of Horizontal Servo

Servo servoverti;
int servov = 0;
int servovLimitHigh = 170;  //Maximum Limit Of Vertical Servo
int servovLimitLow = 30;    //Minimum Limit Of Vertical Servo

//Pin Definations of LDRs
int ldrtopr = A4;  //Top R LDR
int ldrbotr = A3;  //Bottom R LDR
int ldrtopl = A1;  //Top L LDR
int ldrbotl = A0;  //Bottom L LDR

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  servohori.attach(11); //Horizontal Servo Pin
  servohori.write(90);  //Horizontal Servo Angle
  delay(1500);
  servoverti.attach(9); //Vertical Servo Pin
  servoverti.write(90); //Vertical Servo Angle
  Serial.begin(9600); 

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  delay(20);
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    delay(50);
    if (solar_mode == 1) {  //Mode 1: Start Position
      for (int i = 0; i < 3; i++) {
        digitalWrite(led1, HIGH);
        delay(300);
        digitalWrite(led1, LOW);
        delay(300);
      }
      servohori.attach(11);
      servohori.write(90);
      delay(1500);
      servoverti.attach(9);
      servoverti.write(90);
      solar_mode++;
    } else if (solar_mode == 2) {  //Mode 2: Sunlight Position
      TOLERANCE = 3;
      for (int i = 0; i < 3; i++) {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        delay(300);
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        delay(300);
      }
      solar_mode++;
    } else if (solar_mode == 3) {  //Mode 3: Flashlight Position
      TOLERANCE = 20;
      for (int i = 0; i < 3; i++) {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        digitalWrite(led3, HIGH);
        delay(300);
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led3, LOW);
        delay(300);
      }
      solar_mode = 1;
    }
  }

  //Serial.println("Solar Mode:");
  //Serial.println(solar_mode);

  int solarvalue = analogRead(A6);
  int batvalue = analogRead(A7);
  //Calculating Percent Of Battery
    

  //Serial.println("solarvalue:");
  //Serial.println(solarvalue);

  //Serial.println("batvalue:");
  //Serial.println(batvalue);

  //Serial.println("voltage:");
  //Serial.println(voltage);

  //Capturing Analog Values Of Each LDR
  int topl = analogRead(ldrtopl);
  int topr = analogRead(ldrtopr);
  int botl = analogRead(ldrbotl);
  int botr = analogRead(ldrbotr);

  /*
  Serial.print(topl);
  Serial.print("\t\t");
  Serial.print(topr);
  Serial.print("\t\t");
  Serial.print(botl);
  Serial.print("\t\t");
  Serial.print(botr);
  Serial.print("\t\t");
  Serial.print(servov);
  Serial.print("\t\t");
  Serial.println(servoh);
  */
  
  //Calculating average of LDR
  int avgtop = (topl + topr) / 2;    //average of top LDRs
  int avgbot = (botl + botr) / 2;    //average of bottom LDRs
  int avgleft = (topl + botl) / 2;   //average of left LDRs
  int avgright = (topr + botr) / 2;  //average of right LDRs
  
/*
  Serial.print(avgtop);
  Serial.print("\t\t");
  Serial.print(avgbot);
  Serial.print("\t\t");
  Serial.print(avgleft);
  Serial.print("\t\t");
  Serial.println(avgright);
*/
  
  //Battery Measure
  if (voltage <= 3.3) {  //%0 - %20
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, HIGH);
    delay(500);
    digitalWrite(led4, LOW);
    delay(500);
  } else if (voltage > 3.3 && voltage <= 3.5) {  //%20 - %40
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, HIGH);
  } else if (voltage > 3.5 && voltage <= 3.75) {  //%40 - %60
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
  } else if (voltage > 3.75 && voltage <= 4) {  //%60 - %80
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
  } else if (voltage > 4) {  //%80 - %100
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
  }

  //Servo Move - Top & Bottom
  servov = servoverti.read(); //Read Last Position Of Vertical Servo

  if (avgbot - avgtop > TOLERANCE) {
    if (servov <= servovLimitLow) {
      servov = servovLimitLow;
    } else
      servoverti.write(servov - 1);


  } else if (avgtop - avgbot > TOLERANCE) {
    if (servov >= servovLimitHigh) {
      servov = servovLimitHigh;
    } else
      servoverti.write(servov + 1);
  } 
  else {
    delay(5);
  }
  delay(STEP_DELAY);

  //Servo Move - Right & Left
  servoh = servohori.read();  //Read Last Position Of Horizontal Servo

  if (avgleft - avgright > TOLERANCE) {
    if (servoh >= servohLimitHigh) {
      servoh = servohLimitHigh;
    } else
      servohori.write(servoh + 1);
  } else if (avgright - avgleft > TOLERANCE) {
    if (servoh <= servohLimitLow) {
      servoh = servohLimitLow;
    } else
      servohori.write(servoh - 1);
  } else {
    delay(5);
  }
  delay(STEP_DELAY);
}
