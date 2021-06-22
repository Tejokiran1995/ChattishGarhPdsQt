#include "include/misc.h"
#include "widget.h"
#include<QDateTime>
CMisc::CMisc()
{
}

/*
void CMisc::AudioTest(char *filename)
{
    char frame[128];
    int DBFlag = GetAudioFlag();

    memset(frame,0,sizeof(frame));
    sprintf(frame,"mplayer -ao alsa:device=hw=0.0 /mnt/jffs2/mp3/%d%05d.mp3 < /dev/null > /dev/null 2<&1 &",DBFlag,atoi(filename));
    qDebug() << "Audio File = " << frame;

    system(frame);
}
*/

void CMisc::AudioTest(char *filename)
{
    char frame[128];
    int volCount = 192;

    memset(frame,0,sizeof(frame));
    sprintf(frame,"amixer cset numid=4 %d",volCount);
    system(frame);

    int DBFlag = GetAudioFlag();

    memset(frame,0,sizeof(frame));

    //sprintf(frame,"mplayer -ao alsa:device=hw=0.0 /mnt/jffs2/mp3/%d%05d.mp3 < /dev/null > /dev/null 2<&1 &",DBFlag,atoi(filename));

    sprintf(frame,"mplayer -ao alsa:device=hw=0.0 /mnt/jffs2/mp3/%d%05d.mp3 < /dev/null &> /dev/null &",DBFlag,atoi(filename));

    qDebug() << "Audio File = " << frame;

    system(frame);
    sleep(1);
}

int CMisc::GetAudioFlag()
{
    int LangID = 0;

    QSqlQuery query;
    query.prepare("SELECT LangMode FROM Settings");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            LangID = query.value(0).toInt(0);
        }
    }
    return LangID;
}

int CMisc::getFusionCount()
{
    int FusionAttemts = 0;

    QSqlQuery query;
    query.prepare("SELECT fusion_attempts FROM Settings");

    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            FusionAttemts = query.value(0).toInt(0);
        }
    }
    return FusionAttemts;
}

int CMisc::getHelpDeskNo(char *szHelpDeskNo)
{
    QSqlQuery query;
    query.prepare("SELECT helpline_number FROM Settings");

    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            strcpy(szHelpDeskNo, query.value(0).toString().toAscii().data());
        }
    }
    return 0;
}

int CMisc::getAlertMsg(char *szAlertMsg, QString *QSAlertMsgTel)
{
    QSqlQuery query;
    query.prepare("SELECT message_eng, message_tel FROM Settings");

    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            strcpy(szAlertMsg, query.value(0).toString().toAscii().data());
            *QSAlertMsgTel = query.value(1).toString();
        }
    }
    return 0;
}

int CMisc::DisplayDecisionBox(char *msgPerm)
{
    QFont newFont("Courier", 14, QFont::Bold, true);

    QMessageBox msg;
    msg.setStyleSheet("background-color: rgb(35, 173, 111)");
    msg.setWindowFlags(Qt::FramelessWindowHint);
    msg.setText(msgPerm);
    QPushButton *yesBtn = msg.addButton("Yes",QMessageBox::ActionRole);
    QPushButton *noBtn = msg.addButton("No",QMessageBox::ActionRole);
    msg.setFont(newFont);
    yesBtn->setFont(newFont);
    noBtn->setFont(newFont);
    msg.exec();
    if(msg.clickedButton() == noBtn)
        return -1;
    else if(msg.clickedButton() == yesBtn)
        return 0;
}

void CMisc::DisplayMsgBox(char *str)
{
    QFont newFont("Courier", 14, QFont::Bold, true);
    QMessageBox msg;
    msg.setStyleSheet("background-color: rgb(35, 173, 111)");
    msg.setWindowFlags(Qt::FramelessWindowHint);
    msg.setFont(newFont);
    msg.setText(str);

    msg.move(20,40);
    QPushButton *okBtn = msg.addButton("OK",QMessageBox::ActionRole);
    okBtn->setFont(newFont);
    msg.exec();

    return;
}

void CMisc::tel_DisplayMsgBox(char *str)
{
    QFont newFont("Courier", 14, QFont::Bold, true);

    QMessageBox msg;
    msg.setStyleSheet("background-color: rgb(35, 173, 111)");
    msg.setWindowFlags(Qt::FramelessWindowHint);
    msg.setFont(newFont);
    msg.setText(trUtf8(str));
    msg.move(20,40);
    QPushButton *okBtn = msg.addButton(trUtf8("ओके"),QMessageBox::ActionRole);
    okBtn->setFont(newFont);
    msg.exec();

    return;
}

void CMisc::DisplayWideMsgBox(char *str)
{
    QMessageBox msgBox;
    QSqlQuery query;
    query.clear();
    if(!query.exec("select ShopID from ShopOwnerDetails"))
    {

    }
    query.next();
    char TmpStr[1024];
    memset(TmpStr,0x00,sizeof(TmpStr));
    sprintf(TmpStr,"PDS-P%s%s\n%s\nDate:%s T:%s\nFps Id:%s",NEW_VERSION,SUB_VERSION,str,QDateTime::currentDateTime().toString("dd-MM-yyyy").toAscii().data(),QDateTime::currentDateTime().toString("hh:mm:ss").toAscii().data(),query.value(0).toString().toAscii().data());

    QFont newFont("Courier", 12, QFont::Bold, true);

    QSpacerItem* horizontalSpacer = new QSpacerItem(250, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setStyleSheet("background-color: rgb(35, 173, 111)");
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText(TmpStr);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setFont(newFont);
    msgBox.move(20,45);
    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    msgBox.exec();
}

void CMisc::tel_DisplayWideMsgBox(char *str)
{
    QMessageBox msgBox;

    QFont newFont("Courier", 12, QFont::Bold, true);
    QString TmpString;
    TmpString.clear();
    TmpString = "PDS-P";
    TmpString += NEW_VERSION;
    TmpString += SUB_VERSION;
    //        TmpString += "1";
    TmpString +="\n";
    TmpString += trUtf8(str);
    QSqlQuery query;
    query.clear();
    if(!query.exec("select ShopID from ShopOwnerDetails"))
    {

    }
    query.next();
    char TmpStr[1024];
    memset(TmpStr,0x00,sizeof(TmpStr));
    sprintf(TmpStr,"\nDate:%s T:%s\nFps Id:%s",QDateTime::currentDateTime().toString("dd-MM-yyyy").toAscii().data(),QDateTime::currentDateTime().toString("hh:mm:ss").toAscii().data(),query.value(0).toString().toAscii().data());
    TmpString  += TmpStr;
    QSpacerItem* horizontalSpacer = new QSpacerItem(250, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setStyleSheet("background-color: rgb(35, 173, 111)");
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText(TmpString);

    msgBox.addButton(trUtf8("ओके"),QMessageBox::ActionRole);
    msgBox.setFont(newFont);
    msgBox.move(20,45);

    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.exec();
}

void CMisc::TimerMessageBox(char *str, int itime)
{
    QMessageBox *popup = new QMessageBox(QMessageBox::Information, "Info", str, QMessageBox::Ok);
    QTimer *msgBoxCloseTimer = new QTimer(this);
    msgBoxCloseTimer->setInterval(itime*1000);    //3sec
    msgBoxCloseTimer->setSingleShot(true);
    connect(msgBoxCloseTimer, SIGNAL(timeout()), popup, SLOT(reject()));
    msgBoxCloseTimer->start();
    popup->setStandardButtons(0);
    popup->exec();
}

void CMisc::Tel_TimerMessageBox(char *str, int itime)
{
    QMessageBox *popup = new QMessageBox(QMessageBox::Information, "Info", trUtf8(str), QMessageBox::Ok);
    QTimer *msgBoxCloseTimer = new QTimer(this);
    msgBoxCloseTimer->setInterval(itime *1000);    //3sec
    msgBoxCloseTimer->setSingleShot(true);
    connect(msgBoxCloseTimer, SIGNAL(timeout()), popup, SLOT(reject()));
    msgBoxCloseTimer->start();
    popup->setStandardButtons(0);
    popup->exec();
}

int CMisc::DisplayWideDecisionMsgBox(char *str, int i)
{
    QPushButton *yesBtn, *noBtn;
    yesBtn = new QPushButton();
    noBtn = new QPushButton();

    QMessageBox msgBox;
    msgBox.setStyleSheet("background-color: rgb(35, 173, 111)");
    msgBox.setWindowFlags(Qt::FramelessWindowHint);

    if(i == 0)
    {
        msgBox.setText(str);
        yesBtn = msgBox.addButton("Yes",QMessageBox::ActionRole);
        noBtn = msgBox.addButton("No",QMessageBox::ActionRole);
    }
    else
    {
        msgBox.setText(trUtf8(str));
        yesBtn = msgBox.addButton(trUtf8("हाँ"),QMessageBox::ActionRole);
        noBtn = msgBox.addButton(trUtf8("नहीं"),QMessageBox::ActionRole);
    }

    QSpacerItem* horizontalSpacer = new QSpacerItem(300, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    msgBox.exec();
    if(msgBox.clickedButton() == noBtn)
    {
        delete(yesBtn);
        delete(noBtn);
        return -1;
    }
    else if(msgBox.clickedButton() == yesBtn)
    {
        delete(yesBtn);
        delete(noBtn);
        return 0;
    }
    return 0;
}

void CMisc::DisplayProcessingBox(char *str)
{
    QMessageBox msgBox;

    QSpacerItem* horizontalSpacer = new QSpacerItem(250, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setStyleSheet("background-color: rgb(35, 173, 111)");
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText(str);
    msgBox.setStandardButtons(QMessageBox::Ok);

    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.exec();
}

int CMisc::get_line_init(char *buff, FILE  *fd)
{
    char ch=' ';
    int i=-1;
    memset(buff,0x00,sizeof(buff));
    ch=fgetc(fd);
    while(!feof(fd))
    {
        i++;
        if(i>1023)
            return -1;
        buff[i]=ch;
        if(ch=='\n')       break;
        ch=fgetc(fd);
        buff[i+1]='\0';
    }
    if(strlen(buff)>1)
        buff[strlen(buff)-1] = '\0';
    return i;
}

int CMisc::get_line(char *buff, FILE  *fd)
{
    char ch=' ';
    int i=-1;
    memset(buff,0x00,sizeof(buff));
    ch=fgetc(fd);
    while(!feof(fd))
    {
        i++;
        if(i>1023)
            return -1;
        buff[i]=ch;
        if(ch=='\n')       break;
        ch=fgetc(fd);
        buff[i+1]='\0';
    }
    buff[strlen(buff)] = '\0';
    return i;
}

int CMisc::Check_GPRS_Available() //THis Function is not working
{
    return 0;
}

int CMisc::strChkId(char *s)
{
    int i = 0;

    while(s[i] != '\0')
    {
        if((s[i]>='A' && s[i]<='Z') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >= '0' && s[i] <= '9'))
        {
            i++;
            continue;
        }
        else if(s[i]=='-' || s[i]=='/')
        {
            i++;
            continue;
        }
        else
        {
            qDebug()<<"In valid Id format: "<<s[i];
            return -1;
        }
    }
    return 0;
}

int CMisc::strChkOnlyChar(char *s)
{
    int i = 0;

    while(s[i] != '\0')
    {
        if((s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= 'a' && s[i] <= 'z'))
        {
            i++;
            continue;
        }
        else
        {
            qDebug()<<"In valid Name: "<<s[i];
            return -1;
        }
    }
    return 0;
}

int CMisc::strChkName(char *s)
{
    int i = 0;
    while(s[i] != '\0')
    {
        if((s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= 'a' && s[i] <= 'z'))
        {
            i++;
            continue;
        }
        else if(s[i]=='_' || s[i]==',' || s[i]=='.' || s[i]==' ' || s[i]=='\'')
        {
            i++;
            continue;
        }
        else
        {
            qDebug()<<"In valid Name: "<<s[i];
            return -1;
        }
    }
    return 0;
}

int CMisc::strChkPipe(char *s)
{
    if(strchr(s, '|') != NULL )
        return -2;
    return 0;
}

int CMisc::strChkPIN(char *s)
{
    if(strlen(s) != 6 )
    {
        qDebug()<<"Invalid PIN";
        return -1;
    }
    if(strcmp(s, "000000") == 0)
    {
        qDebug()<<"Invalid PIN 000000";
        return -1;
    }
    return 0;
}

int CMisc::strChkPhoneNo(char *s)
{
    int i =0;
    while(s[i] != '\0')
    {
        if(s[i] != '0' )
        {
            return 0;
        }
        i++;
    }
    if(i == strlen(s))
    {
        qDebug()<<"Invalid Phone No";
        return -1;
    }
    return 0;
}

int CMisc::removeFile(char *File)
{
    char szRmCmd[128];
    memset(szRmCmd, 0, sizeof(szRmCmd));
    sprintf(szRmCmd, "rm -f %s", File);
    qDebug()<<"remove file cmd : "<<szRmCmd;
    system(szRmCmd);
}

int CMisc::CHECK_IPADDRESS(char *Buff)
{
    register int i=0;
    int DOT=0;
    int res=0;
    int digit=0;
    int len;
    char buff[3];
    int num;

    len=strlen(Buff);
    //err_printf("length: %d",len);
    //DO not accept dots before numerics
    if(Buff[0]=='.')return 1;
    for(i=0;i<len;i++)
    {
        //checking for two consecutive dots
        if((Buff[i]=='.') && (Buff[i+1]=='.'))return 1;

        //check for dot occuring
        if(Buff[i]=='.'){DOT++;digit=0;}
        else if(DOT>=4){res=1;goto end;}

        //err_printf("%c",Buff[i]);
        //checking for numerics otherthan alpha,special chars.
        if(Buff[i]>='0' && Buff[i] <='9')
        {
            buff[digit]=Buff[i];
            digit++;if(digit<1 || digit>3){res=1;goto end;}
        }
        else if(!(Buff[i]>='0' && Buff[i]<='9') && !(Buff[i]=='.')){res=1;goto end;}

        //checking IP Address ranging from 0-255 only, if not simply thrown away
        if(digit>0)
        {
            buff[digit]='\0';
            num=atoi(buff);
            if(num<0 || num>255){res=1;goto end;}
        }
    }
    //Dots must be 3, nor lessthan 3 or greaterthan 3
    if(Buff[len-1]=='.' || DOT<3)return 1;

end:
    return res;
}

int CMisc::DDMMYYYY2DD_MM_YYYY(char *DATE, char *szDashedDt)
{
    szDashedDt[0] = DATE[0];
    szDashedDt[1] = DATE[1];
    szDashedDt[2] = '-';
    szDashedDt[3] = DATE[2];
    szDashedDt[4] = DATE[3];
    szDashedDt[5] = '-';
    szDashedDt[6] = DATE[4];
    szDashedDt[7] = DATE[5];
    szDashedDt[8] = DATE[6];
    szDashedDt[9] = DATE[7];

    return 0;
}

int CMisc::HHMMSS2HH_MM_SS(char *TIME, char *szDashedTm)
{
    szDashedTm[0] = TIME[0];
    szDashedTm[1] = TIME[1];
    szDashedTm[2] = ':';
    szDashedTm[3] = TIME[2];
    szDashedTm[4] = TIME[3];
    szDashedTm[5] = ':';
    szDashedTm[6] = TIME[4];
    szDashedTm[7] = TIME[5];

    return 0;
}

int CMisc::DD_MM_YYYY2DDMMYYYY(char *szDashedDt, char *DATE)
{
    DATE[0] = szDashedDt[0];
    DATE[1] = szDashedDt[1];
    DATE[2] = szDashedDt[3];
    DATE[3] = szDashedDt[4];
    DATE[4] = szDashedDt[6];
    DATE[5] = szDashedDt[7];
    DATE[6] = szDashedDt[8];
    DATE[7] = szDashedDt[9];
    DATE[8] = '\0';
    return 0;
}

int CMisc::GetMachineId(char *machineid)
{
    FILE *fp;
    char buff[64];
    char *p, *q;
    char filename[]="/mnt/jffs2/fmachineid";

    system("fw_printenv machineid > /mnt/jffs2/fmachineid");
    memset(machineid,0,sizeof(machineid));

    fp=fopen(filename,"r");
    if(fp != NULL)
    {
        memset(buff, 0, sizeof(buff));
        get_line_init(buff,fp);
        fclose(fp);

        if(strlen(buff) > 0)
        {
            p = strchr(buff, '=');
            if(p == NULL)
                return -1;

            q = p+1;

            strcpy(machineid, q);
//            strcpy(machineid, "1907689730");//for testing commom PDS
//            strcpy(machineid, "1902636161");//for testing commom PDS

            machineid[10] = '\0';
            fprintf(stdout, "machine id = %s####\n", machineid);
        }
        else
        {
            fprintf(stdout, "Machine ID Not Found\n");
            return -1;
        }
    }
    else
    {
        fprintf(stdout, "Machine ID Not Found\n");
        return -1;
    }
    return 0;
}

int CMisc::play_amt(char *buff, int iflag)
{
    char tmp[4];

    int LangIdentifier = GetAudioFlag();

    memset(tmp, 0, sizeof(tmp));
    strncpy(tmp, &buff[0], 2);
    if(atoi(tmp) != 0)
    {
        frame(atoi(tmp), 2, LangIdentifier);  //Thousands
    }

    memset(tmp, 0, sizeof(tmp));
    strncpy(tmp, &buff[2], 1);
    if(atoi(tmp) !=0)
    {
        frame(atoi(tmp), 3, LangIdentifier);  //Hundreds
    }

    memset(tmp, 0, sizeof(tmp));
    strncpy(tmp, &buff[3], 2);
    if(atoi(tmp) !=0)
    {
        if(atoi(tmp) <= 20)
        {
            frame(atoi(tmp), 4, LangIdentifier);  //Tens
        }
        else
        {
            char tens[4];
            memset(tens, 0 , sizeof(tens));
            sprintf(tens, "%02d", (atoi(tmp)/10) * 10);

            frame((atoi(tmp)/10)*10, 4, LangIdentifier);
            if(atoi(tmp)%10 != 0)
            {
                frame((atoi(tmp)%10), 4, LangIdentifier);
            }
        }
    }

    char framebuff[256];
    if(iflag == 1)
    {
        memset(framebuff, 0, sizeof(framebuff));
        sprintf(framebuff,"mplayer -ao alsa:device=hw=0.0 /mnt/jffs2/mp3/%d01999.mp3", LangIdentifier);
        system(framebuff);

    }
    else if(iflag == 2)
    {
        memset(framebuff, 0, sizeof(framebuff));
        sprintf(framebuff,"mplayer -ao alsa:device=hw=0.0 /mnt/jffs2/mp3/%d03000.mp3", LangIdentifier);
        system(framebuff);

    }
    return 0;
}

void CMisc::frame(int istr, int flag, int LangIdentifier)
{
    char frame[256];

    memset(frame,0,sizeof(frame));
    sprintf(frame,"mplayer -ao alsa:device=hw=0.0 /mnt/jffs2/mp3/%d%05d.mp3", LangIdentifier, istr);
    system(frame);

    memset(frame,0,sizeof(frame));

    if(flag == 1)
        sprintf(frame,"mplayer -ao alsa:device=hw=0.0 /mnt/jffs2/mp3/%d00888.mp3", LangIdentifier);
    else if(flag == 2)
        sprintf(frame,"mplayer -ao alsa:device=hw=0.0 /mnt/jffs2/mp3/%d00777.mp3", LangIdentifier);
    else if(flag == 3)
        sprintf(frame,"mplayer -ao alsa:device=hw=0.0 /mnt/jffs2/mp3/%d00666.mp3", LangIdentifier);

    system(frame);

    return;
}

void CMisc::SetServerDateToDevice(char *datebuff)
{
    struct tm *curt;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    curt = gmtime(&tv.tv_sec);

    char datestr[16];
    memset(datestr, 0, sizeof(datestr));
    sprintf(datestr,"%c%c%c%c%c%c%c%c", datebuff[3], datebuff[4], datebuff[0], datebuff[1], datebuff[6], datebuff[7], datebuff[8], datebuff[9]);    //DDMMYYYY

    qDebug() << "datestr = " << datestr;

    if(chk_date(datestr) == 0)
    {
        qDebug() << "Setting Date = " << datestr;

        curt->tm_mday = (datestr[0]-0x30)*10+(datestr[1]-0x30);
        curt->tm_mon  = (datestr[2]-0x30)*10+(datestr[3]-0x30) -1;
        curt->tm_year = ((datestr[4]-0x30)*1000 + (datestr[5]-0x30)*100 + (datestr[6]-0x30)*10 + (datestr[7]-0x30)) -1900 ;

        if(gl11_setrtc(curt) == 0)
        {
            qDebug() << "Setting Date Successful";
        }
    }
}

int CMisc::chk_date (char *str)
{
    qDebug() << "Check Date = " << str;

    int epos_date=0,epos_month=0,epos_year=0;

    epos_date  = ( str[0]-0x30)*10+(str[1]-0x30);
    epos_month = ( str[2]-0x30)*10+(str[3]-0x30);
    epos_year  = ( str[4]-0x30)*1000+ (str[5]-0x30)*100 + (str[6]-0x30)*10 + (str[7]-0x30);

    if( epos_month < 1 || epos_date < 1 || epos_date > 31 || epos_month > 12  ||  epos_year < 2008 )
        return -1 ;
    else if(epos_month == 1 || epos_month == 3 || epos_month == 5 || epos_month == 7 || epos_month == 8 || epos_month ==10 ||epos_month == 12)
    {
        if(epos_date > 31)
            return -1;
    }
    else if(epos_month == 4 || epos_month == 6 || epos_month == 9 || epos_month == 11)
    {
        if(epos_date > 30)
            return -1;
    }
    else if(epos_month == 2)
    {
        if ( !(epos_year%400) || (epos_year%100 != 0 && epos_year%4==0))
        {
            if(epos_date > 29)
                return -1;
        }
        else if(epos_date > 28)
            return -1;
    }

    return 0;
}

void CMisc::SetServerTimeToDevice(char *timestr)
{
    struct timeval tv;
    struct tm *curt;

    gettimeofday(&tv,NULL);
    curt = gmtime(&tv.tv_sec);

    if(chk_time(timestr) == 0)
    {
        qDebug() << "Setting Time" << timestr << endl;

        curt->tm_hour = (timestr[0]-0x30)*10+(timestr[1]-0x30);
        curt->tm_min = (timestr[3]-0x30)*10+(timestr[4]-0x30);
        curt->tm_sec = (timestr[6]-0x30)*10+(timestr[7]-0x30);

        if(gl11_setrtc(curt) == 0)
        {
            qDebug() << "Setting Time is Successful";
        }
    }
    else
    {
        qDebug() << "Setting Time UnSuccessful";
    }
}

int CMisc::chk_time(char *str)
{
    qDebug() << "Check Time = " << str;

    int HH,MM,SS;
    HH = (str[0]-0x30)*10+(str[1]-0x30);
    MM = (str[3]-0x30)*10+(str[4]-0x30);
    SS = (str[6]-0x30)*10+(str[7]-0x30);

    if( HH < 0 || HH > 23 || MM < 0 || MM > 59 || SS < 0 || SS > 59 )
        return -1;

    return 0;
}

int CMisc::GetLap(int t1, int ms, char *szLapTime)
{
    int diff = 0, sec=0, microsec=0;

    if(ms<0)
    {
        t1 -= 1;
        ms = 1000000 + ms;
    }

    fprintf(stdout, "%d %d\n", t1, ms);

    diff = (t1 * 1000000) + ms;

    fprintf(stdout, "%d\n", diff);

    sec = diff/1000000;
    microsec = diff % 1000000;

    fprintf(stdout, "%d.%d\n", sec, microsec);

    sprintf(szLapTime, "%d.%06d", sec, microsec);

    return 0;
}

int CMisc::getPacket(unsigned char *buff, int *iLenBuff)
{
    int len = 0;
    char respFile[64];
    FILE *fp;
    int i=0;

    memset(respFile, 0, sizeof(respFile));
    strcpy(respFile,"/root/response.txt");
    fp = fopen(respFile,"rb");
    if(fp == NULL)
    {
        printf("\n Cannot Open File\n");
        return -1;//RESPONSE_FILE_NOT_FOUND;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);

    fseek(fp, 0, SEEK_SET);
    printf("Len = %d\n", len);

    unsigned char ch, firstbyte, secondbyte;
    firstbyte = fgetc(fp);
    secondbyte = fgetc(fp);

    fprintf(stdout, "========>  %02X %02X\n", firstbyte, secondbyte);

    len = getLength(firstbyte, secondbyte);

    printf("Length : %02X == %d\n", len, len);

    for(i = 0; i< len; i++)
    {
        ch = fgetc(fp);
        buff[i] = ch;
    }

    fclose(fp);

    printf("\n......HEX..%d...\n", i);
    for(i = 0; i<len ; i++)
    {
        fprintf(stdout, "%02X ", buff[i]);
    }
    printf("\n");

    *iLenBuff = len;

    return 0;
}
int CMisc::getLength(char f, char s)
{
    return (f<<4|s);
}

unsigned int CMisc::ifb(unsigned char const* nbls, size_t length)
{
    unsigned int result = 0;

    qDebug()<<"No Of BCD Characters = "<<length;
    while (length--) {
        fprintf(stdout, "--->%02X ", (unsigned char)*nbls);
        qDebug()<<"inIfb";
        result = result * 100 + ((unsigned char)*nbls >> 4) * 10 + ((unsigned char)(*nbls) & 15);
        ++nbls;
    }
    return result;
}

void CMisc::GetCurrentDate(char *CurrentDate)
{
    struct tm tt;
    memset(&tt,0,sizeof(struct tm));
    gl11_getrtc(&tt);
    sprintf(CurrentDate,"%02d/%02d/%04d",tt.tm_mday,tt.tm_mon+1,tt.tm_year+1900);
}

void CMisc::GetDateTime(char *DATE, char *TIME, char *DateTime)
{
    struct tm *tt;
    time_t  t;
    time(&t);
    tt=localtime(&t);

    sprintf(DATE, "%02d%02d", tt->tm_mon+1, tt->tm_mday);
    qDebug()<<"DATE = "<<DATE;

    sprintf(TIME, "%02d%02d%02d", tt->tm_hour, tt->tm_min, tt->tm_sec);
    qDebug()<<"TIME = "<<TIME;

    sprintf(DateTime, "%02d%02d%02d%02d%02d", tt->tm_mon+1, tt->tm_mday, tt->tm_hour, tt->tm_min, tt->tm_sec);

}

void CMisc::LeftPad(char *dst,char *src, int len, char pad_char)
{
    int l=0,i=0;

    l = (len-strlen(src));
    while(l>0)
    {
        dst[i]=pad_char;
        i++;
        l--;
    }

    strcat(dst,src);
    dst[strlen(dst)]='\0';
    qDebug()<<dst;
    return;
}
void CMisc::RightPad(char *dst,char *src, int len, char pad_char)
{
    int i=0;
    for(i=strlen(src); i<len; i++)
    {
        dst[i]=pad_char;
    }
    dst[strlen(dst)]='\0';
    return;
}

int CMisc::splitWords(char *PrnMsg, char *MsgHead1, char *MsgHead2)
{
    char *p, *q;
    p = PrnMsg;
    q = strchr(p, '|');
    qDebug()<<"Prn Msg : "<<PrnMsg;
    qDebug()<<"P : "<<p;
    qDebug()<<"Q : "<<q;
    if(q==NULL)
    {
        qDebug()<<"Next Line Character in not found";
        return -1;
    }
    strncpy(MsgHead1, p, q-p);
    qDebug()<<MsgHead1<<"####";
    p = q+1;
    q = strchr(p, '\0');
    if(q==NULL)
    {
        qDebug()<<"EOL Not Found";
        return -1;
    }
    strncpy(MsgHead2, p, q-p);
    qDebug()<<MsgHead2<<"####";

    return 0;
}

int CMisc::GetAgentId(char *szAgentID)
{
    QSqlQuery query;

    query.clear();
    query.prepare("SELECT AgentID FROM BankMasterTable");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            strcpy(szAgentID, query.value(0).toString().toAscii().data());
        }
    }
    return 0;
}

bool  CMisc::fileExists(QString path)
{
    QFileInfo checkFile(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (checkFile.exists() && checkFile.isFile()) {
        return true;
    }
    else{
        return false;
    }
}

bool CMisc::fileDelete(QString path)
{
    QFileInfo checkFile(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (checkFile.exists() && checkFile.isFile()) {
        QFile::remove(checkFile.absoluteFilePath());
        return true;
    }
    else{
        return false;
    }
}

char *CMisc::Convertstr(string  str)
{
    char* cstr;
    string fname = str + '\0';
    cstr = new char [fname.size()+1];
    strcpy( cstr, fname.c_str());

    return cstr;
}

int CMisc :: Battery_check()
{
    int AdapPresent = 0;
    int BattSt = 0;
    int BattVt = 0;
    int ret = 0;
    char message[80]="";
    int batVolt =  BattVt;
    double stripBatVolt = ((8.4)*batVolt)/1024;

    ret = gl11_GetBatteryInfo(&AdapPresent, &BattSt, &BattVt);

    fprintf(stdout,"\n******Battery Info*******\n");
    printf("\n BattVt=%d\n",BattVt);

    stripBatVolt = ((8.4) * BattVt) /1024;

    printf("\n Final Battery volatage=%0.2f V\n",stripBatVolt);

    if (AdapPresent)
    {
        fprintf(stdout,"Adaptor Status:Available\n");
    }
    else
        fprintf(stdout,"Adaptor Status:Not Available\n");

    if(BattVt < 865 && (!AdapPresent))
    {
        printf("Low battery 6.8V \n");
        return -1;
    }
    if (BattVt > 400 )
    {
        memset(message,0,sizeof(message));
        sprintf(message,"Battery Voltage%0.2fV\n",stripBatVolt);
        printf("%s",message);
        printf("Battery ADC Reading:%d\n",BattVt);
    }
    else
    {
        fprintf(stdout,"Battery  Not inserted\n");
        printf("Battery ADC Reading:%d\n",BattVt);
    }

    if(BattSt && AdapPresent)
        fprintf(stdout,"Battery Is Charging\n");
    else  if( !BattSt && AdapPresent )
        fprintf(stdout,"Battery Fully charged\n");
    else if (!BattSt && !AdapPresent )
        fprintf(stdout,"Battery Discharging\n");

    return 0;
}

/*
void CMisc::Check_mplayer_Exists()//dec4
{
    FILE *fp;
    int systemret=0;
    char buff[128];
    char chBuffer[16];
    memset(chBuffer, 0, sizeof(chBuffer));
    fp = popen("pidof mplayer", "r");
    while(fgets(chBuffer, 10, fp) != NULL)
    {
        qDebug() << atoi(chBuffer);
        memset(buff, 0, sizeof(buff));
        sprintf(buff,"kill -9 %d", atoi(chBuffer));
        qDebug() << "Killing audio - " << buff;
        systemret = system(buff);
    }
    pclose(fp);
    return;
}
*/

void CMisc::Check_mplayer_Exists()
{
    /*
    FILE *fp;
    int systemret=0;
    char buff[128];
    char chBuffer[16];
    memset(chBuffer, 0, sizeof(chBuffer));
    fp = popen("pidof mplayer", "r");
    while(fgets(chBuffer, 10, fp) != NULL)
    {
        qDebug() << atoi(chBuffer);
        memset(buff, 0, sizeof(buff));
        sprintf(buff,"kill -1 %d", atoi(chBuffer));
        qDebug() << "Killing audio - " << buff;
        systemret = system(buff);
    }
    pclose(fp); */

    system("kill -1 `pidof mplayer`");
    sleep(1);
    return;
}

int CMisc::GetTransactionId(char *TxnId)
{
//    struct tm *tt;
//    time_t  t;
//    char buff[32];

//    time(&t);
//    tt=localtime(&t);

//    memset(buff, 0, sizeof(buff));

//    int tmp = 0;
//    tmp = (((tt->tm_year-100) - 10) * 366) + (tt->tm_yday+1);
//    qDebug()<<"first 4 digits = "<<tmp;

//    int time_sec = 0;
//    time_sec = (tt->tm_hour * 60 * 60) + (tt->tm_min * 60) + tt->tm_sec;

//    qDebug()<<"time sec = "<<time_sec;
//    qDebug()<<"Last 5 digits = "<<time_sec + 10000;

    char cMachineID[16];
    memset(cMachineID, 0, sizeof(cMachineID));

    QDate Dt = QDate::currentDate();
    QString dateString = Dt.toString("yyyy");

    dateString = "0101" + dateString;
    QDate date = QDate::fromString(dateString, "ddMMyyyy");

    int JulianDay = Dt.toJulianDay() - date.toJulianDay();

    qDebug() << "Julian day:: " << JulianDay;

    QString TimeStamp = QTime::currentTime().toString("hhmmss");

    GetMachineId(cMachineID);
//    sprintf(TxnId, "%s%04d%05d", &cMachineID[6], tmp, time_sec+10000);
    sprintf(TxnId, "%s%03d%s", &cMachineID[6], JulianDay, TimeStamp.toAscii().data());
    qDebug() <<"NewTxnID :"<<TxnId;

    return 0;
}

int CMisc::validate_uid( char *uId )
{
    int dMultTable[10][10] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                              {1, 2, 3, 4, 0, 6, 7, 8, 9, 5},
                              {2, 3, 4, 0, 1, 7, 8, 9, 5, 6},
                              {3, 4, 0, 1, 2, 8, 9, 5, 6, 7},
                              {4, 0, 1, 2, 3, 9, 5, 6, 7, 8},
                              {5, 9, 8, 7, 6, 0, 4, 3, 2, 1},
                              {6, 5, 9, 8, 7, 1, 0, 4, 3, 2},
                              {7, 6, 5, 9, 8, 2, 1, 0, 4, 3},
                              {8, 7, 6, 5, 9, 3, 2, 1, 0, 4},
                              {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}};
    int permTable[8][10] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                            {1, 5, 7, 6, 2, 8, 3, 0, 9, 4},
                            {5, 8, 0, 3, 7, 9, 6, 1, 4, 2},
                            {8, 9, 1, 6, 0, 4, 3, 5, 2, 7},
                            {9, 4, 5, 3, 1, 2, 6, 8, 7, 0},
                            {4, 2, 8, 6, 5, 7, 3, 9, 0, 1},
                            {2, 7, 9, 3, 8, 0, 6, 4, 1, 5},
                            {7, 0, 4, 6, 9, 1, 3, 2, 5, 8}};

    short int i=0, c=0;
    //printf("validate_uid: %d:", c);
    for (i=0; i < 12; i++) {
        short int ni=0, newC=0;

        if (isdigit(uId[11-i]))
            ni = uId[11-i] - '0';
        else {
            printf("\n");
            return(-1);
        }
        newC = dMultTable[c][permTable[i%8][ni]];
        //printf("%d:", newC);
        c = newC;
    }
    printf("\n");
    if (c == 0)
        return(0);
    return(-1);
}

int CMisc::Round_TotalAmount(float totAmt,char *totalRoundamt)
{
    QString acttotAmt;
    acttotAmt.sprintf("%0.3f",totAmt);

    float tot=0;
    char thirddecimal[24] = "",twodigit_float[24],amt[24];

    QStringList list = acttotAmt.split(".");
    QString first_int = list.at(0).trimmed();
    QString second_float = list.at(1).trimmed();

    memset(thirddecimal,0,sizeof(thirddecimal));
    strncpy(thirddecimal,second_float.toAscii().data()+2,1);

    memset(twodigit_float,0,sizeof(twodigit_float));
    strncpy(twodigit_float,second_float.toAscii().data(),2);

    if(atof(thirddecimal) >= 5)
    {
        tot=atof(first_int.toAscii().data())+(atof(twodigit_float)/100)+0.01;
        memset(amt,0,sizeof(amt));
        sprintf(amt,"%0.2f",tot);
    }
    else
    {
        memset(amt,0,sizeof(amt));
        strncpy(amt,acttotAmt.toAscii().data(),acttotAmt.length()-1);
        qDebug()<<"less than 5="<<amt;
    }
    strcpy(totalRoundamt,amt);

    return 0;
}
