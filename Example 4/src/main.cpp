#include <stdlib.h>
#include <Arduino.h>
#include <FreeRTOSConfig.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const uint8_t len_que = 5;
static QueueHandle_t msg_que;
static QueueHandle_t delay_que;
const uint8_t led = 1;
const char command[] = "delay";

typedef struct Message
{
  char body[20];
  int count;
} Message;

void blinkLed(void *param)
{
  Message msg;
  int delay = 50;
  uint8_t counter = 0;
  while (1)
  {
    if (xQueueReceive(delay_que, (void *)&delay, 1) == pdTRUE)
    {
      strcpy(msg.body, "Message received ");
      msg.count = 1;
      xQueueSend(msg_que, (void *)&msg, 10);
    }
    digitalWrite(led, HIGH);
    vTaskDelay(delay / portTICK_PERIOD_MS);
    Serial.println("Led works!\n");
    digitalWrite(led, LOW);
    vTaskDelay(delay / portTICK_PERIOD_MS);
    counter++;
    if (counter >= 100)
    {
      strcpy(msg.body, "Message received: ");
      msg.count = counter;
      xQueueSend(msg_que, (void *)&msg, 10);
      counter = 0;
    }
  }
}

void readTextFromSerial(void *parameters)
{
  uint8_t len = 255;
  char c, buffer[len];
  uint8_t command_len = strlen(command);
  uint8_t idx = 0;
  int delay = 0;
  Message msg;
  memset(buffer, 0, len);
  while (1)
  {
    if (xQueueReceive(msg_que, (void *)&msg, 1) == pdTRUE)
    {
      Serial.print(msg.body);
      Serial.println(msg.count);
    }
    if (Serial.available() > 0)
    {
      c = Serial.read();
      if (idx < len - 1)
      {
        buffer[idx] = c;
        idx++;
      }
      if (c == '\n' || c == '\r')
      {
        if (memcmp(buffer, command, command_len) == 0)
        {
          char *point = buffer + command_len;
          delay = atoi(point);
          delay = abs(delay);
          if (xQueueSend(delay_que, (void *)&delay, 10) != pdTRUE)
          {
            Serial.println("Erorr!");
          }
        }
        else
        {
          Serial.println(buffer);
        }
        memset(buffer, 0, len);
        idx = 0;
      }
    }
  }
}

void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Enter the command 'delay xxx'.");
  Serial.println("LED blink delay time in milliseconds.");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  delay_que = xQueueCreate(len_que, sizeof(int));
  msg_que = xQueueCreate(len_que, sizeof(Message));
  xTaskCreatePinnedToCore(blinkLed, "Blink", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(readTextFromSerial, "Read Text", 1024, NULL, 1, NULL, app_cpu);
  vTaskDelete(NULL);
}

void loop()
{
}