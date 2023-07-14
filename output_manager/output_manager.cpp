#include "stdio.h"
#include "output_manager.h"
#include "pico/stdlib.h"
#include "gpio_defs.h"

void OutputManager::initialise(MCP23017 *pOutputPort)
{
    this->pOutputPort = pOutputPort;

    this->pOutputPort->set_port_mode(PORT_16_BIT);
    this->pOutputPort->set_ioconfig(0b00111000);
    this->pOutputPort->set_iodir_a(0x00);
    this->pOutputPort->set_iodir_b(0x00);
    this->pOutputPort->write_configuration();
}

void OutputManager::single_blink(PORT port, uint8_t pin)
{
    set_one(port, pin, LOW);
    set_one(port, pin, HIGH);
    sleep_ms(SLOW_BLINK_TIME);
    set_one(port, pin, LOW);
}

void OutputManager::rapid_blink(PORT port, uint8_t led)
{
    uint8_t counter;

    set_one(port, led, LOW);

    while(counter < BLINK_REPEATS)
    {
        set_one(port, led, HIGH);
        sleep_ms(RAPID_BLINK_TIME);
        set_one(port, led, LOW);
        sleep_ms(RAPID_BLINK_TIME);
        counter++;
    }
}

void OutputManager::reset(PORT port)
{
    pOutputPort->write_mask(port, 0x00);
}

void OutputManager::write_relay_port_mask(PORT port, uint8_t mask)
{
    printf("OutputManager::write_relay_port_mask()\n");

    gpio_put(MUTE_OPTORELAY_PIN, HIGH);
    pOutputPort->write_mask(PORT_A, mask);
    gpio_put(MUTE_OPTORELAY_PIN, LOW);

}

void OutputManager::set_one(PORT port, uint8_t pin, uint8_t state)
{
    pOutputPort->write_pin(port, pin, state);
}