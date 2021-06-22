#ifndef USB_H
#define USB_H

#include <QDialog>
#include <QDebug>

#include <sys/stat.h>

extern "C"{
        #include "gl11.h"
      }


namespace Ui {
class Dialog;
}

class USB : public QDialog
{
    Q_OBJECT

public:
    explicit USB(QWidget *parent = 0);
    ~USB();

    int USBMount();
    void USBUnMount();
    int DosToUnix(char *);
    int statFile(char *);
    int IsMemorySufficientInUSB();
};



#endif // USB_H
