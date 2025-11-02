#include "pico/stdlib.h"

void blink_led(int pinNum);

int main()
{
    const int pinNum = 22;

    gpio_init(pinNum);
    gpio_set_dir(pinNum, GPIO_OUT);

    while (true)
    {
        blink_led(pinNum);
    }
}

void blink_led(int pinNum)
{
    const uint BLINK_DELAY_MS = 1000;

    gpio_put(pinNum, true);
    sleep_ms(BLINK_DELAY_MS);
    
    gpio_put(pinNum, false);
    sleep_ms(BLINK_DELAY_MS);
}