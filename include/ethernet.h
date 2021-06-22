#ifndef ETHERNET_H
#define ETHERNET_H

#include <QWidget>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QFile>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QCoreApplication>
#include <QPalette>
#include <QPixmap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <sys/stat.h>

extern "C"
{
#include "gl11.h"
#include "CHeader.h"
}

class Ethernet
{
public:
    Ethernet();
    int DHCP();
    int STATIC();
    char ipInfostr[50];
    int action_ping();
    char* get_sec_dns();
    char* get_pri_dns();
    char* get_subnet();
    char* get_gateway();
    char* get_ip();
    int action_ipView();
};

#endif // ETHERNET_H
