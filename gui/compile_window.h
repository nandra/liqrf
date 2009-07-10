#ifndef COMPILE_WINDOW_H
#define COMPILE_WINDOW_H

#include <QWidget>

class QPushButton;
class QTextEdit;

class PreviewWindow : public QWidget
{
    Q_OBJECT

public:
    PreviewWindow(QWidget *parent = 0);
    QTextEdit *textEdit;
private:

     QPushButton *clearButton;
private slots:
     void clear_text_edit();
 };


#endif // COMPILE_WINDOW_H
