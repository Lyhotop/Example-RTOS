#include <stdlib.h>
#include <Arduino.h>
#include <FreeRTOSConfig.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif
static const int led_pin = 1;
static const TickType_t dim_delay = 5000 / portTICK_PERIOD_MS;
static TimerHandle_t timer = NULL;

void autoDimmerCallback(TimerHandle_t xTimer)
{
  digitalWrite(led_pin, LOW);
}

void blinkLed(void *parameters)
{
  char c;
  while (1)
  {
    if (Serial.available() > 0)
    {
      c = Serial.read();
      Serial.print(c);
      digitalWrite(led_pin, HIGH);
      xTimerStart(timer, dim_delay);
    }
  }
}

void setup()
{
  pinMode(led_pin, OUTPUT);
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("Input:");
  timer = xTimerCreate("Timer", dim_delay, pdFALSE, (void *)0, autoDimmerCallback);
  xTaskCreatePinnedToCore(blinkLed, "Blink", 1024, NULL, 1, NULL, app_cpu);
  vTaskDelete(NULL);
}

void loop()
{
}