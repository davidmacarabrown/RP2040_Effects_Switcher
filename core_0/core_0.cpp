/* C/C++ Includes */
#include <array>
#include <stdio.h>
#include <string.h>
#include <memory>

/* Pico SDK Incldes */

/* Project Includes */
#include "core_0.h"

Core0::Core0(i2c_inst_t* i2cInstance, const uint8_t i2cAddress) : itsStorageManager(i2cInstance, i2cAddress)
{

    queue_init_with_spinlock(itsTXQueue, sizeof(QUEUE_MESSAGE), QUEUE_LENGTH, 1);
    queue_init_with_spinlock(itsRXQueue, sizeof(QUEUE_MESSAGE), QUEUE_LENGTH, 2);

    multicore_fifo_push_blocking((uint32_t)itsTXQueue);
    multicore_fifo_push_blocking((uint32_t)itsRXQueue);
}

void Core0::startup_routine(void)
{
    printf("<Core 0> startup_routine\n");
    uint8_t result;
    MODE mode;
    
    result = itsStorageManager.validate_eeprom();

#ifdef DEBUG
    printf("Eprom Validated - Result: %d\n", result);
#endif

    if(result == 0)
    {
        //itsStateManager.load_memory_store();
        itsStorageManager.read_system_data(itsStateManager.get_system_data());
        
        
#ifdef DEBUG
        printf("Memory store loaded\n");
#endif
        sleep_ms(10);
        mode = itsStateManager.get_system_data().current_mode;
    
#ifdef DEBUG
        printf("Read Mode: %d\n", mode);
#endif
        switch(mode)
        {
            case MODE::MANUAL:
                //pOutputManager->reset();
                printf("Hello Mate\n");
                break;
            case MODE::PROGRAM:
                itsStateManager.load_output_state();
                printf("Hello Mate 2\n");
                break;
            default:
                //no-op
                printf("Hello Mate 3\n"); 
                break;
        }
        //pOutputManager->update();
        //itsDisplayManager->update();
        fflush(stdout);
    }
    else
    {
        //TODO: implement with LCD as well
        //itsDisplayManager->write_string((std::string*)&MSG_ROM_ERROR);
    }
}

void Core0::run(void)
{
    while(running)
    {
        read_queue();
    }
}

void Core0::read_queue(void)
{
    if(queue_try_peek(itsRXQueue, NULL))
    {
        queue_remove_blocking(itsRXQueue, &q_buffer);

        switch(q_buffer.command)
        {
            case COMMAND::PORT_INPUT:
                decode_port_input();
                break;
        }
    }
}

void Core0::decode_port_input(void)
{
    switch(q_buffer.subcommand)
    {
        case PORT::PORT_A:
            switch(q_buffer.data[0])
            {
                case PORT_A_INPUT_VALUE::SW_1:
                case PORT_A_INPUT_VALUE::SW_2:
                case PORT_A_INPUT_VALUE::SW_3:
                case PORT_A_INPUT_VALUE::SW_4:
                case PORT_A_INPUT_VALUE::SW_5:
                    port_a_discrete();
                    printf("port_a_discrete: %d\n", q_buffer.data[0]);
                    break;

                //case PORT_A_INPUT_VALUE::FSW_A
                //    break;

                //case PORT_A_INPUT_VALUE::FSW_B
                //    break;

                case PORT_A_INPUT_VALUE::PATCH_UP:
                    /* do something manspider! */
                    itsStateManager.get_system_data().increment_bank();
                    itsStateManager.get_system_data().active_patch = 5; //TODO: this is a placeholder for now
                    //itsDisplayManager->update();
                    break;

                case PORT_A_INPUT_VALUE::PATCH_DOWN:
                    /* Execute order 66! */
                    itsStateManager.get_system_data().decrement_bank();
                    itsStateManager.get_system_data().active_patch = 5; //TODO: this is a placeholder for now
                    //itsDisplayManager->update();
                    break; /* it will be done my lord */
                default:
                    break;
            }
            
        case PORT::PORT_B:
            switch(q_buffer.data[0])
            {
                case PORT_B_INPUT_VALUE::SW_WRITE_MASK:
                    write_command_handler();
                    break;
        
                case PORT_B_INPUT_VALUE::SW_MODE_MASK:
                    mode_command_handler();
                    break;
            }
        default:
            break;
    }
}


void Core0::port_a_discrete(void)
{
    uint8_t position;
    SystemData& system_data = itsStateManager.get_system_data();

#ifdef DEBUG
    printf("Numerical Handler\n");
    printf("Command: %d\n", q_buffer.data[0]);
#endif

    /* Convert the captured bit mask value into a zero-indexed value to enable indexing of patch array locations */
    for(uint8_t i = 0; i < NUM_LOOPS; i++)
    {
        if(input_lookup[i] == q_buffer.data[0])
        {
            position = i;
            break; /* found what we need so break */
        }
    }

#ifdef DEBUG
    printf("Mode: %d\n ", system_data.current_mode);
    printf("Single Input: %d\n", position);
#endif

    switch(system_data.current_mode)
    {
        case MODE::MANUAL: /* Manual mode: change the state of one output and update */
#ifdef DEBUG
            printf("Toggling: %d\n", position);
#endif
            itsStateManager.toggle_single_output_state(position);
            break;

        case MODE::PROGRAM: /* Program mode: load the newly selected patch into the output state */
#ifdef DEBUG
            printf("Now Using Patch: %d - %s\n", (position + 1), itsStateManager.get_active_patch_title());
#endif
            system_data.active_patch = position;
            itsStateManager.load_output_state();
            break;
            
        case MODE::WRITE: /* Write mode: update the write location and set valid flag */
#ifdef DEBUG
            printf("Set Write Loc: %d\n", position);    
#endif
            system_data.write_location = position;
            break;
    }

    /* common calls go here... e.g. update display(s)/outputs... */
    QUEUE_MESSAGE q_msg;
    q_msg.command = PORT_OUTPUT;
    q_msg.subcommand = WRITE_MASK;
    q_msg.data[0] = PORT_A;
    q_msg.data[1] = itsStateManager.get_output_mask();

    queue_add_blocking(itsTXQueue, &q_msg);
    //itsDisplayManager->update();
}



void Core0::mode_command_handler(void)
{
    SystemData& system_data = itsStateManager.get_system_data();

    MODE previous_mode = system_data.current_mode;
    system_data.previous_mode = previous_mode;
    itsStateManager.clear_output_mask();

    switch(previous_mode)
    {
        case MANUAL: // enter Program
            system_data.current_mode = PROGRAM;
            itsStateManager.load_output_state();
            break;

        case PROGRAM: // enter Manual
            system_data.current_mode = MANUAL;
            break;

        case WRITE: // write
            system_data.current_mode = MANUAL;
            system_data.write_location = -1; 
            //TODO: Implement underscore blink for patch location prior to selection
            break;

        case MENU:
            system_data.current_mode = system_data.previous_mode;
            // exit the menu... if/when it is implemented
            break;
        default:
            // should never get here...
            break;
    }

    // common calls go here, e.g. update display... outputs etc.

    //itsDisplayManager->update();
}

void Core0::write_command_handler()
{    
    SystemData& system_data = itsStateManager.get_system_data();

    switch(system_data.current_mode)
    {
        case MODE::MANUAL: // fall through
        case MODE::PROGRAM:
            system_data.current_mode = WRITE;
            system_data.write_location = -1;
            break;

        /* insert current output state into appropriate store location and save to flash */
        case MODE::WRITE:
            /* if a location has been selected */
            if(system_data.write_location >= 0)
            {
                itsStateManager.copy_output_state();

                /* if the write is readback successfully */
                bool success = itsStorageManager.write_patch_relay_output_mask(system_data.active_bank, 
                                                                    system_data.write_location, 
                                                                    itsStateManager.get_output_mask());

                if(success == 0)
                {
                    //itsDisplayManager->clear();
                    //itsDisplayManager->write_string((std::string*)&MSG_OK);
                    system_data.current_mode = PROGRAM;
                    system_data.active_patch = system_data.write_location;
                }
                else
                {
                    //itsDisplayManager->write_string((std::string*)&MSG_ERROR); //TODO: Blinking???
                }
                sleep_ms(500);
            }
            else
            {
                //itsDisplayManager->write_string((std::string*)&MSG_SELECT);
                sleep_ms(500);
            }

            //TODO: stuff here
            break;
        default:
            //should never get here
            break;
    }
    //itsDisplayManager->update();
}

void Core0::create_display_update(void)
{
    SystemData& system_data = itsStateManager.get_system_data();
    printf("<Core 0> display::update\n"); fflush(stdout);
    uint8_t mode = system_data.current_mode;
    uint8_t loc;

    char text[4];
    
    text[0] = mode_lookup[mode];

    switch(mode)
    {
        case MANUAL:
            //maybe other stuff later?
            break;

        case PROGRAM:
            text[1] = '-';

            //update_bank(system_data.get_active_bank());
            text[2] = bank_lookup[system_data.active_bank];

            loc = system_data.active_patch;

            if(loc < 5)
            {
                loc += 49;
                text[3] = loc;
            }
            else
            {
                text[3] = '_';
            }
            break;

        case WRITE:
            text[1] = '-';
            text[2] = bank_lookup[system_data.active_bank];

            loc = system_data.write_location;

            if(loc >= 0)
            {
                loc += 49;
                text[3] = loc;
            }
            else
            {
                text[3] = '_';
            }
            break;

        case MENU:
            // do stuff
            break;

        default:
            // no-op
            break;
    }
    //write_queue_item

}