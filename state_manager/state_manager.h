#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

/* C/C++ Includes */
#include <array>
#include <memory>

/* Pico SDK Incldes */
#include "pico/stdlib.h"

/* Project Includes */
#include "gpio_defs.h"
#include "patch_data.h"
#include "system_data.h"

class StateManager
{
    private:
        /* Malleable register of the current output state, effectively the "source of truth" for what is active at any given time */
        uint8_t output_mask; // TODO: encapsulate this?

        /* In-Memory storage of all patch data */
        std::array<PatchData, NUM_PATCHES> loaded_bank;
        
        /* System Info */
        SystemData system_data;

    public:
        StateManager(void);

        SystemData& get_system_data(void);

        void toggle_single_output_state(uint8_t position);
        void load_output_state(void);
        bool copy_output_state(void);
        uint8_t get_output_mask(void);
        void clear_output_mask(void);

        std::string get_active_patch_title(void);

        int8_t create_midi_pc(void);
        int8_t create_midi_cc(void);

        int8_t delete_midi_pc(int8_t index);
        int8_t delete_midi_cc(int8_t index);
};

#endif
