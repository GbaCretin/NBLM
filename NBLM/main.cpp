#include "nblm.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NBLM w;
    w.show();
    return a.exec();
}
