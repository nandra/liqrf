#ifndef COMPILE_DIALOG_H
#define COMPILE_DIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class compile_dialog;
}

class compile_dialog : public QDialog {
    Q_OBJECT
public:
    compile_dialog(QWidget *parent = 0);
    ~compile_dialog();
    void write_data(QString);
protected:


private:
    Ui::compile_dialog *m_ui;
public slots:
    void done(int r);
private slots:

    void on_compile_close_clicked();
    void on_pushButton_clicked();
};

#endif // COMPILE_DIALOG_H
