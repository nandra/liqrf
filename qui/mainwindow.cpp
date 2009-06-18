#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QStringList arguments;

    ui->setupUi(this);
    parser = new hex_parser();
    usb = new lusb();
    if (usb->usb_dev_found()) {
        ui->UploadTextEdit->insertPlainText("USB device found");

    } else {
         QMessageBox::about(this, tr("USB device not found!"),
                       tr("Please connect a device and restart gui."));
    }
    // menu connections
    connect(ui->action_Exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));

    // button connections
    connect(ui->ResetButton, SIGNAL(clicked(bool)), this, SLOT(resetModule()));
    connect(ui->EnterProgButton, SIGNAL(clicked(bool)), this, SLOT(enterProgMode()));
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
    ui->OpenFileButton->setEnabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_OpenFileButton_clicked(bool checked)
{
    parser->hexfile = QFileDialog::getOpenFileName(this, tr("Open file"), "",
                                            tr("Hex file (*.hex);;All Files (*)"));
    if (parser->hexfile.isEmpty())
         return;
    else {
        if (!parser->read_file()) {
            ui->UploadTextEdit->insertPlainText("Error opening file "+parser->hexfile+'\n');
            return;
        }
        ui->UploadTextEdit->insertPlainText("Opened file "+parser->hexfile+'\n');
        ui->UploadButton->setEnabled(true);
        ui->ApplicationCheckBox->setEnabled(true);
        ui->EepromCheckBox->setEnabled(true);
     }

}
