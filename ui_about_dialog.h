/********************************************************************************
** Form generated from reading ui file 'about_dialog.ui'
**
** Created: Fri May 22 10:50:28 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_ABOUT_DIALOG_H
#define UI_ABOUT_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_AboutDialog
{
public:
    QPushButton *pushButton;
    QLabel *label;

    void setupUi(QDialog *AboutDialog)
    {
    if (AboutDialog->objectName().isEmpty())
        AboutDialog->setObjectName(QString::fromUtf8("AboutDialog"));
    AboutDialog->resize(400, 300);
    pushButton = new QPushButton(AboutDialog);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));
    pushButton->setGeometry(QRect(160, 210, 80, 26));
    label = new QLabel(AboutDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(60, 30, 91, 21));

    retranslateUi(AboutDialog);
    QObject::connect(pushButton, SIGNAL(clicked(bool)), AboutDialog, SLOT(close()));

    QMetaObject::connectSlotsByName(AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *AboutDialog)
    {
    AboutDialog->setWindowTitle(QApplication::translate("AboutDialog", "About", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("AboutDialog", "OK", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("AboutDialog", "About QTLiqrf:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(AboutDialog);
    } // retranslateUi

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUT_DIALOG_H
