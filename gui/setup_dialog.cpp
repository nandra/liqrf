#include "setup_dialog.h"
#include "ui_setup_dialog.h"

#include <QFileDialog>

setup_dialog::setup_dialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::setup_dialog)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Tools Settings"));

    // default values
    compiler_options = OPTIONS_DEFAULT;
    select_index = 0;
}

setup_dialog::~setup_dialog()
{
    delete m_ui;
}

void setup_dialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void setup_dialog::on_CompilerBrowseButton_clicked()
{
   QString tmp_compiler_location = QFileDialog::getOpenFileName(this, tr("Compiler"), "",
                                            tr("Exe file (*.exe)"));

   m_ui->CompilerLocationEdit->setText(tmp_compiler_location);
}

void setup_dialog::on_EditorBrowseButton_clicked()
{
   QString tmp_editor_location = QFileDialog::getOpenFileName(this, tr("Editor"), "",
                                            tr("All Files (*);;Exe file (*.exe)"));

   m_ui->EditorLocationEdit->setText(tmp_editor_location);
}

// apply new settings
void setup_dialog::on_SetupOkButton_clicked()
{
    compiler_location = m_ui->CompilerLocationEdit->text();
    editor_location = m_ui->EditorLocationEdit->text();
    compiler_options = m_ui->OptionsEdit->text();
    select_index = m_ui->SelectMicroBox->currentIndex();

    this->setShown(false);
}

// discard any new settings
void setup_dialog::on_SetupCancelButton_clicked()
{
    m_ui->CompilerLocationEdit->setText(compiler_location);
    m_ui->EditorLocationEdit->setText(editor_location);
    m_ui->OptionsEdit->setText(compiler_options);
    m_ui->SelectMicroBox->setCurrentIndex(select_index);

    this->setShown(false);
}

void setup_dialog::on_SetDefaultButton_clicked()
{
    m_ui->OptionsEdit->setText(OPTIONS_DEFAULT);
}

void setup_dialog::reject()
{
    this->on_SetupCancelButton_clicked();
}

void setup_dialog::reset()
{
    m_ui->CompilerLocationEdit->setText(compiler_location);
    m_ui->EditorLocationEdit->setText(editor_location);
    m_ui->OptionsEdit->setText(compiler_options);
    m_ui->SelectMicroBox->setCurrentIndex(select_index);
}
