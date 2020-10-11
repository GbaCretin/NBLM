#include "opnb.hpp"

#include <iostream>

stream_sample_t* DUMMYBUF[] = { nullptr, nullptr };
UINT8 CHIP_SAMPLING_MODE = 0x00;
INT32 CHIP_SAMPLE_RATE;

namespace chip
{
    int8_t chipCount;
    const size_t SAMPLE_BUFFER_SIZE = 0x100000;

    OPNB::OPNB()
    {
        int chipClock = 3993600 * 2;
        uint8_t AYDisable = 0;	// Enable

        _chipID = chipCount;

        ym2610_set_ay_emu_core(0);
        _internalRate[CHKIND_FMADPCM] =
            device_start_ym2610(_chipID, chipClock, AYDisable, &_internalRate[CHKIND_SSG]);

        device_reset_ym2610(_chipID);

        chipCount++;

        std::cout << "FM/ADPCM internal rate: " << _internalRate[CHKIND_FMADPCM] << std::endl;
        std::cout << "SSG internal rate: " << _internalRate[CHKIND_SSG] << std::endl;

        for (int i = 0; i <= 1; ++i)
        {
            for (int pan = 0; pan <= 1; ++pan)
            {
                _buffer[i][pan] = new stream_sample_t[SAMPLE_BUFFER_SIZE];
            }
        }
    }

    OPNB::~OPNB()
    {
        device_stop_ym2610(_chipID);

        for (int i = 0; i <= 1; ++i)
        {
            for (int pan = 0; pan <= 1; ++pan)
            {
                delete[] _buffer[i][pan];
            }
        }
    }

    void OPNB::setRegister(uint8_t offset, uint8_t data, Register reg)
    {
        switch(reg)
        {
        case Register::A:
            ym2610_control_port_a_w(_chipID, 0, offset);
            ym2610_data_port_a_w(_chipID, 1, data);
            break;

        case Register::B:
            ym2610_control_port_b_w(_chipID, 2, offset);
            ym2610_data_port_b_w(_chipID, 3, data);
            break;
        }
    }

    uint8_t OPNB::getRegister(uint8_t offset, Register reg)
    {
        switch(reg)
        {
        case Register::A:
            ym2610_control_port_a_w(_chipID, 0, offset);
            break;

        case Register::B:
            ym2610_control_port_b_w(_chipID, 2, offset);
            break;
        }

        return ym2610_read_port_r(_chipID, 1);
    }

    void OPNB::mix(int16_t* stream, size_t samples)
    {
        stream_sample_t** bufferSSG;

        ym2610_stream_update_ay(_chipID, _buffer[CHKIND_SSG], samples);
        bufferSSG = _buffer[CHKIND_SSG];

        int16_t* p = stream;
        int amplifier = 4;

        for (size_t i = 0; i < samples; ++i)
        {
            for (int channel = 0; channel <= 1; ++channel) // Loop through the LEFT and RIGHT channels
            {
                double sample = bufferSSG[channel][i] * amplifier;
                *p = static_cast<int16_t>(sample);
                p++;
            }
        }
    }
}
