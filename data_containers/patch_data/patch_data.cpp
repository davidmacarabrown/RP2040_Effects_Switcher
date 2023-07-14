#include "patch_data.h"

const std::string& PatchData::get_title(void)
{
    return title;
}

uint8_t PatchData::get_amp_ctrl_a_enable(void)
{
    return amp_ctrl_a_enable;
}

uint8_t PatchData::get_amp_ctrl_a_value(void)
{
    return amp_ctrl_a_value;
}

uint8_t PatchData::get_amp_ctrl_b_enable(void)
{
    return amp_ctrl_b_enable;
}

uint8_t PatchData::get_amp_ctrl_b_value(void)
{
    return amp_ctrl_b_value;
}

uint8_t PatchData::get_ext_ctrl_a_enable(void)
{
    return ext_ctrl_a_enable;
}

uint8_t PatchData::get_ext_ctrl_b_enable(void)
{
    return ext_ctrl_b_enable;
}

uint8_t PatchData::get_ext_ctrl_a_value(void)
{
    return ext_ctrl_a_value;
}

uint8_t PatchData::get_ext_ctrl_b_value(void)
{
    return ext_ctrl_b_value;
}

uint8_t PatchData::get_output_mask(void)
{
    return output_mask;
}

std::vector<MIDIProgramChange>& PatchData::get_midi_pc_vector(void)
{
    return midi_pc_vector;
}

std::vector<MIDIControlChange>& PatchData::get_midi_cc_vector(void)
{
    return midi_cc_vector;
}


void PatchData::set_title(std::string title)
{
    this->title = title;
}

void PatchData::set_amp_ctrl_a_enable(uint8_t value)
{
    amp_ctrl_a_enable = value;
}

void PatchData::set_amp_ctrl_a_value(uint8_t value)
{
    amp_ctrl_a_value = value;
}

void PatchData::set_amp_ctrl_b_enable(uint8_t value)
{
    amp_ctrl_b_enable = value;
}

void PatchData::set_amp_ctrl_b_value(uint8_t value)
{
    amp_ctrl_b_value = value;
}

void PatchData::set_ext_ctrl_a_enable(uint8_t value)
{
    ext_ctrl_a_enable = value;
}

void PatchData::set_ext_ctrl_b_enable(uint8_t value)
{
    ext_ctrl_b_enable = value;
}

void PatchData::set_ext_ctrl_a_value(uint8_t value)
{
    ext_ctrl_a_value = value;
}

void PatchData::set_ext_ctrl_b_value(uint8_t value)
{
    ext_ctrl_b_value = value;
}

void PatchData::set_output_mask(uint8_t value)
{
    output_mask = value;
}

