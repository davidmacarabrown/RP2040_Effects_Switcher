#include"midi_commands.h"

MIDIProgramChange::MIDIProgramChange() : channel(0), data(0) {}

MIDIProgramChange::MIDIProgramChange(uint8_t channel, uint8_t data) : channel(channel), data(data) {}

void MIDIProgramChange::set_data(uint8_t data)
{
    this->data = data;
}

void MIDIProgramChange::set_channel(uint8_t channel)
{
    this->channel = channel;
}

uint8_t MIDIProgramChange::get_data(void) const
{
    return data;
}

uint8_t MIDIProgramChange::get_channel(void) const
{
    return channel;
}

MIDIControlChange::MIDIControlChange() : MIDIProgramChange(), controller(0) {}

MIDIControlChange::MIDIControlChange(uint8_t channel, uint8_t controller, uint8_t data) : MIDIProgramChange(channel, data), controller(controller) {}

void MIDIControlChange::set_controller(uint8_t value)
{
    controller = value;
}

uint8_t MIDIControlChange::get_controller(void) const
{
    return controller;
}

uint8_t MIDIProgramChange::serialise(uint8_t *buffer)
{
    buffer[0] = channel;
    buffer[1] = data;

    return 2;
}

void MIDIProgramChange::deserialise(uint8_t *buffer)
{
    this->channel = buffer[0];
    this->data    = buffer[1];
}

uint8_t MIDIControlChange::serialise(uint8_t *buffer)
{
    MIDIProgramChange::serialise(buffer);

    buffer[3] = controller;
    
    return 3;
}

void MIDIControlChange::deserialise(uint8_t *buffer)
{
    MIDIProgramChange::deserialise(buffer);
    
    this->controller = buffer[3];
}
