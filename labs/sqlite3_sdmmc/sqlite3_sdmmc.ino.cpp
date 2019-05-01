# 1 "c:\\users\\asus\\appdata\\local\\temp\\tmpxhzai6"
#include <Arduino.h>
# 1 "C:/Users/Asus/Documents/esp32-lab-queue/labs/sqlite3_sdmmc/sqlite3_sdmmc.ino"
# 15 "C:/Users/Asus/Documents/esp32-lab-queue/labs/sqlite3_sdmmc/sqlite3_sdmmc.ino"
#include <stdio.h>

#include <stdlib.h>

#include <sqlite3.h>

#include <SPI.h>

#include <FS.h>

#include "SD_MMC.h"

#include <CMMC_RTC.h>

#include <functional>



typedef std::function<int(void *, int, char **, char **)> sqlite_cb_t;



CMMC_RTC *rtc;



String s_Date = " ";

String s_Time = " ";

String s_Name = " ";

int value = 0;



int rc;

static char *zErrMsg = 0;

sqlite3 *db1;



const char *data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName);
int openDb(const char *filename, sqlite3 **db);
int db_exec(sqlite3 *db, const char *sql);
void setup();
void saveDB();
void loop();
#line 61 "C:/Users/Asus/Documents/esp32-lab-queue/labs/sqlite3_sdmmc/sqlite3_sdmmc.ino"
static int callback(void *data, int argc, char **argv, char **azColName)

{

    int i;

    Serial.printf("%s: \r\n", (const char *)data);

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

    }

    else

    {

        Serial.printf("Opened database successfully, status=%d\n", rc);

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



void writeFile(fs::FS &fs, const char *path, const char *message)

{

    Serial.printf("Writing file: %s\n", path);



    File file = fs.open(path, FILE_WRITE);

    if (!file)

    {

        Serial.println("Failed to open file for writing");

        return;

    }

    if (file.print(message))

    {

        Serial.println("File written");

    }

    else

    {

        Serial.println("Write failed");

    }

}



void setup()

{

    Serial.begin(115200);

    delay(1);



    pinMode(2, INPUT_PULLUP);
# 239 "C:/Users/Asus/Documents/esp32-lab-queue/labs/sqlite3_sdmmc/sqlite3_sdmmc.ino"
    SPI.begin();

    SD_MMC.begin("/sdcard", true);

    sqlite3_initialize();

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

    Serial.println(" ");

    Serial.println(" ");







    rtc = new CMMC_RTC();

    rtc->setup();

    rtc->loop();
# 331 "C:/Users/Asus/Documents/esp32-lab-queue/labs/sqlite3_sdmmc/sqlite3_sdmmc.ino"
    if (openDb("/sdcard/labSqlite1.db", &db1))

    {

        Serial.print("Failed...");

        ESP.deepSleep(1e6);

        return;

    }



    rc = db_exec(db1, "CREATE TABLE IF NOT EXISTS dataman (id INTEGER PRIMARY KEY AUTOINCREMENT, date TEXT, time TEXT, name TEXT, value INTEGER);");

    if (rc != SQLITE_OK)

    {

        Serial.println("CREATE TABLE Failed...");

    }

    else

    {

        Serial.println("CREATE TABLE Successfully...");

    }
# 551 "C:/Users/Asus/Documents/esp32-lab-queue/labs/sqlite3_sdmmc/sqlite3_sdmmc.ino"
    Serial.println("Done process...");

}



uint32_t pevTime = 0;



void saveDB()

{

    char buffer[200];



    s_Date = rtc->getDateString();

    s_Time = rtc->getTimeString();

    s_Name = "ID001";

    value = random(0, 100);





    sprintf(buffer, "INSERT INTO dataman(date, time, name, value) VALUES ('%s', '%s', '%s', %d);", s_Date.c_str(), s_Time.c_str(), s_Name.c_str(), value);

    rc = db_exec(db1, buffer);

    if (rc != SQLITE_OK)

    {

        Serial.println("INSERT Failed...");

    }

    else

    {

        Serial.println("INSERT Done...");

    }

    Serial.println("");

    Serial.println("");

}



void loop()

{

    rtc->loop();

    uint32_t curTime = millis();

    if (curTime - pevTime >= 5000)

    {

        pevTime = curTime;

        saveDB();

    }

    delay(1);

}