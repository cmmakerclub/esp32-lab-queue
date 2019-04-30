#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include <SD_MMC.h>
#include <FreeRTOS.h>
#include <Arduino.h>

#include <functional>
// typedef std::function<int (void*, int, char**, char**)> sqlite_cb_t;
typedef int(*sqlite_cb_t)(void*, int, char**, char**);

int rc;
uint32_t _executedTime = 0;
sqlite3 *db1;
static char *zErrMsg = 0;
const char *data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName) {
  int i;
  Serial.printf("%s: \r\n", (const char *)data);
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

int db_exec(sqlite3 *db, const char *sql, sqlite_cb_t cb = NULL)
{
  Serial.println(sql);
  long start = micros();
  int rc = SQLITE_ERROR;
  if (cb == NULL) {
    rc = sqlite3_exec(db, sql, callback, (void *)data, &zErrMsg);
  }
  else {
    rc = sqlite3_exec(db, sql, cb, (void *)data, &zErrMsg);
  }
  _executedTime = (micros() - start) / 1000;
  Serial.printf("Time taken: %lu ms\r\n", _executedTime);
  return rc;
}

static void heapGraph(void * parameter);

void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);
  // xTaskCreatePinnedToCore(clockSync, "clockSync", 2048, NULL, 4, NULL, 1); // RTC wants to run on core 1 or it fails
  // vTaskDelete(NULL);
  xTaskCreatePinnedToCore(heapGraph, "HeapGraph", 4096, NULL, 4, NULL, 0); /* last = Task Core */

  xTaskCreatePinnedToCore ([&](void * parameter) -> void {
    BaseType_t xStatus;
    // /* time to block the task until data is available */
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
    Serial.println("Task Recv is Running..");
    // Data data;
    while(1) {
     Serial.printf("[%lu] HELLO..\r\n", millis());
     vTaskDelay( 100 );
    }
  }, "receiveTask", 4096, NULL, 1, NULL, 1);

  SD_MMC.begin("/sdcard", true);
  sqlite3_initialize();

  if (openDb("/sdcard/ina219.db", &db1) == SQLITE_OK) {
    rc = db_exec(db1, "CREATE TABLE IF NOT EXISTS datalog (id INTEGER PRIMARY KEY AUTOINCREMENT, date TEXT, time TEXT, heap INTEGER, IDname content, ms INTEGER);");
    if (rc != SQLITE_OK)  {
      sqlite3_close(db1);
      return;
    }
  }
}

uint32_t currentRowId = 0;
int xcallback(void *data, int argc, char **argv, char **azColName) {
  currentRowId = strtoul(argv[0] ? argv[0] : "0", NULL, 10);
  Serial.printf("currentRowId = %lu\r\n", currentRowId);
  return 0;
}

void loop() {
  taskYIELD();
}


static char buffer[100];
static void heapGraph(void * parameter) {
  while (1) {
    sprintf(buffer, "INSERT INTO datalog(time, ms, heap) VALUES(%lu, %lu, %lu);", _executedTime, millis(), ESP.getHeapSize());
    if (db_exec(db1, buffer) == SQLITE_OK) {
      Serial.println("INSERT OK.");
    };

    sprintf(buffer, "SELECT id,heap,ms FROM datalog ORDER BY id DESC LIMIT 1;");
    if (db_exec(db1, buffer, xcallback) == SQLITE_OK) {
      Serial.println("QUERY OK.");
    }

    sprintf(buffer, "DELETE FROM datalog WHERE id = %lu;", currentRowId);
    if (db_exec(db1, buffer) == SQLITE_OK) {
      Serial.println("DELETE OK.");
    }

    vTaskDelay(500);
  }
}
