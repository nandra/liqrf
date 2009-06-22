#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QTime>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QStringList arguments;

    ui->setupUi(this);
    /* read only text edits*/
    ui->UploadTextEdit->setReadOnly(true);
    ui->term_text_edit->setReadOnly(true);
    /* programmer instance */
    prog = new programmer();
    /* parser instance */
    parser = new hex_parser();

    prog->dev->usb->init_usb();

    if (prog->dev->usb->usb_dev_found()) {
        ui->UploadTextEdit->insertPlainText("USB device found\n");

    } else {
         QMessageBox::about(this, tr("USB device not found!"),
                       tr("Please connect a device and press OK."));

    }

    if (prog->dev->usb->open_usb()) {
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
        if (prog->dev->usb->usb_dev_found()) {
            timer->start(500);
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

    /* connectio to custom signals */
    connect(this, SIGNAL(my_signal()), this, SLOT(test_signal()));

}

void MainWindow::deviceAdded()
{
    printf("device added\n");
}

void MainWindow::test_signal()
{
    printf("Test signal emitted %x\n",prog->dev->usb->status);
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
    if ((prog->dev->usb->status != NO_MODULE_ON_USB) && (prog->dev->usb->status != SPI_DISABLED)) {

        prog->enter_prog_mode();
        /* stop timer to avoid get data to wrong place */
        timer->stop();
        while (prog->dev->get_spi_status() != PROGRAMMING_MODE)
            sleep(1);

        if (!prog->request_module_id()) {
            qDebug() << "Internal error : request_module_id";
            goto exit;
        }
        if (!prog->get_module_id()) {
            qDebug() << "Internal error : get_module_id";
            goto exit;
        }

        QString str;
        int i;
        for (i = 0; i < 4; i++)
           str.append(str.number(prog->module_id[i],16).toUpper());

        ui->label_mod_id->setText(str);

        /* disable upload button */
        ui->EnterProgButton->setDisabled(true);
        // enable open file button
        ui->OpenFileButton->setEnabled(true);
    }
exit:
    sleep(1);
}

MainWindow::~MainWindow()
{
    delete parser;
    delete prog;
    delete timer;
}

void MainWindow::update_spi_status()
{
    int stat = 0, len;
    unsigned char buff[36];
    stat = prog->dev->get_spi_status();

    switch(stat) {
    case 0x00:
         ui->label_3->setText("SPI not working");
         break;
    case 0x07:
         ui->label_3->setText("SPI suspended");
         break;
    case 0x3F:
         ui->label_3->setText("SPI not ready last CRCM O.K.)");
         break;
    case 0x3E:
         ui->label_3->setText("SPI not ready last CRCM error)");
         break;
    case 0x80:
         ui->label_3->setText("SPI ready (communication mode)");
         break;
    case 0x81:
         ui->label_3->setText("SPI ready (programming mode)");
         break;
    case 0x82:
         ui->label_3->setText("SPI ready (debugging mode)");
         break;
    case 0x83:
         ui->label_3->setText("SPI not working in background ");
         break;
    case 0xFF:
         ui->label_3->setText("SPI not working (HW error)");
         break;
    default:
         if (stat >= 0x40 && stat <= 0x63) {
              ui->label_3->setText("SPI data ready");
              /* if data are ready print to text array in Terminal label */
              len = prog->dev->get_spi_cmd_data(buff, stat & 0x3F, 0, 0);
              buff[len] = '\0';
              QString str;
              QTime tm;
              str.append(tm.currentTime().toString());
              str.append(" : \"");
              str.append((char *)&buff[0]);
              str.append("\"");

              ui->term_text_edit->insertPlainText(str);

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

void MainWindow::on_clear_upload_btn_clicked()
{
    ui->UploadTextEdit->clear();
}

void MainWindow::on_pushButton_clicked()
{
    ui->term_text_edit->clear();
}
