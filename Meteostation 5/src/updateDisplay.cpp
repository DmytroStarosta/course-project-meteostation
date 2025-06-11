#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "../include/updateDisplay.h"

extern LiquidCrystal_I2C lcd;
extern uint8_t displayState;

void updateDisplay(float temp, float humidity)
{
    lcd.clear();
    lcd.setCursor(2, 0);
    if (displayState == 0)
    {
        lcd.print("Temperature:");
        lcd.setCursor(5, 1);
        lcd.print(temp);
        lcd.write(byte(0));
        lcd.print("C   ");
    }
    else
    {
        lcd.print("Humidity:");
        lcd.setCursor(5, 1);
        lcd.print(humidity);
        lcd.print("%   ");
    }
}
