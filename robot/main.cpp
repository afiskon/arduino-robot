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

const int directionM1 = 4;
const int speedM1 = 5;
const int speedM2 = 6;
const int directionM2 = 7;

const int FORWARD = HIGH;
const int BACKWARD = LOW;

const uint64_t addresses[] = { 0xAF1510009001LL, 0xAF1510009002LL };

RF24 radio(9, 10);
EnginesState enginesState;
unsigned long lastEnginesStateUpdateMs = 0;

void setDefaultEnginesState()
{
  enginesState.directionM1 = FORWARD;
  enginesState.directionM2 = FORWARD;
  enginesState.speedM1 = 0;
  enginesState.speedM2 = 0;
}

void setup()
{
  setDefaultEnginesState();

  pinMode(directionM1, OUTPUT);
  pinMode(directionM2, OUTPUT);
  pinMode(speedM1, OUTPUT);
  pinMode(speedM2, OUTPUT);

  radio.begin();
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  // radio.openWritingPipe(addresses[0]); // not used yet
  radio.openReadingPipe(1, addresses[1]);
  radio.startListening();
}

void loop()
{
  delay(10);

  if(radio.available())
  {
    radio.read( &enginesState, sizeof(enginesState) );
    lastEnginesStateUpdateMs = millis();
  }
  else if(millis() - lastEnginesStateUpdateMs > 500)
  {
    setDefaultEnginesState();
    lastEnginesStateUpdateMs = millis();
  }

  digitalWrite(directionM1, enginesState.directionM1);
  analogWrite(speedM1, enginesState.speedM1);

  digitalWrite(directionM2, enginesState.directionM2);
  analogWrite(speedM2, enginesState.speedM2);
}
