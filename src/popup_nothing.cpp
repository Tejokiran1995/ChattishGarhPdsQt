#include "include/popup_nothing.h"
#include "include/ui_popup_nothing.h"


popup_nothing::popup_nothing(QWidget *parent) :
    QWidget(parent),
    ui2(new Ui::popup_nothing)
{
    ui2->setupUi(this);
    this->setStyleSheet("background-color: white");
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui2->lblDummy->setAutoFillBackground(true);

}

