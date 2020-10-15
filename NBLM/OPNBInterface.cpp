#include "OPNBInterface.hpp"
#include <cassert>

const int OPNBInterface::ADPCMA_CHANNEL_COUNT = 6;
const int OPNBInterface::FM_CHANNEL_COUNT     = 4;
const int OPNBInterface::SSG_CHANNEL_COUNT    = 3;
const double OPNBInterface::BASE_SSG_PITCHES[audioDef::NOTE_COUNT] =
{
    65.41, 69.30, 73.42, 77.78, 82.41, 87.31,
    92.50, 98.00, 103.83, 110.0, 116.54, 123.47
};

OPNBInterface::OPNBInterface(int rate)
  : _opnb(rate), _ssgMixEnableFlags(0xFF)
{
  setDefaults();
}

void OPNBInterface::setDefaults()
{
    for (int i = 0; i < OPNBInterface::ADPCMA_CHANNEL_COUNT; ++i)
      stopADPCMAChannel(i);

    /*for (int i = 0; i < OPNBInterface::FM_CHANNEL_COUNT; ++i)
      stopFMChannel(i);*/

    for (int i = 0; i < OPNBInterface::SSG_CHANNEL_COUNT; ++i)
      stopSSGChannel(i);

    // Silence FM Channels (PLACEHOLDER)
    _opnb.setRegister(0x28, 0x01, chip::OPNB::Register::A);
    _opnb.setRegister(0x28, 0x02, chip::OPNB::Register::A);
    _opnb.setRegister(0x28, 0x05, chip::OPNB::Register::A);
    _opnb.setRegister(0x28, 0x06, chip::OPNB::Register::A);
}

void OPNBInterface::stopADPCMAChannel(uint8_t channel)
{
    // TODO
}

void OPNBInterface::stopFMChannel(uint8_t channel)
{
    // TODO
}

void OPNBInterface::stopSSGChannel(uint8_t channel)
{
    assert(channel < 3);

    _ssgMixEnableFlags |= 0b1001 << channel;
    _opnb.setRegister(chip::OPNB::REG_SSG_VOL_ENV+channel, 0x00, chip::OPNB::Register::A);
    _opnb.setRegister(chip::OPNB::REG_SSG_MIX_ENABLE, _ssgMixEnableFlags, chip::OPNB::Register::A);
}

void OPNBInterface::setSSGVolume(uint8_t channel, uint8_t volume)
{
    assert(channel < 3);
    assert(volume < 0x10);

    _opnb.setRegister(chip::OPNB::REG_SSG_VOL+channel, volume & 0x0F, chip::OPNB::Register::A);
}

void OPNBInterface::setSSGNote(uint8_t channel, audioDef::Note note, uint8_t octave)
{
    assert(channel < 3);
    assert(octave >= 2 && octave < 8);
    assert(note != audioDef::NOTE_COUNT);

    uint16_t ssgPitch = static_cast<uint16_t>(
                std::round(250000 / (BASE_SSG_PITCHES[note] * std::pow(2, octave-2))));

    _opnb.setRegister(chip::OPNB::REG_SSG_FINE_TUNE+channel*2, ssgPitch & 0xFF, chip::OPNB::Register::A);
    _opnb.setRegister(chip::OPNB::REG_SSG_COARSE_TUNE+channel*2, ssgPitch >> 8, chip::OPNB::Register::A);
}

void OPNBInterface::setSSGMix(uint8_t channel, SSGMix mix)
{
    assert(channel < 3);

    _ssgMixEnableFlags &= ~(mix << channel);
    _opnb.setRegister(chip::OPNB::REG_SSG_MIX_ENABLE, _ssgMixEnableFlags, chip::OPNB::Register::A);
}
