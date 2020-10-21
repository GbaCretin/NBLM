#include "OPNBInterface.hpp"
#include <cassert>

/************************************************ OPNB ************************************************/
OPNBInterface::OPNBInterface(int rate)
  : _opnb(rate)
{
    for (int fmCh = 0; fmCh < 4; ++fmCh)
    {
        _fmFbalgoRegisters[fmCh]   = 0x00;
        _fmFnumRegisters[fmCh]     = 0x00;
        _fmFblockRegisters[fmCh]   = 0x00;
        _fmLramspmsRegisters[fmCh] = 0x00;
    }

    setDefaults();
}

void OPNBInterface::setDefaults()
{
    for (int i = 0; i < OPNBInterface::ADPCMA_CHANNEL_COUNT; ++i)
      stopADPCMAChannel(i);

    for (int i = 0; i < OPNBInterface::FM_CHANNEL_COUNT; ++i)
      stopFMChannel(i);

    for (int i = 0; i < OPNBInterface::SSG_CHANNEL_COUNT; ++i)
      stopSSGChannel(i);

    // Silence FM Channels (PLACEHOLDER)
    _opnb.setRegister(0x28, 0x01, chip::OPNB::Register::A);
    _opnb.setRegister(0x28, 0x02, chip::OPNB::Register::A);
    _opnb.setRegister(0x28, 0x05, chip::OPNB::Register::A);
    _opnb.setRegister(0x28, 0x06, chip::OPNB::Register::A);
}

/************************************************ ADPCMA ************************************************/
const int OPNBInterface::ADPCMA_CHANNEL_COUNT = 6;

void OPNBInterface::stopADPCMAChannel(uint8_t channel)
{
    // TODO
}

/************************************************ FM ************************************************/
const int OPNBInterface::FM_CHANNEL_COUNT         = 4;
const uint8_t OPNBInterface::FM_OPERATOR_1_MASK   = 0b00010000;
const uint8_t OPNBInterface::FM_OPERATOR_2_MASK   = 0b00100000;
const uint8_t OPNBInterface::FM_OPERATOR_3_MASK   = 0b01000000;
const uint8_t OPNBInterface::FM_OPERATOR_4_MASK   = 0b10000000;
const uint8_t OPNBInterface::FM_OPERATOR_ALL_MASK = 0b11110000;

void OPNBInterface::stopFMChannel(uint8_t channel)
{
    const FMOperatorData emptyOpData{};

    assert(channel < 4);
    setFMOperatorControl(channel, 0);

    for (int op = 0; op < 4; ++op)
        setFMOperatorRegisters(channel, op, emptyOpData);
}

void OPNBInterface::setFMFrequency(uint8_t channel, uint16_t fnumber, uint8_t block)
{
    chip::OPNB::Register reg;

    assert(channel < 4);
    assert(fnumber < 0x800);
    assert(block < 8);

    uint8_t addr = _calculateFMChannelAddress(channel, chip::OPNB::REG_FM_CH_FBLOCK, reg);

    _opnb.setRegister(addr, (fnumber>>8) | (block<<3), reg);
    addr -= 4; // REG_FM_CH_FBLOCK -> REG_FM_CH_FNUM
    _opnb.setRegister(addr, fnumber & 0xFF, reg);
}

void OPNBInterface::setFMFeedback(uint8_t channel, uint8_t feedback)
{
    const uint8_t FB_MASK = 0b00111000;
    chip::OPNB::Register reg;

    assert(channel < 4);
    assert(feedback < 8);

    uint8_t addr = _calculateFMChannelAddress(channel, chip::OPNB::REG_FM_CH_FBALGO, reg);
    _fmFbalgoRegisters[channel] &= ~FB_MASK;
    _fmFbalgoRegisters[channel] |= feedback<<3;
    _opnb.setRegister(addr, _fmFbalgoRegisters[channel], reg);
}

void OPNBInterface::setFMAlgorithm(uint8_t channel, uint8_t algorithm)
{
    const uint8_t ALGO_MASK = 0b00000111;
    chip::OPNB::Register reg;

    assert(channel < 4);
    assert(algorithm < 8);

    uint8_t addr = _calculateFMChannelAddress(channel, chip::OPNB::REG_FM_CH_FBALGO, reg);
    _fmFbalgoRegisters[channel] &= ~ALGO_MASK;
    _fmFbalgoRegisters[channel] |= algorithm;
    _opnb.setRegister(addr, _fmFbalgoRegisters[channel], reg);
}

void OPNBInterface::setFMPanning(uint8_t channel, audioDef::Panning panning)
{
    const uint8_t PAN_MASK = 0b11000000;
    chip::OPNB::Register reg;

    assert(channel < 4);

    uint8_t addr = _calculateFMChannelAddress(channel, chip::OPNB::REG_FM_CH_LRAMSPMS, reg);
    _fmLramspmsRegisters[channel] &= ~PAN_MASK;
    _fmLramspmsRegisters[channel] |= panning<<6;
    _opnb.setRegister(addr, _fmLramspmsRegisters[channel], reg);
}

void OPNBInterface::setFMAMS(uint8_t channel, uint8_t ams)
{
    const uint8_t AMS_MASK = 0b00110000;
    chip::OPNB::Register reg;

    assert(channel < 4);
    assert(ams < 4);

    uint8_t addr = _calculateFMChannelAddress(channel, chip::OPNB::REG_FM_CH_LRAMSPMS, reg);
    _fmLramspmsRegisters[channel] &= ~AMS_MASK;
    _fmLramspmsRegisters[channel] |= ams<<4;
    _opnb.setRegister(addr, _fmLramspmsRegisters[channel], reg);
}

void OPNBInterface::setFMPMS(uint8_t channel, uint8_t pms)
{
    const uint8_t PMS_MASK = 0b00000111;
    chip::OPNB::Register reg;

    assert(channel < 4);
    assert(pms < 8);

    uint8_t addr = _calculateFMChannelAddress(channel, chip::OPNB::REG_FM_CH_LRAMSPMS, reg);
    _fmLramspmsRegisters[channel] &= ~PMS_MASK;
    _fmLramspmsRegisters[channel] |= pms;
    _opnb.setRegister(addr, _fmLramspmsRegisters[channel], reg);
}

void OPNBInterface::setFMOperatorRegisters(uint8_t channel, uint8_t op, const FMOperatorData &data)
{
    chip::OPNB::Register reg;

    assert(channel < 4);
    assert(op < 4);
    assert(data.dt < 8);
    assert(data.mul < 16);
    assert(data.tl < 128);
    assert(data.ks < 4);
    assert(data.ar < 32);
    assert(data.dr < 32);
    assert(data.sr < 32);
    assert(data.sl < 16);
    assert(data.rr < 16);
    assert(data.ssgEg < 16);

    uint8_t addr = _calculateFMOperatorAddress(channel, op, chip::OPNB::REG_FM_OP_DTMUL, reg);
    _opnb.setRegister(addr, data.mul | (data.dt<<4), reg);
    addr += 0x10;
    _opnb.setRegister(addr, data.tl, reg);
    addr += 0x10;
    _opnb.setRegister(addr, data.ar | (data.ks<<6), reg);
    addr += 0x10;
    _opnb.setRegister(addr, data.dr | (data.am<<7), reg);
    addr += 0x10;
    _opnb.setRegister(addr, data.sr, reg);
    addr += 0x10;
    _opnb.setRegister(addr, data.rr | (data.sl<<4), reg);
    addr += 0x10;
    _opnb.setRegister(addr, data.ssgEg, reg);
}

void OPNBInterface::setFMOperatorControl(uint8_t channel, uint8_t opControl)
{
    assert(channel < 4);
    assert(opControl <= FM_OPERATOR_ALL_MASK);

    uint8_t internalChannel = _toOPNBFMChannel(channel);

    _opnb.setRegister(chip::OPNB::REG_FM_KEY_ON, internalChannel | opControl, chip::OPNB::Register::A);
}

uint8_t OPNBInterface::_calculateFMChannelAddress(uint8_t channel, uint8_t address, chip::OPNB::Register& reg)
{
    assert(channel < 4);

    if (channel % 2 != 0) address++;
    if (channel < 2) reg = chip::OPNB::Register::A;
    else             reg = chip::OPNB::Register::B;

    return address;
}

uint8_t OPNBInterface::_calculateFMOperatorAddress(uint8_t channel, uint8_t op, uint8_t address, chip::OPNB::Register& reg)
{
    const uint8_t OP_ADDRESS_OFFSETS[] =
    {
        0, 8, 4, 12
    };

    assert(channel < 4);
    assert(op < 4);

    address += OP_ADDRESS_OFFSETS[op];

    if (channel % 2 != 0) address++;
    if (channel < 2) reg = chip::OPNB::Register::A;
    else             reg = chip::OPNB::Register::B;

    return address;
}

uint8_t OPNBInterface::_toOPNBFMChannel(uint8_t channel)
{
    static const int INTERNAL_FM_CHANNELS[] =
    {
        1, 2, 5, 6
    };

    assert (channel < 4);
    return INTERNAL_FM_CHANNELS[channel];
}

/************************************************ SSG ************************************************/
const int OPNBInterface::SSG_CHANNEL_COUNT    = 3;
/*const double OPNBInterface::BASE_SSG_PITCHES[audioDef::NOTE_COUNT] =
{
    65.41, 69.30, 73.42, 77.78, 82.41, 87.31,
    92.50, 98.00, 103.83, 110.0, 116.54, 123.47
};*/
const uint8_t OPNBInterface::SSG_MIX_CLEAR_MASK[SSGMix::COUNT] =
{
    0b00000000, 0b00000001, 0b00001000, 0b00001001
};
const uint8_t OPNBInterface::SSG_MIX_SET_MASK[SSGMix::COUNT] =
{
    0b00001001, 0b00001000, 0b00000001, 0b00000000
};

void OPNBInterface::stopSSGChannel(uint8_t channel)
{
    assert(channel < 3);

    setSSGMix(channel, SSGMix::NONE);
}

void OPNBInterface::setSSGVolume(uint8_t channel, uint8_t volume)
{
    assert(channel < 3);
    assert(volume < 0x10);

    _opnb.setRegister(chip::OPNB::REG_SSG_VOL+channel, volume & 0x0F, chip::OPNB::Register::A);
}

/*void OPNBInterface::setSSGNote(uint8_t channel, audioDef::Note note, uint8_t octave)
{
    assert(channel < 3);
    assert(octave >= 2 && octave < 8);
    assert(note != audioDef::NOTE_COUNT);

    uint16_t ssgPitch = static_cast<uint16_t>(
                std::round(250000 / (BASE_SSG_PITCHES[note] * std::pow(2, octave-2))));

    _opnb.setRegister(chip::OPNB::REG_SSG_FINE_TUNE+channel*2, ssgPitch & 0xFF, chip::OPNB::Register::A);
    _opnb.setRegister(chip::OPNB::REG_SSG_COARSE_TUNE+channel*2, ssgPitch >> 8, chip::OPNB::Register::A);
}*/

void OPNBInterface::setSSGTune(uint8_t channel, uint16_t tune)
{
    assert(channel < 3);
    assert(tune < 0x1000);

    _opnb.setRegister(chip::OPNB::REG_SSG_FINE_TUNE+channel*2, tune & 0xFF, chip::OPNB::Register::A);
    _opnb.setRegister(chip::OPNB::REG_SSG_COARSE_TUNE+channel*2, tune >> 8, chip::OPNB::Register::A);
}

void OPNBInterface::setSSGNoiseTune(uint8_t tune)
{
    assert(tune < 0x20);

    _opnb.setRegister(chip::OPNB::REG_SSG_NOISE_TUNE, tune, chip::OPNB::Register::A);
}

void OPNBInterface::setSSGMix(uint8_t channel, SSGMix mix)
{
    assert(channel < 3);

    uint8_t ssgMixEnableFlags = _opnb.getRegister(chip::OPNB::REG_SSG_MIX_ENABLE, chip::OPNB::Register::A);
    ssgMixEnableFlags &= ~(SSG_MIX_CLEAR_MASK[mix] << channel);
    ssgMixEnableFlags |= SSG_MIX_SET_MASK[mix] << channel;
    _opnb.setRegister(chip::OPNB::REG_SSG_MIX_ENABLE, ssgMixEnableFlags, chip::OPNB::Register::A);
}
