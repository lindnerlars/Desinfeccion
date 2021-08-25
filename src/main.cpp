/**************************************************************************************************
 * Desinfeccion
 *
 * Created: 13/08/2021
 * Author: Lars Lindner
 * Revision: 24/08/2021
 *
 * Control de un Arduino Pro Mini (16MHz) para una Estacion de Desinfeccion 
 * en el Instituto de Ingenieria UABC
 * El pedal de la estacion debe estar limpio!
 * 
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

bool serial_flag = true;
int count_detect = 0;
int count_stop = 0;
float dist_min = 10.0;
float dist_max = 100.0;
float dist_value = 0.0;

void setup()
{
  pinMode(relaypin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Start main loop()");
}

void state_machine()
{
  switch (states)
  {
  case DETECT:
    if (serial_flag == true)
    {
      Serial.println("DETECT");
      serial_flag = false;
    }

    dist_value = hc.dist();
    if (dist_value < dist_min)
    {
      delay(50);
      count_detect++;

      // El usuario tiene que tener minimo 4 x delay() la pie dentro del rango dist_min
      if (count_detect >= 4)
      {
        Serial.print("[cm]: ");
        Serial.println(dist_value);
        count_detect = 0;
        serial_flag = true;
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
    digitalWrite(relaypin, HIGH);
    Serial.println("Relay HIGH");
    delay(250);
    digitalWrite(relaypin, LOW);
    Serial.println("Relay LOW");
    states = STOP;
    break;

  case STOP:
    if (serial_flag == true)
    {
      Serial.println("STOP");
      serial_flag = false;
    }

    dist_value = hc.dist();
    if (dist_value > dist_max)
    {
      delay(100);
      count_stop++;

      // Para evitar que por un movimiento leve del pie, no entre rapido al estado DETECT!
      if (count_stop >= 10)
      {
        Serial.print("[cm]: ");
        Serial.println(dist_value);
        count_stop = 0;
        serial_flag = true;
        states = DETECT;
      }
    }
    else
    {
      count_stop = 0;
    }
    break;
  }
}

void loop()
{
  state_machine();
  // delay(10);
}
