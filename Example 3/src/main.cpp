#include <stdlib.h>
#include <Arduino.h>
#include <FreeRTOSConfig.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static const uint8_t len = 255;
static char *msg_ptr = NULL;
static volatile uint8_t flag = 0;

void readTextFromSerial(void *parameters) {
  char c;
  char buffer[len];
  uint8_t idx = 0;
  memset(buffer, 0, len);
  while (1) {
    if (Serial.available() > 0) {
      c = Serial.read();
      if (idx < len - 1) {
        buffer[idx] = c;
        idx++;
      }
      if (c == '\n') {
        buffer[idx - 1] = '\0';
        if (flag == 0) {
          msg_ptr = (char *)pvPortMalloc(idx * sizeof(char));
          configASSERT(msg_ptr);
          memcpy(msg_ptr, buffer, idx);
          flag = 1;
        }
        memset(buffer, 0, len);
        idx = 0;
      }
    }
  }
}

void writeTextInSerial(void *parameters) {
  while (1) {
    if (flag == 1) {
      Serial.println(msg_ptr);
      Serial.println();
      vPortFree(msg_ptr);
      msg_ptr = NULL;
      flag = 0;
    }
  }
}

void setup() {
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("Enter a new string!");
  xTaskCreatePinnedToCore(readTextFromSerial, "Read Serial", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(writeTextInSerial, "Print Message", 1024, NULL, 1, NULL, app_cpu);
  vTaskDelete(NULL);
}

void loop() {

}