#include <LittleFS.h>

#include "../include/updateDb.h"


void updateDatabase(float temp, float humidity) 
{
    File fileRead = LittleFS.open("/database.csv", "r");

    if (!fileRead) {
        Serial.println("Failed to open file for reading");
        return;
    }
    size_t fileSize = fileRead.size();
    fileRead.close();

    File fileUpdate;
    if (fileSize > 10000) {
        fileUpdate = LittleFS.open("/database.csv", "w");
    }
    else {
        fileUpdate = LittleFS.open("/database.csv", "a");
    }

    if (!fileUpdate) {
        Serial.println("Failed to open file for writing");
        return;
    }

    String data = "Temp: " + String(temp) + ", " + "Hum: " + String(humidity) + "\n";
    fileUpdate.println(data);
    fileUpdate.close();
}