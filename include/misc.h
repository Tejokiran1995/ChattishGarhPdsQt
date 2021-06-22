#ifndef MISC_H
#define MISC_H

#include <string.h>
#include <unistd.h>
#include <QDebug>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QMessageBox>
#include <QFile>
#include <QFont>
#include <QPushButton>
#include <QSpacerItem>
#include <QGridLayout>
#include <QDialog>
#include <QTimer>

#include <sys/time.h>
#include <time.h>
#include <QFileInfo>


extern "C"
{
#include "gl11.h"
}
using std::string;


class CMisc : public QObject
{
public:
    CMisc();

    int getAlertMsg(char *, QString *);

    void AudioTest(char *);
    int GetAudioFlag();
    int GetAgentId(char *);
    int getHelpDeskNo(char *);
    int getFusionCount();

    void DisplayMsgBox(char *);
    void tel_DisplayMsgBox(char *);
    void DisplayWideMsgBox(char *);
    void tel_DisplayWideMsgBox(char *);
    int DisplayDecisionBox(char *);
    int DisplayWideDecisionMsgBox(char *, int);
    void DisplayProcessingBox(char *);
    int get_line_init(char *, FILE  *fd);
    int get_line(char *, FILE  *fd);
    int Check_GPRS_Available();
    int strChkOnlyChar(char *);
    int strChkPipe(char *);
    int strChkName(char *);
    int strChkId(char *);
    int strChkPIN(char *);
    int strChkPhoneNo(char *);
    int removeFile(char *);
    int CHECK_IPADDRESS(char *);
    void TimerMessageBox(char *, int);
    void Tel_TimerMessageBox(char *, int);

    int DDMMYYYY2DD_MM_YYYY(char *, char *);
    int HHMMSS2HH_MM_SS(char *, char *);
    int DD_MM_YYYY2DDMMYYYY(char *, char *);
    int GetMachineId(char *);
    void SetServerDateToDevice(char *);
    int chk_date (char *);
    void SetServerTimeToDevice(char *);
    int chk_time(char *);
    int LapTimeCalc();
    int GetLap(int, int, char *);
    void ConsentForm();
    void GetCurrentDate(char *);
    void GetDateTime(char *, char *, char *);
//    int GetOutFileName(char *);

    int play_amt(char *, int);
    void frame(int, int, int);

    int getPacket(unsigned char *, int *);
    int getLength(char, char);
    unsigned int ifb(unsigned char const*, size_t);
    void LeftPad(char *, char *, int , char);
    void RightPad(char *, char *, int , char);
    int splitWords(char *PrnMsg, char *MsgHead1, char *MsgHead2);

//Added by Vivek
    bool fileExists(QString path);
    bool fileDelete(QString path);
    char * Convertstr(string str);
    int Battery_check();

    void Check_mplayer_Exists();
    int GetTransactionId(char *);
    int validate_uid( char *uId );

    int Round_TotalAmount(float,char *);

};

#endif // MISC_H
