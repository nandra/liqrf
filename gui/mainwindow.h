#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QTimer>
#include <QCheckBox>
#include <QThread>

#include "programmer.h"
#include "compile_dialog.h"
#include "setup_dialog.h"

namespace Ui
{
    class MainWindow;
}

// supported file types
typedef enum {
    HEX,
    C
}file_types;

class Thread : public QThread
{
    Q_OBJECT

    public:
        void run(QString filename);
        QString editor_name;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    enum mcu_type mcu;
    setup_dialog *setup_win;

private:
    Ui::MainWindow *ui;
    file_types file_type;
    compile_dialog *window;
    hex_parser *parser;
    programmer *prog;
    QTimer *timer;
    Thread *editor_thread;
    QString opened_file;
    QString directory;      // last opened directory

    void writeSettings();
    void readSettings();
    void debug(QString str);
public slots:
    void on_checkBox_stateChanged(int);

private slots:
    void on_spi_m2_clicked();
    void on_spi_m1_clicked();
    void on_btn_add_00_clicked();
    void on_btn_crcm_clicked();
    void on_send_spi_data_clicked();
    void on_btn_teminal_spi_send_clicked();
    void on_UploadButton_clicked();
    void on_EditFileButton_clicked();
    void on_CompileButton_clicked();
    void on_pushButton_clicked();
    void on_clear_upload_btn_clicked();
    void on_OpenFileButton_clicked();
    void about();
    void enterProgMode();
    void resetModule();
    void update_spi_status();
    void test_signal();
    void mcu_16f88();
    void mcu_16f886();
    void deviceAdded(const QString &udi);
    void deviceRemoved(const QString &udi);
    void toolsSetting();

signals:
    void my_signal();

};


#endif // MAINWINDOW_H
