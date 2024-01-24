#include <stdlib.h>
#include <Arduino.h>
#include <FreeRTOSConfig.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

const uint8_t led = 1;
static int delayLed = 0;
SemaphoreHandle_t xMutex;

void task1(void *parameter)
{
  while (1)
  {
    if (xSemaphoreTake(xMutex, portMAX_DELAY))
    {
      Serial.println("Task1: Acquired mutex!");
      while (delayLed != 10)
      {
        digitalWrite(led, HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        Serial.print("Led works! ");
        Serial.println(delayLed);
        digitalWrite(led, LOW);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        delayLed++;
      }
      xSemaphoreGive(xMutex);
      Serial.println("Task1: Released mutex!");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void task2(void *parameter)
{
  while (1)
  {
    if (xSemaphoreTake(xMutex, portMAX_DELAY))
    {
      Serial.println("Task2: Acquired mutex!");
      while (delayLed != 0)
      {
        digitalWrite(led, HIGH);
        vTaskDelay(25 / portTICK_PERIOD_MS);
        Serial.print("Led works! ");
        Serial.println(delayLed);
        digitalWrite(led, LOW);
        vTaskDelay(25 / portTICK_PERIOD_MS);
        delayLed--;
      }
      xSemaphoreGive(xMutex);
      Serial.println("Task2: Released mutex!");
    }
    vTaskDelay(700 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  xMutex = xSemaphoreCreateMutex();
  if (xMutex != NULL)
  {
    xTaskCreate(task1, "Task1", 1000, NULL, 1, NULL);
    xTaskCreate(task2, "Task2", 1000, NULL, 1, NULL);
    // vTaskStartScheduler();
  }
  else
  {
    Serial.println("Mutex creation failed!");
  }
}

void loop()
{
}