#include <iostream>
#include "nblm.h"
#include "ui_nblm.h"

NBLM::NBLM(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NBLM)
{
    ui->setupUi(this);
}

NBLM::~NBLM()
{
    delete ui;
}


void NBLM::on_helloWorldPushButton_clicked()
{
    std::cout << "Hell World :)" << std::endl;
}
