#include <QApplication>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <QByteArray>
#include <QFile>
#include "mainwindow.hpp"
#include "ui_mainwindow.h"
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
const int SAMPLE_RATE = 44100;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _ui(new Ui::MainWindow)
    , _opnbInterface(SAMPLE_RATE)
{
    _ui->setupUi(this);
    const uint CHANNEL_COUNT = 2;
    const size_t SAMPLE_COUNT = SAMPLE_RATE*1; // A second long stream

    int16_t* stream = new int16_t[SAMPLE_COUNT*CHANNEL_COUNT];
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
            _opnbInterface.buildAndWriteADPCMARom(pcmaFileBuffers);
    stdLog::debug("Built PCM Rom");

    _opnbInterface.setADPCMAMasterVolume(63);
    _opnbInterface.setADPCMAChannelVolume(adpcmaChannel, 31);
    _opnbInterface.setADPCMAPanning(adpcmaChannel, audioDef::Panning::CENTER);
    _opnbInterface.setADPCMASample(adpcmaChannel, adpcmaSampleAddrs[0]);
    _opnbInterface.playADPCMAChannel(adpcmaChannel);
    stdLog::debug("Set OPNB Registers");

    _opnbInterface.mix(stream, SAMPLE_COUNT);
    stdLog::debug("Mixed OPNB stream");

    // save stream to file
    std::ofstream outputFile;
    outputFile.open("stream.raw", std::ios::binary | std::ios::out);

    char tmp[2];
    for (size_t i = 0; i < SAMPLE_COUNT*CHANNEL_COUNT; ++i)
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
    delete _ui;
}

void MainWindow::on_pushButton_clicked()
{
    stdLog::info("Push Button Clicked!");
}
