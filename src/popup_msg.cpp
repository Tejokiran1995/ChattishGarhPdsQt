#include "include/popup_msg.h"
#include "include/ui_popup_msg.h"


popup_msg::popup_msg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::popup_msg)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: rgb(35, 173, 111)");
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->lblPopup->setAutoFillBackground(true);

}

