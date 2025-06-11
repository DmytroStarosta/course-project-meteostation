#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#include "../include/initPins.h"
#include "../include/config.h"

#define DHTPIN 18
#define DHTTYPE DHT22

byte degreeChar[] = {
    B00111, B00101, B00111, B00000,
    B00000, B00000, B00000, B00000};

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

extern volatile bool buttonPressed;

volatile uint32_t lastButtonTime = 0;
void IRAM_ATTR buttonFunc()
{
    uint32_t nowTime = millis();
    if ((nowTime - lastButtonTime) > debounceTime)
    {
        lastButtonTime = nowTime;
        buttonPressed = true;
    }
}

void initPins()
{
    pinMode(buttonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPin), buttonFunc, RISING);

    dht.begin();

    Wire.begin(21, 22);
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, degreeChar);
    lcd.clear();
}