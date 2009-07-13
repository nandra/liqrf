#include "setup_dialog.h"
#include "ui_setup_dialog.h"

setup_dialog::setup_dialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::setup_dialog)
{
    m_ui->setupUi(this);
    setWindowTitle(tr("Tools Settings"));
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
