/* C/C++ Includes */

/* Project Includes */
#include "debug.h"
#include "storage_manager.h"
#include "state_manager.h"

StorageManager::StorageManager(i2c_inst_t* i2cInstance, const uint8_t i2cAddress) : itsEEPROM(i2cInstance, i2cAddress)
{}

void StorageManager::factory_reset(void)
{
    itsEEPROM.erase();
    validate_eeprom();
}

int16_t StorageManager::address_lookup(uint8_t bank, uint8_t patch, uint8_t parameter, int8_t index)
{
    int16_t result = LUT_INVALID;

    bank*= BANK_DATA_SIZE;
    patch*= PATCH_DATA_SIZE;

    switch(parameter)
    {
        case LUT_PATCH_GENERAL:
            result = (PATCH_DATA_OFFSET + bank + patch);
            break;

        case LUT_PATCH_OUTPUT:
            result = (PATCH_DATA_OFFSET + bank + patch + PATCH_GENERAL_OFFSET + OUTPUT_BITMASK_OFFSET);
            break;

        case LUT_MIDI_PC_ADDRESS_MAP:
            result = (PATCH_DATA_OFFSET + bank + patch + PATCH_MIDI_PC_ADDRESS_MAP_OFFSET);
            break;

        case LUT_MIDI_PC_DATA:
            if(index >= 0)
            {
                result = (PATCH_DATA_OFFSET + bank + patch + PATCH_MIDI_PC_ADDRESS_MAP_OFFSET + (PATCH_MIDI_PC_OFFSET * index));
            }
            break;

        case LUT_MIDI_CC_ADDRESS_MAP:    
            result = (PATCH_DATA_OFFSET + bank + patch + PATCH_MIDI_CC_ADDRESS_MAP_OFFSET);
            break;

        case LUT_MIDI_CC_DATA:
            if(index >= 0)
            {
                result = (PATCH_DATA_OFFSET + bank + patch + PATCH_MIDI_PC_ADDRESS_MAP_OFFSET + (PATCH_MIDI_CC_DATA_OFFSET * index));
            }
            break;

        default:
            result = LUT_INVALID;
    }
    return result;
}

void StorageManager::read_system_data(SystemData& system_data)
{
    uint8_t read_buffer[CAT24C32_PAGE_SIZE];

#ifdef STORAGE_DEBUG
    printf("StorageManager::read_system_data()\n");
#endif

    itsEEPROM.read_multiple_bytes(SYSTEM_INFO_OFFSET, SYSTEM_INFO_SIZE, read_buffer);

#ifdef STORAGE_DEBUG
    printf(" - Read EEPROM\n");
    fflush(stdout);
#endif

    /* Read and decode system data flags */
    system_data.current_mode = (MODE)((read_buffer[FLAGS_OFFSET]) & MODE_FLAG_MASK);

#ifdef STORAGE_DEBUG
    printf(" - Mode Read As: %d\n", system_data.get_startup_mode());
    fflush(stdout);
#endif

    //TODO:
    system_data.ext_ctrl_a_type = ((CTRL_TYPE)((read_buffer[FLAGS_OFFSET] & EXT_CTRL_A_MASK) >> 1));
    system_data.ext_ctrl_b_type = ((CTRL_TYPE)((read_buffer[FLAGS_OFFSET] & EXT_CTRL_B_MASK) >> 2));

    /* Read Last Bank/Patch */
    system_data.active_bank = (read_buffer[LAST_BANK_OFFSET]);
    system_data.active_patch = (read_buffer[LAST_PATCH_OFFSET]);
}


void StorageManager::read_bank(uint8_t bank, std::array<PatchData, NUM_PATCHES>& patch_array)
{
#ifdef STORAGE_DEBUG
    printf("StorageManager::read_bank()\n");
#endif

    read_patch(bank, 0, patch_array.at(0));
    read_patch(bank, 1, patch_array.at(1));
    read_patch(bank, 2, patch_array.at(2));
    read_patch(bank, 3, patch_array.at(3));
    read_patch(bank, 4, patch_array.at(4));
}

void StorageManager::read_patch(uint8_t bank, uint8_t patch, PatchData& p_patch_data)
{
#ifdef STORAGE_DEBUG
    printf("StorageManager::read_patch()\n");
#endif

    int16_t location = address_lookup(bank, patch, LUT_PATCH_GENERAL, LUT_INVALID);
    uint8_t read_buffer[CAT24C32_PAGE_SIZE];

#ifdef STORAGE_DEBUG
    printf(" -Reading Bank %d, Patch %d\n", bank, patch);
#endif

    /* Title */
#ifdef STORAGE_DEBUG
    printf(" - Reading Title\n");
#endif

    if(location)
    {
        itsEEPROM.read_multiple_bytes(location, PATCH_TITLE_SIZE, read_buffer);
    
        std::string title((const char*)read_buffer, PATCH_TITLE_SIZE);
        
        p_patch_data.set_title(title);

    #ifdef STORAGE_DEBUG
        printf(" - Reading General\n");
    #endif

        /* General Data */
        location+= PATCH_GENERAL_OFFSET;
        itsEEPROM.read_multiple_bytes(location, PATCH_GENERAL_SIZE, read_buffer);

        p_patch_data.set_amp_ctrl_a_enable ( (read_buffer[PATCH_CTRL_FLAGS_OFFSET] & PATCH_AMP_A_ENABLE_MASK) );
        p_patch_data.set_amp_ctrl_a_value  ( (read_buffer[PATCH_CTRL_FLAGS_OFFSET] & PATCH_AMP_A_VALUE_MASK) );
        p_patch_data.set_amp_ctrl_b_enable ( (read_buffer[PATCH_CTRL_FLAGS_OFFSET] & PATCH_AMP_B_ENABLE_MASK) );
        p_patch_data.set_amp_ctrl_b_value  ( (read_buffer[PATCH_CTRL_FLAGS_OFFSET] & PATCH_AMP_B_VALUE_MASK) );
        p_patch_data.set_ext_ctrl_a_enable ( (read_buffer[PATCH_CTRL_FLAGS_OFFSET] & PATCH_EXT_CTRL_A_ENABLE_MASK) );
        p_patch_data.set_ext_ctrl_a_value  ( (read_buffer[PATCH_CTRL_FLAGS_OFFSET] & PATCH_EXT_CTRL_A_VALUE_MASK) );
        p_patch_data.set_ext_ctrl_b_enable ( (read_buffer[PATCH_CTRL_FLAGS_OFFSET] & PATCH_EXT_CTRL_B_ENABLE_MASK) );
        p_patch_data.set_ext_ctrl_b_value  ( (read_buffer[PATCH_CTRL_FLAGS_OFFSET] & PATCH_EXT_CTRL_B_VALUE_MASK) );

        p_patch_data.set_output_mask(read_buffer[OUTPUT_BITMASK_OFFSET]);


        /* MIDI Program Changes */
    #ifdef STORAGE_DEBUG
        printf(" - Reading MIDI PC\n");
    #endif

        /* import address map - maps 1:1 <array location> : <ROM position> -1 == unused */
        read_midi_pc_address_map(bank, patch);

        for(int i = 0; i < MAX_MIDI_PC; i++)
        {
            if(midi_pc_address_map[i] >= 0) // if lut value >= 0 it is in use
            {
                itsEEPROM.read_multiple_bytes(address_lookup(bank, patch, LUT_MIDI_PC_DATA, i), MIDI_PC_SIZE, read_buffer);

                MIDIProgramChange midi_pc;
                midi_pc.deserialise(read_buffer);
                p_patch_data.get_midi_pc_vector().push_back(midi_pc);
            }
        }
        
        /* MIDI Control Changes */
    #ifdef STORAGE_DEBUG
        printf(" - Reading MIDI CC\n");
    #endif

        read_midi_cc_address_map(bank, patch);

        for(int i = 0; i < MAX_MIDI_CC; i++)
        {
            if(midi_cc_address_map[i] >= 0)
            {
                itsEEPROM.read_multiple_bytes(address_lookup(bank, patch, LUT_MIDI_CC_DATA, midi_cc_address_map[i]), MIDI_CC_SIZE, read_buffer);

                MIDIControlChange midi_cc;
                midi_cc.deserialise(read_buffer);
                p_patch_data.get_midi_cc_vector().push_back(midi_cc);
            }
        }
    }
    else
    {
        //TODO:
        // return error
    }
}

int8_t StorageManager::write_active_bank(uint8_t bank)
{
    int8_t result = itsEEPROM.write_byte(bank, SYSTEM_INFO_OFFSET + LAST_BANK_OFFSET);
    return result;
}

int8_t StorageManager::write_active_patch(uint8_t patch)
{
    int8_t result = itsEEPROM.write_byte(patch, SYSTEM_INFO_OFFSET + LAST_PATCH_OFFSET);
    return result;
}

int8_t StorageManager::write_system_flags(uint8_t mode, uint8_t ctrl_a, uint8_t ctrl_b)
{
    uint8_t mask = 0;
    int8_t result;

    mask |= mode;
    mask |= (ctrl_a << 1);
    mask |= (ctrl_b << 2);

    itsEEPROM.write_byte(mask, SYSTEM_INFO_OFFSET);

    result = itsEEPROM.read_byte(SYSTEM_INFO_OFFSET);  
    return result;
}

int8_t StorageManager::write_patch_title(uint8_t bank, uint8_t patch, const std::string& title)
{
    int8_t result;
    uint8_t read_buffer[CAT24C32_PAGE_SIZE];
    uint8_t write_buffer[CAT24C32_PAGE_SIZE];

    int16_t byte_address = address_lookup(bank, patch, LUT_PATCH_GENERAL, LUT_INVALID);

    if(byte_address)
    {
    #ifdef STORAGE_DEBUG
        printf("Write Path Title Calculated Offset: %d\n", byte_address);
    #endif
        memcpy(write_buffer, title.c_str(), PATCH_TITLE_SIZE);

        result = itsEEPROM.write_multiple_bytes(write_buffer, byte_address, PATCH_TITLE_SIZE);
    }
    else
    {
        result = -1;
    }
    return result;
}

int8_t StorageManager::write_patch_relay_output_mask(uint8_t active_bank, uint8_t write_location, uint8_t bit_mask)
{
    uint8_t result = 0;
    int16_t byte_address = address_lookup(active_bank, write_location, LUT_PATCH_OUTPUT, LUT_INVALID);

    if(byte_address)
    {
#ifdef STORAGE_DEBUG
        printf("StorageManager::write_patch_relay_output_mask()\n");
        printf(" - Bank: %d, Patch: %d, Mask: %02x\n", itsStateManager->get_active_bank(), itsStateManager->get_write_location(), itsStateManager->get_output_mask());
#endif

        result = itsEEPROM.write_byte(bit_mask, byte_address);
    
#ifdef STORAGE_DEBUG
        printf("Readback: %02x\n", itsEEPROM.read_byte(byte_address));
#endif
    }
    else
    {
        return -1;
    }
    return result;
}

void StorageManager::read_midi_pc_address_map(uint8_t bank, uint8_t patch)
{
    uint8_t read_buffer[10];
    itsEEPROM.read_multiple_bytes(address_lookup(bank, patch, LUT_MIDI_PC_ADDRESS_MAP, LUT_INVALID), MAX_MIDI_PC, read_buffer);
    memcpy(midi_pc_address_map.data(), read_buffer, MAX_MIDI_PC);
}
        
void StorageManager::read_midi_cc_address_map(uint8_t bank, uint8_t patch)
{
    uint8_t read_buffer[10];
    itsEEPROM.read_multiple_bytes(address_lookup(bank, patch, LUT_MIDI_CC_ADDRESS_MAP, LUT_INVALID), MAX_MIDI_CC, read_buffer);
    memcpy(midi_cc_address_map.data(), read_buffer, MAX_MIDI_CC);
}

int8_t StorageManager::write_new_midi_program_change(uint8_t bank, uint8_t patch, MIDIProgramChange& midi_program_change, int8_t index)
{
    uint8_t data[MIDI_PC_SIZE];
    bool position_valid;
    int8_t found_location = -1;

    read_midi_pc_address_map(bank, patch);

    for(int rom_position = 0; rom_position < MAX_MIDI_PC; rom_position++) /* for each potential location check for a list entry already using it */
    {
        position_valid = true;

        for(int array_position = 0; array_position < MAX_MIDI_PC; array_position++)
        {
            if(midi_pc_address_map[array_position] == rom_position) 
            {
                position_valid = false;
                break; /* this location is already in use so no need to search further array positions */
            }
        }

        if(position_valid)
        {
            found_location = rom_position; /* valid location found so break outer loop */
            break;
        }
    }

    int8_t result = -1;

    if(found_location >= 0)
    {
        /* update address map */
        midi_pc_address_map[index] = found_location;
        itsEEPROM.write_byte(found_location, (address_lookup(bank, patch, LUT_MIDI_PC_ADDRESS_MAP, LUT_INVALID) + index));

        midi_program_change.serialise(data);
        result = itsEEPROM.write_multiple_bytes(data, address_lookup(bank, patch, LUT_MIDI_PC_DATA, found_location), MIDI_PC_SIZE);
    }
    return result;
    
}

int8_t StorageManager::erase_midi_program_change(uint8_t bank, uint8_t patch, int8_t index)
{
    if(midi_pc_address_map[index + 1] == -1) /* deletion is last index so just mark it as free */
    {
        midi_pc_address_map[index] = -1;
    }
    else
    {
        for(int i = (index + 1); i < MAX_MIDI_PC; i++)
        {
            if(midi_pc_address_map[i])
            {
                midi_pc_address_map[i - 1] = midi_pc_address_map[i];
            }
            else
            {
                midi_pc_address_map[i - 1] = -1;
                break;
            }
        }
    }

    uint8_t data[MAX_MIDI_PC];
    memcpy(data, midi_pc_address_map.data(), MAX_MIDI_PC);

    return itsEEPROM.write_multiple_bytes(data, address_lookup(bank, patch, LUT_MIDI_PC_ADDRESS_MAP, LUT_INVALID), MAX_MIDI_PC);
}


int8_t StorageManager::write_new_midi_control_change(uint8_t bank, uint8_t patch, MIDIControlChange& midi_control_change, int8_t index)
{
    uint8_t data[MIDI_CC_SIZE];
    bool location_valid = true;
    int8_t found_location = -1;

    read_midi_cc_address_map(bank, patch);

    for(int possible_rom_position = 0; possible_rom_position < MAX_MIDI_CC; possible_rom_position++) /* for each potential location check for a list entry already using it */
    {
        location_valid = true;

        for(int array_position = 0; array_position < MAX_MIDI_CC; array_position++)
        {
            if(midi_cc_address_map[array_position] == possible_rom_position)
            {
                location_valid = false;
                break;
            }
        }

        if(location_valid)
        {
            found_location = possible_rom_position;
            break;
        }
    }

    int8_t result = -1;

    if(found_location >= 0)
    {
        /* update address map */
        midi_cc_address_map[index] = found_location;
        itsEEPROM.write_byte(found_location, (address_lookup(bank, patch, LUT_MIDI_CC_ADDRESS_MAP, LUT_INVALID) + index));
        
        midi_control_change.serialise(data);

        result = itsEEPROM.write_multiple_bytes(data, address_lookup(bank, patch, LUT_MIDI_CC_DATA, found_location), MIDI_CC_SIZE);
    }
    return result;
}

int8_t StorageManager::erase_midi_control_change(uint8_t bank, uint8_t patch, int8_t index)
{
    if(midi_cc_address_map[index + 1] == -1) /* deletion is last index so just mark it as free */
    {
        midi_cc_address_map[index] = -1;
    }
    else
    {
        for(int i = (index + 1); i < MAX_MIDI_PC; i++)
        {
            if(midi_cc_address_map[i])
            {
                midi_cc_address_map[i - 1] = midi_cc_address_map[i];
            }
            else
            {
                midi_cc_address_map[i - 1] = -1;
                break;
            }
        }
    }

    uint8_t data[MAX_MIDI_CC];
    memcpy(data, midi_cc_address_map.data(), MAX_MIDI_CC);

    return itsEEPROM.write_multiple_bytes(data, address_lookup(bank, patch, LUT_MIDI_CC_ADDRESS_MAP, LUT_INVALID), MAX_MIDI_CC);
}

int8_t StorageManager::validate_eeprom(void)
{
    int header_not_found;
    int footer_not_found;
    int8_t result = 0;
    int byte_address;
    uint8_t offset;
    char string_buffer[17];
    uint8_t write_buffer[CAT24C32_PAGE_SIZE];
    uint8_t read_buffer[CAT24C32_PAGE_SIZE];

    /* Check first 4 bytes of EEPROM for the boot flag */
    itsEEPROM.read_multiple_bytes(BOOT_FLAG_OFFSET, BOOT_FLAG_SIZE, read_buffer);

#ifdef STORAGE_DEBUG
    //sleep_ms(5000);
    printf("Checking Boot Header:\n");
    print_buff(read_buffer, BOOT_FLAG_SIZE);
    fflush(stdout);
#endif

    header_not_found = memcmp(read_buffer, BOOT_FLAG_ARRAY, BOOT_FLAG_SIZE);

#ifdef STORAGE_DEBUG
    printf("Header Result: %d\n", header_not_found);
#endif

    itsEEPROM.read_multiple_bytes(BOOT_FLAG_END_OFFSET, BOOT_FLAG_SIZE, read_buffer);

#ifdef STORAGE_DEBUG
    printf("Checking Boot Footer:\n");
    print_buff(read_buffer, BOOT_FLAG_SIZE);
    fflush(stdout);
#endif

    footer_not_found = memcmp(read_buffer, BOOT_FLAG_ARRAY, BOOT_FLAG_SIZE);
    
#ifdef STORAGE_DEBUG
    printf("Footer Result %d\n", footer_not_found);
#endif

    //if(1)
    if((header_not_found != 0) || (footer_not_found != 0))
    {
        // TODO: maybe put something in here to inform that the pedal is doing some "setup" or whatever
        
#ifdef STORAGE_DEBUG
        printf("First Boot!\n");
        printf("Erasing...\n");
#endif

        /* Erase all 4096 bytes of EEPROM */
        itsEEPROM.erase();

#ifdef STORAGE_DEBUG
        printf("All Erased, Readback:\n"); 
        for(int i = 0; i < CAT24C32_PAGE_COUNT; i++)
        {
            address = i * CAT24C32_PAGE_SIZE;
            itsEEPROM.read_multiple_bytes(address, CAT24C32_PAGE_SIZE, read_buffer);
            printf("Reading Page: %d\n", i);
            print_buff(read_buffer, CAT24C32_PAGE_SIZE);
            fflush(stdout);
            sleep_ms(5);
        }
#endif

        /* First boot, so write boot flag to byes 0-3 */
        result = itsEEPROM.write_multiple_bytes(BOOT_FLAG_ARRAY, BOOT_FLAG_OFFSET, BOOT_FLAG_SIZE);

#ifdef STORAGE_DEBUG
        printf("Buffer written:\n");
        print_buff(write_buffer, BOOT_FLAG_SIZE);
        fflush(stdout);

        printf("\n");
        printf("Readback:\n");
        itsEEPROM.read_multiple_bytes(read_buffer, BOOT_FLAG_OFFSET, BOOT_FLAG_SIZE);
        print_buff(read_buffer, BOOT_FLAG_SIZE);
        fflush(stdout);
#endif
        if(result)
        {   

#ifdef STORAGE_DEBUG
            printf("WRITE FAILED\n");
#endif
            return result;

        }
#ifdef STORAGE_DEBUG
        else
        {
            printf("Boot flag written and readback successfully!\n");
        }
#endif

        /* Write default system data */
#ifdef STORAGE_DEBUG
        printf("Writing System Info Defaults...\n");
#endif

        write_buffer[0] = 0b00000000; // setting defaults for mode/ext ctrl A/B type
        write_buffer[1] = 0;          // default bank: 1
        write_buffer[2] = 0;          // default patch: 1

#ifdef STORAGE_DEBUG
        printf("Sys Info Buffer:\n");
        print_buff(write_buffer, SYSTEM_INFO_SIZE); // remaining bytes reserved for IP address/other data in future
#endif

        result = itsEEPROM.write_multiple_bytes(write_buffer, SYSTEM_INFO_OFFSET, SYSTEM_INFO_SIZE);

#ifdef STORAGE_DEBUG
        /* Readback the data to check */
        printf("Sys Info Readback:\n");
        itsEEPROM.read_multiple_bytes(read_buffer, SYSTEM_INFO_OFSET, SYSTEM_INFO_SIZE);
        print_buff(read_buffer, SYSTEM_INFO_SIZE);
#endif
        if(result)
        {
            //TODO: Different return values to indicate different failure?
#ifdef STORAGE_DEBUG
            printf("ERROR WRITING SYS INFO\n");
#endif
            return result;
        }    


        for(int bank = 0; bank < NUM_BANKS; bank++)
        {
            for(int patch = 0; patch < NUM_PATCHES; patch++)
            {
                sprintf(string_buffer, "New Patch %d", patch);
                std::string title(string_buffer);
                
#ifdef STORAGE_DEBUG
                printf("Formatted String: [%s]\n", title.c_str());
                fflush(stdout);
#endif
                result = write_patch_title(bank, patch, title);

                if(result) 
                {
#ifdef STORAGE_DEBUG
                printf("Failed to write patch title for %d:%d\n", bank, patch);
                fflush(stdout);
#endif
                    return result;
                }        

#ifdef STORAGE_DEBUG
                itsEEPROM.read_multiple_bytes(PATCH_DATA_OFFSET + (bank * BANK_DATA_SIZE) + (patch * PATCH_DATA_SIZE), PATCH_TITLE_SIZE, read_buffer);
                memcpy(string_buffer, read_buffer, sizeof(read_buffer));
                printf("Bank: %d Patch: %d [%s]\n", bank, patch, string_buffer);
                fflush(stdout);
#endif
            }
        }
        
        itsEEPROM.write_multiple_bytes(BOOT_FLAG_ARRAY, BOOT_FLAG_END_OFFSET, BOOT_FLAG_SIZE);

#ifdef STORAGE_DEBUG
        printf("All Readback:\n"); 

        for(int i = 0; i < CAT24C32_PAGE_COUNT; i++)
        {
            address = i * CAT24C32_PAGE_SIZE;
            memset(read_buffer, 0, CAT24C32_PAGE_SIZE);
            itsEEPROM.read_multiple_bytes(address, CAT24C32_PAGE_SIZE, read_buffer);
            printf("Reading Page: %d\n", i);
            print_buff(read_buffer, CAT24C32_PAGE_SIZE);
            fflush(stdout);
        }
#endif
    }
#ifdef STORAGE_DEBUG
    else
    {
        printf("Boot Flags Detected, Pass Validation\n");
    }
#endif
    return result;
}

void StorageManager::test(void)
{
    uint16_t address;
    uint8_t test_buffer[64];
    uint8_t read_buffer[CAT24C32_PAGE_SIZE];

    itsEEPROM.erase();

    for(int i = 0; i < 8; i++)
    {
        address = i * CAT24C32_PAGE_SIZE;
        memset(read_buffer, 0, CAT24C32_PAGE_SIZE);
        itsEEPROM.read_multiple_bytes(address, CAT24C32_PAGE_SIZE, read_buffer);
        printf("Reading Page: %d\n", i);
        print_buff(read_buffer, CAT24C32_PAGE_SIZE);
        fflush(stdout);
    }   

    memset(test_buffer, 0xAA, 32);
    memset(&test_buffer[32], 0xBB, 32);

    itsEEPROM.write_multiple_bytes(test_buffer, 0, 64);

    printf("All Readback:\n"); 

    for(int i = 0; i < 8; i++)
    {
        address = i * CAT24C32_PAGE_SIZE;
        memset(read_buffer, 0, CAT24C32_PAGE_SIZE);
        itsEEPROM.read_multiple_bytes(address, CAT24C32_PAGE_SIZE, read_buffer);
        printf("Reading Page: %d\n", i);
        print_buff(read_buffer, CAT24C32_PAGE_SIZE);
        fflush(stdout);
    }    
}

