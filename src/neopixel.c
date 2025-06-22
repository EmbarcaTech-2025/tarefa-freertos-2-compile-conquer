#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "../include/neopixel.h"
#include "ws2818b.pio.h"

float global_brightness = 0.25;

np_led_color_t led_strip[LED_COUNT];
PIO np_pio;
uint sm;

void np_init(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;

    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
      np_pio = pio1;
      sm = pio_claim_unused_sm(np_pio, true);
    }
  
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
  
    for (uint i = 0; i < LED_COUNT; ++i) {
      led_strip[i].R = 0;
      led_strip[i].G = 0;
      led_strip[i].B = 0;
    }
    np_write();
}

void np_write() {
    for (uint i = 0; i < LED_COUNT; ++i) {
      pio_sm_put_blocking(np_pio, sm, led_strip[i].G);
      pio_sm_put_blocking(np_pio, sm, led_strip[i].R);
      pio_sm_put_blocking(np_pio, sm, led_strip[i].B);
    }
}

void np_set_led(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    if (index >= LED_COUNT) return;

    led_strip[index].R = r;
    led_strip[index].G = g;
    led_strip[index].B = b;
}

void np_clear() {
    for (uint i = 0; i < LED_COUNT; ++i)
      np_set_led(i, 0, 0, 0);
}


