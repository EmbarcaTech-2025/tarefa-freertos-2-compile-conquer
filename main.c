#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "neopixel.h"
#include "neopixel.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include <stdlib.h>
#include "semphr.h"

#define MIC_PIN 28
#define ADC_CHANEL (MIC_PIN - 26)

QueueHandle_t raw_audio_queue;
QueueHandle_t processed_audio_queue;
SemaphoreHandle_t led_update_semaphore;

void audio_capture_task(void *pvParameters)
{
  TickType_t last_wake_time = xTaskGetTickCount();
  const TickType_t freq = pdMS_TO_TICKS(1);
  ;

  while (true)
  {
    uint mic_value = adc_read();
    uint deviation = mic_value - 2048;
    uint amplitude = abs(deviation);

    xQueueSend(raw_audio_queue, &amplitude, 0);

    vTaskDelayUntil(&last_wake_time, freq);
  }
}

void audio_processing_task(void *pvParameters)
{
  uint raw_amplitude;

  while (true)
  {
    if (xQueueReceive(raw_audio_queue, &raw_amplitude, pdMS_TO_TICKS(2)))
    {
      uint processed_amplitude = raw_amplitude * 4;
      xQueueSend(processed_audio_queue, &processed_amplitude, 0);
      printf("Processing : %d -> %d\n", raw_amplitude, processed_amplitude);
      xSemaphoreGive(led_update_semaphore);
    }
  }
}

void led_animation_task(void *pvParameters)
{
  np_init(LED_PIN);
  uint amplitude = 0;

  while (true)
  {
    if (xSemaphoreTake(led_update_semaphore, pdMS_TO_TICKS(3)))
    {
      if (xQueueReceive(processed_audio_queue, &amplitude, pdMS_TO_TICKS(1)))
      {
        uint active_rows = (amplitude * 5) / 2048;
        if (active_rows > 5)
          active_rows = 5;
        np_clear();

        for (uint row = 0; row < active_rows; row++)
        {
          for (uint col = 0; col < 5; col++)
          {
            uint led_index = row * 5 + col;

            switch (row)
            {
            case 0:
              np_set_led(led_index, 0, 255, 0);
              break;
            case 1:
              np_set_led(led_index, 255, 255, 0);
              break;
            case 2:
              np_set_led(led_index, 255, 165, 0);
              break;
            case 3:
              np_set_led(led_index, 255, 0, 0);
              break;
            case 4:
              np_set_led(led_index, 255, 0, 0);
              break;
            }
          }
        }
        np_write();
      }
    }
    else
    {
      np_clear();
      np_write();
    }
  }
}

int main()
{
  stdio_init_all();

  adc_init();
  adc_gpio_init(MIC_PIN);
  adc_select_input(ADC_CHANEL);

  raw_audio_queue = xQueueCreate(6, sizeof(uint));
  processed_audio_queue = xQueueCreate(8, sizeof(uint));

  led_update_semaphore = xSemaphoreCreateBinary();

  xTaskCreate(audio_capture_task, "audio_cap_task", 256, NULL, 3, NULL);
  xTaskCreate(audio_processing_task, "audio_proc_task", 256, NULL, 2, NULL);
  xTaskCreate(led_animation_task, "led_task", 256, NULL, 1, NULL);

  vTaskStartScheduler();

  while (1)
  {
  };
}