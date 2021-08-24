/**************************************************************************************************
 * Desinfeccion
 *
 * Created: 13/08/2021
 * Author: Lars Lindner
 * Revision: 20/08/2021
 *
 * Control de un Arduino Pro Mini (16MHz) para una Estacion de Desinfeccion 
 * en el Instituto de Ingenieria UABC
 * 
**************************************************************************************************/

#include <Arduino.h>
#include <HCSR04.h>

#define echopin 2
#define trigpin 3
#define relaypin 4
// #define ledPin 13

enum machineStates
{
  DETECT,
  RUN,
  STOP
};
machineStates states = DETECT;
HCSR04 hc(trigpin, echopin);

int count_detect = 0;
int count_stop = 0;
float dist_min = 10.0;
float dist_max = 50.0;
float dist_value = 0.0;

void setup()
{
  // pinMode(ledPin, OUTPUT);
  pinMode(relaypin, OUTPUT);
  Serial.println("Start main loop()");
  Serial.begin(9600);
}

void state_machine()
{
  switch (states)
  {
  case DETECT:
    Serial.println("DETECT");
    dist_value = hc.dist();
    if (dist_value < dist_min)
    {
      delay(50);
      count_detect++;
      if (count_detect >= 6)
      {
        count_detect = 0;
        states = RUN;
      }
    }
    else
    {
      count_detect = 0;
    }
    break;

  case RUN:
    Serial.println("RUN");
    Serial.print("[cm]: ");
    Serial.println(dist_value);
    digitalWrite(relaypin, HIGH);
    Serial.println("Relay HIGH");
    delay(250);
    digitalWrite(relaypin, LOW);
    Serial.println("Relay LOW");
    states = STOP;
    break;

  case STOP:
    Serial.println("STOP");
    dist_value = hc.dist();
    if (dist_value > dist_max)
    {
      delay(100);
      count_stop++;
      if (count_stop >= 10) // Para evitar que por un movimiento leve del pie, no entre rapido al estado DETECT!
      {
        count_stop = 0;
        digitalWrite(relaypin, LOW); // Para asegurar que realmente se apaga el relay
        Serial.print("Relay LOW");
        states = DETECT;
      }
    }
    else
    {
      count_stop = 0;
    }

    // if (hc.dist() > val_max)
    // {
    //   digitalWrite(relaypin, LOW); // Para asegurar que realmente se apaga el relay
    //   delay(1000);                 // Para evitar que por un movimiento leve del pie, no entre rapido al estado DETECT!
    //   Serial.println("Enter DETECT");
    //   states = DETECT;
    // }

    break;
  }
}

void loop()
{
  state_machine();
  // delay(10);
}
