/***************************************************************************
 *   Copyright (C) 2009 by open-nandra                                     *
 *   matej.haulik@gmail.com                                                *
 *   marek.belisko@open-nandra.com                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QTime>
#include <QProcess>
#include <QDBusConnection>
#include <QtDBus>


/* window contructor */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QStringList arguments;
    QString str;
    int max_setup = 3;

    ui->setupUi(this);
    this->window = new PreviewWindow();
    /* read only text edits*/
    ui->UploadTextEdit->setReadOnly(true);
    ui->term_text_edit->setReadOnly(true);
    /* programmer instance */
    prog = new programmer();

    /* thread for editor instance */
    editor_thread = new Thread();

    /* user has 2 possibilities to conect usb */
    while (max_setup) {
        /* usb initialization */
        prog->dev->usb->init_usb();

        /* check if usb device was found */
        if (prog->dev->usb->usb_dev_found()) {
            ui->UploadTextEdit->insertPlainText("USB device found\n");
             /* open usb and also clain interface */
            if (prog->dev->usb->open_usb()) {
                ui->UploadTextEdit->insertPlainText("USB device opened\n");
                break;
            }
        } else {
            if (max_setup != 1)
                QMessageBox::about(this, tr("USB device not found!"),
                               tr("Please connect a device and press OK."));
        }
        max_setup--;
    }

    /* create timer for checking SPI status */
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_spi_status()));

    /* start checking spi status device is presented */
    if (ui->checkBox->isChecked()) {
        if (prog->dev->usb->usb_dev_found()) {
            timer->start(500);
        } else {
            ui->label_3->setText("No USB device found");
            ui->indicatorSPI->setStyleSheet("QLineEdit { background-color: red; }");
            /* disable enter prog button */
            ui->EnterProgButton->setDisabled(true);
        }
    }

    /* set table parameters */
    for (int i=0; i < ui->tableWidget->rowCount(); i++) {
        ui->tableWidget->setRowHeight(i, 15);
    }
    ui->tableWidget->resizeColumnsToContents();
    //ui->tableWidget->resizeRowsToContents();
    for (int i=0; i < ui->tableWidget_2->rowCount(); i++) {
        ui->tableWidget_2->setRowHeight(i, 15);
    }
    ui->tableWidget_2->resizeColumnsToContents();
    //ui->tableWidget_2->resizeRowsToContents();


    /* menu connections */
    connect(ui->action_Exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
    connect(ui->actionPIC_16F88, SIGNAL(triggered(bool)), this, SLOT(mcu_16f88()));
    connect(ui->actionPIC_16F886, SIGNAL(triggered(bool)), this, SLOT(mcu_16f886()));
    connect(ui->actionTools, SIGNAL(triggered(bool)), this, SLOT(toolsSetting()));

    /* button connections */
    connect(ui->ResetButton, SIGNAL(clicked(bool)), this, SLOT(resetModule()));
    connect(ui->EnterProgButton, SIGNAL(clicked(bool)), this, SLOT(enterProgMode()));

    /* connections to custom signals */
    connect(this, SIGNAL(my_signal()), this, SLOT(test_signal()));

    /* default values for MCU */
    this->mcu = MCU_16F88;

    if (this->mcu == MCU_16F88)
        str.append("MCU 16F88");
    else
        str.append("MCU 16F886");
    ui->label_mcu->setText(str);

    /* register signal for HAL via DBus to retrieve
     * notifications about usb device status
     */
    if (!QDBusConnection::systemBus().isConnected()) {
        qDebug() << "Cannot connect to system bus";
        return;
    }

    QDBusConnection::systemBus().connect(
            "org.freedesktop.Hal",
            "/org/freedesktop/Hal/Manager",
            "org.freedesktop.Hal.Manager",
            "DeviceAdded",
            this, SLOT(deviceAdded(const QString &)));

    QDBusConnection::systemBus().connect(
                "org.freedesktop.Hal",
                "/org/freedesktop/Hal/Manager",
                "org.freedesktop.Hal.Manager",
                "DeviceRemoved",
                this, SLOT(deviceRemoved(const QString &)));
}

/* template to retrieve data from HAL */
template<typename T>
T dbusRequest(QDBusInterface &i, const QString &method, const QString &param)
{
    QDBusReply<T> reply = i.call(method, param);

    return reply.value();
}

void MainWindow::deviceAdded(const QString &udi)
{
    QDBusInterface i("org.freedesktop.Hal",
                     udi,
                     "org.freedesktop.Hal.Device",
                     QDBusConnection::systemBus());

    QString caps = dbusRequest<QString>(i, "GetProperty", "info.subsystem");

    int vendor = dbusRequest<int>(i, "GetPropertyInteger", "usb.vendor_id");
    qDebug() << "Vendor" << vendor;

    int product = dbusRequest<int>(i, "GetPropertyInteger", "usb.product_id");
    qDebug() << "Product" << product;


    if ((vendor == 0x1de6) && (product == 0x1)) {
        qDebug() << "Device added";
        prog->dev->usb->init_usb();

        /* check if usb device was found */
        if (prog->dev->usb->usb_dev_found()) {
            if (!prog->dev->usb->open_usb()) {
                qDebug() << "Cannot init USB";
                goto end;
            }
         }
        timer->start();
        ui->EnterProgButton->setEnabled(true);
        ui->label_3->setStyleSheet("QLabel {}");
        ui->label_3->setText("USB device connected");
        ui->indicatorSPI->setStyleSheet("QLineEdit { background-color: green; }");
    }
end:
    return;
}

void MainWindow::deviceRemoved(const QString &udi)
{
    bool ok;
    QString str_vendor = udi.section('_',2,2);
    QString str_product = udi.section('_',3,3);
    /* this is kind of hack because after remove
     * HAL emmit sinal 2 times with different ui
     */
    QString str = udi.section('_',5,5);

    if (str != "") {
        qDebug() << str_vendor << str_product;

        if ((str_vendor.toInt(&ok,16) == 0x1de6) && (str_product.toInt(&ok, 16) == 0x1)) {
            qDebug() << "Device removed";
            /* in case of disconnection usb device */
            timer->stop();
            prog->release();
            ui->EnterProgButton->setDisabled(true);
            ui->label_3->setText("USB device disconnected");
            ui->label_3->setStyleSheet("QLabel {background-color: red;}");
            ui->indicatorSPI->setStyleSheet("QLineEdit { background-color: red; }");
        }
    }
}


void MainWindow::test_signal()
{
    printf("Test signal emitted %x\n",prog->dev->usb->status);
}

void MainWindow::toolsSetting()
{

}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About QTLiqrf"),
                       tr("<h2>QTLiqrf</h2>"
                          "<p>Version: xx"
                          "<p>open nandra 2009"
                          "<p><a href=\"http://open-nandra.com\">open-nandra.com</a>"));
}

/* reset module command */
void MainWindow::resetModule()
{
    prog->reset_module();
    /* start SPI status check after module reset */
    if (!ui->checkBox->isChecked())
        ui->checkBox->setChecked(true);
}

/* mcu type selection for compilation */
void MainWindow::mcu_16f88()
{
    this->mcu = MCU_16F88;
    QString str;

    str.append("MCU 16F88");
    ui->label_mcu->setText(str);

    /* reread hex file if opened */
    if (!prog->parser->hexfile.isEmpty())
        if(!prog->parser->read_file(this->mcu)) {
            ui->UploadTextEdit->insertPlainText("Error opening file "+prog->parser->hexfile+'\n');
            return;
        }

}

void MainWindow::mcu_16f886()
{
    this->mcu = MCU_16F886;
    QString str;

    str.append("MCU 16F886");
    ui->label_mcu->setText(str);

    // reread hex file if opened
    if (!prog->parser->hexfile.isEmpty())
        if(!prog->parser->read_file(this->mcu)) {
            ui->UploadTextEdit->insertPlainText("Error opening file "+prog->parser->hexfile+'\n');
            return;
        }

}

/*
   parse data to retrieve modele info
   data[0-3] = module id
   data[4] = os version
   data[5] = mcu type
   data[6-7] = build number
*/
void parse_module_info(unsigned char *data, QString *str_out, QString *str1_out)
{
    QString str, str1, t, s ;
    int i;

    if (data[3] > 0)
        str.append("Coordinator: ");
    else
        str.append("Node: ");

    t = str.number((data[4] & 0xF0)>>4, 16);
    s = str.number((data[4] & 0x0F), 16);
    if(s.size() < 2)
        s = "0"+s;
    str.append("IQRF OS ver "+t+"."+s);

    str1.append("ID:");
    for (i = 0; i < 4; i++) {
        QString t = str1.number(data[i],16).toUpper();
        if (t.size() < 2)
            t = "0" + t;
        str1.append(t);
    }
    str1.append(" ");
    if (data[5] == 0x02)
        str1.append(" MCU PIC16LF88");
    else
        str1.append(" MCU PIC16LF886");

    t = str.number(data[6], 16);
    if (t.size() < 2)
        t = "0"+t;
    s = str.number(data[7], 16);
    if (s.size() < 2)
        s = "0"+s;

    str.append(" (0x"+s+t+")");

    *str_out = str;
    *str1_out = str1;
}

/* enter programming mode */
void MainWindow::enterProgMode()
{
    QString str, str1;

    /* maybe needs to be extended for other SPI statuses */
    if ((prog->dev->usb->status != NO_MODULE_ON_USB) && (prog->dev->usb->status != SPI_DISABLED)) {

        /* stop timer to avoid get data to wrong place */
        timer->stop();
        /* send some data to negotiate prog. mode */
        prog->enter_prog_mode();

        /* unti status == programming */
        while (prog->dev->get_spi_status() != PROGRAMMING_MODE)
            sleep(1);
        /* ask for module id (checksums are wrong don't know why)*/
        if (!prog->request_module_id()) {
            qDebug() << "Internal error : request_module_id";
            goto exit;
        }
        /* read module id and other info */
        if (!prog->get_module_id()) {
            qDebug() << "Internal error : get_module_id";
            goto exit;
        }
        parse_module_info(prog->module_id, &str, &str1);
        ui->label_os->setText(str);
        ui->label_mod_id->setText(str1);

        /* disable upload button */
        ui->EnterProgButton->setDisabled(true);
        /* enable open file button */
        ui->UploadButton->setEnabled(true);
    }
exit:
    return;
}

/* main destructor */
MainWindow::~MainWindow()
{
    delete prog;
    delete timer;
    delete editor_thread;
    delete window;
}

/* timer slot (check for spi status) */
void MainWindow::update_spi_status()
{
    int stat = 0, len;
    unsigned char buff[36];
    stat = prog->dev->get_spi_status();

    switch(stat) {
    case 0x00:
         ui->label_3->setText("SPI not working");
         ui->indicatorSPI->setStyleSheet("QLineEdit { background-color: grey; }");
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
         ui->indicatorSPI->setStyleSheet("QLineEdit { background-color: green; }");
         break;
    case 0x81:
         ui->label_3->setText("SPI ready (programming mode)");
         /* if user press on CKUSB02 button it must also trigger
          * programming (disable enter prog mode btn, enable upload btn)
          */
         ui->EnterProgButton->setDisabled(true);
         /* avoid overwriting status */
         timer->stop();
         break;
    case 0x82:
         ui->label_3->setText("SPI ready (debugging mode)");
         break;
    case 0x83:
         ui->label_3->setText("SPI not working in background ");
         break;
    case 0xFF:
         ui->label_3->setText("SPI not working (HW error)");
         ui->indicatorSPI->setStyleSheet("QLineEdit { background-color: red; }");
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
              str.append("\"\n");

              ui->term_text_edit->insertPlainText(str);

         } else {
            printf("Unkown SPI response!\n");
         }
         break;
    }
}


void MainWindow::on_OpenFileButton_clicked()
{

    opened_file = QFileDialog::getOpenFileName(this, tr("Open file"), "",
                                            tr("Hex file (*.hex);;C file (*.c);;All Files (*)"));
    if (opened_file.isEmpty())
         return;

    /* HEX files */
    else if (opened_file.endsWith(".hex")) {
        prog->parser->hexfile = opened_file;

        /* read with correct hex format */
        if (!prog->parser->read_file(this->mcu)) {
            ui->UploadTextEdit->insertPlainText("Error opening file "+prog->parser->hexfile+'\n');
            return;
        }

        ui->ApplicationCheckBox->setEnabled(true);
        ui->ApplicationCheckBox->setChecked(true);
        QString text = QString::number(prog->parser->flash_size);
        ui->ApplicationCheckBox->setText("APPLICATION   " + text + " instructions");

        ui->EepromCheckBox->setEnabled(true);
        ui->EepromCheckBox->setChecked(true);
        text = QString::number(prog->parser->app_eeprom_size + prog->parser->usr_eeprom_size);
        ui->EepromCheckBox->setText("EEPROM         " + text + " bytes");

        ui->CompileButton->setDisabled(true);
        ui->EditFileButton->setDisabled(true);

    // C files
    } else if(opened_file.endsWith(".c")) {
        ui->UploadButton->setDisabled(true);
        ui->ApplicationCheckBox->setDisabled(true);
        ui->EepromCheckBox->setDisabled(true);
        ui->CompileButton->setEnabled(true);
        ui->EditFileButton->setEnabled(true);

    // Other
    } else {
        ui->UploadTextEdit->insertPlainText("Unsupported file format!\n");
        ui->UploadButton->setDisabled(true);
        ui->ApplicationCheckBox->setDisabled(true);
        ui->EepromCheckBox->setDisabled(true);
        ui->CompileButton->setDisabled(true);
        ui->EditFileButton->setDisabled(true);
        ui->file_label->setText(" ");
        return;
    }

    ui->UploadTextEdit->insertPlainText("Opened file "+opened_file+'\n');
    ui->file_label->setText(opened_file.rightRef(opened_file.size() - opened_file.lastIndexOf("/") - 1).toString());

}

void MainWindow::on_checkBox_stateChanged(int )
{
  if (ui->checkBox->isChecked())
        timer->start(500);
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

// Compile c program for iqrf
void MainWindow::on_CompileButton_clicked()
{
    QProcess compile_process;
    QStringList arguments;
    QString directory;          // working directory
    QString filename;           // name of file being compiled

    /* display compilation output in separate window */
    this->window->show();

    if ((!opened_file.isEmpty()) && (opened_file.endsWith(".c"))) {

        // get separate file name and directory
        int dir_index = opened_file.lastIndexOf("/");
        QStringRef reference = opened_file.leftRef(dir_index);
        directory = reference.toString();
        this->window->textEdit->insertPlainText("working dir is "+directory+'\n');
        reference = opened_file.rightRef(opened_file.size() - dir_index - 1);
        filename = reference.toString();
        this->window->textEdit->insertPlainText("file is "+filename+'\n');

        // setup and run compile process
        arguments << "CC5x.exe" << "-a" << "-bu" << "-Q" <<
                "-Vn";
        if (this->mcu == MCU_16F88)
            arguments << "-p16F88";
        else
            arguments << "-p16F886";
        arguments << filename;

        qDebug() << arguments;
        this->window->textEdit->insertPlainText("args are "+arguments.join(" ")+'\n');
        compile_process.setWorkingDirectory(directory);
        compile_process.start("wine", arguments);

        if (!compile_process.waitForStarted()) {
            fprintf(stderr, "Compile error.\n");
            this->window->textEdit->insertPlainText("Compilation error. Please chcek wine and CC5X compiler.\n");

            return;
        }

        if (!compile_process.waitForFinished())
            return;

        // print output of compile process
        QByteArray output, test = "marek123";
        output = compile_process.readAllStandardOutput();
        this->window->textEdit->insertPlainText(QString(output));
        output.append(compile_process.readAllStandardError());
        this->window->textEdit->insertPlainText(QString(output));
        this->window->textEdit->insertPlainText("\n\n\n");

        /* check if compilation contain errors*/
        if (output.contains("Error")) {
            printf("Error in compilation ( hex file not created )\n");
        } else {
            /* this is maybe hack and using same code 2 times */
            prog->parser->hexfile = directory+"/"+filename.remove(".c").append(".hex");
            ui->UploadTextEdit->insertPlainText("File openned after compilation "+prog->parser->hexfile+'\n');
            /* read with correct hex format */
            if (!prog->parser->read_file(this->mcu)) {
                ui->UploadTextEdit->insertPlainText("Error opening file "+prog->parser->hexfile+'\n');
                return;
            }
            ui->ApplicationCheckBox->setEnabled(true);
            ui->ApplicationCheckBox->setChecked(true);
            QString text = QString::number(prog->parser->flash_size);
            ui->ApplicationCheckBox->setText("APPLICATION   " + text + " instructions");

            ui->EepromCheckBox->setEnabled(true);
            ui->EepromCheckBox->setChecked(true);
            text = QString::number(prog->parser->app_eeprom_size + prog->parser->usr_eeprom_size);
            ui->EepromCheckBox->setText("EEPROM         " + text + " bytes");

        }

    }
    ui->UploadButton->setEnabled(true);

}

// start editor
void MainWindow::on_EditFileButton_clicked()
{
    editor_thread->run(opened_file);
}

// run editor in thread
void Thread::run(QString filename)
{
    QProcess editor_process;
    QStringList arguments;
    QString editor_name;

    editor_name = "kate";

    if ((!filename.isEmpty()) && (filename.endsWith(".c"))) {
        arguments << filename;
        editor_process.start(editor_name, arguments);
        if (!editor_process.waitForStarted()) {
            fprintf(stderr, "Editor error.\n");
            return;
        }
        exec();
        /*
        if (!editor_process.waitForFinished())
            return;
            */
    }

}

void MainWindow::on_UploadButton_clicked()
{
    int block_count, c, len;
    int flash_addr = (this->mcu == MCU_16F88) ? FLASH_BASE_ADDR_16F88 : FLASH_BASE_ADDR_16F886;
    int size = 0;

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(prog->parser->usr_eeprom_size +
                                prog->parser->app_eeprom_size +
                                prog->parser->flash_size);

    while (prog->dev->spi_status != PROGRAMMING_MODE)
        prog->dev->get_spi_status();

    if (prog->parser->usr_eeprom_size) {
        if (!prog->send_prog_data(EEPROM_USER, prog->parser->usr_eeprom, prog->parser->usr_eeprom_size,
                                    USR_EEPROM_BASE_ADDR)) {
            qDebug() << "Error occured";
            return;
        }
        printf("Programming user eeprom (%d)\n", prog->parser->usr_eeprom_size);
        size += prog->parser->usr_eeprom_size;
        ui->progressBar->setValue(size);
    }

    if (prog->parser->app_eeprom_size) {
        if (!prog->send_prog_data(EEPROM_APP, prog->parser->app_eeprom, prog->parser->app_eeprom_size,
                                    APP_EEPROM_BASE_ADDR)) {
            qDebug() << "Error occured";
            return;
        }
        printf("Programming app eeprom (%d)\n", prog->parser->app_eeprom_size);
        size += prog->parser->app_eeprom_size;
        ui->progressBar->setValue(size);
    }

    /* prepare flash data by 32 bytes blocks */
    block_count = prog->parser->flash_size / FLASH_BLOCK_SIZE;

    if (prog->parser->flash_size % FLASH_BLOCK_SIZE)
        block_count++;

    for (c = 0; c < block_count; c++) {
        if ((prog->parser->flash_size - (c * FLASH_BLOCK_SIZE)) < FLASH_BLOCK_SIZE)
            len = prog->parser->flash_size - (c * FLASH_BLOCK_SIZE);
        else
            len = FLASH_BLOCK_SIZE;

        if (!prog->send_prog_data(FLASH_PROG, &prog->parser->flash[c*FLASH_BLOCK_SIZE], len,
                                    flash_addr)) {
            qDebug() << "Error occured";
            return;
        }
        size += len;
        ui->progressBar->setValue(size);

        flash_addr += FLASH_ADDR_STEP;
    }

    prog->enter_endprog_mode();
    /* star timer for checking SPI */
    timer->start(500);

    prog->dev->usb->reset_usb();

    ui->UploadButton->setDisabled(true);
    ui->EnterProgButton->setEnabled(true);

}

