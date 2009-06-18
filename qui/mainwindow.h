#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QTimer>
#include <QCheckBox>
#include "lusb.h"

namespace Ui
{
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    lusb *usb;
    QTimer *timer;

public slots:
     void on_checkBox_stateChanged(int);
private slots:

    void about();
    void enterProgMode();
    void resetModule();
    void update_spi_status();

};



#endif // MAINWINDOW_H
