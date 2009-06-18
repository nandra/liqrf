#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QStringList arguments;

    ui->setupUi(this);

    parser = new hex_parser();

    // usb setup
    usb = new lusb();

    usb->init_usb();

    if (usb->usb_dev_found()) {
        ui->UploadTextEdit->insertPlainText("USB device found\n");

    } else {
         QMessageBox::about(this, tr("USB device not found!"),
                       tr("Please connect a device and press OK."));

    }

    if (usb->open_usb()) {
        QMessageBox::about(this, tr("USB device could't be opened!"),
                       tr("Please check USB connection!"));
    } else {
        ui->UploadTextEdit->insertPlainText("USB device opened\n");
    }

    // create timer for checking SPI status
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_spi_status()));

    /* start checking if device is presented */
    if (ui->checkBox->isChecked()) {
        if (usb->usb_dev_found()) {
            timer->start(1000);
        } else {
            ui->label_3->setText("No USB device found");
            /* disable enter prog button */
            ui->EnterProgButton->setEnabled(false);
        }

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
    /* maybee needs to be extended for other SPI statuses */
    if ((usb->status != NO_MODULE_ON_USB) && (usb->status != SPI_DISABLED)) {
        // send command to enter prog mode

        // enable open file button
        ui->OpenFileButton->setEnabled(true);
    }


}

MainWindow::~MainWindow()
{
    /* close usb */
    delete parser;
    delete usb;
    delete ui;
}

void MainWindow::update_spi_status()
{
    /* just for test purposes */
    unsigned char buff[64] = {1,0,0,0,0};
    int len = 0;

    usb->set_tx_len(5);
    usb->set_rx_len(4);
    usb->write_tx_buff(buff, 5);
    usb->send_receive_packet();
    len = usb->read_rx_buff(buff);

    switch(buff[1]) {
    case 0x00:
         ui->label_3->setText("SPI not working");
         usb->status = SPI_DISABLED;
         break;
    case 0x07:
         ui->label_3->setText("SPI suspended");
         usb->status = SPI_USER_STOP;
         break;
    case 0x3F:
         ui->label_3->setText("SPI not ready last CRCM O.K.)");
         usb->status = SPI_CRCM_OK;
         break;
    case 0x3E:
         ui->label_3->setText("SPI not ready last CRCM error)");
         usb->status = SPI_CRCM_ERR;
         break;
    case 0x80:
         ui->label_3->setText("SPI ready (communication mode)");
         usb->status = COMMUNICATION_MODE;
         break;
    case 0x81:
         ui->label_3->setText("SPI ready (programming mode)");
         usb->status = PROGRAMMING_MODE;
         break;
    case 0x82:
         ui->label_3->setText("SPI ready (debugging mode)");
         usb->status = DEBUG_MODE;
         break;
    case 0x83:
         ui->label_3->setText("SPI not working in background ");
         usb->status = SPI_SLOW_MODE;
         break;
    case 0xFF:
         ui->label_3->setText("SPI not working (HW error)");
         usb->status = NO_MODULE_ON_USB;
         break;
    default:
         if (buff[1] >= 0x40 && buff[1] <= 0x63) {
              ui->label_3->setText("SPI data ready");
              usb->status = SPI_DATA_READY;
         } else {
            printf("Unkown SPI response!\n");
         }
         break;
    }

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

void MainWindow::on_checkBox_stateChanged(int )
{
  if (ui->checkBox->isChecked())
        timer->start(1000);
    else
        timer->stop();
}
