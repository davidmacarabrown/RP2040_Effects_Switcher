/* C Includes */
#include <stdio.h>
#include <memory.h>

/* Pico SDK Includes */
#include "pico/multicore.h"
#include "pico/util/queue.h"

/* Project Includes */
#include "gpio_defs.h"
#include "core_1.h"
#include "MCP23017.H"
#include "HT16K33.h"
#include "WEH001602A.h"
#include "output_manager.h"
#include "input_manager.h"
#include "display_manager.h"
#include "debug.h"

queue_t* core1_tx_queue;
queue_t* core1_rx_queue;

MCP23017 *input_port;
MCP23017 *output_port;
MCP23017 *display_port;

DisplayManager* display_mgr;
OutputManager* output_mgr;
InputManager* input_mgr;

Core1 core_1;

using namespace WEH001602A;

void core_1_entry(void)
{

#ifdef DEBUG
    printf("<1> GPIO Init Done\n");
#endif

    gpio_put(PICO_LED, false);
    sleep_ms(1000);
    gpio_put(PICO_LED, true);

    input_port = new MCP23017(i2c1, 0x20);
    output_port = new MCP23017(i2c1, 0x21);
    display_port = new MCP23017(i2c1, 0x22);

    /* Display */
    std::shared_ptr<HT16K33> alphanumeric = std::make_shared<HT16K33>(i2c1, QUAD_ADDR);

    output_mgr = new OutputManager();
    output_mgr->initialise(output_port);

    output_mgr->write_relay_port_mask(PORT_B, 0xF0);

    input_mgr = new InputManager();
    input_mgr->initialise(input_port, core1_tx_queue);

    display_mgr = new DisplayManager();
    display_mgr->set_associations(alphanumeric);

    display_port->write_mask(PORT_B, 0xF0);

    sleep_ms(100000);
#if 0
    WEH001602A_INST oled(display_port, EIGHT, TWO, HEIGHT_8, ENGLISH_JAPANESE, false, INCREMENT, true, false);

    uint8_t test_char = 0b01000100;

    oled.set_ddram_address(0);
    oled.write_data(&test_char, 1);
#endif
    

    /* Ports */
    //TODO: make smart_ptr's

    //display_port = new MCP23017(i2c1, 0x22);
    

#ifdef DEBUG
    printf("<1> Port Config Done\n");
#endif

    /* Tx/Rx Queue */
    core1_rx_queue = (queue_t *)multicore_fifo_pop_blocking();
    printf("<1> RX Q: %d\n", (uint32_t)core1_rx_queue);
    core1_tx_queue = (queue_t *)multicore_fifo_pop_blocking();
    printf("<1> TX Q: %d\n", (uint32_t)core1_tx_queue);

    
    
#ifdef DEBUG
    printf("<1> Queue Pointers Assigned\n"); fflush(stdout);
#endif
    core_1.initialise(input_mgr, output_mgr, display_mgr);

    multicore_fifo_pop_blocking(); // synchronisation semaphore
    printf("Sema Taken\n"); fflush(stdout);
    
    while(1)
    {
        printf("<1> Running\n");
        core_1.read_queue();
        core_1.poll_input_ports();

        sleep_ms(3000);
    }
}

