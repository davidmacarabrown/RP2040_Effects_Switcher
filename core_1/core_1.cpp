#include "core_1.h"
#include "pico/sync.h"

bool alarm_active = false;
alarm_id_t active_alarm_id;
PORT port_status;

int64_t input_alarm_callback(alarm_id_t id, void *user_data)
{
#ifdef DEBUG
    printf("Alarm Callback\n"); fflush(stdout);
#endif
    //((QueueManager*)user_data)->schedule_event(POLL_INPUT);
    printf("Event Added\n");
    cancel_alarm(active_alarm_id);
    alarm_active = false;
    return 1;
}

void Core1::initialise(InputManager *input_mgr, OutputManager *output_mgr, DisplayManager *display_mgr)
{
    this->input_mgr = input_mgr;
    this->output_mgr = output_mgr;
    this->display_mgr = display_mgr;
}

void Core1::decode_port_output(void)
{
    switch(q_buffer.subcommand)
    {
        case RESET_PORT:
            output_mgr->reset((PORT)q_buffer.data[0]);
            break;

        case WRITE_SINGLE:
            output_mgr->set_one((PORT)q_buffer.data[0], q_buffer.data[1], q_buffer.data[2]);
            break;
            
        case WRITE_MASK:
            output_mgr->write_relay_port_mask((PORT)q_buffer.data[0], q_buffer.data[1]);
            break;
    }
}

int8_t Core1::alphanumeric_write(void)
{
    switch(q_buffer.data[0])
    {
        case ALPHANUMERIC_WRITE::SINGLE:
            display_mgr->alphanumeric_write_character(q_buffer.data[1], q_buffer.data[2]);
            break;

        case ALPHANUMERIC_WRITE::SEQUENCE:
            std::string text;
            int i = 1;
            text.push_back(q_buffer.data[i++]);
            text.push_back(q_buffer.data[i++]);
            text.push_back(q_buffer.data[i++]);
            text.push_back(q_buffer.data[i++]);
            
            display_mgr->alphanumeric_write_string(&text);
            break;
    }
    return 0;
}

int8_t Core1::decode_alphanumeric_command(void)
{
    switch(q_buffer.subcommand) //subcommand
    {
        case ALPHANUMERIC_CMD::SET_DISPLAY:
            alphanumeric_write();
            break;
        case ALPHANUMERIC_CMD::CLEAR:
            display_mgr->alphanumeric_clear();
            break;
        case ALPHANUMERIC_CMD::SET_BLINK:
            //TBD
            break;

    }
    return 0;
}

void Core1::read_queue(void)
{
    printf("read_queue\n");
    if(queue_try_peek(itsRXQueue, NULL))
    {
        queue_remove_blocking(itsRXQueue, &q_buffer);

        switch(q_buffer.command)
        {
#if 0 //TODO: hoist this out somewhere later
            case COMMAND::POLL_INPUT:
                printf("Event Rec\n");
                write_queue(input_mgr->get_port_status(), input_mgr->get_active_port_value());
                break;
#endif
            case COMMAND::PORT_OUTPUT:
                decode_port_output();
                break;
            
            case COMMAND::ALPHANUMERIC:
                decode_alphanumeric_command();
                break;

            default:
                break;
        }
    }
}

void Core1::write_queue(PORT port, uint8_t value)
{
    QUEUE_MESSAGE q_item;
    q_item.command = PORT_INPUT;
    q_item.subcommand = (uint8_t)port;
    q_item.data[0] = value;

    queue_add_blocking(itsTXQueue, &q_item);
}

InputManager* Core1::get_input_mgr(void)
{
    return this->input_mgr;
}

void Core1::poll_input_ports(void)
{
    port_status = input_mgr->poll_input_ports();

    if(port_status != PORT_NULL)
    {
        printf("Port is not null\n"); fflush(stdout);
        if(alarm_active == false)
        {
            printf("Alarm Trigger\n"); fflush(stdout);
            //active_alarm_id = add_alarm_in_ms(50, input_alarm_callback, queue_mgr, false); //TODO: fix this
            alarm_active = true;
        }
    }
}


