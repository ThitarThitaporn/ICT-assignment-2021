#include <Arduino_FreeRTOS.h>
#include "queue.h"

#define SW1 10
#define SW2 11
#define SW3 12

#define RED 2
#define YELLOW 3
#define GREEN 4

const uint16_t *RedLed= (uint16_t *) RED;
const uint16_t *YellowLed= (uint16_t *) YELLOW;
const uint16_t *GreenLed= (uint16_t *) GREEN;

QueueHandle_t = ledQueue;


void setup()
{
  Serial.begin(9600);
  ledQueue =  xQueueCreate(5,sizeof(int32_t));
  
  
  xTaskCreate(ReceiverTask,"Receiver RedLed", 64, (void*)RedLed, 1, NULL);
  xTaskCreate(vSenderTask,"Sender Task",100, NULL, 1, NULL);
}

void vSenderTask(void *pvParameters)
{
  BaseType_tqStatus;
  int 32_t   valueToSend= 0;
  pinMode(SW  1, INPUT);  
  while(1)
  {
    valueToSend= digitalRead(SW1);
    qStatus= xQueueSend(ledQueue,&valueToSend,0); 
    vTaskDelay(10);
  }
}

void vReceiverTask(void *pvParameters)
{
  int 32_t   valueReceived;
  BaseType_tqStatus;
  const TickType_txTicksToWait= pdMS_TO_TICKS(100);
  pinMode(RED, OUTPUT);  
  while(1)
  {
    xQueueReceive(ledQueue,&valueReceived,xTicksToWait);
    Serial.print("Received value  : ");
    Serial.println(valueReceived);
    digitalWrite(RED, valueReceived);
    vTaskDelay(1);
  }
}

void loop()
{
}
