#include <Arduino.h>


struct AMessage
{
    char ucMessageID;
    char ucData[ 20 ];
};

void vATask( void *pvParameters )
{
    QueueHandle_t xQueue1, xQueue2;

    /* Create a queue capable of containing 10 unsigned long values. */
    xQueue1 = xQueueCreate( 10, sizeof( unsigned long ) );

    if( xQueue1 == NULL )
    {
        /* Queue was not created and must not be used. */
    }

    /* Create a queue capable of containing 10 pointers to AMessage
    structures.  These are to be queued by pointers as they are
    relatively large structures. */
    xQueue2 = xQueueCreate( 10, sizeof( struct AMessage * ) );

    if( xQueue2 == NULL )
    {
        /* Queue was not created and must not be used. */
    }

    /* ... Rest of task code. */
 }

QueueHandle_t queue;

void setup() {

  Serial.begin(115200);

  queue = xQueueCreate( 10, sizeof( int ) );

  if(queue == NULL){
    Serial.println("Error creating the queue");
  }

}

void loop() {

  if(queue == NULL)return;

  for(int i = 0; i<10; i++){
    xQueueSend(queue, &i, portMAX_DELAY);
  }

  int element;

  for(int i = 0; i<10; i++){
    xQueueReceive(queue, &element, portMAX_DELAY);
    Serial.print(element);
    Serial.print("|");
  }

  Serial.println();
  delay(1000);
}
