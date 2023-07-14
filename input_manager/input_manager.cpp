#include "input_manager.h"

void InputManager::initialise(MCP23017 *input_port, queue_t* tx_queue)
{
    this->input_port = input_port;
    this->input_port->set_port_mode(PORT_16_BIT);
    this->input_port->set_ioconfig(0b00101000);

    this->input_port->set_iodir_a(0xFF);
    this->input_port->set_iodir_b(0xFF);
    this->input_port->set_gppu_a (0xFF);
    this->input_port->set_gppu_b (0xFF);
    this->input_port->set_ipol_a (0xFF);
    this->input_port->set_ipol_b (0xFF);
    this->input_port->set_defval_a(0x00);
    this->input_port->set_defval_b(0x00);
    this->input_port->set_intcon_a(0x00);
    this->input_port->set_intcon_b(0x00);
    this->input_port->set_gpint_a(0xFF);
    this->input_port->set_gpint_b(0xFF);
    this->input_port->write_configuration();
}

PORT InputManager::poll_input_ports(void)
{
    if(input_latch == false)
    {
        if(read_input_port(PORT_A))
        {
            input_latch = true;
            port_status = PORT_A;
        }
        else if (read_input_port(PORT_B))
        {
            input_latch = true;
            port_status = PORT_B;
        }
        else
        {
            port_status = PORT_NULL;
        }
        return port_status;
    }
    else //if input has been latched
    {
        if( (read_input_port(PORT_A) == 0) && (read_input_port(PORT_B)) == 0) // if all buttons are released reset the latch
        {
            input_latch = false;
        }
        return PORT_NULL;
    }
    
}

uint8_t InputManager::read_input_port(PORT port)
{
#ifdef DEBUG
uint8_t val = input_port->read_input_mask(port);
    printf("<Core 1> Polling Port: %d Val: %d\n", port, val);
    return val;
#else
    return input_port->read_input_mask(port);
#endif
}

bool InputManager::get_latch(void)
{
    return this->input_latch;
}

uint8_t InputManager::get_active_port_value(void)
{
    return this->port_value;
}

PORT InputManager::get_port_status(void)
{
    return this->port_status;
}