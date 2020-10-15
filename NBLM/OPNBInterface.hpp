#ifndef OPNBINTERFACE_HPP_INCLUDED
#define OPNBINTERFACE_HPP_INCLUDED

#include "chip/opnb.hpp"
#include "audioDef.hpp"

class OPNBInterface
{
public:
    enum SSGMix : int {
        NONE      = 0b0000,
        TONE      = 0b0001,
        NOISE     = 0b1000,
        TONENOISE = 0b1001,
    };

    static const int ADPCMA_CHANNEL_COUNT;
    static const int FM_CHANNEL_COUNT;
    static const int SSG_CHANNEL_COUNT;
    static const double BASE_SSG_PITCHES[audioDef::NOTE_COUNT];

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
