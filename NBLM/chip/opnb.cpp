#include "opnb.hpp"
#include "chip/chip_misc.hpp"
#include "math.hpp"

stream_sample_t* DUMMYBUF[] = { nullptr, nullptr };
UINT8 CHIP_SAMPLING_MODE = 0x00;
INT32 CHIP_SAMPLE_RATE;

namespace chip
{
    int8_t chipCount;
    const size_t INTERNAL_BUFFER_SIZE = 0x80000;
    const size_t RESAMPLER_MAX_DURATION = 0x8000;

    const int OPNB::REG_FM_LFO_CNT        = 0x22;
    const int OPNB::REG_FM_KEY_ON         = 0x28;
    const int OPNB::REG_FM_CH_FNUM        = 0xA1;
    const int OPNB::REG_FM_CH_FBLOCK      = 0xA5;
    const int OPNB::REG_FM_CH_FBALGO      = 0xB1;
    const int OPNB::REG_FM_CH_LRAMSPMS    = 0xB5;
    const int OPNB::REG_FM_OP_DTMUL       = 0x31;
    const int OPNB::REG_FM_OP_TVOL        = 0x41;
    const int OPNB::REG_FM_OP_KSAR        = 0x51;
    const int OPNB::REG_FM_OP_AMDR        = 0x61;
    const int OPNB::REG_FM_SUSR           = 0x71;
    const int OPNB::REG_FM_SLRR           = 0x81;
    const int OPNB::REG_FM_ENVGN          = 0x91;

    const int OPNB::REG_SSG_FINE_TUNE     = 0x00;
    const int OPNB::REG_SSG_COARSE_TUNE   = 0x01;
    const int OPNB::REG_SSG_NOISE_TUNE    = 0x06;
    const int OPNB::REG_SSG_MIX_ENABLE    = 0x07;
    const int OPNB::REG_SSG_VOL           = 0x08;
    const int OPNB::REG_SSG_VOL_ENV       = 0x0B;
    const int OPNB::REG_SSG_COARSE_ENV    = 0x0C;
    const int OPNB::REG_SSG_VOL_ENV_SHAPE = 0x0D;

    const int OPNB::REG_PA_CTRL   = 0x00;
    const int OPNB::REG_PA_MVOL   = 0x01;
    const int OPNB::REG_PA_CVOL   = 0x08;
    const int OPNB::REG_PA_STARTL = 0x10;
    const int OPNB::REG_PA_STARTH = 0x18;
    const int OPNB::REG_PA_ENDL   = 0x20;
    const int OPNB::REG_PA_ENDH   = 0x28;

    OPNB::OPNB(int rate)
        : masterAmplifier(1)
    {
        int chipClock = 3993600 * 2;
        uint8_t AYDisable = 0;	// Enable

        _chipID = chipCount;
        _destRate = rate;

        ym2610_set_ay_emu_core(0);
        _internalRate[ADSOURCE_FMADPCM] =
            device_start_ym2610(_chipID, chipClock, AYDisable, &_internalRate[ADSOURCE_SSG]);

        device_reset_ym2610(_chipID);

        chipCount++;

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
        stream_sample_t** buffers[ADSOURCE_COUNT];

        if (_internalRate[ADSOURCE_SSG] == _destRate)
        {
            ym2610_stream_update_ay(_chipID, _internalBuffers[ADSOURCE_SSG], samples);
            buffers[ADSOURCE_SSG] = _internalBuffers[ADSOURCE_SSG];
        }
        else
        {
            size_t intrSize = _resamplers[ADSOURCE_SSG]->calculateInternalSampleSize(samples);
            ym2610_stream_update_ay(_chipID, _internalBuffers[ADSOURCE_SSG], intrSize);
            buffers[ADSOURCE_SSG] = _resamplers[ADSOURCE_SSG]->interpolate(_internalBuffers[ADSOURCE_SSG], samples, intrSize);
        }

        if (_internalRate[ADSOURCE_FMADPCM] == _destRate)
        {
            ym2610_stream_update(_chipID, _internalBuffers[ADSOURCE_FMADPCM], samples);
            buffers[ADSOURCE_FMADPCM] = _internalBuffers[ADSOURCE_FMADPCM];
        }
        else
        {
            size_t intrSize = _resamplers[ADSOURCE_FMADPCM]->calculateInternalSampleSize(samples);
            ym2610_stream_update(_chipID, _internalBuffers[ADSOURCE_FMADPCM], intrSize);
            buffers[ADSOURCE_FMADPCM] =
                _resamplers[ADSOURCE_FMADPCM]->interpolate(_internalBuffers[ADSOURCE_FMADPCM], samples, intrSize);
        }

        int16_t* p = stream;

        for (size_t i = 0; i < samples; ++i)
        {
            for (int channel = 0; channel <= 1; ++channel) // Loop through the LEFT and RIGHT channels
            {
                double sample = buffers[ADSOURCE_SSG][channel][i] + buffers[ADSOURCE_FMADPCM][channel][i];
                *p = static_cast<int16_t>(math::clamp(sample * masterAmplifier, -32768.0, 32767.0));
                p++;
            }
        }
    }

    void OPNB::writeRomADPCM(size_t romSize, offs_t dataStart, offs_t dataLength, const uint8_t* data)
    {
        const uint8_t ROM_ID_ADPCM = 0x01;

        ym2610_write_data_pcmrom(_chipID, ROM_ID_ADPCM, romSize, dataStart, dataLength, data);
    }
}
