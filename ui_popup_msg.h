/********************************************************************************
** Form generated from reading UI file 'popup_msg.ui'
**
** Created: Thu Jun 10 17:58:08 2021
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POPUP_MSG_H
#define UI_POPUP_MSG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_popup_msg
{
public:
    QLabel *lblPopup;

    void setupUi(QWidget *popup_msg)
    {
        if (popup_msg->objectName().isEmpty())
            popup_msg->setObjectName(QString::fromUtf8("popup_msg"));
        popup_msg->resize(320, 240);
        lblPopup = new QLabel(popup_msg);
        lblPopup->setObjectName(QString::fromUtf8("lblPopup"));
        lblPopup->setGeometry(QRect(0, 70, 320, 70));
        QFont font;
        font.setPointSize(15);
        font.setBold(true);
        font.setWeight(75);
        lblPopup->setFont(font);
        lblPopup->setLayoutDirection(Qt::LeftToRight);
        lblPopup->setStyleSheet(QString::fromUtf8("border-radius: 6px;\n"
"border: 1px solid black;\n"
"color: rgb(255, 255, 255);"));
        lblPopup->setFrameShape(QFrame::NoFrame);
        lblPopup->setAlignment(Qt::AlignCenter);

        retranslateUi(popup_msg);

        QMetaObject::connectSlotsByName(popup_msg);
    } // setupUi

    void retranslateUi(QWidget *popup_msg)
    {
        popup_msg->setWindowTitle(QApplication::translate("popup_msg", "Form", 0, QApplication::UnicodeUTF8));
        lblPopup->setText(QApplication::translate("popup_msg", "*", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class popup_msg: public Ui_popup_msg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POPUP_MSG_H
