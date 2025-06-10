#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "../include/config.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTPIN 18   
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


volatile bool buttonPressed = false;
unsigned long lastTime = 0;
const long debounceTime = 200;

byte degreeChar[] = {
    B00111, B00101, B00111, B00000,
    B00000, B00000, B00000, B00000
};

void IRAM_ATTR buttonFunc() {
    buttonPressed = true;
}

void setup() {
    pinMode(buttonPin, INPUT_PULLUP);
    Serial.begin(115200);
    dht.begin();
    
    Wire.begin(21, 22);
    lcd.init();
    lcd.backlight();

    lcd.createChar(0, degreeChar);

    attachInterrupt(digitalPinToInterrupt(buttonPin), buttonFunc, FALLING);
    lcd.clear();
}

void loop() {
    static uint8_t displayState = 0;
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (buttonPressed) {
        unsigned long nowTime = millis();
        if (nowTime - lastTime > debounceTime) {
            displayState = (displayState + 1) % 2;
            lcd.clear();
            lastTime = nowTime;
        }
        buttonPressed = false;
    }

    lcd.setCursor(2, 0);
    if (displayState == 0) {
        lcd.print("Temperature:");
        lcd.setCursor(5, 1);
        lcd.print(temp);
        lcd.write(byte(0));
        lcd.print("C   ");
    } else {
        lcd.print("Humidity:");
        lcd.setCursor(5, 1);
        lcd.print(humidity);
        lcd.print("%   ");
    }

    delay(500);
}