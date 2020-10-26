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
        static const int REG_FM_LFO_CNT;
        static const int REG_FM_KEY_ON;
        static const int REG_FM_CH_FNUM;
        static const int REG_FM_CH_FBLOCK;
        static const int REG_FM_CH_FBALGO;
        static const int REG_FM_CH_LRAMSPMS;
        static const int REG_FM_OP_DTMUL;
        static const int REG_FM_OP_TVOL;
        static const int REG_FM_OP_KSAR;
        static const int REG_FM_OP_AMDR;
        static const int REG_FM_SUSR;
        static const int REG_FM_SLRR;
        static const int REG_FM_ENVGN;

        static const int REG_SSG_FINE_TUNE;
        static const int REG_SSG_COARSE_TUNE;
        static const int REG_SSG_NOISE_TUNE;
        static const int REG_SSG_MIX_ENABLE;
        static const int REG_SSG_VOL;
        static const int REG_SSG_VOL_ENV;
        static const int REG_SSG_COARSE_ENV;
        static const int REG_SSG_VOL_ENV_SHAPE;

        static const int REG_PA_CTRL;
        static const int REG_PA_MVOL;
        static const int REG_PA_CVOL;
        static const int REG_PA_STARTL;
        static const int REG_PA_STARTH;
        static const int REG_PA_ENDL;
        static const int REG_PA_ENDH;

        double masterAmplifier;

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
        void writeRomADPCM(size_t romSize, offs_t dataStart, offs_t dataLength, const uint8_t* data);

    private:
        int _internalRate[ADSOURCE_COUNT];
        int8_t _chipID;
        int _destRate;
        stream_sample_t* _internalBuffers[ADSOURCE_COUNT][2];
        LinearResampler* _resamplers[ADSOURCE_COUNT];
    };
}

#endif // OPNB_HPP_INCLUDED
