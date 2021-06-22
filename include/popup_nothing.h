#ifndef POPUP_NOTHING_H
#define POPUP_NOTHING_H

#include <QWidget>
#include "widget.h"

namespace Ui {
  class popup_nothing;
}

class popup_nothing : public QWidget
{
    Q_OBJECT
    
public:
    explicit popup_nothing(QWidget *parent = 0);
//    ~popup_msg();
        Ui::popup_nothing *ui2;
private:
//    Ui::popup_msg *ui;
};

#endif // POPUP_NOTHING_H
