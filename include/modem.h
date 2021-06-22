#ifndef MODEM_H
#define MODEM_H



#include <QDebug>
#include <QTimer>
#include <QString>
#include <QTime>
#include <QMovie>
#include <QPixmap>
#include <QDateTime>
#include <QMessageBox>
//#include <QListWidgetItem>
//#include <QStandardItem>
//#include <QTableWidgetItem>
//#include <QPalette>
//#include <QPixmap>
#include <QFontDatabase>


#include <signal.h>
#include <sys/signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <net/if.h>
#include <dirent.h>
#include<arpa/inet.h> //inet_addr

#include <netdb.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

class modem
{
public:
    modem();
//    int Modem_Open(char *);
    int Modem_Open(char *, char *);
    int Parse(char *, char *);
    void setmodemparams(int ,int);
    int Socket();
    int sockComm();
    int GetWtFromWeingMachine(char *, char * );
};

#endif // MODEM_H
