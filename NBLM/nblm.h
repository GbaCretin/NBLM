#ifndef NBLM_H
#define NBLM_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class NBLM; }
QT_END_NAMESPACE

class NBLM : public QMainWindow
{
    Q_OBJECT

public:
    NBLM(QWidget *parent = nullptr);
    ~NBLM();

private slots:
    void on_helloWorldPushButton_clicked();

private:
    Ui::NBLM *ui;
};
#endif // NBLM_H
