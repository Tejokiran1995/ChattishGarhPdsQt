#include "include/widget.h"
#include "ui_widget.h"
#include "include/misc.h"
#include "include/usb.h"
#include "include/gprs.h"
#include "include/database.h"
#include "include/popup_msg.h"
#include "include/popup_nothing.h"
#include "include/ui_popup_msg.h"
#include "include/ui_popup_nothing.h"

extern char  HD_PASSWORD[64];

extern "C"
{
    #include"include/gl11.h"
}

extern int nLogout;

bool Widget::eventFilter(QObject *obj, QEvent *event)
{
//    qDebug()<<"Qlogout"<<Qlogout;
    if(nLogout)
    {
        int servertime = atoi(Qlogout.toAscii().data());
        int time = 1000*60*servertime;
        if(event->type() == QEvent::MouseButtonPress)
        {
            logout_timer->stop();
            logout_timer->start(time);
        }
        else if(event->type() == QEvent::MouseMove)
        {
            logout_timer->stop();
            logout_timer->start(time);
        }
        else if(event->type() == QEvent::KeyPress)
        {
            logout_timer->stop();
            logout_timer->start(time);
        }
    }
    return false;
}

void Widget::logout()
{
    if(nLogout)
    {
        nLogout =0;
        FILE *fp;
        fp = fopen("/mnt/jffs2/logout_Reg.txt","w");
        if(fp == NULL)
        {
            qDebug() << "File Not Found" << endl;
            return ;

        }
        CMisc miscObj;
        char cmachineID[16];
        memset(cmachineID, 0, sizeof(cmachineID));
        miscObj.GetMachineId(cmachineID);

        char cUrlName[256];
        QSqlQuery query;
        query.prepare("SELECT URL FROM Settings");
        if(!query.exec())
            qDebug() << "In SendTxnToServer" << query.lastError();
        else
        {
            while(query.next())
            {
                memset(cUrlName, 0, sizeof(cUrlName));
                strcpy(cUrlName, query.value(0).toString().toAscii().data());
            }
        }

        fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        fprintf(fp, "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ser=\"http://service.fetch.rationcard/\">");
        fprintf(fp, "<SOAP-ENV:Body>");
        fprintf(fp, "<ser:logOut>");
        fprintf(fp, "<fpsId>%s</fpsId>", ui->ldtShpId->text().toAscii().data());
        fprintf(fp, "<fpsSessionId>%s</fpsSessionId>", SessionId);
        fprintf(fp, "<password>%s</password>", HD_PASSWORD);
        fprintf(fp, "<deviceId>%s</deviceId>", cmachineID);
        fprintf(fp, "<stateCode>%d</stateCode>", stateCode.toInt());
        fprintf(fp, "</ser:logOut>");
        fprintf(fp, "</SOAP-ENV:Body>");
        fprintf(fp, "</SOAP-ENV:Envelope>");
        fclose(fp);

        char cmdCurl[1024];

        memset(cmdCurl, 0, sizeof(cmdCurl));

        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/logout_Reg.txt", cUrlName,  "/mnt/jffs2/logout_Resp.txt", responseTimedOutTimeInSec.toInt());
        qDebug()<<"\nCurl Cmd : "<<cmdCurl;
        system(cmdCurl);
        miscObj.DisplayWideMsgBox((char *)"Logout Sucessfully");

        ui->stackedWidget->setCurrentIndex(0);
        return;
    }
}

int Widget::UrlPingCheck(char * cUrlName)
{
    system("rm /mnt/jffs2/http_code.txt");
    char check_url_online[1024]="";

    memset(check_url_online, 0, sizeof(check_url_online));
    sprintf(check_url_online, "curl -s -o /dev/null -w '%{http_code}' %s -m %d > /mnt/jffs2/http_code.txt",cUrlName,responseTimedOutTimeInSec.toInt());
    qDebug()<<"check_url_online"<<check_url_online;

    int curl_sys = system(check_url_online);
    qDebug() << "curl_sys = " << curl_sys;

    int ret_http_code = 0;
    char cmdCurl[1024];
    memset(cmdCurl, 0, sizeof(cmdCurl));

    ret_http_code = check_http_error_code();
    return ret_http_code;
}




