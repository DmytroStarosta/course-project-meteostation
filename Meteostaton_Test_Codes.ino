-----------------------1----------------------------------
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
    if (rtc.lostPower() || digitalRead(buttonPin) == LOW) { 
        Serial.println("Setting RTC to compile time...");
        rtc.adjust(DateTime(F(DATE), F(TIME)));
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
---------------------------------------------------------------------




-----------------------2----------------------------------
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
volatile uint8_t displayState = 0;
unsigned long lastTime = 0;
const long debounceTime = 50;

void IRAM_ATTR buttonFunc() {
    long nowTime = millis();
    if (nowTime - lastTime > debounceTime) {
        displayState = (displayState + 1) % 3;
        lastTime = nowTime;
    }
}

void setup() {
    pinMode(buttonPin, INPUT_PULLUP);
    Serial.begin(115200);
    dht.begin();
    
    Wire.begin(21, 22);
    lcd.init();
    lcd.backlight();

    if (!rtc.begin()) {
        Serial.println("RTC init failed");
        for (;;);
    }

    if (rtc.lostPower()) {
        rtc.adjust(DateTime(DATE,TIME));
    }

    attachInterrupt(digitalPinToInterrupt(buttonPin), buttonFunc, FALLING);
}

void loop() {
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();
    DateTime now = rtc.now();

    lcd.clear();

    if (displayState == 0) {  
        lcd.setCursor(0, 0);
        lcd.print("Humidity:");
        lcd.setCursor(5, 1);
        lcd.print(humidity);
        lcd.print("%");

    } else if (displayState == 1) {  
        lcd.setCursor(0, 0);
        lcd.print("Temperature:");
        lcd.setCursor(5, 1);
        lcd.print(temp);
        lcd.print("C");

    } else if (displayState == 2) {  
        lcd.setCursor(0, 0);
        lcd.print(now.year(), DEC);
        lcd.print('/');
        lcd.print(now.month(), DEC);
        lcd.print('/');
        lcd.print(now.day(), DEC);

        lcd.setCursor(0, 1);
        lcd.print(now.hour(), DEC);
        lcd.print(':');
        lcd.print(now.minute(), DEC);
        lcd.print(':');
        lcd.print(now.second(), DEC);
    }

    delay(500);
}
---------------------------------------------------------------------





-----------------------3---------це для налаштування часу вручну, гортай далі-------------------------
#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;

void setup() {
  Wire.begin(21, 22);  // ESP32 SDA=21, SCL=22
  Serial.begin(115200);

  if (!rtc.begin()) {
    Serial.println("RTC не знайдено!");
    while (1);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC не працює! Встановлюємо час...");
  }

  // ТУТ ВСТАНОВЛЮЄМО ЧАС ВРУЧНУ
  rtc.adjust(DateTime(2025, 2, 11, 20, 45, 0));  
  // Рік, Місяць, День, Година, Хвилина, Секунда

  Serial.println("Час встановлено!");
}

void loop() {
  DateTime now = rtc.now();
  Serial.print(now.year());
  Serial.print('-');
  Serial.print(now.month());
  Serial.print('-');
  Serial.print(now.day());
  Serial.print(' ');
  Serial.print(now.hour());
  Serial.print(':');
  Serial.print(now.minute());
  Serial.print(':');
  Serial.print(now.second());
  Serial.println();

  delay(1000);
}
---------------------------------------------------------------------






-----------------------4----------------------------------
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Адреса модуля LCD I2C (може бути 0x27 або 0x3F)

void setup() {
  Wire.begin(21, 22);  // ESP32 SDA=21, SCL=22
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("RTC не знайдено!");
    while (1);  // Зупиняємо виконання програми
  }

  if (!rtc.isrunning()) {
    lcd.setCursor(0, 0);
    lcd.print("RTC не працює!");
    Serial.println("RTC не працює. Потрібно налаштувати час.");
    while (1);  // Зупиняємо виконання
  }

  lcd.setCursor(0, 0);
  lcd.print("RTC працює.");
  delay(1000);
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now();  // Зчитуємо час з DS1307

  char dateBuffer[17];
  char timeBuffer[9];

  // Форматуємо дату у вигляді: YYYY-MM-DD
  snprintf(dateBuffer, sizeof(dateBuffer), "%04d-%02d-%02d", now.year(), now.month(), now.day());
  // Форматуємо час у вигляді: HH:MM:SS
  snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

  lcd.setCursor(0, 0);
  lcd.print(dateBuffer);

  lcd.setCursor(0, 1);
  lcd.print(timeBuffer);

  delay(1000);  // Оновлення раз на секунду
}
---------------------------------------------------------------------











-----------------------5---------------Як пройшов твій день? :)-------------------
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

// Ініціалізація об'єктів для RTC та LCD
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Адреса I2C дисплея зазвичай 0x27 або 0x3F

void setup() {
  Wire.begin(21, 22); // SDA=21, SCL=22 для ESP32
  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    lcd.print("RTC не знайдено!");
    while (1);
  }

  if (!rtc.isrunning()) {
    lcd.clear();
    lcd.print("RTC не налашт.");
    delay(2000);
    rtc.adjust(DateTime(DATE, TIME)); // Встановлює час компіляції
  }

  lcd.clear();
  lcd.print("RTC OK");
  delay(1000);
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now();

  char dateBuffer[17]; // Буфер для дати
  char timeBuffer[9];  // Буфер для часу

  snprintf(dateBuffer, sizeof(dateBuffer), "%04d-%02d-%02d",
           now.year(), now.month(), now.day());

  snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d",
           now.hour(), now.minute(), now.second());

  lcd.setCursor(0, 0);
  lcd.print(dateBuffer);

  lcd.setCursor(0, 1);
  lcd.print(timeBuffer);

  delay(1000);
}
---------------------------------------------------------------------