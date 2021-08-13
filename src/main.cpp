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
#include <avr/wdt.h>

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
float threshold = 10.00;

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
  Serial.println("Enter DETECT");
  Serial.begin(9600);

  // Enables the Watchdog Timer (WDT) for 4s
  // If in 4s the WDT is not resetted, the WDT resets the MCU
  wdt_enable(WDTO_4S);
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
    // blinkLED(3);                   // Sustituye eso con delay(), ya que a veces se queda trabado usando la subrutina blinkLED()
    delay(250);
    digitalWrite(relaypin, LOW);
    Serial.println("Enter STOP");
    states = STOP;
  }

  if ((states == STOP) && (hc.dist() > threshold))
  {
    Serial.println("Enter DETECT");
    delay(1000); // Para evitar que por un movimiento leve del pie, no entre rapido al estado DETECT!
    states = DETECT;
  }

  // Resets the Watchdog Timer
  wdt_reset();
}
