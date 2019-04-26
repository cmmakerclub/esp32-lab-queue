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
    Serial.begin(9600);
    char *zErrMsg = 0;

    rtc = new CMMC_RTC();
    rtc->setup();

    SPI.begin();
    SD_MMC.begin("/sdcard", true);

    sqlite3_initialize();

    deleteFile(SD_MMC, "/ina219.db");

    // Open database 1
    if (openDb("/sdcard/ina219.db", &db1))
        return;

    rc = db_exec(db1, "CREATE TABLE datalog (date TEXT, time TEXT, id INTEGER, value INTEGER, IDname content);");
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db1);
        return;
    }

    // const char *data = "INSERT INTO test1 VALUES (1, 'Hello, World from test1');";
    // rc = db_exec(db1, data);
    // rc = db_exec(db1, "INSERT INTO datalog VALUES (" +s_Date.c_str+ "," +s_Time.c_str+ "," +String(id)+ "," +String(value)+ ", 'superman');");
    // rc = db_exec(db1, "INSERT INTO datalog (date, time, id, value, IDname) VALUES ('25/04/2019', 20.14, 1, 50, 'ID001');");
    // if (rc != SQLITE_OK)
    // {
    //     sqlite3_close(db1);
    //     return;
    // }

    saveDB();

    sqlite3_close(db1);
}

void saveDB()
{
    Serial.print("save db....");
    // Open database 1
    if (openDb("/sdcard/ina219.db", &db1))
        return;

    s_Date = "25/04/2019";
    s_Time = "20.14";
    id++;
    value = random(0, 100);
    s_Name = "ID001";

    char buffer[100];
    sprintf(buffer, "INSERT INTO datalog VALUES (%s, %s, %d, %d, %s);", s_Date.c_str(), s_Time.c_str(), id, value, s_Name.c_str());
    Serial.println(buffer);

    rc = db_exec(db1, buffer);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db1);
        return;
    }

    Serial.println("done");
    sqlite3_close(db1);
}

uint32_t pevTime = 0;

void loop()
{
    rtc->loop();
    uint32_t curTime = millis();
    if (curTime - pevTime >= 1000)
    {
        pevTime = curTime;
        Serial.println("save db.");
        Serial.print("get dateTime: ");
        Serial.println(rtc->getDateTimeString());
        Serial.print("get gettDatestamp: ");
        Serial.println(rtc->getDateString());
        Serial.print("get gettTimestamp: ");
        Serial.println(rtc->getTimeString());
        // saveDB();
    }
}