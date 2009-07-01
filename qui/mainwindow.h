#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QTimer>
#include <QCheckBox>
#include <QThread>

//#include "lusb.h"
//#include "hex_parser.h"
//#include "iqrf_dev.h"
#include "programmer.h"

enum mcu_type {
    MCU_16F88 = 1,
    MCU_16F886,
};

namespace Ui
{
    class MainWindow;
}

class Thread : public QThread
{
    Q_OBJECT

    public:
        void run(QString filename);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    enum mcu_type mcu;

private:
    Ui::MainWindow *ui;
    hex_parser *parser;
    programmer *prog;
    QTimer *timer;
    Thread *editor_thread;
    QString opened_file;

public slots:
     void on_checkBox_stateChanged(int);
     void deviceAdded();
private slots:
    void on_UploadButton_clicked();
    void on_EditFileButton_clicked(bool checked);
    void on_CompileButton_clicked(bool checked);
    void on_pushButton_clicked();
    void on_clear_upload_btn_clicked();
    void on_OpenFileButton_clicked(bool checked);
    void about();
    void enterProgMode();
    void resetModule();
    void update_spi_status();
    void test_signal();
    void mcu_16f88();
    void mcu_16f886();
signals:
    void my_signal();

};


#endif // MAINWINDOW_H
