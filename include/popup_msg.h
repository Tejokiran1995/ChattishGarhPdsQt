#ifndef POPUP_MSG_H
#define POPUP_MSG_H

#include <QWidget>
#include "widget.h"

namespace Ui {
  class popup_msg;
}

class popup_msg : public QWidget
{
    Q_OBJECT
    
public:
    explicit popup_msg(QWidget *parent = 0);
//    ~popup_msg();
        Ui::popup_msg *ui;
private:
//    Ui::popup_msg *ui;
};

#endif // POPUP_MSG_H
//253 - 4.13
//313 - 5.13
