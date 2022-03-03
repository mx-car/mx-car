/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
 */

#include <Wire.h>        // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy: #include "SSD1306.h"
#include <Arduino.h>
#include <stdio.h>

#define MAX_BUFFER_SIZE 0xFF
char dbuffer[MAX_BUFFER_SIZE];
SSD1306Wire display(0x3c, SDA, SCL); // ADDRESS, SDA, SCL  -  SDA and SCL usually populate
uint16_t loop_count = 0;

int led = 13;
void setup()
{
  pinMode(led, OUTPUT); // wait for a second
  digitalWrite(led, LOW);
  display.init();
  display.setFont(ArialMT_Plain_16);
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Hallo");
  display.display();
  Serial.begin ( 115200 );
  while (!Serial)
    display.drawString(0, 16, "Connected");
  display.display();
}


void loop()
{

  sprintf(dbuffer, "%2d", loop_count);
  display.clear();
  display.drawString(0, 16, dbuffer);
  display.display();
  loop_count++;
}
