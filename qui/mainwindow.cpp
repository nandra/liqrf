#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QTime>
#include <QProcess>

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
    /* prog->parser instance */
    //prog->parser = new hex_prog->parser();
    /* thread for editor instance */
    editor_thread = new Thread();

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
    connect(ui->actionPIC_16F88, SIGNAL(triggered(bool)), this, SLOT(mcu_16f88()));
    connect(ui->actionPIC_16F886, SIGNAL(triggered(bool)), this, SLOT(mcu_16f886()));

    // button connections
    connect(ui->ResetButton, SIGNAL(clicked(bool)), this, SLOT(resetModule()));
    connect(ui->EnterProgButton, SIGNAL(clicked(bool)), this, SLOT(enterProgMode()));

    /* connections to custom signals */
    connect(this, SIGNAL(my_signal()), this, SLOT(test_signal()));

    /* default values */
    this->mcu = MCU_16F88;
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

/* mcu type selction for compilation */
void MainWindow::mcu_16f88()
{
    this->mcu = MCU_16F88;
}

void MainWindow::mcu_16f886()
{
    this->mcu = MCU_16F886;
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

void MainWindow::enterProgMode()
{
    QString str, str1;

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
        parse_module_info(prog->module_id, &str, &str1);
        ui->label_os->setText(str);
        ui->label_mod_id->setText(str1);

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
    //delete prog->parser;
    delete prog;
    delete timer;
    delete editor_thread;
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
    opened_file = QFileDialog::getOpenFileName(this, tr("Open file"), "",
                                            tr("Hex file (*.hex);;C file (*.c);;All Files (*)"));
    if (opened_file.isEmpty())
         return;
    // HEX files
    else if (opened_file.endsWith(".hex")) {
        prog->parser->hexfile = opened_file;
        if (!prog->parser->read_file()) {
            ui->UploadTextEdit->insertPlainText("Error opening file "+prog->parser->hexfile+'\n');
            return;
        }
        ui->UploadButton->setEnabled(true);
        ui->ApplicationCheckBox->setEnabled(true);
        ui->EepromCheckBox->setEnabled(true);
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

// Compile c program for iqrf
void MainWindow::on_CompileButton_clicked(bool checked)
{
    QProcess compile_process;
    QStringList arguments;
    QString directory;          // working directory
    QString filename;           // name of file being compiled

    if ((!opened_file.isEmpty()) && (opened_file.endsWith(".c"))) {

        // get separate file name and directory
        int dir_index = opened_file.lastIndexOf("/");
        QStringRef reference = opened_file.leftRef(dir_index);
        directory = reference.toString();
        ui->UploadTextEdit->insertPlainText("working dir is "+directory+'\n');
        reference = opened_file.rightRef(opened_file.size() - dir_index - 1);
        filename = reference.toString();
        ui->UploadTextEdit->insertPlainText("file is "+filename+'\n');

        // setup and run compile process
        arguments << "CC5x.exe" << "-a" << "-bu" << "-Q" <<
                "-Vn";
        if (this->mcu == MCU_16F88)
            arguments << "-p16F88";
        else
            arguments << "-p16F886";
        arguments << filename;

        qDebug() << arguments;
        //ui->UploadTextEdit->insertPlainText("args are "+arguments.join(" ")+'\n');
        compile_process.setWorkingDirectory(directory);
        compile_process.start("wine", arguments);

        if (!compile_process.waitForStarted()) {
            fprintf(stderr, "Compile error.\n");
            return;
        }

        if (!compile_process.waitForFinished())
            return;

        // print output of compile process
        QByteArray output;
        output = compile_process.readAllStandardOutput();
        ui->UploadTextEdit->insertPlainText(QString(output));
        output = compile_process.readAllStandardError();
        ui->UploadTextEdit->insertPlainText(QString(output));

    }
}

// start editor
void MainWindow::on_EditFileButton_clicked(bool checked)
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
    int flash_addr = FLASH_BASE_ADDR;

    while (prog->dev->spi_status != PROGRAMMING_MODE)
        prog->dev->get_spi_status();

    if (prog->parser->usr_eeprom_size) {
        if (!prog->send_prog_data(EEPROM_USER, prog->parser->usr_eeprom, prog->parser->usr_eeprom_size,
                                    USR_EEPROM_BASE_ADDR)) {
            qDebug() << "Error occured";
            return;
        }
        printf("Programming user eeprom (%d)\n", prog->parser->usr_eeprom_size);
    }

    if (prog->parser->app_eeprom_size) {
        if (!prog->send_prog_data(EEPROM_APP, prog->parser->app_eeprom, prog->parser->app_eeprom_size,
                                    APP_EEPROM_BASE_ADDR)) {
            qDebug() << "Error occured";
            return;
        }
        printf("Programming app eeprom (%d)\n", prog->parser->app_eeprom_size);
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

        flash_addr += FLASH_ADDR_STEP;
    }

    prog->enter_endprog_mode();
    /* star timer for checking SPI */
    timer->start(1000);

    prog->dev->usb->reset_usb();

}
