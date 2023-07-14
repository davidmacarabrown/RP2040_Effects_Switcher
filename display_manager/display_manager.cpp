/* C Includes */
// #include <cstdio>

/* Project Includes */
#include "display_manager.h"

void DisplayManager::set_associations(std::shared_ptr<HT16K33> itsAlphaNumeric)
{
    printf("<core0> init display_manager\n");
    this->itsAlphaNumeric = itsAlphaNumeric;
}

void DisplayManager::alphanumeric_clear(void)
{
    itsAlphaNumeric->clear_buffer();
    itsAlphaNumeric->update();
}

#if 0
void DisplayManager::update_patch(uint8_t value)
{
    value += 49;
    
#ifdef DEBUG
    printf("DisplayManager::update_patch\n");
    printf(" - Setting Patch To: %c\n", (char)value);
#endif

    itsAlphaNumeric->write_character(value, 3);
}

void DisplayManager::update_bank(uint8_t value)
{
    uint8_t bank_code = bank_lookup[value];
    uint16_t found_value = itsAlphaNumeric->character_lookup((char)bank_code);

    found_value |= itsAlphaNumeric->character_lookup('.');

    itsAlphaNumeric->load_character_mask(found_value, 2);
}
#endif


void DisplayManager::alphanumeric_write_string(std::string *str)
{
    itsAlphaNumeric->put_string(str);
}

void DisplayManager::alphanumeric_write_character(char character, uint8_t position)
{
    itsAlphaNumeric->write_character(character, position);
}

void DisplayManager::alphanumeric_test()
{
    itsAlphaNumeric->set_oscillator(true);
    itsAlphaNumeric->output_enable(true);
    itsAlphaNumeric->clear_buffer();
    itsAlphaNumeric->update();
    itsAlphaNumeric->write_character('C', 0);

    sleep_ms(1000);

    std::string test = "Test";
    alphanumeric_write_string(&test);
    sleep_ms(1000);
}

//TODO: Implement blinking for underscore when waiting for a selection of write location or new patch after changing bank
