#include <Arduino.h>
#include <FreeRTOSConfig.h>
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

const char msg[] = "\nThis is example 2!";

static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

void startTask1(void *param)
{
  int len = strlen(msg);
  while (1)
  {
    for (int i = 0; i < len; i++)
      Serial.print(msg[i]);
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
};

void startTask2(void *param)
{
  Serial.println();
  while (1)
  {
    Serial.print("#");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
};

void setup()
{
  Serial.begin(115200);
  xTaskCreatePinnedToCore(startTask1, "Task1", 1024, NULL, 1, &task_1, app_cpu);
  xTaskCreatePinnedToCore(startTask2, "Task2", 1024, NULL, 2, &task_2, app_cpu);
}

void loop()
{
  for (size_t i = 0; i < 3; i++)
  {
    vTaskSuspend(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    vTaskResume(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
  if (task_1 != NULL)
  {
    vTaskDelete(task_1);
    task_1 = NULL;
  }
}