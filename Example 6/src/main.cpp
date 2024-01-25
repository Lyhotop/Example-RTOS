#include <stdlib.h>
#include <Arduino.h>
#include <FreeRTOSConfig.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

enum
{
  BUF_SIZE = 5
};
static const int num_prod_tasks = 5;
static const int num_cons_tasks = 2;
static const int num_writes = 3;

static int buf[BUF_SIZE];
static int head = 0;
static int tail = 0;
static SemaphoreHandle_t bin_sem;
static SemaphoreHandle_t mutex;
static SemaphoreHandle_t empty_sem;
static SemaphoreHandle_t filled_sem;

void producer(void *parameters)
{
  int num = *(int *)parameters;
  xSemaphoreGive(bin_sem);
  for (int i = 0; i < num_writes; i++)
  {
    xSemaphoreTake(empty_sem, portMAX_DELAY);
    xSemaphoreTake(mutex, portMAX_DELAY);
    buf[head] = num;
    head = (head + 1) % BUF_SIZE;
    xSemaphoreGive(mutex);
    xSemaphoreGive(filled_sem);
  }
  vTaskDelete(NULL);
}

void consumer(void *parameters)
{
  int val;
  while (1)
  {
    xSemaphoreTake(filled_sem, portMAX_DELAY);
    xSemaphoreTake(mutex, portMAX_DELAY);
    val = buf[tail];
    tail = (tail + 1) % BUF_SIZE;
    Serial.println(val);
    xSemaphoreGive(mutex);
    xSemaphoreGive(empty_sem);
  }
}

void setup()
{
  char task_name[12];
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Semaphore Alternate Solution---");
  bin_sem = xSemaphoreCreateBinary();
  mutex = xSemaphoreCreateMutex();
  empty_sem = xSemaphoreCreateCounting(BUF_SIZE, BUF_SIZE);
  filled_sem = xSemaphoreCreateCounting(BUF_SIZE, 0);
  for (int i = 0; i < num_prod_tasks; i++)
  {
    sprintf(task_name, "Producer %i", i);
    xTaskCreatePinnedToCore(producer, task_name, 1024, (void *)&i, 1, NULL, app_cpu);
    xSemaphoreTake(bin_sem, portMAX_DELAY);
  }
  for (int i = 0; i < num_cons_tasks; i++)
  {
    sprintf(task_name, "Consumer %i", i);
    xTaskCreatePinnedToCore(consumer, task_name, 1024, NULL, 1, NULL, app_cpu);
  }
  xSemaphoreTake(mutex, portMAX_DELAY);
  Serial.println("All tasks created!");
  xSemaphoreGive(mutex);
}

void loop()
{
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}