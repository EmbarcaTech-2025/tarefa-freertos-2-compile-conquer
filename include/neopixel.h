#pragma once

#define LED_COUNT 25
#define LED_PIN 7

typedef struct {
    uint8_t G, R, B;
} np_led_color_t;

void np_init(uint pin);
void np_set_led(const uint index, const uint8_t r, const uint8_t g, const uint8_t b);
void np_clear(void);
void np_write(void);