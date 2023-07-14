#ifndef C0RE_0_HANDLER_H
#define C0RE_0_HANDLER_H

/* C/C++ Includes */


/* Pico SDK Incldes */
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "hardware/timer.h"

/* Project Includes */
#include "gpio_defs.h"

#include "CAT24C32.h"
#include "HT16K33.h"

#include "output_manager.h"
#include "state_manager.h"
#include "display_manager.h"
#include "storage_manager.h"

class Core0
{
    private:
        bool running = false;
        
        /* TX/RX Queue */
        queue_t* itsTXQueue;
        queue_t* itsRXQueue;
        QUEUE_MESSAGE q_buffer; // provide simple data persistence between decode stages

        /* Parts */
        StateManager   itsStateManager;
        StorageManager itsStorageManager;

        void write_queue_item(void);
        void decode_port_input(void);
        void port_a_command(void);
        void port_b_command(void);
        void port_a_discrete(void);
        void mode_command_handler(void);
        void write_command_handler(void);
        void write_port_output_command(uint8_t command, uint8_t subcommand, PORT port, uint8_t data);
        void read_queue(void);

        void create_display_update(void);

    public:
        Core0(i2c_inst_t* i2CInstance, const uint8_t i2cAddress);                            
        void startup_routine(void);
        void run(void);
};

#endif
