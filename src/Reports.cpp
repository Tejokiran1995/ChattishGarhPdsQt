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

#include<cstdlib>
#include<math.h>
#include<json_request.h>
#include<json.h>
#include <cstdio>
#include <cstring>
#include<iostream>
#include<fstream>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>


extern char  HD_PASSWORD[64];
extern char  HD_HTS[8];

extern "C"
{
#include"include/gl11.h"
}

void Widget::on_btnReports_clicked()
{
//    miscObj.Check_mplayer_Exists();
    QCalendarWidget *CalendarWid = new QCalendarWidget();
    CalendarWid->setGridVisible(true);
    CalendarWid->setNavigationBarVisible (true);

    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setCalendarPopup(true);
    ui->dateEdit->setCalendarWidget(CalendarWid);

    ui->dateEdit->setMinimumDate(QDate::currentDate().addDays(-90));
    ui->dateEdit->setMaximumDate(QDate::currentDate());
    ui->dateEdit->setDisplayFormat("dd/MM/yyyy");

    ui->stackedWidget->setCurrentIndex(2);
    return;
}

void Widget::on_btnGotoHome_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Widget::on_btnStockRegister_clicked()
{
    QStockReportType = "S";
    ui->tableWidgetStockreport->clearContents();
    PrintCurrentStockReport();

    return;
}

void Widget::PrintCurrentStockReport()
{
    database_queries obj;
    memset(SessionId,0,sizeof(SessionId));

    if(obj.getSessionId(SessionId) < 0)
    {
        miscObj.DisplayMsgBox("Session Id not found\nPlease try again");
        return;
    }

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.."));

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    if(iLCD_Display == 1)
    {
        QStockReportType = "S";// "STATE";
    }
    else
    {
        QStockReportType = "S";// "STATE";
    }
    systemret = system("rm /mnt/jffs2/CurrentStockReportReq.xml");
    systemret = system("rm /mnt/jffs2/CurrentStockReportResp.xml");

    FILE *fp;
    fp = fopen("/mnt/jffs2/CurrentStockReportReq.xml","w");
    if(fp == NULL)
    {
        qDebug() << "/mnt/jffs2/StockUpdateReq.xml File Not Found" << endl;
        return;
    }

    fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ser=\"http://service.fetch.rationcard/\">");
    fprintf(fp, "<%s/>", "soapenv:Header");
    fprintf(fp, "<%s>", "soapenv:Body");
    fprintf(fp, "<%s>", "ser:getStockReportDetails");
    fprintf(fp, "<shop_no>%s</shop_no>", ui->ldtShpId->text().toAscii().data());
    fprintf(fp, "<report_type>%s</report_type>", QStockReportType.toAscii().data());

    fprintf(fp, "<token>%s</token>", HD_PASSWORD);
    fprintf(fp, "<fpsSessionId>%s</fpsSessionId>",SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());

    fprintf(fp, "</%s>", "ser:getStockReportDetails");
    fprintf(fp, "</%s>", "soapenv:Body");
    fprintf(fp, "</%s>", "soapenv:Envelope");

    fclose(fp);

    qDebug() << "******** /mnt/jffs2/CurrentStockReportReq.xml ***********";
    systemret = system("cat /mnt/jffs2/CurrentStockReportReq.xml");
    qDebug();
    qDebug() << "*********************************************************";

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

    char cmdCurl[1024];
    memset(cmdCurl, 0, sizeof(cmdCurl));

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/CurrentStockReportReq.xml", cUrlName, "/mnt/jffs2/CurrentStockReportResp.xml", responseTimedOutTimeInSec.toInt());

    qDebug() << "\nCurl Cmd : " << cmdCurl;

    int sysret = system(cmdCurl);
    qDebug() << "sysret = " << sysret;

    if(SystemCurlResponse(sysret, EPDS_SERVER) < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();

        return;
    }

    qDebug() << "***********/mnt/jffs2/CurrentStockReportResp.xml************";
    systemret = system("cat /mnt/jffs2/CurrentStockReportResp.xml");
    qDebug() << "***************************************************";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/CurrentStockReportResp.xml") < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();

        return;
    }
    ui->stackedWidget->setCurrentIndex(46);
    parseCurrentStockReportandinsert();

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    return;
}

void Widget::parseCurrentStockReportandinsert()
{
    ui->tableWidgetStockreport->showMaximized();
    ui->tableWidgetStockreport->clearContents();

    QFile file("/mnt/jffs2/CurrentStockReportResp.xml");
    if(!file.exists())
    {
        qDebug() << "File Not Found";
        if(LangMode==1)
            miscObj.DisplayWideMsgBox((char *)"File Not Downloaded");
        else if(LangMode==2)
            miscObj.tel_DisplayWideMsgBox((char *)"फ़ाइल डाउनलोड नहीं हुआ ");

        return;
    }

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error Reading File" << endl;
        return;
    }

    QDomDocument document;
    if(!document.setContent( &file ) )
    {
        qDebug( "Failed to parse the file into a DOM tree." );
        file.close();
        return;
    }
    file.close();

    int ChildCount=0,i = 0;
    QDomNodeList list ;

    list = document.elementsByTagName("astockBean");
    ChildCount = list.count();
    qDebug() << "parseCurrentStockReportandinsert:: Main List Child Count = " << ChildCount;

    int row=0;
    ui->tableWidgetStockreport->setRowCount(ChildCount);

    for(i = 0; i < ChildCount; i++)
    {
        QString Qallot_qty = list.at(i).childNodes().at(0).toElement().text();
        QString Qclosing_balance = list.at(i).childNodes().at(1).toElement().text();
        QString QcommNamell = list.at(i).childNodes().at(2).toElement().text(); //new tag added 15052017 vivek
        QString Qcomm_name = list.at(i).childNodes().at(3).toElement().text();
        QString Qcommoditycode = list.at(i).childNodes().at(4).toElement().text();
        QString Qissued_qty = list.at(i).childNodes().at(5).toElement().text();
        QString Qopening_balance = list.at(i).childNodes().at(6).toElement().text();
        QString Qreceived_qty = list.at(i).childNodes().at(7).toElement().text();

        QString Qscheme_desc_en = list.at(i).childNodes().at(8).toElement().text();
        QString Qscheme_desc_ll = list.at(i).childNodes().at(9).toElement().text();

        QString Qtotal_quantity = list.at(i).childNodes().at(10).toElement().text();

        qDebug()<<"***" << Qallot_qty << " " << Qclosing_balance << " "<< QcommNamell.toUtf8() << " " << Qcomm_name << " " << Qcommoditycode << " " << Qissued_qty << " " << Qopening_balance << " " << Qreceived_qty << " " <<  Qtotal_quantity <<"***"; //QcommNamell (Language Converstion from English to hind vivek 17052017)

        if(Qallot_qty == "Server Error")
        {
            if(LangMode==1)
                miscObj.DisplayWideMsgBox((char *)"Server Error");
            else if(LangMode==2)
                miscObj.tel_DisplayWideMsgBox((char *)"सर्वर एरर ");

            return;
        }
        if(Qcommoditycode.contains("Success"))
            continue;
        else if(!Qallot_qty.isNull())
        {
            row = i;

            if(LangMode == 1)
                ui->tableWidgetStockreport->setItem(row, 0, new QTableWidgetItem(Qcomm_name));
            else if(LangMode == 2)
                ui->tableWidgetStockreport->setItem(row, 0, new QTableWidgetItem(QcommNamell));

            ui->tableWidgetStockreport->item(row, 0)->setFlags( ui->tableWidgetStockreport->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetStockreport->item(row, 0)->setFlags( ui->tableWidgetStockreport->item(row,0)->flags() | Qt::ItemIsSelectable);

            //SCHEME
            if(LangMode == 1)
                ui->tableWidgetStockreport->setItem(row, 1, new QTableWidgetItem(Qscheme_desc_en));
            else if(LangMode == 2)
                ui->tableWidgetStockreport->setItem(row, 1, new QTableWidgetItem(Qscheme_desc_ll));
            ui->tableWidgetStockreport->item(row, 1)->setFlags( ui->tableWidgetStockreport->item(row,1)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetStockreport->item(row, 1)->setFlags( ui->tableWidgetStockreport->item(row,1)->flags() | Qt::ItemIsSelectable);

            //stock
            ui->tableWidgetStockreport->setItem(row, 2, new QTableWidgetItem(Qtotal_quantity));
            ui->tableWidgetStockreport->item(row, 2)->setFlags( ui->tableWidgetStockreport->item(row,2)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetStockreport->item(row, 2)->setFlags( ui->tableWidgetStockreport->item(row,2)->flags() | Qt::ItemIsSelectable);

            //issued
            ui->tableWidgetStockreport->setItem(row, 3, new QTableWidgetItem(Qissued_qty));
            ui->tableWidgetStockreport->item(row, 3)->setFlags( ui->tableWidgetStockreport->item(row,3)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetStockreport->item(row, 3)->setFlags( ui->tableWidgetStockreport->item(row,3)->flags() | Qt::ItemIsSelectable);

            //CB
            ui->tableWidgetStockreport->setItem(row, 4, new QTableWidgetItem(Qclosing_balance));
            ui->tableWidgetStockreport->item(row, 4)->setFlags( ui->tableWidgetStockreport->item(row,4)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetStockreport->item(row, 4)->setFlags( ui->tableWidgetStockreport->item(row,4)->flags() | Qt::ItemIsSelectable);

            ui->tableWidgetStockreport->setColumnWidth(0,130);
            ui->tableWidgetStockreport->setColumnWidth(1,50);
            ui->tableWidgetStockreport->setColumnWidth(2,70);
            ui->tableWidgetStockreport->setColumnWidth(3,70);
            ui->tableWidgetStockreport->setColumnWidth(4,70);

        }
    }
}

void Widget::on_btncurrstkback_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void Widget::on_btncurrstkprint_clicked()
{
    parseCurrentStockReportRespDetails();
    ui->stackedWidget->setCurrentIndex(2);

}

void Widget::parseCurrentStockReportRespDetails()
{
    char RTC[32], DATE[16], TIME[16], JULIAN_DAYS[8];
    char szDashedDt[16],szDashedTm[16];
    char prn[512];

    memset(RTC, 0, sizeof(RTC));
    memset(DATE, 0, sizeof(DATE));
    memset(TIME, 0, sizeof(TIME));
    memset(JULIAN_DAYS, 0, sizeof(JULIAN_DAYS));

    GetRTC_Params(RTC, DATE, TIME, JULIAN_DAYS);

    memset(szDashedDt, 0, sizeof(szDashedDt));
    memset(szDashedTm, 0, sizeof(szDashedTm));

    miscObj.DDMMYYYY2DD_MM_YYYY(DATE, szDashedDt);
    miscObj.HHMMSS2HH_MM_SS(TIME, szDashedTm);

    prn_open();

    if(prn_paperstatus() < 0)
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया कागज डालें.");

        prn_close();

        return;
    }
    if(miscObj.Battery_check() != 0) //01072017
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद को प्रिंट करने में असमर्थ\nअपनी बैटरी चार्ज करें");

        prn_close();

        return;

    }

    LogoPrint objLogo;
    objLogo.PrintLogo(stateCode.toInt());
    database_queries dbqry;
    LangMode = dbqry.getLangMode();

    if(LangMode == 1)
    {
        memset(prn, 0, sizeof(prn));
        sprintf(prn, " Current Stock Report");
        prn_write_text((unsigned char *)prn, strlen(prn), 2);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, " Date: %s        Time: %s", szDashedDt, szDashedTm);
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        if(QStockReportType == "N")
            QStockReportType = "NFSA";
        else if(QStockReportType == "S")
            QStockReportType = "PDS";
        else if(QStockReportType == "I")
            QStockReportType = "ICDS";
        else if(QStockReportType == "M")
            QStockReportType = "MDM";


        memset(prn, 0, sizeof(prn));
        sprintf(prn, " Report Type : %s", QStockReportType.toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, " FPS ID     : %s", ui->ldtShpId->text().toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "Name       Sch   Stock(Kgs) Issued   CB");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }
    else if(LangMode == 2)
    {
        QString tHeader = QString::fromUtf8("वर्तमान स्टॉक रिपोर्ट");
        prepareBmp(tHeader, 20, tHeader, 18, ALIGN_CENTER);


        QString tDateTime = QString::fromUtf8("तारीख") + ":" + szDashedDt + QString::fromUtf8("  समय") + ":" + szDashedTm;
        prepareBmp(tDateTime, 20, tDateTime, 18, ALIGN_CENTER);

        QString tReportType;
        if(QStockReportType == "N")
            tReportType  = QString::fromUtf8("रिपोर्ट प्रकार :  ")+ "" + QString::fromUtf8("एन.एफ.एस.ए");
        else if(QStockReportType == "S")
            tReportType  = QString::fromUtf8("रिपोर्ट प्रकार :  ")+ "" + QString::fromUtf8("पी.डी.एस");
        else if(QStockReportType == "I")
            tReportType  = QString::fromUtf8("रिपोर्ट प्रकार :  ")+ "" + QString::fromUtf8("आइ.सी.डी.एस");
        else if(QStockReportType == "M")
            tReportType  = QString::fromUtf8("रिपोर्ट प्रकार : ")+ "" + QString::fromUtf8("एम्.डी.एम्");

        prepareBmp(tReportType, 22, tReportType, 18, ALIGN_LEFT);

        QString tShopNo  = QString::fromUtf8("शॉप आईडी :")+ "" + ui->ldtShpId->text();
        prepareBmp(tShopNo, 20, tShopNo, 18, ALIGN_LEFT);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        QString tHeading  = QString::fromUtf8("नाम            ")+QString::fromUtf8("योजना  ") +QString::fromUtf8(" भण्डार(Kgs) ")+QString::fromUtf8("  प्राप्त किया   ")+QString::fromUtf8(" सी.बी");
        prepareBmp(tHeading, 22, tHeading, 18, ALIGN_LEFT);
    }
    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    QFile file("/mnt/jffs2/CurrentStockReportResp.xml");
    if(!file.exists())
    {
        qDebug() << "File Not Found";
        if(LangMode==1)
            miscObj.DisplayWideMsgBox((char *)"File Not Downloaded");
        else if(LangMode==2)
            miscObj.tel_DisplayWideMsgBox((char *)"फ़ाइल डाउनलोड नहीं हुआ ");

        return;
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error Reading File" << endl;
        return;
    }
    QDomDocument document;
    if(!document.setContent( &file ) )
    {
        qDebug( "Failed to parse the file into a DOM tree." );
        file.close();
        return;
    }
    file.close();

    int ChildCount=0,i = 0;
    QDomNodeList list ;

    list = document.elementsByTagName("astockBean");
    ChildCount = list.count();
    qDebug() << "parseCurrentStockReportRespDetails:: Main List Child Count = " << ChildCount;

    for(i = 0; i < ChildCount; i++)
    {
        QString Qallot_qty = list.at(i).childNodes().at(0).toElement().text();
        QString Qclosing_balance = list.at(i).childNodes().at(1).toElement().text();
        QString QcommNamell = list.at(i).childNodes().at(2).toElement().text(); //new tag added 15052017 vivek
        QString Qcomm_name = list.at(i).childNodes().at(3).toElement().text();
        QString Qcommoditycode = list.at(i).childNodes().at(4).toElement().text();
        QString Qissued_qty = list.at(i).childNodes().at(5).toElement().text();
        QString Qopening_balance = list.at(i).childNodes().at(6).toElement().text();
        QString Qreceived_qty = list.at(i).childNodes().at(7).toElement().text();

        QString Qscheme_desc_en = list.at(i).childNodes().at(8).toElement().text();
        QString Qscheme_desc_ll = list.at(i).childNodes().at(9).toElement().text();

        QString Qtotal_quantity = list.at(i).childNodes().at(10).toElement().text();


        qDebug()<<"***" << Qallot_qty << " " << Qclosing_balance << " "<< QcommNamell.toUtf8() << " " << Qcomm_name << " " << Qcommoditycode << " " << Qissued_qty << " " << Qopening_balance << " " << Qreceived_qty << " " <<  Qtotal_quantity <<"***"; //QcommNamell (Language Converstion from English to hind vivek 17052017)

        if(Qallot_qty == "Server Error")
        {
            if(LangMode==1)
                miscObj.DisplayWideMsgBox((char *)"Server Error");
            else if(LangMode==2)
                miscObj.tel_DisplayWideMsgBox((char *)"सर्वर एरर ");

            return;
        }

        if(Qcommoditycode.contains("Success"))
            continue;
        else if(!Qallot_qty.isNull())
        {

            if(LangMode == ENGLISH)
            {
                memset(prn, 0, sizeof(prn));

                //sprintf(prn, "%-12s %s %-5.2f  %-5.2f  %-7s",Qcomm_name.toAscii().data(),Qscheme_desc_en.toAscii().data(),atof(Qtotal_quantity.toAscii().data()) ,atof(Qissued_qty.toAscii().data()),Qclosing_balance.toAscii().data());
                sprintf(prn, "%-12s %s %-7s  %-7s  %-7s",Qcomm_name.toAscii().data(),Qscheme_desc_en.toAscii().data(),Qtotal_quantity.toAscii().data() ,Qissued_qty.toAscii().data(),Qclosing_balance.toAscii().data());

                prn_write_text((unsigned char *)prn, strlen(prn), 0);

            }
            else
            {
                char Entitlement[512]="";
                memset(Entitlement,0, sizeof(Entitlement));
                sprintf(Entitlement,"  %-9s  %-9s  %-9s ", Qtotal_quantity.toAscii().data() ,Qissued_qty.toAscii().data(),Qclosing_balance.toAscii().data());

                QString QcommNamellalign;
                QcommNamellalign.clear();
                QcommNamellalign = QString(QcommNamell).leftJustified(14, ' ');

                QString llBuff = QcommNamellalign.append(Qscheme_desc_ll)+" "+Entitlement;

                qDebug() <<"Stock Report => llBuff ===" << llBuff.length();
                prepareBmp(llBuff, 22, llBuff, 18, ALIGN_LEFT);

            }
        }
    }

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    if(LangMode == 1)
    {
        memset(prn,0x00,sizeof(prn));
        sprintf(prn,"     Public Distribution Dept  ");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn,0x00,sizeof(prn));
        sprintf(prn," (Note : Quantity's in Kgs/Ltrs)");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }
    else
    {
        QString TmpString;
        TmpString.clear();
        TmpString= QString::fromUtf8("    सार्वजनिक वितरण विभाग:    ");
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),22,QString::fromUtf8(TmpString.toUtf8()),18,ALIGN_CENTER);

        TmpString.clear();
        TmpString= QString::fromUtf8("    (नोट: किग्रा/लीटर में मात्रा)    ");
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),22,QString::fromUtf8(TmpString.toUtf8()),18,ALIGN_CENTER);
    }

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "\n\n");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    prn_paper_feed(4);

    prn_close();

    return;
}


void Widget::on_btnDailySales_clicked()
{
    DayReportFlag = 0;
    ui->btnDtSalesView->show();
    ui->tblSalesDaily->show();
    ui->pushButton_7->show();
    if(ui->tblSalesDaily->rowCount() != 0){
        for(int i = ui->tblSalesDaily->rowCount() - 1; i >= 0; i--){
            ui->tblSalesDaily->removeRow(i);
        }
    }
    ui->tblSalesDaily->clearContents();

    ui->tblSalesDaily->setColumnHidden(3,true);
    ui->tblSalesDaily->setColumnHidden(4,true);

    ui->stackedWidget->setCurrentIndex(11);
    return;
}

void Widget::on_btnBackDt_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void Widget::on_btnDtSalesView_clicked()
{
    popup_msg *dlgPopup1 = new popup_msg(this);

    if(LangMode==1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...\nDownloading Reports...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.\n रिपोर्ट डाउनलोड हो रहा है."));

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    PrepareReportRequest();

    char cUrlName[256];
    QSqlQuery query;
    query.prepare("SELECT URL FROM Settings");
    if(!query.exec())
        qDebug() << "In SendTxnToServer" << query.lastError();
    else{
        while(query.next()){
            memset(cUrlName, 0, sizeof(cUrlName));
            strcpy(cUrlName, query.value(0).toString().toAscii().data());
        }
    }

    char cmdCurl[1024];
    memset(cmdCurl, 0, sizeof(cmdCurl));
    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/ReportReq.xml", cUrlName, "/mnt/jffs2/ReportResp.xml", responseTimedOutTimeInSec.toInt());

    qDebug() << "Curl Cmd: " << cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0){
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    qDebug() << "***************/mnt/jffs2/ReportResp.xml*****************";
    system("cat /mnt/jffs2/ReportResp.xml");
    qDebug() << "************************************************";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/ReportResp.xml") < 0)
        return;

    QFile file("/mnt/jffs2/ReportResp.xml");
    if(!file.exists()){
        qDebug() << "File Not Found";
        return;
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error Reading File" << endl;
        return;
    }
    QDomDocument document;
    if(!document.setContent( &file ) ){
        qDebug( "Failed to parse the file into a DOM tree." );
        file.close();
        return;
    }
    file.close();

    int ChildCount=0,i = 0;
    QDomNodeList list ;

    list = document.elementsByTagName("drBean");
    ChildCount = list.count();
    qDebug() << "prnDayWiseReport: Main List Child Count = " << ChildCount;

    int row=0;

    ui->tblSalesDaily->setRowCount(ChildCount);

    for(i = 0; i < ChildCount; i++)
    {
//        QString Qaay = list.at(i).childNodes().at(0).toElement().text();
//        QString commNamell= list.at(i).childNodes().at(1).toElement().text();
//        QString Qcomm_name = list.at(i).childNodes().at(2).toElement().text();
//        QString Qphh = list.at(i).childNodes().at(3).toElement().text();
//        QString Qsfy = list.at(i).childNodes().at(4).toElement().text();
//        QString Qtotal_comm = list.at(i).childNodes().at(5).toElement().text();

        QString commNamell= list.at(i).childNodes().at(0).toElement().text();
        QString Qcomm_name = list.at(i).childNodes().at(1).toElement().text();
        QString sale = list.at(i).childNodes().at(2).toElement().text();
        QString schemeName = list.at(i).childNodes().at(3).toElement().text();
        QString total_cards = list.at(i).childNodes().at(4).toElement().text();
        QString Qtotal_comm = list.at(i).childNodes().at(5).toElement().text();

        if(!sale.isNull())
        {
            row=i;

            if(LangMode == 1)
                ui->tblSalesDaily->setItem(row, 0, new QTableWidgetItem(Qcomm_name));
            else if(LangMode == 2)
                ui->tblSalesDaily->setItem(row, 0, new QTableWidgetItem(commNamell));

            ui->tblSalesDaily->item(row, 0)->setFlags( ui->tblSalesDaily->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tblSalesDaily->item(row, 0)->setFlags( ui->tblSalesDaily->item(row,0)->flags() | Qt::ItemIsSelectable);

            qDebug()<<"????? &&&&&"<<endl;

//            ui->tblSalesDaily->setItem(row, 1, new QTableWidgetItem(Qaay));
            ui->tblSalesDaily->setItem(row, 1, new QTableWidgetItem(schemeName));
            ui->tblSalesDaily->item(row, 1)->setFlags( ui->tblSalesDaily->item(row,1)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tblSalesDaily->item(row, 1)->setFlags( ui->tblSalesDaily->item(row,1)->flags() | Qt::ItemIsSelectable);

//            ui->tblSalesDaily->setItem(row, 2, new QTableWidgetItem(Qsfy));
            ui->tblSalesDaily->setItem(row, 2, new QTableWidgetItem(sale));
            ui->tblSalesDaily->item(row, 2)->setFlags( ui->tblSalesDaily->item(row,2)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tblSalesDaily->item(row, 2)->setFlags( ui->tblSalesDaily->item(row,2)->flags() | Qt::ItemIsSelectable);

//            ui->tblSalesDaily->setItem(row,3, new QTableWidgetItem(Qphh));
            ui->tblSalesDaily->setItem(row,3, new QTableWidgetItem(total_cards));
            ui->tblSalesDaily->item(row, 3)->setFlags( ui->tblSalesDaily->item(row,3)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tblSalesDaily->item(row, 3)->setFlags( ui->tblSalesDaily->item(row,3)->flags() | Qt::ItemIsSelectable);

            ui->tblSalesDaily->setItem(row, 4, new QTableWidgetItem(Qtotal_comm)); // hidden -BSK.
            ui->tblSalesDaily->item(row, 4)->setFlags( ui->tblSalesDaily->item(row,4)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tblSalesDaily->item(row, 4)->setFlags( ui->tblSalesDaily->item(row,4)->flags() | Qt::ItemIsSelectable);

            ui->tblSalesDaily->setColumnWidth(0, 110);
            ui->tblSalesDaily->setColumnWidth(1, 60);
            ui->tblSalesDaily->setColumnWidth(2, 60);
            ui->tblSalesDaily->setColumnWidth(3, 60);
//            ui->tblSalesDaily->setColumnWidth(4, 50);
//            ui->tblSalesDaily->setColumnWidth(5, 50);
            ui->tblSalesDaily->setColumnHidden(3,true);
            ui->tblSalesDaily->setColumnHidden(4,true);

            ui->tblSalesDaily->setRowHeight(row,20);
        }
    }
    return;
}

void Widget::PrepareReportRequest()
{
    database_queries obj;
    memset(SessionId,0,sizeof(SessionId));

    if(obj.getSessionId(SessionId) < 0)
    {
        miscObj.DisplayMsgBox("Session Id not found\nPlease try again");
        return;
    }

    system("rm /mnt/jffs2/ReportResp.xml");
    system("rm /mnt/jffs2/ReportReq.xml");

    QFile file("/mnt/jffs2/ReportReq.xml");

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug() << "/mnt/jffs2/ReportReq.xml File Open Failed";
        return;
    }

    QTextStream out(&file);

    char buff[2048];
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getDailyReport><shop_no>%s</shop_no><from_date>%s</from_date><token>%s</token><fpsSessionId>%s</fpsSessionId><stateCode>%s</stateCode></ns1:getDailyReport></SOAP-ENV:Body></SOAP-ENV:Envelope>", ui->ldtShpId->text().toAscii().data(), ui->dateEdit->text().toAscii().data(),HD_PASSWORD,SessionId,stateCode.toStdString().c_str());

    out << buff;

    file.close();

    return;
}

void Widget::on_btnOkDt_clicked()
{
    if(DayReportFlag==2)
    {
        PrintOfflineDayReport();
        return;
    }
    if(ui->tblSalesDaily->rowCount() == 0)
        return;

    QFile file("/mnt/jffs2/ReportResp.xml");

    if(!file.exists())
    {
        qDebug() << "File Not Found";
        return;
    }
    prnDayWiseReport();
    system("rm /mnt/jffs2/ReportResp.xml");

    ui->stackedWidget->setCurrentIndex(1);

    return;
}

void Widget::prnDayWiseReport()
{
    char RTC[32], DATE[16], TIME[16], JULIAN_DAYS[8];
    char szDashedDt[16],szDashedTm[16];
    char prn[1024];

    memset(RTC, 0, sizeof(RTC));
    memset(DATE, 0, sizeof(DATE));
    memset(TIME, 0, sizeof(TIME));
    memset(JULIAN_DAYS, 0, sizeof(JULIAN_DAYS));

    GetRTC_Params(RTC, DATE, TIME, JULIAN_DAYS);

    memset(szDashedDt, 0, sizeof(szDashedDt));
    memset(szDashedTm, 0, sizeof(szDashedTm));

    miscObj.DDMMYYYY2DD_MM_YYYY(DATE, szDashedDt);
    miscObj.HHMMSS2HH_MM_SS(TIME, szDashedTm);

    prn_open();
    if(prn_paperstatus() < 0)
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया कागज डालें.");
        prn_close();
        return;
    }
    if(miscObj.Battery_check() != 0)
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद को प्रिंट करने में असमर्थ\nअपनी बैटरी चार्ज करें");
        prn_close();
        return;

    }

    LogoPrint objLogo;
    objLogo.PrintLogo(stateCode.toInt());

    if(LangMode == 1)
    {
        memset(prn, 0, sizeof(prn));
        sprintf(prn, "     DAY REPORT");
        prn_write_text((unsigned char *)prn, strlen(prn), 2);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "   Date: %s        Time: %s", szDashedDt, szDashedTm);
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "   Day Report Date: %s ", ui->dateEdit->text().toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "   FPS ID: %s", ui->ldtShpId->text().toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "Commodity    Scheme    Sale      ");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);


        memset(prn, 0, sizeof(prn));
        sprintf(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }
    else if(LangMode == 2)
    {

        QString header=QString::fromUtf8("डे रिपोर्ट");
        prepareBmp(header, 22, header, 20, ALIGN_CENTER);

        QString tDateTime = QString::fromUtf8("तारीख ") + ":" + szDashedDt + QString::fromUtf8("   समय") + ":" + szDashedTm;
        prepareBmp(tDateTime, 22, tDateTime, 20, ALIGN_LEFT);

        QString DayreportDate = QString::fromUtf8("डे रिपोर्ट की तारीख") + ":" +ui->dateEdit->text();
        prepareBmp(DayreportDate, 22, DayreportDate, 20, ALIGN_LEFT);

        QString tShopNo  = QString::fromUtf8("शॉप आईडी :  ")+ "" + ui->ldtShpId->text();
        prepareBmp(tShopNo, 22, tShopNo, 20, ALIGN_LEFT);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
//        QString subheader = QString::fromUtf8(" नाम  एएवाई  एसएफवाई  पीएचएच  कुल");
        QString subheader = QString::fromUtf8("वस्तु    योजना    बिक्री");

        prepareBmp(QString::fromUtf8(subheader.toUtf8()),20,QString::fromUtf8(subheader.toUtf8()),17, ALIGN_LEFT);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }

    QFile file("/mnt/jffs2/ReportResp.xml");

    if(!file.exists())
    {
        qDebug() << "File Not Found";
        return;
    }

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error Reading File" << endl;
        return;
    }
    QDomDocument document;
    if(!document.setContent( &file ) )
    {
        qDebug( "Failed to parse the file into a DOM tree." );
        file.close();
        return;
    }
    file.close();

    int ChildCount=0,i = 0;
    QDomNodeList list ;

    list = document.elementsByTagName("drBean");
    ChildCount = list.count();
    qDebug() << "prnDayWiseReport: Main List Child Count = " << ChildCount;

    for(i = 0; i < ChildCount; i++)
    {
//        QString Qaay = list.at(i).childNodes().at(0).toElement().text();
//        QString commNamell= list.at(i).childNodes().at(1).toElement().text();
//        QString Qcomm_name = list.at(i).childNodes().at(2).toElement().text();
//        QString Qphh = list.at(i).childNodes().at(3).toElement().text();
//        QString Qsfy = list.at(i).childNodes().at(4).toElement().text();
//        QString Qtotal_comm = list.at(i).childNodes().at(5).toElement().text();

        QString commNamell= list.at(i).childNodes().at(0).toElement().text();
        QString Qcomm_name = list.at(i).childNodes().at(1).toElement().text();
        QString sale = list.at(i).childNodes().at(2).toElement().text();
        QString schemeName = list.at(i).childNodes().at(3).toElement().text();
        QString total_cards = list.at(i).childNodes().at(4).toElement().text();
        QString Qtotal_comm = list.at(i).childNodes().at(5).toElement().text();


//        if(Qapl.contains("Success"))
//            continue;
        if(!sale.isNull())
        {
            if(LangMode == ENGLISH )
            {
                memset(prn, 0, sizeof(prn));
                //sprintf(prn, "%-10s %-0.2f %-0.2f %-0.2f %-0.2f",Qcomm_name.toAscii().data(),atof(Qaay.toAscii().data()),atof(Qapl.toAscii().data()),atof(Qphh.toAscii().data()),atof(Qtotal_comm.toAscii().data()));
                sprintf(prn, "%-10s %-7s %-7s",Qcomm_name.toAscii().data(),schemeName.toAscii().data(),sale.toAscii().data());
                prn_write_text((unsigned char *)prn, strlen(prn), 0);
            }
            else if(LangMode == 2)
            {
                memset(prn, 0, sizeof(prn));
//                sprintf(prn, "%-6s  %-6s  %-6s  %-6s",Qaay.toAscii().data(),Qsfy.toAscii().data(),Qphh.toAscii().data(),Qtotal_comm.toAscii().data());
                sprintf(prn, "%-7s %-7s",schemeName.toAscii().data(),sale.toAscii().data());
                QString QcommNamellalign;
                QcommNamellalign.clear();
                QcommNamellalign = QString(commNamell).leftJustified(11, ' ');

                QString print = QcommNamellalign+" "+prn;
                prepareBmp(print, 22, print, 18, ALIGN_LEFT);

            }
        }
    }

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    if(LangMode == 1)
    {
        memset(prn,0x00,sizeof(prn));
        sprintf(prn,"     Public Distribution Dept  ");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn,0x00,sizeof(prn));
        sprintf(prn," (Note : Quantity's in Kgs/Ltrs)");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }
    else
    {
        QString TmpString;
        TmpString.clear();
        TmpString= QString::fromUtf8("    सार्वजनिक वितरण विभाग:    ");
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),22,QString::fromUtf8(TmpString.toUtf8()),18,ALIGN_CENTER);

        TmpString.clear();
        TmpString= QString::fromUtf8("    (नोट: किग्रा/लीटर में मात्रा)    ");
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),22,QString::fromUtf8(TmpString.toUtf8()),18,ALIGN_CENTER);
    }

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "\n\n");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    prn_paper_feed(4);
    prn_close();

    return;
}


