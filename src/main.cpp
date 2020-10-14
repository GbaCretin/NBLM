#include <fstream>
#include <iostream>
#include "chip/opnb.hpp"

int main(int argc, char* argv[])
{
    const int rate = 44100;
    int16_t* stream = new int16_t[rate*2];
    chip::OPNB opnb(rate);

    // Silence FM channels
    opnb.setRegister(0x28, 0x01, chip::OPNB::Register::A);
    opnb.setRegister(0x28, 0x02, chip::OPNB::Register::A);
    opnb.setRegister(0x28, 0x05, chip::OPNB::Register::A);
    opnb.setRegister(0x28, 0x06, chip::OPNB::Register::A);

    // Set SSG A pitch to A4
    opnb.setRegister(0x00, 0x38, chip::OPNB::Register::A);
    opnb.setRegister(0x01, 0x02, chip::OPNB::Register::A);

    // Enable SSG A tone channel
    opnb.setRegister(0x07, 0xFE, chip::OPNB::Register::A);

    // Set volume
    opnb.setRegister(0x08, 0x0F, chip::OPNB::Register::A);

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

	return 0;
}
