#include <Arduino.h>
#include <Servo.h>

// Servos
int servo1 = 5;
Servo Servo1;

int servoClaw = 6;
Servo ServoClaw;

int servoHeight = 9;
Servo ServoHeight;

int clawHeight = 10;
Servo ClawHeight;

// Base Encoder
int CLK = 2;
int DT = 3;
int SW = 4;

int SWopen;
int currentState;
int lastState;

int counter = 80; // Inital state for the base rotary

// Height Encoder
int heightCLK = 8;
int heightDT = 7;
int heightSW = 11;

int heightCurrentState;
int heightLastState;
bool goingUp = true;

int clawHeightCounter = 70;
int heightCounter = 90; // Initial State for the height

void clawServo()
{
  int btnPressed = digitalRead(SW);

  if (btnPressed == 0)
  {
    Serial.println("Button Pressed");
    SWopen = !SWopen;

    if (SWopen)
    {
      Serial.println("Open");
      ServoClaw.write(120); // Open
      delay(250);
    }
    else
    {
      Serial.println("Close");
      ServoClaw.write(145); // Close
      delay(250);
    }
  }
}

void clawHeightServo()
{

  if (goingUp) {

    if (digitalRead(heightSW) == 0)
    {

      if (clawHeightCounter >= 120)
      {
        goingUp = false;
        return;
      };

      clawHeightCounter = clawHeightCounter + 10;
      delay(200);
      Serial.println(clawHeightCounter);

    }

  }
  else
  {

    if (digitalRead(heightSW) == 0)
    {
      if (clawHeightCounter <= 70)
      {
        goingUp = true;
        return;
      };

      clawHeightCounter = clawHeightCounter - 10;
      delay(200);
      Serial.println(clawHeightCounter);

    }
  }

  ClawHeight.write(clawHeightCounter);

}

void heightServo()
{

  heightCurrentState = digitalRead(heightCLK);

  if (heightLastState != heightCurrentState)
  {
    if(digitalRead(heightDT) == 0){

      if (heightCounter >= 120) 
      return;

      heightCounter++;

      Serial.println(heightCounter);

    } else {

      if (heightCounter <= 40)
        return;

        heightCounter--;

        Serial.println(heightCounter);

    }
  }

  ServoHeight.write(heightCounter);
}

void baseServo()
{

  currentState = digitalRead(CLK); // Starts at 1 but will turn to 0 when turned

  if (currentState != lastState) // remember, current state will be 0, last state will be 1
  {
    if (digitalRead(DT) == 1) // digitalRead(DT) reads 0 when turning right, reads 1 when turning left
    {
      Serial.println("Turning right");
      if ((counter) >= 170)
        return;
      (counter++);
    }
    else
    {
      Serial.println("Turning Left");
      if ((counter) <= 0)
        return;
      (counter--);
    }
  }

  Servo1.write((counter));
}

void setup()
{

  Serial.begin(9600);
  // Servos
  Servo1.attach(servo1); // attaches the servo on pin 9 to the servo object
  ServoClaw.attach(servoClaw);
  ServoHeight.attach(servoHeight);
  ClawHeight.attach(clawHeight);

  // Servo initial States
  Servo1.write(counter);
  ServoClaw.write(145); // claw starts closed
  ServoHeight.write(heightCounter);
  ClawHeight.write(clawHeightCounter);


  // Encoders
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  pinMode(heightCLK, INPUT);
  pinMode(heightDT, INPUT);
  pinMode(heightSW, INPUT);

  lastState = (digitalRead(CLK)); // Starts at 1
  heightLastState = (digitalRead(heightCLK));

}

void loop()
{
  baseServo();
  clawServo();
  heightServo();
  clawHeightServo();
}