#include "nblm.h"
#include "OPNBInterface.hpp"
#include <QApplication>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
    const int rate = 44100;
    int16_t* stream = new int16_t[rate*2];
    OPNBInterface opnb(rate);

    // Silence FM channels

    opnb.setSSGNote(0, audioDef::Note::C, 2);
    opnb.setSSGNote(1, audioDef::Note::E, 2);
    opnb.setSSGNote(2, audioDef::Note::G, 2);

    opnb.setSSGMix(0, OPNBInterface::SSGMix::TONE);
    opnb.setSSGMix(1, OPNBInterface::SSGMix::TONE);
    opnb.setSSGMix(2, OPNBInterface::SSGMix::TONE);

    // Set volume
    opnb.setSSGVolume(0, 0xF);
    opnb.setSSGVolume(1, 0xF);
    opnb.setSSGVolume(2, 0xF);

    opnb.mix(stream, rate);

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
