#include <iostream>
#include "nblm.hpp"
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
