/* C Includes */
#include <memory.h>
#include <stdio.h>
#include <malloc.h>

/* Pico SDK Incldes */
#include "pico/stdlib.h"

/* Project Includes */
#include "state_manager.h"
#include "storage_manager.h"

StateManager::StateManager(void) : output_mask(0), system_data() {}

SystemData& StateManager::get_system_data(void)
{
    return this->system_data;
}
/****************************************************************
Function:   toggle_single_output_state
Arguments:  (uint8_t) index
Return:     void

Toggles a single output state index position to the opposite.
****************************************************************/
void StateManager::toggle_single_output_state(uint8_t position)
{
    uint8_t new_mask;
    uint8_t xor_mask = (OUTPUT_SHIFT_MASK << position);
    printf("XOR Mask: %02x\n", xor_mask); 
    new_mask = output_mask ^ xor_mask;
    printf("OUTPUT Mask: %02x\n", new_mask);

    output_mask = new_mask;
}


/****************************************************************
Function:   load_output_state
Arguments:  void
Return:     void

Loads a complete set of patch output values into the output_state.
****************************************************************/
void StateManager::load_output_state(void)
{
    output_mask = loaded_bank.at(system_data.active_patch).get_output_mask();
}

void StateManager::clear_output_mask(void)
{
    output_mask = 0;
}

/****************************************************************
Function:   copy_output_state
Arguments:  void
Return:     void

Copies the malleable output_state array values into the currently
selected BANK/PATCH position in the bank_array to be written to
flash.
****************************************************************/
bool StateManager::copy_output_state(void)
{
    if(system_data.write_location >= 0)
    {
        loaded_bank.at(system_data.active_patch).set_output_mask(output_mask);
        return true;
    }
    else
    {
        return false;
    }
}


/****************************************************************
Public Get/Set functions for data access and modification.
****************************************************************/

uint8_t StateManager::get_output_mask(void)
{
    return output_mask;
}

std::string StateManager::get_active_patch_title(void)
{
    return loaded_bank.at(system_data.active_patch).get_title();
}


int8_t StateManager::create_midi_pc(void)
{
    std::vector<MIDIProgramChange>& ref = loaded_bank.at(system_data.active_patch).get_midi_pc_vector();

    if(ref.size() < 10)
    {
        ref.emplace_back();
        return (ref.size() - 1); /* return the index of the created program change */
    }
    else
    {
        return -1;
    }
}

int8_t StateManager::create_midi_cc(void)
{
    std::vector<MIDIControlChange>& ref = loaded_bank.at(system_data.active_patch).get_midi_cc_vector();

    if(ref.size() < 10)
    {
         ref.emplace_back();
        return (ref.size() - 1); /* return the index of the created control change */
    }
    else
    {
        return -1;
    }
}

int8_t StateManager::delete_midi_pc(int8_t index)
{
    std::vector<MIDIProgramChange>& ref = loaded_bank.at(system_data.active_patch).get_midi_pc_vector();

    ref.erase(ref.begin() + index);
    return 0; //TODO: maybe don't need this?
}

int8_t StateManager::delete_midi_cc(int8_t index)
{
    std::vector<MIDIControlChange>& ref = loaded_bank.at(system_data.active_patch).get_midi_cc_vector();
    
    ref.erase(ref.begin() + index);
    return 0; //TODO: maybe don't need this?
}