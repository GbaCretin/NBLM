#ifndef OPNB_HPP_INCLUDED
#define OPNB_HPP_INCLUDED

#include <cstdint>
#include <cstring> // for size_t
#include "resampler.hpp"

extern "C" {
    #include "mame/mamedef.h"
    #include "mame/2610intf.h"
}

namespace chip
{
    class OPNB
    {
    public:
        int masterAmplifier = 4;

        enum AudioSource : int {
            ADSOURCE_FMADPCM,
            ADSOURCE_SSG,
            ADSOURCE_COUNT,
        };

        enum Register : int {
            A,
            B,
        };

        OPNB(int rate);
        ~OPNB();

        void setRegister(uint8_t offset, uint8_t data, Register reg);
        uint8_t getRegister(uint8_t offset, Register reg);
        void mix(int16_t* stream, size_t samples);

    private:
        int _internalRate[ADSOURCE_COUNT];
        int8_t _chipID;
        int _destRate;
        stream_sample_t* _internalBuffers[ADSOURCE_COUNT][2];
        LinearResampler* _resamplers[ADSOURCE_COUNT];
    };
}

#endif // OPNB_HPP_INCLUDED
