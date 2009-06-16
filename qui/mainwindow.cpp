#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QStringList arguments;

    ui->setupUi(this);
    liqrfProcess = new QProcess(this);

    // menu connections
    connect(ui->action_Exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));

    // button connections
    connect(ui->ResetButton, SIGNAL(clicked(bool)), this, SLOT(resetModule()));
    connect(ui->EnterProgButton, SIGNAL(clicked(bool)), this, SLOT(enterProgMode()));

    // process connections
    connect(liqrfProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(processStdOutput()));
    connect(liqrfProcess, SIGNAL(readyReadStandardError()), this, SLOT(processErrOutput()));
    //connect(liqrfProcess, SIGNAL(finished(int)), this, SLOT(processFinish()));

    // run liqrf program (in QProcess)
    arguments << "-m";
    liqrfProcess->start("./liqrf", arguments);
    if (!liqrfProcess->waitForStarted()) {
        ui->UploadTextEdit->insertPlainText("Err.: Could not start liqrf.\n");
        return;
    }

    //ui->UploadTextEdit->insertPlainText("started\n");
    //if (!liqrfProcess->waitForFinished())
      //  return;

    /*
    liqrfProcess->waitForReadyRead(-1);
    QByteArray str;
    str = liqrfProcess->readAllStandardOutput();
    ui->UploadTextEdit->insertPlainText((const QString &) str);
    str = liqrfProcess->readAllStandardError();
    ui->UploadTextEdit->insertPlainText((const QString &) str);
    */
}

/*
void MainWindow::processFinish()
{
    ui->UploadTextEdit->insertPlainText("finished\n");
}
*/

void MainWindow::processStdOutput()
{
    QByteArray str;

    //ui->UploadTextEdit->insertPlainText("stdout\n");
    str = liqrfProcess->readAllStandardOutput();
    ui->UploadTextEdit->insertPlainText((const QString &) str);

}

void MainWindow::processErrOutput()
{
    QByteArray str;

    //ui->UploadTextEdit->insertPlainText("stderr\n");
    str = liqrfProcess->readAllStandardError();
    ui->UploadTextEdit->insertPlainText((const QString &) str);

}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About QTLiqrf"),
                       tr("<h2>QTLiqrf</h2>"
                          "<p>Version: xx"
                          "<p>open nandra 2009"
                          "<p><a href=\"http://open-nandra.com\">open-nandra.com</a>"));
}

void MainWindow::resetModule()
{
    //send usb command reset
}

void MainWindow::enterProgMode()
{
    //ui->UploadTextEdit->insertPlainText("Prog mode button pressed\n");

    // send command to enter prog mode

    // enable open file button
}

MainWindow::~MainWindow()
{
    liqrfProcess->close();
    delete liqrfProcess;
    delete ui;
}
