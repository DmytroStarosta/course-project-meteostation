#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <RTClib.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTPIN 18
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

RTC_DS3231 rtc;

const uint8_t buttonPin = 5;
volatile bool buttonPressed = false;
unsigned long lastTime = 0;
const long debounceTime = 200;

byte degreeChar[] = {
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
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

    if (!rtc.begin()) {
        Serial.println("RTC init failed");
        for (;;);
    }

    if (rtc.lostPower()) { 
        Serial.println("Setting RTC to compile time...");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        Serial.println("RTC updated!");
    }

    attachInterrupt(digitalPinToInterrupt(buttonPin), buttonFunc, FALLING);
    lcd.clear();
}

void loop() {
    static uint8_t displayState = 0;
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();
    DateTime now = rtc.now();

    if (buttonPressed) {  
        unsigned long nowTime = millis();
        if (nowTime - lastTime > debounceTime) {  
            displayState = (displayState + 1) % 3;
            lcd.clear();
            lastTime = nowTime;
        }
        buttonPressed = false;
    }

    if (displayState == 0) {  
        lcd.setCursor(2, 0);
        lcd.print("Temperature:");
        lcd.setCursor(5, 1);
        lcd.print(temp);
        lcd.write(byte(0));
        lcd.print("C   ");

    } else if (displayState == 1) {  
        lcd.setCursor(4, 0);
        lcd.print("Humidity:");
        lcd.setCursor(5, 1);
        lcd.print(humidity);
        lcd.print("%   ");

    } else if (displayState == 2) {  
        lcd.setCursor(3, 1);
        lcd.print(now.day() < 10 ? "0" : "");
        lcd.print(now.day(), DEC);
        lcd.print('/');
        lcd.print(now.month() < 10 ? "0" : "");
        lcd.print(now.month(), DEC);
        lcd.print('/');
        lcd.print(now.year(), DEC);
        
        lcd.setCursor(4, 0);
        lcd.print(now.hour() < 10 ? "0" : "");  
        lcd.print(now.hour(), DEC);
        lcd.print(':');
        lcd.print(now.minute() < 10 ? "0" : "");  
        lcd.print(now.minute(), DEC);
        lcd.print(':');
        lcd.print(now.second() < 10 ? "0" : "");  
        lcd.print(now.second(), DEC);
    }

    delay(500);
}
