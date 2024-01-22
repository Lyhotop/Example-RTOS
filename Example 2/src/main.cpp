
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
    Serial.print("Led ON!\n");
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
  Serial.println("Multi-task LED Demo");
  Serial.println("Enter a number in milliseconds to change the LED delay.");
  xTaskCreatePinnedToCore(  
            toggleLED,     
            "Toggle LED",   
            1024,           
            NULL,           
            1,              
            NULL,           
            app_cpu);       

  xTaskCreatePinnedToCore(  
            readSerial,     
            "Read Serial",  
            1024,           
            NULL,           
            1,              
            NULL,           
            app_cpu);       
  vTaskDelete(NULL);
}

void loop() {

}

// #include <Arduino.h>
// #include <FreeRTOSConfig.h>
// #if CONFIG_FREERTOS_UNICORE
// static const BaseType_t app_cpu = 0;
// #else
// static const BaseType_t app_cpu = 1;
// #endif

// const char msg[] = "Oleg, you are stupid, boy!";

// static TaskHandle_t task_1 = NULL;
// static TaskHandle_t task_2 = NULL;

// static const int led_num = 1;
// static int led_delay = 500;

// void toggleLED(void *param){
//   digitalWrite(led_num, HIGH);
//   vTaskDelay(led_delay / portTICK_PERIOD_MS);
//   digitalWrite(led_num, LOW);
//   vTaskDelay(led_delay / portTICK_PERIOD_MS);
// }

// void 

// void startTask1(void *param){
//   int len = strlen(msg);
//   while(1){
//     for(int i = 0; i< len; i++)
//       Serial.print(msg[i]);
//     Serial.println();
//     vTaskDelay(1000 / portTICK_PERIOD_MS);
//   }
// };

// void startTask2(void *param){
//   Serial.println();
//   while(1){
//     Serial.print("*");
//     vTaskDelay(100 / portTICK_PERIOD_MS);
//   }
// };

// void setup() {
//   Serial.begin(115200);
//   xTaskCreatePinnedToCore(startTask1, "Task1", 1024, NULL, 1, &task_1, app_cpu);
//   xTaskCreatePinnedToCore(startTask2, "Task2", 1024, NULL, 2, &task_2, app_cpu);
// }

// void loop() {
//   for (size_t i = 0; i < 3; i++)
//   {
//     vTaskSuspend(task_2);
//     vTaskDelay(2000 / portTICK_PERIOD_MS);
//     vTaskResume(task_2);
//      vTaskDelay(2000 / portTICK_PERIOD_MS);
//   }
//   if(task_1 != NULL){
//     vTaskDelete(task_1);
//     task_1 = NULL;
//   }
// }

