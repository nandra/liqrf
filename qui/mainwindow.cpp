#include "mainwindow.h"
#include "ui_mainwindow.h"


QProcess liqrfProcess;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // menu connections
    connect(ui->action_Exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));

    // button connections
    connect(ui->ResetButton, SIGNAL(clicked(bool)), this, SLOT(resetModule()));
    connect(ui->EnterProgButton, SIGNAL(clicked(bool)), this, SLOT(enterProgMode()));

    // run liqrf program (in QProcess)
    liqrfProcess.start("./liqrf");
    if (!liqrfProcess.waitForStarted())
        return;

    if (!liqrfProcess.waitForFinished())
        return;

    QByteArray str;
    str = liqrfProcess.readAllStandardOutput();
    ui->UploadTextEdit->insertPlainText((const QString &) str);
    str = liqrfProcess.readAllStandardError();
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
    // send command to enter prog mode

    // enable open file button
}

MainWindow::~MainWindow()
{
    delete ui;
}
