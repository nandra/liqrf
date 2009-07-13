#ifndef SETUP_DIALOG_H
#define SETUP_DIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class setup_dialog;
}

class setup_dialog : public QDialog {
    Q_OBJECT
public:
    setup_dialog(QWidget *parent = 0);
    ~setup_dialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::setup_dialog *m_ui;
};

#endif // SETUP_DIALOG_H
