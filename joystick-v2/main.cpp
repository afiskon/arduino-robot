#include <Arduino.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

typedef struct {
  uint8_t directionM1;
  uint8_t directionM2;
  uint8_t speedM1;
  uint8_t speedM2;
} EnginesState;

/* depends on your point of view */
/*
const int FORWARD = HIGH;
const int BACKWARD = LOW;
*/
const int FORWARD = LOW;
const int BACKWARD = HIGH;

const int joystickX = A0;
const int joystickY = A1;
const int joystickBtn = 2;

const int THRESHOLD_LOW = 490;
const int THRESHOLD_HIGH = 510;

const uint64_t addresses[] = { 0xAF1510009001LL, 0xAF1510009002LL };

RF24 radio(9, 10);

void setup()
{
  Serial.begin(9600);

  pinMode(joystickX, INPUT);
  pinMode(joystickY, INPUT);
  pinMode(joystickBtn, INPUT);

  radio.begin();
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(addresses[1]);
  // radio.openReadingPipe(1, addresses[0]); // not used yet
}

void loop()
{
  EnginesState enginesState;
  delay(10);

  int x = analogRead(joystickX);
  int y = analogRead(joystickY);

  Serial.println(String("x = ") + x + ", y = " + y);

  if(y < THRESHOLD_LOW)
  {
    enginesState.directionM1 = FORWARD;
    enginesState.directionM2 = FORWARD;
    enginesState.speedM1 = map(y, THRESHOLD_LOW, 0, 0, 255) ;
    enginesState.speedM2 = enginesState.speedM1;
  }
  else if(y > THRESHOLD_HIGH)
  {
    enginesState.directionM1 = BACKWARD;
    enginesState.directionM2 = BACKWARD;
    enginesState.speedM1 = map(y, THRESHOLD_HIGH, 1023, 0, 255);
    enginesState.speedM2 = enginesState.speedM1;
  }
  else
  {
    enginesState.directionM1 = FORWARD;
    enginesState.directionM2 = FORWARD;
    enginesState.speedM1 = 0;
    enginesState.speedM2 = enginesState.speedM1;
  }

  if(x < THRESHOLD_LOW)
  {
    enginesState.speedM2 = map(x, THRESHOLD_LOW, 0, enginesState.speedM2, 0);
  }
  else if(x > THRESHOLD_HIGH)
  {
    enginesState.speedM1 = map(x, THRESHOLD_HIGH, 1023, enginesState.speedM1, 0); 
  }

  if(digitalRead(joystickBtn) == HIGH) /* rotation mode */
  {
    if(x < THRESHOLD_LOW)
    {
      enginesState.directionM1 = FORWARD;
      enginesState.directionM2 = BACKWARD;
      enginesState.speedM1 = map(x, THRESHOLD_LOW, 0, 0, 255);
      enginesState.speedM2 = enginesState.speedM1;
    }
    else if(x > THRESHOLD_HIGH)
    {
      enginesState.directionM1 = BACKWARD;
      enginesState.directionM2 = FORWARD;
      enginesState.speedM1 = map(x, THRESHOLD_HIGH, 1023, 0, 255);
      enginesState.speedM2 = enginesState.speedM1;
    }
  }

  radio.write( &enginesState, sizeof(enginesState) );
}
