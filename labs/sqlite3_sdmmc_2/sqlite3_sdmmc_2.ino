/*
    This example opens Sqlite3 databases from SD Card and
    retrieves data from them.
    Before running please copy following files to SD Card:
    data/mdr512.db
    data/census2000names.db
*/
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include "SD_MMC.h"
#include <CMMC_RTC.h>

CMMC_RTC *rtc;

String s_Date = " ";
String s_Time = " ";
String s_Name = " ";

int id = 0;
int value = 0;
int rc;
uint32_t _executedTime = 0;

sqlite3 *db1;
static char *zErrMsg = 0;
const char *data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName) {
    int i;
    Serial.printf("%s: ", (const char *)data);
    for (i = 0; i < argc; i++)
    {
        Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    Serial.printf("\n");
    return 0;
}

int openDb(const char *filename, sqlite3 **db) {
    int rc = sqlite3_open(filename, db);
    if (rc) {
        Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
        ESP.deepSleep(1e6);
    }
    else {
        Serial.printf("Opened database successfully, status=%d\n", rc);
    }
    return rc;
}

int db_exec(sqlite3 *db, const char *sql) {
    Serial.println(sql);
    long start = micros();
    int rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        Serial.printf("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        Serial.printf("Operation done successfully. ");
    }

    _executedTime = (micros() - start)/1000;
    Serial.printf("Time taken: %lu ms\r\n", _executedTime);
    return rc;
}

void deleteFile(fs::FS &fs, const char *path) {
    Serial.printf("Deleting file: %s\n", path);
    if (fs.remove(path))
    {
        Serial.println("File deleted");
    }
    else
    {
        Serial.println("Delete failed");
    }
}

void printCardInfo() {
    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD_MMC card attached");
    }
    else {
        Serial.print("SD_MMC Card Type: ");
        if (cardType == CARD_MMC) {
            Serial.println("MMC");
        }
        else if (cardType == CARD_SD) {
            Serial.println("SDSC");
        }
        else if (cardType == CARD_SDHC) {
            Serial.println("SDHC");
        }
        else {
            Serial.println("UNKNOWN");
        }
    }
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
}

void setup()
{
    Serial.begin(115200);
    pinMode(2, INPUT_PULLUP);
    SD_MMC.begin("/sdcard", true);
    printCardInfo();
    sqlite3_initialize();
    // rtc = new CMMC_RTC();
    // rtc->setup();
    // rtc->loop();
    deleteFile(SD_MMC, "/ina219.db");


    if (openDb("/sdcard/ina219.db", &db1) == SQLITE_OK) {
      rc = db_exec(db1, "CREATE TABLE IF NOT EXISTS datalog (id INTEGER PRIMARY KEY AUTOINCREMENT, date TEXT, time TEXT, heap INTEGER, IDname content, ms INTEGER);");
      if (rc != SQLITE_OK)  {
          sqlite3_close(db1);
          return;
      }

      static char buffer[100];
      sprintf(buffer, "INSERT INTO datalog(time, ms, heap) VALUES(%lu, %lu, %lu);", _executedTime, millis(), ESP.getHeapSize());
      rc = db_exec(db1, buffer);
      if (rc == SQLITE_OK) {
        Serial.println("INSERTED.");
      }
    }


    id++;
    value = random(0, 100);
    s_Name = "ID001";
}


void loop()
{
    static char buffer[100];
    sprintf(buffer, "INSERT INTO datalog(time, ms, heap) VALUES(%lu, %lu, %lu);", _executedTime, millis(), ESP.getHeapSize());
    rc = db_exec(db1, buffer);
    if (rc == SQLITE_OK) {
      // Serial.println("INSERTED.");
    }
    // delay(100);
}
