#include <QtGui/QApplication>
#include "include/widget.h"

extern "C"
{
    #include "include/gl11.h"
    void Write_RunningApp_info(char *ApplicationType,char *ApplicationName,char *Version);
}

#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont newFont("Courier", 14, QFont::Bold, false);
    QApplication::setFont(newFont);

    CMisc miscObj;
    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));
    if(miscObj.GetMachineId(cmachineID) < 0)
    {
        miscObj.DisplayWideMsgBox((char *)"Machine-ID Not Found...\nPlease Configure at Boot Prompt");
        return -1;
    }

    FILE *fp;
    char chBuffer[16];
    memset(chBuffer, 0, sizeof(chBuffer));
    fp = popen("pidof commonpds |wc -w", "r");
    while (fgets(chBuffer, 10, fp) != NULL)
    {
        qDebug() << atoi(chBuffer);
    }
    pclose(fp);

    if(atoi(chBuffer)>1)
    {
        qDebug() << "Already running!";
        exit(0);
    }
    /*prn_open();
    LogoPrint logo;
    logo.PrintLogo();*/

    QPixmap pixmap(":/rsc/images/processing.png");
    QSplashScreen splash(pixmap);

    splash.show();

    a.processEvents();

    QString filename_ver = "/mnt/sysuser/app_ver";
    QFile file( filename_ver );
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );
        stream << "APP_TYPE:100" << endl;
        stream << "APP_VERSION:"<<NEW_VERSION << endl;
    }

    file.close();

    Write_RunningApp_info("PDS","commonpds",NEW_VERSION);

    gl11_iodev_open();
    gl11_audio_power_on();
    gl11_host_pwron();
    sleep(2);

    gl11_lcdbklt_on();
    gl11_lcdbklt_time_duration(90);

    system("chmod 777 /vision/DevInfo");
    system("/vision/DevInfo commonpds &");

    Widget w;

    w.show();

    /*TxnUpload UploadData;
    UploadData.start();
    UploadData.moveToThread(&UploadData);*/

    splash.finish(&w);

    return a.exec();
}

