#include "system_data.h"

SystemData::SystemData() 
    : current_mode(MANUAL), 
      previous_mode(MANUAL), 
      ext_ctrl_a_type(LATCHING), 
      ext_ctrl_b_type(LATCHING), 
      active_bank(0), 
      active_patch(0), 
      selected_bank(0), 
      selected_patch(0), 
      write_location(-1) 
{
    //empty
}

SystemData::SystemData(MODE current_mode, CTRL_TYPE ext_ctrl_a, CTRL_TYPE ext_ctrl_b, uint8_t active_bank, uint8_t active_patch)
    : current_mode(current_mode), 
      previous_mode(current_mode), 
      ext_ctrl_a_type(ext_ctrl_a), 
      ext_ctrl_b_type(ext_ctrl_b), 
      active_bank(active_bank), 
      active_patch(active_patch), 
      selected_bank(-1), 
      selected_patch(-1), 
      write_location(-1)
{
    //empty
}

void SystemData::increment_bank(void)
{
    if(active_bank < (NUM_BANKS - 1))
    {
        active_bank++;
    }
}

void SystemData::decrement_bank(void)
{
    if(active_bank > 0)
    {
        active_bank--;
    }
}

