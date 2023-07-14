#ifndef GPIO_DEFS_H
#define GPIO_DEFS_H

#include "pico/stdlib.h"
#include "hardware/flash.h"

/* This file defines GPIO macros, structures and data storage offsets */
#define DEBUG 1

const uint8_t q_item_size = 5;

typedef struct queue_message
{
    uint8_t command;
    uint8_t subcommand;
    uint8_t data[16];

} QUEUE_MESSAGE;

/* CTRL Types */
typedef enum ctrl_type
{
    MOMENTARY,
    LATCHING 
} CTRL_TYPE;

/* LED Indicators */
const uint32_t PICO_LED           = 25;

const uint8_t OUTPUT_SHIFT_MASK = 0x01;


/* Number of times and cadence which an LED should blink during rapid_blink() */
const uint8_t BLINK_REPEATS       = 6;
const uint8_t RAPID_BLINK_TIME    = 50;
const uint16_t SLOW_BLINK_TIME     = 300;

const uint8_t NUM_LOOPS     = 5;
const uint8_t NUM_BANKS     = 5;
const uint8_t NUM_PATCHES   = 5;
const uint8_t TOTAL_PATCHES = 25;

/* Command Decode Values */
typedef enum command
{
    PORT_INPUT,
    PORT_OUTPUT,
    POLL_INPUT,
    ALPHANUMERIC 
} COMMAND;

typedef enum alphanumeric_command
{
    SET_DISPLAY,
    CLEAR,
    SET_BLINK
} ALPHANUMERIC_CMD; 

typedef enum alphanumeric_write
{
    SINGLE,
    SEQUENCE
} ALPHANUMERIC_WRITE;

/* Port Output Commands */
typedef enum port_output_command
{
    RESET_PORT,
    WRITE_SINGLE,
    WRITE_MASK
} PORT_OUTPUT_COMMAND;

typedef enum port_a_input_value
{                  
    SW_1       = 0x01,
    SW_2       = 0x02,
    SW_3       = 0x04,
    SW_4       = 0x08,
    SW_5       = 0x16,
    FSW_A      = 0x32,
    FSW_B      = 0x64,
    PATCH_UP   = 0x03,
    PATCH_DOWN = 0x06
} PORT_A_INPUT_VALUE;

typedef enum port_b_input_value
{        
    SW_MODE_MASK    = 0x01,
    SW_WRITE_MASK   = 0x02
} PORT_B_INPUT_VALUE;

const uint8_t INVALID = 0;

const uint32_t QUEUE_LENGTH = 5;

/* State Macros */
const uint8_t HIGH = 1;
const uint8_t LOW  = 0;

/* Modes */
typedef enum mode //TODO: move this somewhere more relevant
{
    MANUAL      = 0,
    PROGRAM     = 1,
    WRITE       = 2,
    MENU        = 3
} MODE;

typedef enum core_id
{
    CORE_0,
    CORE_1
} CORE_ID;

/* Lookup Tables - Enables index based lookup of GPIO values, input bit masks and character codes. 
Pins in use may not always be consecutive, so cannot be accessed by looping with a fixed offset */

/* 32Bit masks for decoding input */
const uint8_t input_lookup[] = {SW_1, SW_2, SW_3, SW_4, SW_5};

/* Bank Position to ASCII */
const char bank_lookup[] = {0x41, 0x42, 0x43, 0x44, 0x45};

/* Array to provide ASCII character lookup for modes... e.g. MANUAL = 0, mode_lookup[0] = ASCII value for M - 77 */
const char mode_lookup[] = {77, 80, 87};

/* i2c Pinouts */
const uint8_t I2C0_DATA_PIN    = 12;
const uint8_t I2C0_CLOCK_PIN   = 13;

const uint8_t I2C1_DATA_PIN    = 2;
const uint8_t I2C1_CLOCK_PIN   = 3;

const uint8_t I2C1_DATA_SECONDARY = 26;
const uint8_t I2C1_CLOCK_SECONDARY = 27;


/* i2c Device Addresses */
const uint8_t QUAD_ADDR            = 0x70;
const uint8_t EEPROM_ADDR          = 0x50;
const uint8_t OUTPUT_PORT_I2C_ADDR = 0x20;
const uint8_t LCD_ADDR             = 0x27;

/* Generic GPIO Pinouts*/
const uint8_t MUTE_OPTORELAY_PIN = 0;
const uint8_t I2C_INT_A          = 2;
const uint8_t I2C_INT_B          = 3;

const uint8_t PORT_RESET_PIN     = 22;

#endif