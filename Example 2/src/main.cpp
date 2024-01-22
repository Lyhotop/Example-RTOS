#include <stdlib.h>
#include <Arduino.h>
#include <FreeRTOSConfig.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static const uint8_t len = 20;
static const int led_pin = 1;
static int led_delay = 500;  

void toggleLED(void *parameter) {
  while (1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(led_delay / portTICK_PERIOD_MS);
    Serial.print("Led works!\n");
    digitalWrite(led_pin, LOW);
    vTaskDelay(led_delay / portTICK_PERIOD_MS);
  }
}

void readSerial(void *parameters) {
  char c;
  char buf[len];
  uint8_t idx = 0;
  memset(buf, 0, len);
  while (1) {
    if (Serial.available() > 0) {
      c = Serial.read();
      if (c == '\n') {
        led_delay = atoi(buf);
        Serial.print("Updated LED delay to: ");
        Serial.println(led_delay);
        memset(buf, 0, len);
        idx = 0;
      } 
      else {
        if (idx < len - 1) {
          buf[idx] = c;
          idx++;
        }
      }
    }
  }
}

void setup() {
  pinMode(led_pin, OUTPUT);
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("Enter a number in milliseconds to change the LED delay.");
  xTaskCreatePinnedToCore(toggleLED, "Toggle LED", 1024, NULL, 1, NULL, app_cpu);       
  xTaskCreatePinnedToCore(  readSerial, "Read Serial", 1024, NULL, 1, NULL, app_cpu);       
  vTaskDelete(NULL);
}

void loop() {

}