#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "src/constants.h"
#include "src/app.h"
#include "src/ui/ui.h"
#include <string.h>
#include <map>

std::map<uint, volatile bool> buttons = {
    {LEFT_BTN_PIN, true},
    {HOME_BTN_PIN, true},
    {RIGHT_BTN_PIN, true}};

App *app = nullptr;

void button_callback(uint gpio, uint32_t events)
{
    if (!buttons[gpio] || !app)
        return;

    app->onButtonPress(gpio);

    buttons[gpio] = false;
    uint *gpio_ptr = new uint(gpio);

    add_alarm_in_ms(DEBOUNCE_TIME_MS, [](alarm_id_t, void *user_data) -> int64_t
                    {
                        uint gpio = *reinterpret_cast<uint *>(user_data);
                        buttons[gpio] = true;
                        delete reinterpret_cast<uint *>(user_data);
                        return 0; }, gpio_ptr, false);
}

int main()
{
    stdio_init_all();

    app = new App();

    const uint buttonPins[] = {LEFT_BTN_PIN, HOME_BTN_PIN, RIGHT_BTN_PIN};

    for (uint8_t pin : buttonPins)
    {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
        gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    }

    while (true)
    {
        tight_loop_contents();
    }
}
