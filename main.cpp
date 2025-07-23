#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "src/constants.h"
#include "src/app.h"
#include <string.h>

volatile int battery_level = 100;
volatile bool button_enabled = true; // Flag to enable/disable button

void button_callback(uint gpio, uint32_t events)
{
    if (gpio != HOME_BTN_PIN || !button_enabled)
        return;

    button_enabled = false;

    battery_level -= 10;
    if (battery_level < 0)
    {
        battery_level = 100;
    }

    add_alarm_in_ms(DEBOUNCE_TIME_MS, [](alarm_id_t, void *) -> int64_t
                    {
        button_enabled = true;
        return 0; }, nullptr, false);
}

int main()
{
    stdio_init_all();

    gpio_init(HOME_BTN_PIN);
    gpio_set_dir(HOME_BTN_PIN, GPIO_IN);
    gpio_pull_up(HOME_BTN_PIN);

    gpio_set_irq_enabled_with_callback(HOME_BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    App *app = new App();

    while (true)
    {
        tight_loop_contents();
    }
}
