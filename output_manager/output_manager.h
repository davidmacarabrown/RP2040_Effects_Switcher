#ifndef OUTPUT_MANAGER_H
#define OUTPUT_MANAGER_H

#include "gpio_defs.h"
#include "pico/stdlib.h"
#include "state_manager.h"
#include "MCP23017.H"

class OutputManager
{
    private:
        MCP23017* pOutputPort;

        uint8_t port_state[2];

    public:
        void initialise(MCP23017 *pOutputPort);
        void single_blink(PORT port, uint8_t led);
        void rapid_blink(PORT port, uint8_t led);
        void reset(PORT port);
        void write_relay_port_mask(PORT port, uint8_t mask);

        void set_one(PORT port, uint8_t pin, uint8_t state);
};
#endif