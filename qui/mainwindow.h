#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QProcess>


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
    QProcess *liqrfProcess;

private slots:
    //void processFinish();
    void processStdOutput();
    void processErrOutput();
    void about();
    void enterProgMode();
    void resetModule();
};

#endif // MAINWINDOW_H
