#include <fstream>
#include <iostream>
#include "opnb.hpp"

int main(int argc, char* argv[])
{
    chip::OPNB opnb;
    int16_t* stream = new int16_t[249600*2];

    // Set SSG A pitch to A4
    opnb.setRegister(0x00, 0x38, chip::OPNB::Register::A);
    opnb.setRegister(0x01, 0x02, chip::OPNB::Register::A);

    // Enable SSG A tone channel
    opnb.setRegister(0x07, 0xFE, chip::OPNB::Register::A);

    // Set volume
    opnb.setRegister(0x08, 0x0F, chip::OPNB::Register::A);

    opnb.mix(stream, 249600);

    // save stream to file
    std::ofstream outputFile;
    outputFile.open("stream.raw", std::ios::binary | std::ios::out);

    char tmp[2];
    for (size_t i = 0; i < 249600*2; ++i)
    {
        tmp[0] = stream[i] & 0xFF;
        tmp[1] = (stream[i] >> 8) & 0xFF;

        outputFile.write(tmp, 2);
    }

    outputFile.close();

	return 0;
}
