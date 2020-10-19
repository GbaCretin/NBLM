#ifndef OPNBINTERFACE_HPP_INCLUDED
#define OPNBINTERFACE_HPP_INCLUDED

#include "chip/opnb.hpp"
#include "audioDef.hpp"

class OPNBInterface
{
public:
    enum SSGMix : int
    {
        NONE,
        TONE,
        NOISE,
        TONENOISE,
        COUNT,
    };

    struct FMOperatorData
    {
        uint8_t dt;
        uint8_t mul;
        uint8_t tl;
        uint8_t ks;
        uint8_t ar;
        bool    am;
        uint8_t dr;
        uint8_t sr;
        uint8_t sl;
        uint8_t rr;
        uint8_t ssgEg;
    };

    static const int ADPCMA_CHANNEL_COUNT;
    static const int FM_CHANNEL_COUNT;
    static const int SSG_CHANNEL_COUNT;
    //static const double BASE_SSG_PITCHES[audioDef::NOTE_COUNT];
    static const uint8_t SSG_MIX_CLEAR_MASK[SSGMix::COUNT];
    static const uint8_t SSG_MIX_SET_MASK[SSGMix::COUNT];
    static const uint8_t FM_OPERATOR_1_MASK;
    static const uint8_t FM_OPERATOR_2_MASK;
    static const uint8_t FM_OPERATOR_3_MASK;
    static const uint8_t FM_OPERATOR_4_MASK;
    static const uint8_t FM_OPERATOR_ALL_MASK;

    OPNBInterface(int rate);
    void setDefaults();

    void stopADPCMAChannel(uint8_t channel);
    void setADPCMAChannelVolume(uint8_t channel, uint8_t volume);
    void setADPCMAMasterVolume(uint8_t volume);
    void setADPCMASample(uint8_t channel, uint16_t sampleStart, uint16_t sampleEnd);

    void stopFMChannel(uint8_t channel);
    void setFMFrequency(uint8_t channel, uint16_t fnumber, uint8_t block);
    void setFMFeedback(uint8_t channel, uint8_t feedback);
    void setFMAlgorithm(uint8_t channel, uint8_t algorithm);
    void setFMPanning(uint8_t channel, audioDef::Panning panning);
    void setFMAMS(uint8_t channel, uint8_t ams);
    void setFMPMS(uint8_t channel, uint8_t pms);
    void setFMOperatorRegisters(uint8_t channel, uint8_t op, const FMOperatorData& data);
    void setFMOperatorControl(uint8_t channel, uint8_t opControl);

    void stopSSGChannel(uint8_t channel);
    void setSSGVolume(uint8_t channel, uint8_t volume);
    void setSSGNoiseTune(uint8_t tune);
    void setSSGTune(uint8_t channel, uint16_t tune);
    void setSSGMix(uint8_t channel, SSGMix mix);
    //void setSSGNote(uint8_t channel, audioDef::Note note, uint8_t octave);

    inline void mix(int16_t* stream, size_t samples)
    {
        _opnb.mix(stream, samples);
    }

private:
    uint8_t _calculateFMChannelAddress(uint8_t channel, uint8_t address, chip::OPNB::Register& reg);
    uint8_t _calculateFMOperatorAddress(uint8_t channel, uint8_t op, uint8_t address, chip::OPNB::Register& reg);

    /**
     * @brief _toOPNBFMChannel Used to abstract away the inconsistent FM channel numbering.
     * @param FM channel (from 0 to 3)
     * @return internal OPNB FM channel (1, 2, 5, 6)
     */
    uint8_t _toOPNBFMChannel(uint8_t channel);

    chip::OPNB _opnb;
};

#endif // OPNBINTERFACE_HPP_INCLUDED
