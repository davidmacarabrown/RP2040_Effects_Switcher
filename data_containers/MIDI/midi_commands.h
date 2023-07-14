#ifndef MIDI_COMMANDS_H
#define MIDI_COMMANDS_H

#include "pico/stdlib.h"

class MIDIProgramChange
{
    protected:
        uint8_t channel;
        uint8_t data;

    public:
        MIDIProgramChange();
        MIDIProgramChange(uint8_t channel, uint8_t data);

        void set_channel(uint8_t);
        uint8_t get_channel(void) const;

        void set_data(uint8_t data);
        uint8_t get_data(void) const;

        uint8_t serialise(uint8_t *buffer);
        void deserialise(uint8_t *buffer);
};


class MIDIControlChange : public MIDIProgramChange
{
    private:
        uint8_t controller;

    public:
        MIDIControlChange();
        MIDIControlChange(uint8_t channel, uint8_t controller, uint8_t data);

        void set_controller(uint8_t control);
        uint8_t get_controller(void) const;

        uint8_t serialise(uint8_t *buffer);
        void deserialise(uint8_t *buffer);
};
#endif