#include "compile_dialog.h"
#include "ui_compile_dialog.h"

compile_dialog::compile_dialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::compile_dialog)
{
    m_ui->setupUi(this);
}

compile_dialog::~compile_dialog()
{
    //delete m_ui;
}


void compile_dialog::on_pushButton_clicked()
{
    m_ui->compile_edit->clear();
}

void compile_dialog::write_data(QString str)
{
    m_ui->compile_edit->insertPlainText(str);
}

void compile_dialog::done(int r)
{
    m_ui->compile_edit->clear();
    this->setShown(false);
}
