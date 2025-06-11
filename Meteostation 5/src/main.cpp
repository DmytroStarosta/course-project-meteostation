#include <Arduino.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#include "../include/initPins.h"
#include "../include/config.h"
#include "../include/initServer.h"

extern WebServer server;
extern LiquidCrystal_I2C lcd;
extern DHT dht;
extern void updateDatabase(float temp, float humidity);
extern void updateDisplay(float temp, float humidity);
volatile bool buttonPressed = false;
uint32_t lastProgramTime = 0; // Yurii del

uint8_t displayState = 0;

float lastTemp = 0;
float lastHumidity = 0;

void setup()
{
    Serial.begin(115200);
    initPins();
    initServer();
}

void loop()
{
    server.handleClient();
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (buttonPressed)
    {
        displayState = (displayState + 1) % 2;
        buttonPressed = false;
    }

    if (lastTemp - temp > 0.2 || lastTemp - temp < -0.2 || lastHumidity - humidity > 0.2 || lastHumidity - humidity < -0.2)
    {
        lastTemp = temp;
        lastHumidity = humidity;
        updateDatabase(temp, humidity);
        updateDisplay(temp, humidity);
    }
}
