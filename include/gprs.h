#ifndef GPRS_H
#define GPRS_H

#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QPixmap>
#include <QSplashScreen>
#include <QCloseEvent>
#include <QApplication>
#include <QSettings>
#include <QFile>
#include <QTextStream>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

extern "C"
{
#include "gsm_modem.h"
#include "gl11.h"
#include "2dimager.h"

}

class GPRS
{
public:
    GPRS();

    char gprsip[50];

    QSettings *m_ptrSettings;
    QString  m_sSettingsFile;

    int Check_GPRS_Available();
    int Check_Network_Available();


};

#endif
