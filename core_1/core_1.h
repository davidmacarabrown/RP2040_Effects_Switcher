#ifndef CORE_1_H
#define CORE_1_H

#include "output_manager.h"
#include "input_manager.h"
#include "display_manager.h"

class Core1
{
    private:
        queue_t* itsRXQueue;
        queue_t* itsTXQueue;
        QUEUE_MESSAGE q_buffer;

        DisplayManager *display_mgr;
        OutputManager *output_mgr;
        InputManager *input_mgr;

    public:
        void initialise(InputManager *input_mgr, OutputManager *output_mgr, DisplayManager *display_mgr);
        void run(void);

        void read_queue(void);
        void write_queue(PORT port, uint8_t value);

        void decode_port_output(void);
        void poll_input_ports(void);

        int8_t decode_alphanumeric_command(void);
        int8_t alphanumeric_write(void);
        
        InputManager* get_input_mgr(void);
};

#endif