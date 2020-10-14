#include "opnb.hpp"

#include <iostream>

stream_sample_t* DUMMYBUF[] = { nullptr, nullptr };
UINT8 CHIP_SAMPLING_MODE = 0x00;
INT32 CHIP_SAMPLE_RATE;

namespace chip
{
    int8_t chipCount;
    const size_t INTERNAL_BUFFER_SIZE = 0x80000;
    const size_t RESAMPLER_MAX_DURATION = 0x8000;
    OPNB::OPNB(int rate)
    {
        int chipClock = 3993600 * 2;
        uint8_t AYDisable = 0;	// Enable

        _chipID = chipCount;
        _destRate = rate;

        ym2610_set_ay_emu_core(0);
        _internalRate[CHKIND_FMADPCM] =
            device_start_ym2610(_chipID, chipClock, AYDisable, &_internalRate[CHKIND_SSG]);

        device_reset_ym2610(_chipID);

        chipCount++;

        std::cout << "FM/ADPCM internal rate: " << _internalRate[CHKIND_FMADPCM] << std::endl;
        std::cout << "SSG internal rate: " << _internalRate[CHKIND_SSG] << std::endl;

        for (int snd = 0; snd <= 1; ++snd)
        {
            for (int pan = 0; pan <= 1; ++pan)
            {
                _internalBuffers[snd][pan] = new stream_sample_t[INTERNAL_BUFFER_SIZE];
            }

            _resamplers[snd] = new LinearResampler();
            _resamplers[snd]->init(_internalRate[snd], _destRate, RESAMPLER_MAX_DURATION);
        }
    }

    OPNB::~OPNB()
    {
        device_stop_ym2610(_chipID);

        for (int snd = 0; snd <= 1; ++snd)
        {
            for (int pan = 0; pan <= 1; ++pan)
            {
                delete[] _internalBuffers[snd][pan];
            }

            delete _resamplers[snd];
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

        if (_internalRate[CHKIND_SSG] == _destRate)
        {
            ym2610_stream_update_ay(_chipID, _internalBuffers[CHKIND_SSG], samples);
            bufferSSG = _internalBuffers[CHKIND_SSG];
        }
        else
        {
            size_t intrSize = _resamplers[CHKIND_SSG]->calculateInternalSampleSize(samples);
            ym2610_stream_update_ay(_chipID, _internalBuffers[CHKIND_SSG], intrSize);
            bufferSSG = _resamplers[CHKIND_SSG]->interpolate(_internalBuffers[CHKIND_SSG], samples, intrSize);
        }

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
