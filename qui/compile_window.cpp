#include <QtGui>
#include "compile_window.h"


 PreviewWindow::PreviewWindow(QWidget *parent)
     : QWidget(parent)
 {
     textEdit = new QTextEdit;
     textEdit->setReadOnly(true);
     textEdit->setLineWrapMode(QTextEdit::NoWrap);

     clearButton = new QPushButton(tr("&Clear"));
     connect(clearButton, SIGNAL(clicked()), this, SLOT(clear_text_edit()));

     QVBoxLayout *layout = new QVBoxLayout;
     layout->addWidget(textEdit);
     layout->addWidget(clearButton);
     setLayout(layout);

     setWindowTitle(tr("Compilation output"));

     resize(500,400);
 }

 void PreviewWindow::clear_text_edit()
 {
    textEdit->clear();
 }
