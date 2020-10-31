#ifndef NBLM_H
#define NBLM_H

#include <QMainWindow>
#include "OPNBInterface.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *_ui;
    OPNBInterface _opnbInterface;
};
#endif // NBLM_H
