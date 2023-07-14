#ifndef SYSTEM_DATA_H
#define SYSTEM_DATA_H

#include "gpio_defs.h"

class SystemData
{
    public:
        MODE current_mode;
        MODE previous_mode;

        CTRL_TYPE ext_ctrl_a_type;
        CTRL_TYPE ext_ctrl_b_type;

        uint8_t active_patch;
        uint8_t active_bank;

        int8_t selected_bank;
        int8_t selected_patch;
        
        int8_t write_location;

        SystemData();
        SystemData(MODE current_mode, CTRL_TYPE ext_ctrl_a, CTRL_TYPE ext_ctrl_b, uint8_t active_bank, uint8_t active_patch);

        void increment_bank(void);
        void decrement_bank(void);
};
#endif