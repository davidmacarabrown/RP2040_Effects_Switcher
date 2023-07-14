#ifndef PATCH_DATA_H
#define PATCH_DATA_H


#include <string>
#include <vector>
#include <memory>

#include "midi_commands.h"

class PatchData
{
    private:
        std::string title;

        uint8_t amp_ctrl_a_enable;
        uint8_t amp_ctrl_a_value;

        uint8_t amp_ctrl_b_enable;
        uint8_t amp_ctrl_b_value;

        uint8_t ext_ctrl_a_enable;
        uint8_t ext_ctrl_b_enable;

        uint8_t ext_ctrl_a_value;
        uint8_t ext_ctrl_b_value;

        uint8_t output_mask;

        std::vector<MIDIProgramChange> midi_pc_vector;
        std::vector<MIDIControlChange> midi_cc_vector;

    public:
        const std::string& get_title(void);
        
        uint8_t get_amp_ctrl_a_enable(void);
        uint8_t get_amp_ctrl_a_value(void);
        uint8_t get_amp_ctrl_b_enable(void);
        uint8_t get_amp_ctrl_b_value(void);
        uint8_t get_ext_ctrl_a_enable(void);
        uint8_t get_ext_ctrl_b_enable(void);
        uint8_t get_ext_ctrl_a_value(void);
        uint8_t get_ext_ctrl_b_value(void);
        uint8_t get_output_mask(void);

        std::vector<MIDIProgramChange>& get_midi_pc_vector(void);
        std::vector<MIDIControlChange>& get_midi_cc_vector(void);

        void set_title(std::string title);
        void set_amp_ctrl_a_enable(uint8_t);
        void set_amp_ctrl_a_value(uint8_t);
        void set_amp_ctrl_b_enable(uint8_t);
        void set_amp_ctrl_b_value(uint8_t);
        void set_ext_ctrl_a_enable(uint8_t);
        void set_ext_ctrl_b_enable(uint8_t);
        void set_ext_ctrl_a_value(uint8_t);
        void set_ext_ctrl_b_value(uint8_t);
        void set_output_mask(uint8_t);

};
#endif