#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>

#include "lusb.h"
#include "hex_parser.h"

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
    hex_parser *parser;

private slots:
    void on_OpenFileButton_clicked(bool checked);
    void about();
    void enterProgMode();
    void resetModule();
};



#endif // MAINWINDOW_H
