#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

/* C/C++ Includes */
#include <memory>
#include <string>

/* Pico Includes */
#include "pico/stdlib.h"
#include "hardware/sync.h"

/* Project Includes */
//#include "gpio_defs.h"
#include "CAT24C32.h"
#include "midi_commands.h"
#include "patch_data.h"
#include "storage_defintions.h"

class SystemData;

class StorageManager
{
    private:
        CAT24C32 itsEEPROM;

        int16_t address_lookup(uint8_t bank, uint8_t patch, uint8_t parameter, int8_t index);

        std::array<int8_t, MAX_MIDI_PC> midi_pc_address_map;
        std::array<int8_t, MAX_MIDI_CC> midi_cc_address_map;

        void read_midi_pc_address_map(uint8_t bank, uint8_t patch);
        void read_midi_cc_address_map(uint8_t bank, uint8_t patch);

    public:
        StorageManager(i2c_inst_t* i2cInstance, const uint8_t i2cAddress);
        void factory_reset(void);

        int8_t validate_eeprom(void);

        void read_system_data(SystemData& system_data);

        void read_preset_data(void);
        void read_bank(uint8_t bank, std::array<PatchData, NUM_PATCHES>& patch_array);
        void read_patch(uint8_t bank, uint8_t patch, PatchData& patch_data);

        int8_t write_system_flags(uint8_t mode, uint8_t ctrl_a, uint8_t ctrl_b);
        int8_t write_active_bank(uint8_t bank);
        int8_t write_active_patch(uint8_t patch);

        int8_t write_patch_title(uint8_t bank, uint8_t patch, const std::string& title);
        int8_t write_patch_relay_output_mask(uint8_t active_bank, uint8_t write_location, uint8_t bit_mask);
        
        int8_t write_new_midi_program_change(uint8_t bank, uint8_t patch, MIDIProgramChange& midi_program_change, int8_t index);
        int8_t erase_midi_program_change(uint8_t bank, uint8_t patch, int8_t index);

        int8_t write_new_midi_control_change(uint8_t bank, uint8_t patch, MIDIControlChange& midi_control_change, int8_t index);
        int8_t erase_midi_control_change(uint8_t bank, uint8_t patch, int8_t index);
        
        void test(void);
};

#endif