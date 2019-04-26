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
sqlite3 *db1;
char *zErrMsg = 0;

const char *data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    Serial.printf("%s: ", (const char *)data);
    for (i = 0; i < argc; i++)
    {
        Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    Serial.printf("\n");
    return 0;
}

int openDb(const char *filename, sqlite3 **db)
{
    int rc = sqlite3_open(filename, db);
    if (rc)
    {
        Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
        ESP.deepSleep(1e6);
        return rc;
    }
    else
    {
        Serial.printf("Opened database successfully\n");
    }
    return rc;
}

int db_exec(sqlite3 *db, const char *sql)
{
    Serial.println(sql);
    long start = micros();
    int rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        Serial.printf("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        Serial.printf("Operation done successfully\n");
    }
    Serial.print(F("Time taken:"));
    Serial.println(micros() - start);
    return rc;
}

void deleteFile(fs::FS &fs, const char *path)
{
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

void setup()
{
    Serial.begin(115200);
    // char *zErrMsg = 0;

    SPI.begin();
    SD_MMC.begin("/sdcard", true);
    delay(50);

    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE)
    {
        Serial.println("No SD_MMC card attached");
    }
    else
    {
        Serial.print("SD_MMC Card Type: ");
        if (cardType == CARD_MMC)
        {
            Serial.println("MMC");
        }
        else if (cardType == CARD_SD)
        {
            Serial.println("SDSC");
        }
        else if (cardType == CARD_SDHC)
        {
            Serial.println("SDHC");
        }
        else
        {
            Serial.println("UNKNOWN");
        }
    }
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

    sqlite3_initialize();
    delay(10);

    rtc = new CMMC_RTC();
    rtc->setup();
    rtc->loop();

    Serial.print("get dateTime: ");
    Serial.println(rtc->getDateTimeString());
    Serial.print("get gettDatestamp: ");
    Serial.println(rtc->getDateString());
    Serial.print("get gettTimestamp: ");
    Serial.println(rtc->getTimeString());


    // deleteFile(SD_MMC, "/ina219.db");

    // Open database 1
    if (openDb("/sdcard/ina219.db", &db1))
        return;

    // rc = db_exec(db1, "CREATE TABLE datalog (date TEXT, time TEXT, id INTEGER, value INTEGER, IDname content);");
    // if (rc != SQLITE_OK)
    // {
    //     // sqlite3_close(db1);
    //     // return;
    // }

    // const char *data = "INSERT INTO test1 VALUES (1, 'Hello, World from test1');";
    // rc = db_exec(db1, data);
    // rc = db_exec(db1, "INSERT INTO datalog VALUES (" +s_Date.c_str+ "," +s_Time.c_str+ "," +String(id)+ "," +String(value)+ ", 'superman');");
    // char buffer[100];
    // sprintf(buffer, "INSERT INTO datalog VALUES ('%s', '%s', %d, %d, '%s');", rtc->getDateString().c_str(),rtc->getTimeString().c_str(), id, value, s_Name.c_str());
    // Serial.println(buffer);

    id++;
    value = random(0, 100);
    s_Name = "ID001";

    char buffer[100];
    sprintf(buffer, "INSERT INTO datalog VALUES ('%s', '%s', %d, %d, '%s');", rtc->getDateString().c_str(), rtc->getTimeString().c_str(), id, value, s_Name.c_str());
    Serial.println(buffer);

    rc = db_exec(db1, buffer);
    Serial.print("SQLITE status: ");
    Serial.print(rc);
    delay(1000);

    if (rc != SQLITE_OK)
    {
        Serial.print("Save Failed...");
        sqlite3_close(db1);
    }
    else
    {
        Serial.print("Save OK...");
    }
    sqlite3_close(db1);
}

uint32_t pevTime = 0;

void loop()
{
    // rtc->loop();
    // uint32_t curTime = millis();
    // if (curTime - pevTime >= 5000)
    // {
    //     pevTime = curTime;
    //     saveDB();
    //     Serial.println("save db.");
    // }
}