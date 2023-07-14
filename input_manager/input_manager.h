#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <iostream>
#include "gpio_defs.h"
#include "MCP23017.H"
#include "pico/util/queue.h"

class InputManager
{
    private:
        MCP23017 *input_port;
        bool input_latch = false;
        PORT port_status;
        uint8_t port_value;
        
    public:
        void initialise(MCP23017 *input_port, queue_t* tx_queue);
        PORT poll_input_ports(void);
        uint8_t read_input_port(PORT port);
        PORT get_port_status(void);
        uint8_t get_active_port_value(void);
        void clear_port_data(void);
        bool get_latch(void);
};

#endif