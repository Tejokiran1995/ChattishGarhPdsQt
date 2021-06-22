#include "include/gprs.h"
#include "include/misc.h"
#include "include/database_queries.h"

GPRS::GPRS()
{

}

int GPRS::Check_GPRS_Available()
{
    system("rm /mnt/jffs2/gprs.txt");

    system("cp /var/gprs /var/gprsdummy");
    QFile file("/var/gprsdummy");
    if(!file.exists())
        qDebug() << "/var/gprsdummy File Not Found";

    file.open(QIODevice::ReadOnly);
    QString s;
    QTextStream s1(&file);
    s.append(s1.readAll());
    file.close();
    qDebug() << "GPRS Connect = " << s;

    if(s.contains("1,E", Qt::CaseInsensitive) == true)      //GPRS 2G success
        return 0;

    else if(s.contains("1,H", Qt::CaseInsensitive) == true) //GPRS 3G    "
        return 0;

    else if(s.contains("2,E", Qt::CaseInsensitive) == true) //GPRS 2G    "
        return 0;

    else if(s.contains("2,H", Qt::CaseInsensitive) == true) //GPRS 3G    "
        return 0;

    else if(s.contains("1,L", Qt::CaseInsensitive) == true) //GPRS 4G    "
        return 0;

    else if(s.contains("2,L", Qt::CaseInsensitive) == true) //GPRS 4G    "
        return 0;

    else if(s.contains("0,X", Qt::CaseInsensitive) == true) //ETHERNET   "
        return 0;

    else if(s.contains("0,W", Qt::CaseInsensitive) == true) //WiFi       "
        return 0;

    else if(s.contains("7,I", Qt::CaseInsensitive) == true) //ETHERNET Init
        return -3;

    else if(s.contains("8,N", Qt::CaseInsensitive) == true) //ETHERNET Failure
        return -3;

    else if(s.contains("W,N", Qt::CaseInsensitive) == true) //WiFi       "
        return -2;

    else if(s.contains("W,I", Qt::CaseInsensitive) == true) //WiFi    Init
        return -2;

    else if(s.contains("3,N", Qt::CaseInsensitive) == true) //GPRS    Init
        return -1;

    else if(s.contains("1,N", Qt::CaseInsensitive) == true) //GPRS    Failure
        return -1;

    else if(s.contains("2,N", Qt::CaseInsensitive) == true) //GPRS       "
        return -1;

    else if(s.contains("4,N", Qt::CaseInsensitive) == true) //GPRS       "
        return -1;

    else if(s.contains("6,N", Qt::CaseInsensitive) == true) //GPRS       "
        return -1;
    else
        return -4;

    return 0;
}

int GPRS::Check_Network_Available()
{
    CMisc miscObj;
    database_queries dbqry;
    int LangMode = dbqry.getLangMode();

    if(Check_GPRS_Available() == -1)
    {
        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"GPRS Not Available...\nPlease Check GPRS");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"जीपीआरएस उपलब्ध नहीं है ...\nकृपया जीपीआरएस कनेक्शन जांच करे");

        return -1;
    }
    if(Check_GPRS_Available() == -2)
    {
        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"WIFI Not Available...\nPlease Check");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"वाईफ़ाई उपलब्ध नहीं है ...\nकृपया वाईफ़ाई कनेक्शन जांच करे");

        return -1;
    }
    if(Check_GPRS_Available() == -3)
    {
        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"Ethernet connection Failed...\nPlease Check");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"ईथरनेट उपलब्ध नहीं है ...\nकृपया ईथरनेट कनेक्शन जांच करें");

        return -1;
    }
    return 0;
}
