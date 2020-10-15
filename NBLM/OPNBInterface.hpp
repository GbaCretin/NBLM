#ifndef OPNBINTERFACE_HPP_INCLUDED
#define OPNBINTERFACE_HPP_INCLUDED

#include "chip/opnb.hpp"
#include "audioDef.hpp"

class OPNBInterface
{
public:
    enum SSGMix : int {
        NONE,
        TONE,
        NOISE,
        TONENOISE,
        COUNT,
    };

    static const int ADPCMA_CHANNEL_COUNT;
    static const int FM_CHANNEL_COUNT;
    static const int SSG_CHANNEL_COUNT;
    static const double BASE_SSG_PITCHES[audioDef::NOTE_COUNT];
    static const uint8_t SSG_MIX_CLEAR_MASK[SSGMix::COUNT];
    static const uint8_t SSG_MIX_SET_MASK[SSGMix::COUNT];

    OPNBInterface(int rate);

    void setDefaults();
    void stopADPCMAChannel(uint8_t channel);
    void stopFMChannel(uint8_t channel);
    void stopSSGChannel(uint8_t channel);

    void setADPCMAChannelVolume(uint8_t channel, uint8_t volume);
    void setADPCMAMasterVolume(uint8_t volume);
    void setFMVolume(uint8_t channel, uint8_t volume);
    void setSSGVolume(uint8_t channel, uint8_t volume);

    void setADPCMASample(uint8_t channel, uint16_t sampleStart, uint16_t sampleEnd);
    void setFMNote(uint8_t channel, audioDef::Note note, uint8_t octave);
    void setSSGNote(uint8_t channel, audioDef::Note note, uint8_t octave);
    void setSSGNoiseTune(uint8_t channel, uint8_t tune);
    void setSSGMix(uint8_t channel, SSGMix mix);

    inline void mix(int16_t* stream, size_t samples)
    {
        _opnb.mix(stream, samples);
    }
private:
    chip::OPNB _opnb;
    uint8_t _ssgMixEnableFlags;
};

#endif // OPNBINTERFACE_HPP_INCLUDED
