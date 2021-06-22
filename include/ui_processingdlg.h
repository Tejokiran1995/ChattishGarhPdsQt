/********************************************************************************
** Form generated from reading UI file 'processingdlg.ui'
**
** Created: Mon Oct 27 14:55:06 2014
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROCESSINGDLG_H
#define UI_PROCESSINGDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_CProcessingDlg
{
public:
    QLabel *lblProcessing;

    void setupUi(QDialog *CProcessingDlg)
    {
        if (CProcessingDlg->objectName().isEmpty())
            CProcessingDlg->setObjectName(QString::fromUtf8("CProcessingDlg"));
        CProcessingDlg->resize(320, 230);
        lblProcessing = new QLabel(CProcessingDlg);
        lblProcessing->setObjectName(QString::fromUtf8("lblProcessing"));
        lblProcessing->setGeometry(QRect(30, 20, 260, 151));
        lblProcessing->setPixmap(QPixmap(QString::fromUtf8(":/processing.png")));

        retranslateUi(CProcessingDlg);

        QMetaObject::connectSlotsByName(CProcessingDlg);
    } // setupUi

    void retranslateUi(QDialog *CProcessingDlg)
    {
        CProcessingDlg->setWindowTitle(QApplication::translate("CProcessingDlg", "Dialog", 0, QApplication::UnicodeUTF8));
        lblProcessing->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CProcessingDlg: public Ui_CProcessingDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROCESSINGDLG_H
