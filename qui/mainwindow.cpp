#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QStringList arguments;

    ui->setupUi(this);
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
        if (usb->usb_dev_found())
            timer->start(1000);
        else
            ui->label_3->setText("No USB device found");
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_spi_status()
{
    /* just for test purposes */
    unsigned char buff[64] = {1,0,0,0,0};
    int len=0;

    usb->set_tx_len(5);
    usb->set_rx_len(4);
    usb->write_tx_buff(buff, 5);
    usb->send_receive_packet();
    len = usb->read_rx_buff(buff);

    switch(buff[1]) {
    case 0x00:
         ui->label_3->setText("SPI not working (disabled by the disableSPI() command)");
    case 0x07:
         ui->label_3->setText("SPI suspended by the stopSPI() command");
    case 0x3F:
         ui->label_3->setText("SPI not ready (buffer full, last CRCM O.K.)");
    case 0x3E:
         ui->label_3->setText("SPI not ready (buffer full, last CRCM error)");
    case 0x80:
         ui->label_3->setText("SPI ready (communication mode)");
    case 0x81:
         ui->label_3->setText("SPI ready (programming mode)");
    case 0x82:
         ui->label_3->setText("SPI ready (debugging mode)");
    case 0x83:
         ui->label_3->setText("SPI not working in background ");
    case 0xFF:
         ui->label_3->setText("SPI not working (HW error)");
    default:
         if (buff[1] >= 0x40 && buff[1] <= 0x63)
              ui->label_3->setText("SPI data ready");
    }

}


void MainWindow::on_checkBox_stateChanged(int )
{
  if (ui->checkBox->isChecked())
        timer->start(1000);
    else
        timer->stop();
}
