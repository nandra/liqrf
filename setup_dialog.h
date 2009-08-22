#ifndef SETUP_DIALOG_H
#define SETUP_DIALOG_H

#include <QtGui/QDialog>

#define OPTIONS_DEFAULT "-a -bu -Q -Vn -cif"

namespace Ui {
    class setup_dialog;
}

class setup_dialog : public QDialog {
    Q_OBJECT

public:
    setup_dialog(QWidget *parent = 0);
    ~setup_dialog();
     QString compiler_location;
     QString editor_location;
     QString compiler_options;
     int select_index;     // index of selected microcontroller type
     void reset();         // resets dialog according to saved settings

protected:
    void changeEvent(QEvent *e);

private:
    Ui::setup_dialog *m_ui;

private slots:
    void reject();
    void on_SetDefaultButton_clicked();
    void on_SetupCancelButton_clicked();
    void on_SetupOkButton_clicked();
    void on_EditorBrowseButton_clicked();
    void on_CompilerBrowseButton_clicked();
};

#endif // SETUP_DIALOG_H
