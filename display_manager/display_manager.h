#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

/* C/C++ Includes */
#include <string>
#include <memory>

/* Pico SDK Includes */
#include "pico/stdlib.h"

/* Project Includes */
#include "gpio_defs.h"
#include "HT16K33.h"
#include "state_manager.h"

const std::string MSG_OK     = ">OK<";
const std::string MSG_ERROR  = "-Err";
const std::string MSG_SELECT = "Sel_";
const std::string MSG_ROM_ERROR  = "EROM";

class DisplayManager
{
    private:
        std::shared_ptr<HT16K33> itsAlphaNumeric;

    public:
        void set_associations(std::shared_ptr<HT16K33> itsAlphaNumeric);
        void alphanumeric_clear(void);
        void alphanumeric_write_string(std::string *str);
        void alphanumeric_write_character(char character, uint8_t position);
        void alphanumeric_test(void);
};

#endif