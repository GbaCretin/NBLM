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

    opnb.setSSGNoiseTune(1, 0x10);
    opnb.setSSGMix(1, OPNBInterface::SSGMix::NOISE);
    opnb.setSSGVolume(1, 0xF);

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
