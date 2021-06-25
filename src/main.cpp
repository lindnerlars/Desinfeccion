/**************************************************************************************************
 * Desinfeccion
 *
 * Created: 25/01/2021
 * Author: Lars Lindner
 *
 * Control de un Arduino Pro Mini para una Estacion de Desinfeccion
 * 
 * 
**************************************************************************************************/

#include <Arduino.h>
#include <HCSR04.h>

#define echopin 2
#define trigpin 3
#define relaypin 4
#define ledPin 13

enum machineStates
{
  DETECT,
  RUN,
  STOP
};
machineStates states = DETECT;

HCSR04 hc(trigpin, echopin);

int count1 = 0;
float threshold = 30.00;

void blinkLED(int count2)
{
  for (int i = 0; i < count2; i++)
  {
    digitalWrite(ledPin, !digitalRead(ledPin));
    delay(100);
    digitalWrite(ledPin, !digitalRead(ledPin));
    delay(400);
  }
}

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(relaypin, OUTPUT);
  blinkLED(2);
  Serial.println("Enter DETECT");
  Serial.begin(9600);
}

void loop()
{

  if (states == DETECT)
  {
    if (hc.dist() < threshold)
    {
      delay(100);
      count1++;
      if (count1 == 5)
      {
        count1 = 0;
        Serial.println("Enter RUN");
        states = RUN;
      }
    }
    else
    {
      count1 = 0;
    }
  }

  if (states == RUN)
  {
    Serial.print("[cm]: ");
    Serial.println(hc.dist());
    digitalWrite(relaypin, HIGH);
    // blinkLED(3);                   // Sustituye eso con _delay_ms(), ya que se a veces se quedo trabado usando la subrutina blinkLED()
    _delay_ms(250);
    digitalWrite(relaypin, LOW);
    Serial.println("Enter STOP");
    states = STOP;
  }

  if ((states == STOP) && (hc.dist() > threshold))
  {
    Serial.println("Enter DETECT");
    states = DETECT;
  }
}
