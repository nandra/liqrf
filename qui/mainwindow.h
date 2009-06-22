#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QTimer>
#include <QCheckBox>

#include "lusb.h"
#include "hex_parser.h"
#include "iqrf_dev.h"
#include "programmer.h"

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
    hex_parser *parser;
    programmer *prog;
    QTimer *timer;

public slots:
     void on_checkBox_stateChanged(int);
     void deviceAdded();
private slots:
    void on_pushButton_clicked();
    void on_clear_upload_btn_clicked();
    void on_OpenFileButton_clicked(bool checked);
    void about();
    void enterProgMode();
    void resetModule();
    void update_spi_status();
    void test_signal();
signals:
    void my_signal();

};



#endif // MAINWINDOW_H
