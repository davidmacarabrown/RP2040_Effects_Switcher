#include "pico/stdlib.h"

typedef enum address_lut
{
    LUT_INVALID = -1,
    LUT_PATCH_GENERAL = 0,
    LUT_PATCH_OUTPUT,
    LUT_MIDI_PC_ADDRESS_MAP,
    LUT_MIDI_PC_DATA,
    LUT_MIDI_CC_ADDRESS_MAP,
    LUT_MIDI_CC_DATA

} ADDRESS_LUT;

 /* Boot Flag */
const uint32_t BOOT_FLAG = 0xACC01ADE;
const uint8_t BOOT_FLAG_ARRAY[4] = {0xAC, 0xC0, 0x1A, 0xDE};

/* Bitmasks for decoding System Data Flags */
const uint8_t MODE_FLAG_MASK  = 0b00000001;
const uint8_t EXT_CTRL_A_MASK = 0b00000010;
const uint8_t EXT_CTRL_B_MASK = 0b00000100;

/* Bitmasks for decoding Patch Data Flags */
const uint8_t PATCH_AMP_A_ENABLE_MASK       = 0b00000001;
const uint8_t PATCH_AMP_A_VALUE_MASK        = 0b00000010;
const uint8_t PATCH_AMP_B_ENABLE_MASK       = 0b00000100;
const uint8_t PATCH_AMP_B_VALUE_MASK        = 0b00001000;
const uint8_t PATCH_EXT_CTRL_A_ENABLE_MASK  = 0b00010000;
const uint8_t PATCH_EXT_CTRL_A_VALUE_MASK   = 0b00100000;
const uint8_t PATCH_EXT_CTRL_B_ENABLE_MASK  = 0b01000000;
const uint8_t PATCH_EXT_CTRL_B_VALUE_MASK   = 0b10000000;

const uint8_t MIDI_PC_SIZE = 2;
const uint8_t MIDI_CC_SIZE = 3;

const uint8_t MAX_MIDI_PC = 10;
const uint8_t MAX_MIDI_CC = 10;

/* Data Offsets - Group Offsets are ABSOLUTE, Parameter Offsets are RELATIVE */

/* Offsets within System Info */
const uint8_t  BOOT_FLAG_OFFSET            = 0;
const uint16_t BOOT_FLAG_END_OFFSET        = 4092;
const uint8_t  BOOT_FLAG_SIZE              = 4;
    
const uint8_t SYSTEM_INFO_OFFSET           = BOOT_FLAG_SIZE;
    
const uint8_t FLAGS_OFFSET                 = 0;
const uint8_t LAST_BANK_OFFSET             = 1;
const uint8_t LAST_PATCH_OFFSET            = 2;

const uint8_t SYSTEM_INFO_SIZE             = 3; 


/* ****Patch Data****  */
const uint8_t PATCH_DATA_OFFSET            = SYSTEM_INFO_OFFSET + SYSTEM_INFO_SIZE;
    
/* Offsets Within Patch Data */     
const uint8_t PATCH_TITLE_OFFSET           = 0;
const uint8_t PATCH_TITLE_SIZE             = 16;
    
const uint8_t PATCH_GENERAL_OFFSET         = PATCH_TITLE_SIZE;
const uint8_t PATCH_CTRL_FLAGS_OFFSET      = 0; //whether to modify amp ctrl state values or not
const uint8_t OUTPUT_BITMASK_OFFSET        = 1;
const uint8_t PATCH_GENERAL_SIZE           = 2;

const uint8_t PATCH_MIDI_PC_ADDRESS_MAP_OFFSET     = (PATCH_GENERAL_OFFSET + PATCH_GENERAL_SIZE);
const uint8_t PATCH_MIDI_PC_OFFSET                 = PATCH_MIDI_PC_ADDRESS_MAP_OFFSET + MAX_MIDI_PC;
const uint8_t PATCH_MIDI_PC_DATA_SIZE              = (MIDI_PC_SIZE * MAX_MIDI_PC) + MAX_MIDI_PC; // +10 to account for LUT

const uint8_t PATCH_MIDI_CC_ADDRESS_MAP_OFFSET     = (PATCH_MIDI_PC_OFFSET + PATCH_MIDI_PC_DATA_SIZE);
const uint8_t PATCH_MIDI_CC_DATA_OFFSET            = PATCH_MIDI_CC_ADDRESS_MAP_OFFSET + MAX_MIDI_CC;
const uint8_t PATCH_MIDI_CC_DATA_SIZE              = (MIDI_CC_SIZE * MAX_MIDI_CC) + MAX_MIDI_CC; // +10 to account for LUT
        
const uint32_t PATCH_DATA_SIZE                     = (PATCH_TITLE_SIZE + PATCH_GENERAL_SIZE + PATCH_MIDI_PC_DATA_SIZE + PATCH_MIDI_CC_DATA_SIZE);
const uint32_t BANK_DATA_SIZE                      = (PATCH_DATA_SIZE * NUM_PATCHES);