#include "pico/stdlib.h"

void blink_led(int pinNum);

int main()
{
    while (true)
    {
        blink_led(15);
    }
}

void blink_led(int pinNum)
{
    const uint BLINK_DELAY_MS = 10000;
    gpio_init(pinNum);

    gpio_set_dir(pinNum, GPIO_OUT);

    gpio_put(pinNum, true);
    sleep_ms(BLINK_DELAY_MS);
    
    gpio_put(pinNum, false);
    sleep_ms(BLINK_DELAY_MS);
}