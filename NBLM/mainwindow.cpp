#include <QApplication>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <QByteArray>
#include <QFile>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "OPNBInterface.hpp"
#include "log.hpp"

const char* PCMA_FILE_PATHS[] =
{
    "../samples/GRAND_PIANO_C3.pcma",
    "../samples/GRAND_PIANO_C#3.pcma",
    "../samples/GRAND_PIANO_D3.pcma",
    "../samples/GRAND_PIANO_D#3.pcma",
    "../samples/GRAND_PIANO_E3.pcma",
    "../samples/GRAND_PIANO_F3.pcma",
    "../samples/GRAND_PIANO_F#3.pcma",
    "../samples/GRAND_PIANO_G3.pcma",
    "../samples/GRAND_PIANO_G#3.pcma",
    "../samples/GRAND_PIANO_A3.pcma",
    "../samples/GRAND_PIANO_A#3.pcma",
    "../samples/GRAND_PIANO_B3.pcma"
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const int rate = 44100;
    size_t samples = rate*1;
    int16_t* stream = new int16_t[samples*2];
    OPNBInterface opnbIntf(rate);
    uint8_t adpcmaChannel = 5;

    // Open pcma files and write their contents into the file buffers
    std::vector<QByteArray> pcmaFileBuffers;

    uint pcmaSamplesNum = sizeof(PCMA_FILE_PATHS) / sizeof(PCMA_FILE_PATHS[0]);

    for (uint i = 0; i < pcmaSamplesNum; ++i)
    {
        QFile inFile(PCMA_FILE_PATHS[i]);

        if (!inFile.open(QFile::ReadOnly))
            stdLog::fatal("Failed to read PCMA File '%s'", PCMA_FILE_PATHS[i]);

        pcmaFileBuffers.push_back(inFile.readAll());
        inFile.close();
    }

    std::vector<OPNBInterface::ADPCMASampleAddr> adpcmaSampleAddrs =
            opnbIntf.buildAndWriteADPCMARom(pcmaFileBuffers);
    stdLog::debug("Built PCM Rom");

    opnbIntf.setADPCMAMasterVolume(63);
    opnbIntf.setADPCMAChannelVolume(adpcmaChannel, 31);
    opnbIntf.setADPCMAPanning(adpcmaChannel, audioDef::Panning::CENTER);
    opnbIntf.setADPCMASample(adpcmaChannel, adpcmaSampleAddrs[0]);
    opnbIntf.playADPCMAChannel(adpcmaChannel);
    stdLog::debug("Set OPNB Registers");

    opnbIntf.mix(stream, samples);
    stdLog::debug("Mixed OPNB stream");

    // save stream to file
    std::ofstream outputFile;
    outputFile.open("stream.raw", std::ios::binary | std::ios::out);

    char tmp[2];
    for (size_t i = 0; i < samples*2; ++i)
    {
      tmp[0] = stream[i] & 0xFF;
      tmp[1] = (stream[i] >> 8) & 0xFF;

      outputFile.write(tmp, 2);
    }

    outputFile.close();
    stdLog::debug("Saved OPNB stream");

    delete[] stream;
    stdLog::debug("Freed used memory");
}

MainWindow::~MainWindow()
{
    delete ui;
}
