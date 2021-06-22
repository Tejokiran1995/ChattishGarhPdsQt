/********************************************************************************
** Form generated from reading UI file 'popup_nothing.ui'
**
** Created: Tue Feb 13 14:03:17 2018
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POPUP_NOTHING_H
#define UI_POPUP_NOTHING_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_popup_nothing
{
public:
    QLabel *lblDummy;

    void setupUi(QWidget *popup_nothing)
    {
        if (popup_nothing->objectName().isEmpty())
            popup_nothing->setObjectName(QString::fromUtf8("popup_nothing"));
        popup_nothing->resize(320, 240);
        lblDummy = new QLabel(popup_nothing);
        lblDummy->setObjectName(QString::fromUtf8("lblDummy"));
        lblDummy->setGeometry(QRect(140, 100, 21, 16));

        retranslateUi(popup_nothing);

        QMetaObject::connectSlotsByName(popup_nothing);
    } // setupUi

    void retranslateUi(QWidget *popup_nothing)
    {
        popup_nothing->setWindowTitle(QApplication::translate("popup_nothing", "Form", 0, QApplication::UnicodeUTF8));
        lblDummy->setText(QApplication::translate("popup_nothing", ".", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class popup_nothing: public Ui_popup_nothing {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POPUP_NOTHING_H
