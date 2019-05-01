#include <Arduino.h>
#include "FreeRTOS.h"

struct sensor
{
  int deviceId;
  int measurementType;
  float value;
};

void taskOne(void *parameter)
{
  for (int i = 1; i < 10; i++)
  {
    char msg[20];
    sprintf(msg, "Hello from task %d \n", i);
    Serial.print(msg);
    delay(1000);
  }
  Serial.println("Ending task 1");
  vTaskDelete(NULL);
}

QueueHandle_t queue;    //  reference a FreeRTOS queue.

void setup()
{
  Serial.begin(115200);

  //      xQueueCreate(maximum number of elements, the size (in bytes) of each element)
  queue = xQueueCreate(5, sizeof(struct sensor)); // created queue 

  if (queue == NULL)
  {
    Serial.println("Error creating the queue");
  }

  xTaskCreate(taskOne,   /* Task function. */
              "TaskOne", /* String with name of task. */
              10000,     /* Stack size in bytes. */
              NULL,      /* Parameter passed as input of the task */
              1,         /* Priority of the task. */
              NULL);     /* Task handle. */
}

void loop()
{

  if (queue == NULL)
  {
    Serial.println("Queue was not created");
    return;
  }

  for (int i = 0; i < 5; i++)
  {

    struct sensor mySensor;
    mySensor.deviceId = i;
    mySensor.measurementType = 1;
    mySensor.value = random(0, 100);

    xQueueSend(queue, &mySensor, portMAX_DELAY);
    //  if( xQueueSendToBack(queue, &mySensor, 10) != pdPASS ) {
    //    Serial.printf("id=%d is failed.\r\n", i);
    //  }
    //  else {
    //    Serial.printf("queue id=%d is sent.\r\n", i);
    //  }
  }

  struct sensor element;
  for(int i = 0; i<5; i++){
    xQueueReceive(queue, &element, portMAX_DELAY);
    Serial.print("Device ID: ");
    Serial.println(element.deviceId);
  
    Serial.print("Measurement type: ");
    Serial.println(element.measurementType);
  
    Serial.print("Value: ");
    Serial.println(element.value);
  
    Serial.println("----------------");
  }

  Serial.println();
  delay(4000);
}
