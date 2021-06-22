/********************************************************************************
** Form generated from reading UI file 'BFDwidget.ui'
**
** Created: Tue Mar 3 14:12:47 2015
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BFDWIDGET_H
#define UI_BFDWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BFDWidget
{
public:
    QStackedWidget *stackedWidget;
    QWidget *page;
    QLineEdit *lineEdit;
    QLabel *label;
    QPushButton *pushButton;
    QPushButton *btnBackBFD;
    QLabel *lblAP_Logo1_2;
    QLabel *lblAadhaarLogo1_2;
    QLabel *lblHome_2;
    QWidget *page_2;
    QLabel *left1;
    QLabel *left5;
    QLabel *left4;
    QLabel *left3;
    QLabel *left2;
    QLabel *right5;
    QLabel *right1;
    QLabel *right4;
    QLabel *right2;
    QLabel *right3;
    QLabel *lbl_1;
    QLabel *lbl_2;
    QLabel *lbl_3;
    QLabel *lbl_4;
    QLabel *lbl_5;
    QLabel *lbl_6;
    QLabel *lbl_7;
    QLabel *lbl_8;
    QLabel *lbl_9;
    QLabel *lbl_10;
    QPushButton *pushButton_4;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *btnCancelFP_Image;
    QWidget *page_3;
    QLabel *rightHandPictureLabel;
    QRadioButton *rlbutton;
    QRadioButton *rtbutton;
    QRadioButton *lrbutton;
    QRadioButton *ltbutton;
    QRadioButton *rmbutton;
    QRadioButton *ributton;
    QRadioButton *lmbutton;
    QRadioButton *llbutton;
    QRadioButton *rrbutton;
    QRadioButton *libutton;
    QPushButton *pushButton_2;
    QPushButton *submit_button;
    QPushButton *btnBackFinCapture;
    QLabel *lblHome_3;
    QLabel *lblAadhaarLogo1_3;
    QLabel *lblAP_Logo1_3;

    void setupUi(QWidget *BFDWidget)
    {
        if (BFDWidget->objectName().isEmpty())
            BFDWidget->setObjectName(QString::fromUtf8("BFDWidget"));
        BFDWidget->resize(320, 240);
        stackedWidget = new QStackedWidget(BFDWidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setGeometry(QRect(0, 0, 321, 240));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        page->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 166);"));
        lineEdit = new QLineEdit(page);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(100, 58, 191, 31));
        QFont font;
        font.setPointSize(15);
        font.setBold(true);
        font.setWeight(75);
        lineEdit->setFont(font);
        lineEdit->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        label = new QLabel(page);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 58, 81, 31));
        label->setFont(font);
        label->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 0);"));
        pushButton = new QPushButton(page);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(200, 115, 90, 41));
        pushButton->setFont(font);
        pushButton->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        btnBackBFD = new QPushButton(page);
        btnBackBFD->setObjectName(QString::fromUtf8("btnBackBFD"));
        btnBackBFD->setGeometry(QRect(48, 116, 85, 40));
        QFont font1;
        font1.setPointSize(16);
        font1.setBold(true);
        font1.setWeight(75);
        btnBackBFD->setFont(font1);
        btnBackBFD->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 127);\n"
"background-color: rgb(255, 255, 255);"));
        lblAP_Logo1_2 = new QLabel(page);
        lblAP_Logo1_2->setObjectName(QString::fromUtf8("lblAP_Logo1_2"));
        lblAP_Logo1_2->setGeometry(QRect(-2, 0, 60, 41));
        lblAP_Logo1_2->setStyleSheet(QString::fromUtf8("/*QPushButton{*/\n"
"/*	color: rgb(170, 0, 0);*/\n"
"/*	image: url(:/Attach45796.jpe);*/\n"
"	/*background-image: url(:/Aadhar-E-KYC.jpg);*/\n"
"	/*background-image:url(:/AP_Logo_st.png);*/\n"
"/*    image: url(:/);*/\n"
"    /*border:none;*/\n"
"	/*border-color: rgb(0, 85, 127);*/\n"
"	/*color: rgb(191, 0, 0);*/\n"
"\n"
"/*}*/"));
        lblAP_Logo1_2->setPixmap(QPixmap(QString::fromUtf8(":/rsc/images/AP_Logo_st.png")));
        lblAP_Logo1_2->setScaledContents(true);
        lblAadhaarLogo1_2 = new QLabel(page);
        lblAadhaarLogo1_2->setObjectName(QString::fromUtf8("lblAadhaarLogo1_2"));
        lblAadhaarLogo1_2->setGeometry(QRect(249, 0, 71, 41));
        lblAadhaarLogo1_2->setPixmap(QPixmap(QString::fromUtf8(":/rsc/images/aadhaar_st.jpg")));
        lblAadhaarLogo1_2->setScaledContents(true);
        lblHome_2 = new QLabel(page);
        lblHome_2->setObjectName(QString::fromUtf8("lblHome_2"));
        lblHome_2->setGeometry(QRect(58, 0, 191, 41));
        QFont font2;
        font2.setPointSize(21);
        font2.setBold(true);
        font2.setWeight(75);
        lblHome_2->setFont(font2);
        lblHome_2->setLayoutDirection(Qt::LeftToRight);
        lblHome_2->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);\n"
"color: rgb(0, 85, 127);"));
        lblHome_2->setFrameShape(QFrame::NoFrame);
        lblHome_2->setFrameShadow(QFrame::Plain);
        lblHome_2->setScaledContents(false);
        lblHome_2->setAlignment(Qt::AlignCenter);
        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        left1 = new QLabel(page_2);
        left1->setObjectName(QString::fromUtf8("left1"));
        left1->setEnabled(true);
        left1->setGeometry(QRect(18, 25, 41, 50));
        QFont font3;
        font3.setBold(true);
        font3.setWeight(75);
        left1->setFont(font3);
        left1->setFrameShape(QFrame::Box);
        left5 = new QLabel(page_2);
        left5->setObjectName(QString::fromUtf8("left5"));
        left5->setEnabled(true);
        left5->setGeometry(QRect(267, 26, 41, 50));
        left5->setFont(font3);
        left5->setFrameShape(QFrame::Box);
        left4 = new QLabel(page_2);
        left4->setObjectName(QString::fromUtf8("left4"));
        left4->setEnabled(true);
        left4->setGeometry(QRect(206, 26, 41, 50));
        left4->setFont(font3);
        left4->setFrameShape(QFrame::Box);
        left3 = new QLabel(page_2);
        left3->setObjectName(QString::fromUtf8("left3"));
        left3->setEnabled(true);
        left3->setGeometry(QRect(145, 26, 41, 50));
        left3->setFont(font3);
        left3->setFrameShape(QFrame::Box);
        left2 = new QLabel(page_2);
        left2->setObjectName(QString::fromUtf8("left2"));
        left2->setEnabled(true);
        left2->setGeometry(QRect(79, 25, 41, 50));
        left2->setFont(font3);
        left2->setFrameShape(QFrame::Box);
        right5 = new QLabel(page_2);
        right5->setObjectName(QString::fromUtf8("right5"));
        right5->setEnabled(true);
        right5->setGeometry(QRect(266, 106, 41, 50));
        right5->setFont(font3);
        right5->setFrameShape(QFrame::Box);
        right1 = new QLabel(page_2);
        right1->setObjectName(QString::fromUtf8("right1"));
        right1->setEnabled(true);
        right1->setGeometry(QRect(18, 108, 41, 50));
        right1->setFont(font3);
        right1->setFrameShape(QFrame::Box);
        right4 = new QLabel(page_2);
        right4->setObjectName(QString::fromUtf8("right4"));
        right4->setEnabled(true);
        right4->setGeometry(QRect(206, 106, 41, 50));
        right4->setFont(font3);
        right4->setFrameShape(QFrame::Box);
        right2 = new QLabel(page_2);
        right2->setObjectName(QString::fromUtf8("right2"));
        right2->setEnabled(true);
        right2->setGeometry(QRect(79, 106, 41, 50));
        right2->setFont(font3);
        right2->setFrameShape(QFrame::Box);
        right3 = new QLabel(page_2);
        right3->setObjectName(QString::fromUtf8("right3"));
        right3->setEnabled(true);
        right3->setGeometry(QRect(144, 106, 41, 50));
        right3->setFont(font3);
        right3->setFrameShape(QFrame::Box);
        lbl_1 = new QLabel(page_2);
        lbl_1->setObjectName(QString::fromUtf8("lbl_1"));
        lbl_1->setGeometry(QRect(19, 12, 31, 10));
        lbl_1->setFont(font3);
        lbl_2 = new QLabel(page_2);
        lbl_2->setObjectName(QString::fromUtf8("lbl_2"));
        lbl_2->setGeometry(QRect(80, 13, 31, 10));
        lbl_2->setFont(font3);
        lbl_3 = new QLabel(page_2);
        lbl_3->setObjectName(QString::fromUtf8("lbl_3"));
        lbl_3->setGeometry(QRect(145, 15, 31, 10));
        lbl_3->setFont(font3);
        lbl_4 = new QLabel(page_2);
        lbl_4->setObjectName(QString::fromUtf8("lbl_4"));
        lbl_4->setGeometry(QRect(207, 17, 31, 10));
        lbl_4->setFont(font3);
        lbl_5 = new QLabel(page_2);
        lbl_5->setObjectName(QString::fromUtf8("lbl_5"));
        lbl_5->setGeometry(QRect(267, 17, 31, 10));
        lbl_5->setFont(font3);
        lbl_6 = new QLabel(page_2);
        lbl_6->setObjectName(QString::fromUtf8("lbl_6"));
        lbl_6->setGeometry(QRect(80, 95, 31, 10));
        lbl_6->setFont(font3);
        lbl_7 = new QLabel(page_2);
        lbl_7->setObjectName(QString::fromUtf8("lbl_7"));
        lbl_7->setGeometry(QRect(18, 96, 31, 10));
        lbl_7->setFont(font3);
        lbl_8 = new QLabel(page_2);
        lbl_8->setObjectName(QString::fromUtf8("lbl_8"));
        lbl_8->setGeometry(QRect(144, 94, 31, 10));
        lbl_8->setFont(font3);
        lbl_9 = new QLabel(page_2);
        lbl_9->setObjectName(QString::fromUtf8("lbl_9"));
        lbl_9->setGeometry(QRect(204, 95, 31, 10));
        lbl_9->setFont(font3);
        lbl_10 = new QLabel(page_2);
        lbl_10->setObjectName(QString::fromUtf8("lbl_10"));
        lbl_10->setGeometry(QRect(266, 95, 31, 10));
        lbl_10->setFont(font3);
        pushButton_4 = new QPushButton(page_2);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(230, 164, 80, 27));
        QFont font4;
        font4.setPointSize(14);
        font4.setBold(true);
        font4.setWeight(75);
        pushButton_4->setFont(font4);
        label_2 = new QLabel(page_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, -4, 111, 16));
        label_2->setFont(font3);
        label_3 = new QLabel(page_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 80, 111, 16));
        label_3->setFont(font3);
        btnCancelFP_Image = new QPushButton(page_2);
        btnCancelFP_Image->setObjectName(QString::fromUtf8("btnCancelFP_Image"));
        btnCancelFP_Image->setGeometry(QRect(20, 160, 70, 27));
        btnCancelFP_Image->setFont(font4);
        stackedWidget->addWidget(page_2);
        page_3 = new QWidget();
        page_3->setObjectName(QString::fromUtf8("page_3"));
        rightHandPictureLabel = new QLabel(page_3);
        rightHandPictureLabel->setObjectName(QString::fromUtf8("rightHandPictureLabel"));
        rightHandPictureLabel->setEnabled(true);
        rightHandPictureLabel->setGeometry(QRect(223, 70, 71, 101));
        rightHandPictureLabel->setCursor(QCursor(Qt::BlankCursor));
        rightHandPictureLabel->setFrameShadow(QFrame::Plain);
        rlbutton = new QRadioButton(page_3);
        rlbutton->setObjectName(QString::fromUtf8("rlbutton"));
        rlbutton->setGeometry(QRect(170, 152, 130, 16));
        QFont font5;
        font5.setPointSize(12);
        font5.setBold(true);
        font5.setWeight(75);
        rlbutton->setFont(font5);
        rlbutton->setStyleSheet(QString::fromUtf8("color: rgb(83, 0, 125);"));
        rtbutton = new QRadioButton(page_3);
        rtbutton->setObjectName(QString::fromUtf8("rtbutton"));
        rtbutton->setGeometry(QRect(170, 50, 130, 16));
        rtbutton->setFont(font5);
        rtbutton->setStyleSheet(QString::fromUtf8("color: rgb(83, 0, 125);"));
        rtbutton->setIconSize(QSize(30, 30));
        lrbutton = new QRadioButton(page_3);
        lrbutton->setObjectName(QString::fromUtf8("lrbutton"));
        lrbutton->setGeometry(QRect(10, 127, 130, 16));
        lrbutton->setFont(font5);
        lrbutton->setStyleSheet(QString::fromUtf8("color: rgb(83, 0, 125);"));
        ltbutton = new QRadioButton(page_3);
        ltbutton->setObjectName(QString::fromUtf8("ltbutton"));
        ltbutton->setGeometry(QRect(11, 51, 128, 16));
        ltbutton->setFont(font5);
        ltbutton->setStyleSheet(QString::fromUtf8("color: rgb(83, 0, 125);"));
        ltbutton->setIconSize(QSize(30, 30));
        rmbutton = new QRadioButton(page_3);
        rmbutton->setObjectName(QString::fromUtf8("rmbutton"));
        rmbutton->setGeometry(QRect(170, 101, 130, 16));
        rmbutton->setFont(font5);
        rmbutton->setStyleSheet(QString::fromUtf8("color: rgb(83, 0, 125);"));
        ributton = new QRadioButton(page_3);
        ributton->setObjectName(QString::fromUtf8("ributton"));
        ributton->setGeometry(QRect(170, 76, 130, 16));
        ributton->setFont(font5);
        ributton->setStyleSheet(QString::fromUtf8("color: rgb(83, 0, 125);"));
        lmbutton = new QRadioButton(page_3);
        lmbutton->setObjectName(QString::fromUtf8("lmbutton"));
        lmbutton->setGeometry(QRect(10, 100, 130, 16));
        lmbutton->setFont(font5);
        lmbutton->setStyleSheet(QString::fromUtf8("color: rgb(83, 0, 125);"));
        llbutton = new QRadioButton(page_3);
        llbutton->setObjectName(QString::fromUtf8("llbutton"));
        llbutton->setGeometry(QRect(10, 153, 130, 16));
        llbutton->setFont(font5);
        llbutton->setStyleSheet(QString::fromUtf8("color: rgb(83, 0, 125);"));
        rrbutton = new QRadioButton(page_3);
        rrbutton->setObjectName(QString::fromUtf8("rrbutton"));
        rrbutton->setGeometry(QRect(170, 127, 131, 16));
        rrbutton->setFont(font5);
        rrbutton->setStyleSheet(QString::fromUtf8("color: rgb(83, 0, 125);"));
        libutton = new QRadioButton(page_3);
        libutton->setObjectName(QString::fromUtf8("libutton"));
        libutton->setGeometry(QRect(10, 74, 129, 16));
        libutton->setFont(font5);
        libutton->setStyleSheet(QString::fromUtf8("color: rgb(83, 0, 125);"));
        pushButton_2 = new QPushButton(page_3);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(120, 180, 60, 27));
        submit_button = new QPushButton(page_3);
        submit_button->setObjectName(QString::fromUtf8("submit_button"));
        submit_button->setGeometry(QRect(190, 180, 80, 27));
        submit_button->setFont(font4);
        btnBackFinCapture = new QPushButton(page_3);
        btnBackFinCapture->setObjectName(QString::fromUtf8("btnBackFinCapture"));
        btnBackFinCapture->setGeometry(QRect(30, 180, 80, 27));
        btnBackFinCapture->setFont(font4);
        lblHome_3 = new QLabel(page_3);
        lblHome_3->setObjectName(QString::fromUtf8("lblHome_3"));
        lblHome_3->setGeometry(QRect(60, 0, 191, 41));
        QFont font6;
        font6.setPointSize(17);
        font6.setBold(true);
        font6.setWeight(75);
        lblHome_3->setFont(font6);
        lblHome_3->setLayoutDirection(Qt::LeftToRight);
        lblHome_3->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);\n"
"color: rgb(0, 85, 127);"));
        lblHome_3->setFrameShape(QFrame::NoFrame);
        lblHome_3->setFrameShadow(QFrame::Plain);
        lblHome_3->setScaledContents(false);
        lblHome_3->setAlignment(Qt::AlignCenter);
        lblAadhaarLogo1_3 = new QLabel(page_3);
        lblAadhaarLogo1_3->setObjectName(QString::fromUtf8("lblAadhaarLogo1_3"));
        lblAadhaarLogo1_3->setGeometry(QRect(251, 0, 71, 40));
        lblAadhaarLogo1_3->setPixmap(QPixmap(QString::fromUtf8(":/rsc/images/aadhaar_st.jpg")));
        lblAadhaarLogo1_3->setScaledContents(true);
        lblAP_Logo1_3 = new QLabel(page_3);
        lblAP_Logo1_3->setObjectName(QString::fromUtf8("lblAP_Logo1_3"));
        lblAP_Logo1_3->setGeometry(QRect(0, 0, 60, 41));
        lblAP_Logo1_3->setStyleSheet(QString::fromUtf8("/*QPushButton{*/\n"
"/*	color: rgb(170, 0, 0);*/\n"
"/*	image: url(:/Attach45796.jpe);*/\n"
"	/*background-image: url(:/Aadhar-E-KYC.jpg);*/\n"
"	/*background-image:url(:/AP_Logo_st.png);*/\n"
"/*    image: url(:/);*/\n"
"    /*border:none;*/\n"
"	/*border-color: rgb(0, 85, 127);*/\n"
"	/*color: rgb(191, 0, 0);*/\n"
"\n"
"/*}*/"));
        lblAP_Logo1_3->setPixmap(QPixmap(QString::fromUtf8(":/rsc/images/AP_Logo_st.png")));
        lblAP_Logo1_3->setScaledContents(true);
        stackedWidget->addWidget(page_3);

        retranslateUi(BFDWidget);

        stackedWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(BFDWidget);
    } // setupUi

    void retranslateUi(QWidget *BFDWidget)
    {
        BFDWidget->setWindowTitle(QApplication::translate("BFDWidget", "Widget", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("BFDWidget", "Enter UID", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("BFDWidget", "Next >", 0, QApplication::UnicodeUTF8));
        btnBackBFD->setText(QApplication::translate("BFDWidget", "Home", 0, QApplication::UnicodeUTF8));
        lblAP_Logo1_2->setText(QString());
        lblAadhaarLogo1_2->setText(QString());
        lblHome_2->setText(QApplication::translate("BFDWidget", "BFD", 0, QApplication::UnicodeUTF8));
        left1->setText(QString());
        left5->setText(QString());
        left4->setText(QString());
        left3->setText(QString());
        left2->setText(QString());
        right5->setText(QString());
        right1->setText(QString());
        right4->setText(QString());
        right2->setText(QString());
        right3->setText(QString());
        lbl_1->setText(QApplication::translate("BFDWidget", "1", 0, QApplication::UnicodeUTF8));
        lbl_2->setText(QApplication::translate("BFDWidget", "2", 0, QApplication::UnicodeUTF8));
        lbl_3->setText(QApplication::translate("BFDWidget", "3", 0, QApplication::UnicodeUTF8));
        lbl_4->setText(QApplication::translate("BFDWidget", "4", 0, QApplication::UnicodeUTF8));
        lbl_5->setText(QApplication::translate("BFDWidget", "5", 0, QApplication::UnicodeUTF8));
        lbl_6->setText(QApplication::translate("BFDWidget", "7", 0, QApplication::UnicodeUTF8));
        lbl_7->setText(QApplication::translate("BFDWidget", "6", 0, QApplication::UnicodeUTF8));
        lbl_8->setText(QApplication::translate("BFDWidget", "8", 0, QApplication::UnicodeUTF8));
        lbl_9->setText(QApplication::translate("BFDWidget", "9", 0, QApplication::UnicodeUTF8));
        lbl_10->setText(QApplication::translate("BFDWidget", "10", 0, QApplication::UnicodeUTF8));
        pushButton_4->setText(QApplication::translate("BFDWidget", "Submit", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("BFDWidget", "LEFT HAND", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("BFDWidget", "RIGHT HAND", 0, QApplication::UnicodeUTF8));
        btnCancelFP_Image->setText(QApplication::translate("BFDWidget", "Cancel", 0, QApplication::UnicodeUTF8));
        rightHandPictureLabel->setText(QString());
        rlbutton->setText(QApplication::translate("BFDWidget", "RIGHT_LITTLE", 0, QApplication::UnicodeUTF8));
        rtbutton->setText(QApplication::translate("BFDWidget", "RIGHT THUMB", 0, QApplication::UnicodeUTF8));
        lrbutton->setText(QApplication::translate("BFDWidget", "LEFT_RING", 0, QApplication::UnicodeUTF8));
        ltbutton->setText(QApplication::translate("BFDWidget", "LEFT_THUMB", 0, QApplication::UnicodeUTF8));
        rmbutton->setText(QApplication::translate("BFDWidget", "RIGHT_MIDDLE", 0, QApplication::UnicodeUTF8));
        ributton->setText(QApplication::translate("BFDWidget", "RIGHT_INDEX", 0, QApplication::UnicodeUTF8));
        lmbutton->setText(QApplication::translate("BFDWidget", "LEFT_MIDDLE", 0, QApplication::UnicodeUTF8));
        llbutton->setText(QApplication::translate("BFDWidget", "LEFT_LITTLE", 0, QApplication::UnicodeUTF8));
        rrbutton->setText(QApplication::translate("BFDWidget", "RIGHT_RING", 0, QApplication::UnicodeUTF8));
        libutton->setText(QApplication::translate("BFDWidget", "LEFT_INDEX", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("BFDWidget", "Capture", 0, QApplication::UnicodeUTF8));
        submit_button->setText(QApplication::translate("BFDWidget", "Next >", 0, QApplication::UnicodeUTF8));
        btnBackFinCapture->setText(QApplication::translate("BFDWidget", "< Back", 0, QApplication::UnicodeUTF8));
        lblHome_3->setText(QApplication::translate("BFDWidget", "FINGER CAPTURE", 0, QApplication::UnicodeUTF8));
        lblAadhaarLogo1_3->setText(QString());
        lblAP_Logo1_3->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class BFDWidget: public Ui_BFDWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BFDWIDGET_H
