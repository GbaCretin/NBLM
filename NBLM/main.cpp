#include "nblm.hpp"
#include "OPNBInterface.hpp"
#include <QApplication>
#include <iostream>
#include <fstream>

const OPNBInterface::FMOperatorData instr[4] =
{
    {
        0, 1,   // dt, mul
        7,      // tl
        0, 31,  // ks, ar
        0, 9,   // am, dr
        15,     // sr
        15, 15, // sl, rr
        0       // ssg-eg
    },
    {
        0, 1,   // dt, mul
        8,      // tl
        0, 31,  // ks, ar
        0, 9,   // am, dr
        15,     // sr
        15, 15, // sl, rr
        0       // ssg-eg
    },
    {
        0, 0,   // dt, mul
        36,      // tl
        0, 31,  // ks, ar
        0, 10,   // am, dr
        15,     // sr
        15, 15, // sl, rr
        0       // ssg-eg
    },
    {
        0, 1,   // dt, mul
        17,      // tl
        0, 31,  // ks, ar
        0, 9,   // am, dr
        15,     // sr
        15, 15, // sl, rr
        0       // ssg-eg
    }
};

int main(int argc, char *argv[])
{
    const int rate = 44100;
    const uint8_t fmChannel = 3;
    int16_t* stream = new int16_t[rate*2];
    OPNBInterface opnbIntf(rate);

    opnbIntf.setFMAlgorithm(fmChannel, 4);
    opnbIntf.setFMFeedback(fmChannel, 4);
    opnbIntf.setFMPanning(fmChannel, audioDef::Panning::CENTER);
    opnbIntf.setFMPMS(fmChannel, 0);
    opnbIntf.setFMAMS(fmChannel, 0);

    for (int op = 0; op < 4; ++op)
        opnbIntf.setFMOperatorRegisters(fmChannel, op, instr[op]);

    opnbIntf.setFMFrequency(fmChannel, 309, 3);
    opnbIntf.setFMOperatorControl(fmChannel, OPNBInterface::FM_OPERATOR_ALL_MASK);

    opnbIntf.mix(stream, rate);

    // save stream to file
    std::ofstream outputFile;
    outputFile.open("stream.raw", std::ios::binary | std::ios::out);

    char tmp[2];
    for (size_t i = 0; i < rate*2; ++i)
    {
      tmp[0] = stream[i] & 0xFF;
      tmp[1] = (stream[i] >> 8) & 0xFF;

      outputFile.write(tmp, 2);
    }

    outputFile.close();
    delete[] stream;

    QApplication a(argc, argv);
    NBLM w;
    w.show();
    return a.exec();
}
