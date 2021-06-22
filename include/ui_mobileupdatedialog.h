/********************************************************************************
** Form generated from reading UI file 'mobileupdatedialog.ui'
**
** Created: Wed Aug 2 13:55:55 2017
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOBILEUPDATEDIALOG_H
#define UI_MOBILEUPDATEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_MobileUpdateDialog
{
public:
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *MobileUpdateDialog)
    {
        if (MobileUpdateDialog->objectName().isEmpty())
            MobileUpdateDialog->setObjectName(QString::fromUtf8("MobileUpdateDialog"));
        MobileUpdateDialog->resize(291, 101);
        QFont font;
        font.setPointSize(8);
        MobileUpdateDialog->setFont(font);
        label = new QLabel(MobileUpdateDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 0, 361, 31));
        QFont font1;
        font1.setPointSize(11);
        font1.setBold(true);
        font1.setWeight(75);
        label->setFont(font1);
        lineEdit = new QLineEdit(MobileUpdateDialog);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(190, 30, 121, 31));
        lineEdit->setFont(font1);
        label_2 = new QLabel(MobileUpdateDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 30, 191, 21));
        label_2->setFont(font1);
        pushButton = new QPushButton(MobileUpdateDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(10, 70, 80, 31));
        QFont font2;
        font2.setPointSize(13);
        font2.setBold(true);
        font2.setWeight(75);
        pushButton->setFont(font2);
        pushButton->setStyleSheet(QString::fromUtf8("/*color: rgb(0, 0, 255);\n"
"background-color: rgb(255, 255, 255);*/\n"
"border-radius: 8px;\n"
"background-color: rgb(35, 173, 111);\n"
"color: rgb(0, 0, 0);"));
        pushButton_2 = new QPushButton(MobileUpdateDialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(210, 70, 80, 31));
        pushButton_2->setFont(font2);
        pushButton_2->setStyleSheet(QString::fromUtf8("/*color: rgb(170, 0, 0);\n"
"background-color: rgb(255, 255, 255);*/\n"
"border-radius: 8px;\n"
"background-color: rgb(170, 0, 0);\n"
"color: rgb(255, 255, 255);"));

        retranslateUi(MobileUpdateDialog);

        QMetaObject::connectSlotsByName(MobileUpdateDialog);
    } // setupUi

    void retranslateUi(QDialog *MobileUpdateDialog)
    {
        MobileUpdateDialog->setWindowTitle(QApplication::translate("MobileUpdateDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MobileUpdateDialog", "Would You Like to Update Mobile Number ?", 0, QApplication::UnicodeUTF8));
        lineEdit->setText(QString());
        label_2->setText(QApplication::translate("MobileUpdateDialog", "Enter Mobile Number :", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MobileUpdateDialog", "UPDATE", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("MobileUpdateDialog", "SKIP", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MobileUpdateDialog: public Ui_MobileUpdateDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOBILEUPDATEDIALOG_H
