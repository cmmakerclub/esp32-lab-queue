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

sqlite3 *db1;

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
        return rc;
    }
    else
    {
        Serial.printf("Opened database successfully\n");
    }
    return rc;
}

char *zErrMsg = 0;
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


void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1);
    char *zErrMsg = 0;
    int rc;

    SPI.begin();
    SD_MMC.begin("/sdcard", true);
    sqlite3_initialize();
    delay(50);

    // SD_MMC.end();
    // if (!SD_MMC.begin())
    // {
    //     Serial.println("Card Mount Failed");
    //     return;
    // }

    // SD_MMC.begin("/sdcard", true);

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

    writeFile(SD_MMC, "/hello.txt", "Hello ");

    rtc = new CMMC_RTC();
    rtc->setup();
    rtc->loop();

    // deleteFile(SD_MMC, "/superman.db");

    // Open database 1
    // if (openDb("/sdcard/census2000names.db", &db1))
    //     return;
    if (openDb("/sdcard/superman.db", &db1))
    {
        Serial.print("Failed...");
        ESP.deepSleep(1e6);
        return;
    }

    rc = db_exec(db1, "CREATE TABLE dataman (date TEXT, time TEXT, id INTEGER, value INTEGER, IDname content);");
    if (rc != SQLITE_OK)
    {
        Serial.print("CREATE TABLE Failed...");
        Serial.println(" ");
        Serial.println(" ");
        // sqlite3_close(db1);
        // return;
    }

    // rc = db_exec(db1, "INSERT INTO dataman VALUES ('26/04/19', '15.44', 1, 100 , 'superman');");
    // if (rc != SQLITE_OK)
    // {
    //     Serial.print("INSER Failed...");
    //     Serial.println(" ");
    //     Serial.println(" ");
    //     // sqlite3_close(db1);
    //     // return;
    // }

    // rc = db_exec(db1, "INSERT INTO dataman VALUES ('26/04/19', '15.44', 2, 50 , 'superman');");
    // if (rc != SQLITE_OK)
    // {
    //     Serial.print("INSER Failed...");
    //     Serial.println(" ");
    //     Serial.println(" ");
    //     // sqlite3_close(db1);
    //     // return;
    // }

    // const char *data = "INSERT INTO test1 VALUES (1, 'Hello, World from test1');";
    // rc = db_exec(db1, data);
    // rc = db_exec(db1, "INSERT INTO datalog VALUES (" +s_Date.c_str+ "," +s_Time.c_str+ "," +String(id)+ "," +String(value)+ ", 'superman');");
    // char buffer[100];
    // sprintf(buffer, "INSERT INTO datalog VALUES ('%s', '%s', %d, %d, '%s');", rtc->getDateString().c_str(),rtc->getTimeString().c_str(), id, value, s_Name.c_str());
    // Serial.println(buffer);

    s_Date = rtc->getDateString();
    s_Time = rtc->getTimeString();
    id = 0;
    value = random(0, 100);
    s_Name = "ID001";

    Serial.print("get: ");
    Serial.print(rtc->getDateString());
    Serial.print(" || ");
    Serial.println(s_Date);

    Serial.print("get: ");
    Serial.print(rtc->getTimeString());
    Serial.print(" || ");
    Serial.print(s_Time);
    Serial.println(" ");
    Serial.println(" ");

    char buffer[200];
    sprintf(buffer, "INSERT INTO dataman (date, time, id, value, IDname) VALUES ('%s', '%s', %d, %d, '%s');", rtc->getDateString().c_str(), rtc->getTimeString().c_str(), id, value, s_Name.c_str());
    rc = db_exec(db1, buffer);
    if (rc != SQLITE_OK)
    {
        Serial.print("INSER Failed...");
        Serial.println(" ");
        Serial.println(" ");
        // sqlite3_close(db1);
        // return;
    }
    else
    {
        Serial.println("INSER Done...");
    }

    sprintf(buffer, ""); // clear buffer
    sqlite3_close(db1);

    // char buffer[100];
    // sprintf(buffer, "INSERT INTO datalog (date, time, id, value, IDname) VALUES ('%s', '%s', %d, %d, '%s');", rtc->getDateString().c_str(), rtc->getTimeString().c_str(), id, value, s_Name.c_str());
    // // sprintf(buffer, "INSERT INTO datalog (date, time, id, value, IDname) VALUES ('%s', '%s', %d, %d, '%s');", rtc->getDateString().c_str(), rtc->getTimeString().c_str(), id, value, s_Name.c_str());
    // // Serial.println(buffer);
    // Serial.println("Create Buffer...");
    // Serial.println(" ");
    // Serial.println(" ");

    // if (openDb("/sdcard/ina219.db", &db1))
    // {
    //     Serial.println("Open: ");
    // }

    // rc = db_exec(db1, buffer);
    // if (rc != SQLITE_OK)
    // {
    //     Serial.print("Save Failed...");
    //     rc = db_exec(db1, buffer);
    // }
    // else
    // {
    //     Serial.print("Save OK...");
    //     sqlite3_close(db1);
    // }

    // rc = db_exec(db1, "Select * from datalog");
    // if (rc != SQLITE_OK)
    // {
    //     sqlite3_close(db1);
    // }

    Serial.println("Done process...");
}

uint32_t pevTime = 0;

void saveDB()
{
    int rc;
    char buffer[200];
    id++;
    value = random(0, 100);
    s_Name = "ID001";

    sprintf(buffer, "INSERT INTO dataman (date, time, id, value, IDname) VALUES ('%s', '%s', %d, %d, '%s');", rtc->getDateString().c_str(), rtc->getTimeString().c_str(), id, value, s_Name.c_str());
    rc = db_exec(db1, buffer);
    if (rc != SQLITE_OK)
    {
        Serial.print("INSER Failed...");
        Serial.println(" ");
        Serial.println(" ");
        rc = db_exec(db1, buffer);
    }
    else
    {
        Serial.print("INSER Done...");
    }

    sprintf(buffer, ""); // clear buffer
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