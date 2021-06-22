#include "include/widget.h"
#include "ui_widget.h"
#include "include/misc.h"
#include "include/usb.h"
#include "include/gprs.h"
#include "include/database.h"
#include "include/database_queries.h"

#include "include/popup_msg.h"
#include "include/popup_nothing.h"
#include "include/ui_popup_msg.h"
#include "include/ui_popup_nothing.h"

extern char  HD_PASSWORD[64];
extern pData pid_info;
extern QString Servicename;

QString inspreceiptId;

void Widget::on_btn_inspector_clicked()
{
    char cReqfilename[50]="", cRespfilename[50]="",curlcommand[1000]="";
    int ret = 0;
    GPRS objGprs;

    if(objGprs.Check_Network_Available() < 0)
    {
        return;
    }
    popup_msg *dlgPopup = new popup_msg(this);
    if(LangMode == 1)
    {
        dlgPopup->ui->lblPopup->setText("Please Wait...\nDownloading Inspector Details...");

    }
    else if(LangMode == 2)
    {
        dlgPopup->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें. \nइंस्पेक्टर विवरण डाउनलोड हो रहा है."));

    }

    dlgPopup->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    strcpy(cReqfilename,"/root/getinspectordetails_req.xml");
    strcpy(cRespfilename,"/root/getinspectordetails_resp.xml");

    system("rm /root/getinspectordetails_req.xml");
    system("rm /root/getinspectordetails_resp.xml");

    ret = preparegetInspectorDetailsReq(cReqfilename);
    qDebug()<<"ret form Prepare Out File : "<<ret;

    char catCmd[128];
    memset(catCmd, 0, sizeof(catCmd));
    sprintf(catCmd, "cat %s", cReqfilename);
    system(catCmd);

    //added on RD 2.0 20/11/2017

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

    sprintf(curlcommand,"curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", cReqfilename, cUrlName ,cRespfilename,60);

    qDebug() << "\nCurl Cmd : " << curlcommand;

    int sysret = system(curlcommand);
    qDebug() << "sysret = " << sysret;

    if(SystemCurlResponse(sysret, EPDS_SERVER) < 0)
    {
        dlgPopup->close();
        dlgPopup->deleteLater();
        return;
    }

    dlgPopup->close();
    dlgPopup->deleteLater();

    system("cat /root/getinspectordetails_resp.xml");

    if(ParsingFirstCondition(cRespfilename) != 0)
    {
        return;
    }
    if(parsegetInspectorDetailsResp(cRespfilename) != 0)
    {
        if(LangMode==1)
            miscObj.DisplayMsgBox((char *)"Incorrect XML");
        if(LangMode==2)
            miscObj.tel_DisplayMsgBox((char*)"गलत XML");
        return ;
    }

    if(showInspApprovals() != 0)
        return;

    if(showInspCommDetails() != 0)
        return;
    else
        ui->stackedWidget->setCurrentIndex(44);
}

int Widget::preparegetInspectorDetailsReq(char *FileName)
{
    QFile file(FileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug()<< FileName << " file Open failed";
        return -1;
    }

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0)
    {
        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }
    char szShopId[16];
    memset(szShopId, 0, 16);
    getShopId(szShopId);

    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL)
    {
        qDebug() << "File Not Found" << endl;
        return -1;
    }

    fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ser=\"http://service.fetch.rationcard/\">");
    fprintf(fp, "<%s/>", "soapenv:Header");
    fprintf(fp, "<%s>", "soapenv:Body");
    fprintf(fp, "<%s>", "ser:getInspectorDetails");
    fprintf(fp, "<fpsId>%s</fpsId>",szShopId);
    fprintf(fp, "<fpsSessionId>%s</fpsSessionId>",SessionId );
    fprintf(fp, "<password>%s</password>", HD_PASSWORD);
    fprintf(fp, "</%s>", "ser:getInspectorDetails");
    fprintf(fp, "</%s>", "soapenv:Body");
    fprintf(fp, "</%s>", "soapenv:Envelope");
    fclose(fp);
    return 0;
}

int Widget::parsegetInspectorDetailsResp(char *filename)
{
    QString QapproveKey, QapproveValue;
    QString QclosingBalance, QcommCode, QcommNameEn, QcommNamell;
    QString Qinsp_incharge_bfd1, Qinsp_incharge_bfd2, Qinsp_incharge_bfd3, Qinsp_incharge_fusion, Qinsp_incharge_mobile, Qinsp_incharge_name, Qinsp_incharge_uid ;
    QString Qinsp_nom1_bfd1, Qinsp_nom1_bfd2, Qinsp_nom1_bfd3, Qinsp_nom1_fusion ,Qinsp_nom1_mobile, Qinsp_nom1_name, Qinsp_nom1_uid ;
    QString Qinsp_nom2_bfd1, Qinsp_nom2_bfd2, Qinsp_nom2_bfd3, Qinsp_nom2_fusion, Qinsp_nom2_mobile, Qinsp_nom2_name, Qinsp_nom2_uid;

    QSqlQuery query;

    query.exec("DELETE FROM InspApprovalsDtls");
    query.clear();
    query.exec("DELETE FROM InspCommDtls");
    query.clear();

    query.exec("DELETE FROM InspList");
    query.clear();
    query.exec("DELETE FROM InspNominee");
    int mulflag = 0;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In parseDealerDetails, Error Reading File";
        return -1;
    }

    if(file.size() == 0)
        return -1;

    QXmlStreamReader xml(&file);

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "approveKey")
            {
                xml.readNext();
                QapproveKey = xml.text().toString();
                qDebug() << "approveKey : " << QapproveKey;

            }
            if(xml.name() == "approveValue")
            {
                xml.readNext();
                QapproveValue = xml.text().toString();
                qDebug() << "approveValue " << QapproveValue;

                //Insert Values
                query.clear();
                query.prepare("INSERT INTO InspApprovalsDtls(approveKey, approveValue) VALUES(:approveKey, :approveValue)");
                query.bindValue(":approveKey", QapproveKey);
                query.bindValue(":approveValue",QapproveValue);
                if(!query.exec())
                    qDebug() <<"Failed To Insert Into InspApprovalsDtls";

            }
            /********************commDetails*/
            if(xml.name() == "closingBalance")
            {
                xml.readNext();
                QclosingBalance = xml.text().toString();
                qDebug() << "closingBalance : " << QclosingBalance;
            }

            if(xml.name() == "commCode")
            {
                xml.readNext();
                QcommCode = xml.text().toString();
                qDebug() << "commCode : " << QcommCode;
            }

            if(xml.name() == "commNameEn")
            {
                xml.readNext();
                QcommNameEn = xml.text().toString();
                qDebug() << "commNameEn : " << QcommNameEn;
            }
            if(xml.name() == "commNamell")
            {
                xml.readNext();
                QcommNamell = xml.text().toString();
                qDebug() << "commNamell : " << QcommNamell.toUtf8();

                //Insert CommDetails
                query.clear();
                query.prepare("INSERT INTO InspCommDtls(closingBalance, commCode, commNameEn, commNamell) VALUES(:closingBalance, :commCode, :commNameEn, :commNamell)");
                query.bindValue(":closingBalance", QclosingBalance);
                query.bindValue(":commCode", QcommCode);
                query.bindValue(":commNameEn", QcommNameEn);
                query.bindValue(":commNamell", QcommNamell);
                if(!query.exec())
                    qDebug()<<"Failed to Insert Value in InspCommDtls Tables";

            }

            /************** InsList start*******************/
            /* Service Changed Inspector Details and Nominee Details Need to Remove vivek 01072017*/
            if(xml.name() == "insp_incharge_bfd1")
            {
                xml.readNext();
                Qinsp_incharge_bfd1 = xml.text().toString();
                qDebug() << "insp_incharge_bfd1 : " << Qinsp_incharge_bfd1;
            }

            if(xml.name() == "insp_incharge_bfd2")
            {
                xml.readNext();
                Qinsp_incharge_bfd2 = xml.text().toString();
                qDebug() << "insp_incharge_bfd2 = " << Qinsp_incharge_bfd2;

            }

            if(xml.name() == "insp_incharge_bfd3")
            {
                xml.readNext();
                Qinsp_incharge_bfd3 = xml.text().toString();
                qDebug() << "insp_incharge_bfd3 : " << Qinsp_incharge_bfd3;

                //insert inspector details
            }
            ///
            if(xml.name() == "insp_incharge_fusion")
            {
                xml.readNext();
                Qinsp_incharge_fusion = xml.text().toString();
                qDebug() << "insp_incharge_fusion : " << Qinsp_incharge_fusion;
            }
            if(xml.name() == "insp_incharge_mobile")
            {
                xml.readNext();
                Qinsp_incharge_mobile = xml.text().toString();
                qDebug() << "insp_incharge_mobile : " << Qinsp_incharge_mobile;
            }

            if(xml.name() == "insp_incharge_name")
            {
                xml.readNext();
                Qinsp_incharge_name = xml.text().toString();
                qDebug() << "insp_incharge_name = " << Qinsp_incharge_name;

            }

            if(xml.name() == "insp_incharge_uid")
            {
                xml.readNext();
                Qinsp_incharge_uid = xml.text().toString();
                qDebug() << "insp_incharge_uid : " << Qinsp_incharge_uid;

                //insert inspector details
            }

            // Nominee Details
            if(xml.name() == "insp_nom1_bfd1")
            {
                xml.readNext();
                Qinsp_nom1_bfd1 = xml.text().toString();
                qDebug() << "insp_nom1_bfd1 : " << Qinsp_nom1_bfd1;
            }

            if(xml.name() == "insp_nom1_bfd2")
            {
                xml.readNext();
                Qinsp_nom1_bfd2 = xml.text().toString();
                qDebug() << "insp_nom1_bfd2 : " << Qinsp_nom1_bfd2;
            }

            if(xml.name() == "insp_nom1_bfd3")
            {
                xml.readNext();
                Qinsp_nom1_bfd3 = xml.text().toString();
                qDebug() << "insp_nom1_bfd3 : " << Qinsp_nom1_bfd3;

            }

            if(xml.name() == "insp_nom1_fusion")
            {
                xml.readNext();
                Qinsp_nom1_fusion = xml.text().toString();
                qDebug() << "insp_nom1_fusion : " << Qinsp_nom1_fusion;

            }

            if(xml.name() == "insp_nom1_mobile")
            {
                xml.readNext();
                Qinsp_nom1_mobile = xml.text().toString();
                qDebug() << "insp_nom1_mobile : " << Qinsp_nom1_mobile;


            }

            if(xml.name() == "insp_nom1_name")
            {
                xml.readNext();
                Qinsp_nom1_name = xml.text().toString();
                qDebug() << "insp_nom1_name : " << Qinsp_nom1_name;
                if(Qinsp_nom1_name != "")
                {
                    mulflag = 1;
                    qDebug()<<"Mul Flag 1 = "<< mulflag;
                }
            }

            if(xml.name() == "insp_nom1_uid")
            {
                xml.readNext();
                Qinsp_nom1_uid = xml.text().toString();
                qDebug() << "insp_nom2_uid : " << Qinsp_nom1_uid;

            }

            if(xml.name() == "insp_nom2_bfd1")
            {
                xml.readNext();
                Qinsp_nom2_bfd1 = xml.text().toString();
                qDebug() << "insp_nom2_bfd1 : " << Qinsp_nom2_bfd1;
            }

            if(xml.name() == "insp_nom2_bfd2")
            {
                xml.readNext();
                Qinsp_nom2_bfd2 = xml.text().toString();
                qDebug() << "insp_nom2_bfd2 : " << Qinsp_nom2_bfd2;
            }

            if(xml.name() == "insp_nom2_bfd3")
            {
                xml.readNext();
                Qinsp_nom2_bfd3 = xml.text().toString();
                qDebug() << "insp_nom2_bfd3 : " << Qinsp_nom2_bfd3;

            }

            if(xml.name() == "insp_nom2_fusion")
            {
                xml.readNext();
                Qinsp_nom2_fusion = xml.text().toString();
                qDebug() << "insp_nom2_fusion : " << Qinsp_nom2_fusion;

            }

            if(xml.name() == "insp_nom2_mobile")
            {
                xml.readNext();
                Qinsp_nom2_mobile = xml.text().toString();
                qDebug() << "insp_nom2_mobile : " << Qinsp_nom2_mobile;
                if(Qinsp_nom2_mobile != "")
                {
                    mulflag = 2;
                    qDebug()<<"Mul Flag 2 = "<< mulflag;
                }

            }

            if(xml.name() == "insp_nom2_name")
            {
                xml.readNext();
                Qinsp_nom2_name = xml.text().toString();
                qDebug() << "insp_nom1_name : " << Qinsp_nom2_name;
            }

            if(xml.name() == "insp_nom2_uid")
            {
                xml.readNext();
                Qinsp_nom2_uid = xml.text().toString();
                qDebug() << "insp_nom1_uid : " << Qinsp_nom2_uid;

            }

        }
    }

    file.close();
    //Insert Incharge
    query.clear();
    query.prepare("INSERT INTO InspList(incharge_mobile,incharge_name, incharge_uid, insp_incharge_bfd1, insp_incharge_bfd2, insp_incharge_bfd3, insp_incharge_fusion) VALUES(:incharge_mobile,:incharge_name, :incharge_uid, :insp_incharge_bfd1, :insp_incharge_bfd2, :insp_incharge_bfd3, :insp_incharge_fusion)");
    query.bindValue(":incharge_mobile",Qinsp_incharge_mobile);
    query.bindValue(":incharge_name",Qinsp_incharge_name);
    query.bindValue(":incharge_uid",Qinsp_incharge_uid);
    query.bindValue(":insp_incharge_bfd1",Qinsp_incharge_bfd1);
    query.bindValue(":insp_incharge_bfd2",Qinsp_incharge_bfd2);
    query.bindValue(":insp_incharge_bfd3",Qinsp_incharge_bfd3);
    query.bindValue(":insp_incharge_fusion",Qinsp_incharge_fusion);
    if(!query.exec())
        qDebug() << "Failed to insert values into InspList ====>" << query.lastError();

    //Insert Nominee
    if(mulflag == 1)
    {
        query.clear();
        query.prepare("INSERT INTO InspNominee (insp_nom_mobile,insp_nom_name, insp_nom_uid, insp_nom_bfd1, insp_nom_bfd2, insp_nom_bfd3, insp_nom_fusion) VALUES(:insp_nom_mobile,:insp_nom_name, :insp_nom_uid, :insp_nom_bfd1, :insp_nom_bfd2, :insp_nom_bfd3, :insp_nom_fusion)");
        query.bindValue(":insp_nom_mobile",Qinsp_nom1_mobile);
        query.bindValue(":insp_nom_name",Qinsp_nom1_name);
        query.bindValue(":insp_nom_uid",Qinsp_nom1_uid);
        query.bindValue(":insp_nom_bfd1",Qinsp_nom1_bfd1);
        query.bindValue(":insp_nom_bfd2",Qinsp_nom1_bfd2);
        query.bindValue(":insp_nom_bfd3",Qinsp_nom1_bfd3);
        query.bindValue(":insp_nom_fusion",Qinsp_nom1_fusion);
        if(!query.exec())
            qDebug() << "mulFlag = 1 ====>" << query.lastError();
    }
    else if(mulflag == 2)
    {
        query.clear();
        query.prepare("INSERT INTO InspNominee (insp_nom_mobile,insp_nom_name, insp_nom_uid, insp_nom_bfd1, insp_nom_bfd2, insp_nom_bfd3, insp_nom_fusion) VALUES(:insp_nom_mobile,:insp_nom_name, :insp_nom_uid, :insp_nom_bfd1, :insp_nom_bfd2, :insp_nom_bfd3, :insp_nom_fusion)");
        query.bindValue(":insp_nom_mobile",Qinsp_nom1_mobile);
        query.bindValue(":insp_nom_name",Qinsp_nom1_name);
        query.bindValue(":insp_nom_uid",Qinsp_nom1_uid);
        query.bindValue(":insp_nom_bfd1",Qinsp_nom1_bfd1);
        query.bindValue(":insp_nom_bfd2",Qinsp_nom1_bfd2);
        query.bindValue(":insp_nom_bfd3",Qinsp_nom1_bfd3);
        query.bindValue(":insp_nom_fusion",Qinsp_nom1_fusion);
        if(!query.exec())
            qDebug() << "mulFlag(2) = 1 ===> " << query.lastError();

        query.clear();
        query.prepare("INSERT INTO InspNominee (insp_nom_mobile,insp_nom_name, insp_nom_uid, insp_nom_bfd1, insp_nom_bfd2, insp_nom_bfd3, insp_nom_fusion) VALUES(:insp_nom_mobile,:insp_nom_name, :insp_nom_uid, :insp_nom_bfd1, :insp_nom_bfd2, :insp_nom_bfd3, :insp_nom_fusion)");
        query.bindValue(":insp_nom_mobile",Qinsp_nom2_mobile);
        query.bindValue(":insp_nom_name",Qinsp_nom2_name);
        query.bindValue(":insp_nom_uid",Qinsp_nom2_uid);
        query.bindValue(":insp_nom_bfd1",Qinsp_nom2_bfd1);
        query.bindValue(":insp_nom_bfd2",Qinsp_nom2_bfd2);
        query.bindValue(":insp_nom_bfd3",Qinsp_nom2_bfd3);
        query.bindValue(":insp_nom_fusion",Qinsp_nom2_fusion);
        if(!query.exec())
            if(!query.exec())
                qDebug() << "mulFlag(2) = 2 ===> " << query.lastError();

    }
    return 0;
}

int Widget::showInspApprovals()
{
    qDebug()<<"inside showInspApprovals";

    QSqlQuery query;
    QString cProceed, cSeized;
    int count=0;
    query.prepare("SELECT COUNT(approveKey) FROM InspApprovalsDtls");
    if(!query.exec())
    {
        qDebug()<<"getApprovestatus :" <<query.lastError();
        return -1;
    }
    else
    {
        while(query.next())
        {
            count = query.value(0).toInt(0);
        }
    }

    qDebug() <<"count ="<< count;

    if(count == 0)
    {
        qDebug()<<"Approvals Value not Available";
        miscObj.DisplayMsgBox((char*)"Something Went Wrong..... \nPlease Try Again");
        return -2;
    }
    else
    {
        query.clear();
        query.prepare("SELECT approveValue FROM InspApprovalsDtls");
        if(!query.exec())
        {
            qDebug()<<"showInspApprovals :" <<query.lastError();
            return -1;
        }
        else
        {
            query.next();
            cProceed = query.value(0).toString();
            query.next();
            cSeized = query.value(0).toString();

        }
    }
    ui->radioButton_OK_inspcVerify->setText(cProceed);
    ui->radioButton_seized_inspVer->setText(cSeized);

    return 0;
}

int Widget::showInspCommDetails()
{
    qDebug() <<"inside showInspCommDetails";
    QSqlQuery query;
    QString CommNamll;

    int tableCount = 0;
    query.clear();
    query.prepare("SELECT COUNT(commCode) FROM InspCommDtls");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            tableCount = query.value(0).toInt(0);
        }
    }
    qDebug()<<"tablcount="<<tableCount;
    if(tableCount == 0)
    {

        return -1;
    }

    ui->tableWidget_InspectorVerificationn->showMaximized();
    ui->tableWidget_InspectorVerificationn->clearContents();
    ui->tableWidget_InspectorVerificationn->setRowCount(tableCount);
    char buff[128],buffvar[128];
    int row = 0;
    query.clear();
    query.prepare("SELECT commNameEn, closingBalance, commCode, commNamell  FROM InspCommDtls");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {

            if(LangMode == 1)
            {
                memset(buff,0,sizeof(buff));
                strcpy(buff, query.value(0).toString().toAscii().data());
                qDebug() << "0 commNameEn = " << buff;
                ui->tableWidget_InspectorVerificationn->setItem(row, 0, new QTableWidgetItem(buff));
            }
            else if(LangMode == 2)
            {
                memset(buff,0,sizeof(buff));
                CommNamll = query.value(3).toString();
                //qDebug() << "0 commNamell = " << CommNamll;
                ui->tableWidget_InspectorVerificationn->setItem(row, 0, new QTableWidgetItem(CommNamll));
            }
            ui->tableWidget_InspectorVerificationn->item(row, 0)->setFlags( ui->tableWidget_InspectorVerificationn->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidget_InspectorVerificationn->item(row, 0)->setFlags( ui->tableWidget_InspectorVerificationn->item(row,0)->flags() | Qt::ItemIsSelectable);


            memset(buff,0,sizeof(buff));
            memset(buffvar,0,sizeof(buffvar));

            strcpy(buff, query.value(1).toString().toAscii().data());
            qDebug() << "1 closingBalance = " << buff;
            strcpy(buffvar,buff);

            ui->tableWidget_InspectorVerificationn->setItem(row, 1, new QTableWidgetItem(buff));
            ui->tableWidget_InspectorVerificationn->item(row, 1)->setFlags( ui->tableWidget_InspectorVerificationn->item(row,1)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidget_InspectorVerificationn->item(row, 1)->setFlags( ui->tableWidget_InspectorVerificationn->item(row,1)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, "0");
            sprintf(buff,"%.2f",atof(buff));
            qDebug() << "2 observedClosingBalance = " << buff;
            ui->tableWidget_InspectorVerificationn->setItem(row, 2, new QTableWidgetItem(buff));
            ui->tableWidget_InspectorVerificationn->item(row, 2)->setData(Qt::BackgroundColorRole, QVariant(QColor(Qt::green)));
            ui->tableWidget_InspectorVerificationn->item(row, 2)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled); //test
            ui->tableWidget_InspectorVerificationn->item(row, 2)->setFlags(ui->tableWidget_InspectorVerificationn->item(row, 2)->flags() ^ Qt::ItemIsEditable);

            memset(buff,0,sizeof(buff));

            sprintf(buff,"%.2f",atof(buffvar));

            qDebug() << "3 Variation = " << buff;

            ui->tableWidget_InspectorVerificationn->setItem(row, 3, new QTableWidgetItem("0.00"));
            ui->tableWidget_InspectorVerificationn->item(row, 3)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled); //test
            ui->tableWidget_InspectorVerificationn->item(row, 3)->setFlags(ui->tableWidget_InspectorVerificationn->item(row, 3)->flags() ^ Qt::ItemIsEditable);

            //Hidden Items
            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(2).toString().toAscii().data());
            qDebug() << "4 CommCode = " << buff;
            ui->tableWidget_InspectorVerificationn->setItem(row, 4, new QTableWidgetItem(buff));
            ui->tableWidget_InspectorVerificationn->item(row, 4)->setFlags( ui->tableWidget_InspectorVerificationn->item(row,4)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidget_InspectorVerificationn->item(row, 4)->setFlags( ui->tableWidget_InspectorVerificationn->item(row,4)->flags() | Qt::ItemIsSelectable);

            row++;
        }
    }
    //TableWidget Configuration
    ui->tableWidget_InspectorVerificationn->setColumnWidth(0,80);
    ui->tableWidget_InspectorVerificationn->setColumnWidth(1,70);
    ui->tableWidget_InspectorVerificationn->setColumnWidth(2,100);
    ui->tableWidget_InspectorVerificationn->setColumnWidth(3,100);

    ui->tableWidget_InspectorVerificationn->verticalHeader()->hide();
    //Hide columns
    ui->tableWidget_InspectorVerificationn->setColumnHidden(4, true);
    return 0;
}

void Widget::on_btn_Back_inspectorVerify_clicked()
{
    ui->stackedWidget->setCurrentIndex(1); //go to Home

}

void Widget::on_btn_Submit_InspectorVerify_clicked()
{
    CMisc objMisc;
    char RadioBtnok[10]="",RadioBtnSeized[10]="",AlertMsg[100];

    ui->lEdit_InpsAuth->clear();
    ui->lEdit_InpsAuth->setFocus();

    sprintf(RadioBtnok,"%s",ui->radioButton_OK_inspcVerify->text().toAscii().data());
    sprintf(RadioBtnSeized, "%s", ui->radioButton_seized_inspVer->text().toAscii().data());

    if(ui->radioButton_OK_inspcVerify->isChecked() != true && ui->radioButton_seized_inspVer->isChecked() != true)
    {
        if(LangMode == 1)
        {
            sprintf(AlertMsg,"Please Select Any Approval Status From \n \"%s\" or \"%s\" ",RadioBtnok,RadioBtnSeized);
            objMisc.DisplayMsgBox((char *)AlertMsg);
            //return;
        }
        else
        {
            objMisc.tel_DisplayMsgBox((char *)"ठीक / कृपया सत्रों के अनुमोदित स्तर का चयन करें");
        }
        return;
    }
    int receiveflag=0;
    int row = ui->tableWidget_InspectorVerificationn->rowCount();

    for(int i = 0; i<row; i++)
    {
        if(atof(ui->tableWidget_InspectorVerificationn->item(i,2)->text().toAscii().data()) > 0)
            receiveflag++;
    }
    if(receiveflag == 0)
    {
        char buff[128];
        memset(buff, 0, sizeof(buff));
        sprintf(buff,"%s", "Please Enter Atleast one Observation");
        miscObj.DisplayWideMsgBox((char *)buff);
        return;
    }
    ui->stackedWidget->setCurrentIndex(45); //go to inspector auth

    return ;
}

void Widget::on_tableWidget_InspectorVerificationn_cellChanged(int row, int column)
{
    CMisc objMisc;
    char buf1[625]="";
    if(ui->stackedWidget->currentIndex() == 44)
    {
        if(column == 2)
        {
            qDebug()<<"1";
            double fCB,fObservdQty, fvariation;
            char cVariation[50],cfObservdQty[50];
            memset(cVariation,0,sizeof(cVariation));
            fCB = ui->tableWidget_InspectorVerificationn->item(row,1)->text().toFloat();
            qDebug()<<"2";

            memset(buf1,0,sizeof(buf1));
            strcpy(buf1,ui->tableWidget_InspectorVerificationn->item(row,column)->text().toAscii().data());
            qDebug()<<"3";
            qDebug()<<" ENTERD qty"<<buf1;

            for(int i=0;i<strlen(buf1);i++)
            {
                if(buf1[i] != '.' && buf1[i] != '0' && buf1[i] != '1' && buf1[i] != '2' && buf1[i] != '3' && buf1[i] != '4' && buf1[i] != '5' && buf1[i] != '6' && buf1[i] != '7' && buf1[i] != '8' && buf1[i] != '9')
                {
                    ui->tableWidget_InspectorVerificationn->item(row,column)->setText("0.00");
                    return;
                }
            }
            /*qDebug()<<"4";
            int res_sys = dotremoval(ui->tableWidget_InspectorVerificationn->item(row,column)->text().toAscii().data());
            qDebug()<<"5";
            if(res_sys != 0)
            {
                if(LangMode==1)
                    pop_up((char *)"Please Enter Valid Quantity");
                if(LangMode==2)
                    pop_up((char *)"कृपया मात्रा को \nठीक से दर्ज कीजिये");
                sleep(3);

                return;
            }*/
            qDebug()<<"6";
            fObservdQty = ui->tableWidget_InspectorVerificationn->item(row,2)->text().toFloat();
            qDebug()<<"7";
            sprintf(cfObservdQty,"%.2f" , fObservdQty);
            qDebug()<<"8";
            fvariation = fCB - fObservdQty;
            qDebug()<<"9";
            sprintf(cVariation,"%.2f" , fvariation);
            qDebug()<<"10";
            ui->tableWidget_InspectorVerificationn->setItem(row, 3, new QTableWidgetItem(cVariation));
            ui->tableWidget_InspectorVerificationn->item(row, 3)->setFlags( ui->tableWidget_InspectorVerificationn->item(row,3)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidget_InspectorVerificationn->item(row, 3)->setFlags( ui->tableWidget_InspectorVerificationn->item(row,3)->flags() | Qt::ItemIsSelectable);
        }
    }
}

void Widget::on_btnBack_InspAuth_clicked()
{
    ui->stackedWidget->setCurrentIndex(44);
}

void Widget::on_btnScanFpInspAuth_clicked()
{
    iDealMemSelected = INSPECTOR_SELECTED; //Added by vivek 01072017

    if(ui->lEdit_InpsAuth->text().isEmpty())
    {
        if(LangMode==1)
            miscObj.DisplayMsgBox((char *) "Please Enter UID");
        else if(LangMode==2)
            miscObj.tel_DisplayMsgBox((char *)"कृपया 12 अंक यूआईडी नंबर दर्ज करें.");
        return;
    }

    if(miscObj.validate_uid(ui->lEdit_InpsAuth->text().toAscii().data()) < 0)
    {
        if(LangMode==1)
            miscObj.DisplayWideMsgBox((char *)"INVALID UID");
        else if(LangMode==2)
            miscObj.tel_DisplayMsgBox((char *)"अमान्य यूआईडी");

        ui->lEdit_InpsAuth->clear();
        return;
    }

    if(AuthInspector() < 0)
        return;
    ///Send Observation Details to the Server
    if(Submit_Inspector_Verfication() < 0)
        return ;
    else
    {
        // miscObj.AudioTest((char *)"191");
        char buff[64];
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "Transaction Successfull...\nPrinting, Please Wait...");

        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);

        popup_msg *dlgPopup2 = new popup_msg(this);

        if(LangMode == 1)
            dlgPopup2->ui->lblPopup->setText(buff);
        else if(LangMode == 2)
            dlgPopup2->ui->lblPopup->setText(trUtf8("लेनदेन सफल था ... \n प्रिंट हो रहा है, प्रतीक्षा करें"));


        dlgPopup2->show();
        QCoreApplication::processEvents();
        QCoreApplication::processEvents();

        prn_open();
        if(prn_paperstatus() < 0)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"कृपया कागज डालें");

            dlgPopup2->close();
            dlgPopup2->deleteLater();
            return;
        }
        if(miscObj.Battery_check() != 0) //01072017
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद को प्रिंट करने में असमर्थ\nअपनी बैटरी चार्ज करें");


            dlgPopup2->close();
            dlgPopup2->deleteLater();
            return;

        }
        getHeadings();
        if (LangMode == 1)
            PrintTxnReceiptInspection();
        else
            PrintTxnReceiptInspectionHin();

        prn_close();

        dlgPopup2->close();
        dlgPopup2->deleteLater();

    }

    //Clear Widgets
    ui->tableWidet_InspectorDtls->clearContents();
    ui->tableWidget_InspectorVerificationn->clearContents();
    ui->radioButton_OK_inspcVerify->setChecked(false);
    ui->radioButton_seized_inspVer->setChecked(false);
    memset(cCurrentApprovalValue,0,sizeof(cCurrentApprovalValue)); //Inspector Approval Value
    ui->lEdit_InpsAuth->clear(); //added by vivek 30062017
    ui->stackedWidget->setCurrentIndex(1); //go Back to Main Menu
}

int Widget::AuthInspector()
{
    qDebug() << "Inside AuthInspector()";
    int ret = 0;

    popup_msg *dlgPopup2 = new popup_msg(this);


    GPRS objGprs;
    if(objGprs.Check_Network_Available() < 0)
    {
        dlgPopup2->close();
        dlgPopup2->deleteLater();

        return -1;
    }
    char FileName[128];
    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/AuthInspector_Req.xml");

    char FileNameResp[128];
    memset(FileNameResp, 0, sizeof(FileNameResp));
    strcpy(FileNameResp, "/mnt/jffs2/AuthInspector_Resp.xml");

    system("rm /mnt/jffs2/AuthInspector_Req.xml");
    system("rm /mnt/jffs2/AuthInspector_Resp.xml");
    char AuthUid[16];
    memset(AuthUid, 0, sizeof(AuthUid));
    strcpy(AuthUid, ui->lEdit_InpsAuth->text().toAscii().data());  //30062017 from line edit

    dlgPopup2->close();
    dlgPopup2->deleteLater();

    char b64Templ1[2048];
    memset(b64Templ1, 0, sizeof(b64Templ1));

    char b64Templ2[2048];
    memset(b64Templ2, 0, sizeof(b64Templ2));

    iFusionEnabledFlag = 0;
    int iFusionCount = 0;
    int iFusionLimit = 0;

FUSION_IMPL:

    popup_msg *dlgPopupFusion = new popup_msg(this);
    if(iFusionEnabledFlag == 0)
    {
        if(rdCallinit(1) < 0)
        {
            qDebug()<<"rdCallinit Failed"<<endl;
            return -1;
        }

    }
    /*if(iFusionEnabledFlag == 1)
    {
        qDebug()<<"Fusion Count = "<<iFusionCount;
        iFusionLimit = miscObj.getFusionCount();
        qDebug()<<"Fusion Limit is  "<<iFusionLimit;

        if(rdCallinit(2)< 0)
        {
            qDebug()<<"rdCallinit Failed"<<endl;
            return -1;
        }


    }*/

    ret = prepareAuthReqFile_Insp(FileName, AuthUid, b64Templ1, b64Templ2);
    qDebug()<<"ret form Prepare Out File : "<<ret;

    dlgPopupFusion->close();
    dlgPopupFusion->deleteLater();

    if(ret < 0)
    {
        miscObj.AudioTest((char *)"177");

        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Finger Not Placed On Scanner...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"फिंगर स्कैनर पर प्लेस नहीं किया गया. ");

        return -2;
    }

    miscObj.AudioTest((char *)"178");
    popup_msg *dlgPopup3 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup3->ui->lblPopup->setText("Please Wait...\nAuthenticating Dealer...");
    else if(LangMode == 2)
        dlgPopup3->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..\n डीलर प्रमाणीकरण हो रहा है.."));


    dlgPopup3->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();


    sleep(2); //Kept For Audio

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
            strcpy(cUrlName, query.value(0).toString().toAscii().data()); //Production URL Vivek
        }
    }

    char cmdCurl[1024];
    memset(cmdCurl, 0, sizeof(cmdCurl));
    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, FileNameResp, SERVER_TIME_OUT);

    qDebug() << "\nAuthenticating Shop Owner - Curl Cmd: " << cmdCurl;
    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, AUTH_SERVER) < 0)
    {
        dlgPopup3->close();
        dlgPopup3->deleteLater();

        return -3;
    }

    dlgPopup3->close();
    dlgPopup3->deleteLater();

    qDebug() << "***************AuthInspector_Resp.xml*****************";
    system("cat /mnt/jffs2/AuthInspector_Resp.xml");
    qDebug() << "***********************************************888*";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/AuthInspector_Resp.xml") < 0)
    {
        /*qDebug()<<"Finger Iris Selection Value = "<<Finger_Iris_Selection;
        if(Finger_Iris_Selection == 0)////Only for Finger Print
        {
            if(ParsingFirstConditionFusion("/mnt/jffs2/AuthInspector_Resp.xml") < 0)
            {
                iFusionEnabledFlag = 1;
                memset(b64Templ2, 0, sizeof(b64Templ2));
                strcpy(b64Templ2, b64Templ1);
                goto FUSION_IMPL;
                return -1;
            }
            else
            {
                iFusionEnabledFlag = 0;
            }
        }*/
        ui->lEdit_InpsAuth->clear();;
        return -1;
    }
    if(parseAuthResponse_Insp("/mnt/jffs2/AuthInspector_Resp.xml") < 0)
        return -1;
    if(iFusionEnabledFlag == 1)
    {
        char DealerType[8];
        memset(DealerType, 0, sizeof(DealerType));
        qDebug()<<"Fusion Update To server for Dealer";
        strcpy(DealerType, "INSP"); ///01072017

        qDebug()<<"Dealer Type for Fusion updation :"<<DealerType;

        FusionUpdateToServer(DealerType, "1", AuthUid,NULL);
    }
    return 0;
}

int Widget::prepareAuthReqFile_Insp(char *FileName, char *AuthUid, char *b64Templ1, char *b64Templ2)
{
    qDebug()<<"Out File (" <<FileName << ") Preperation ....";


    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));

    miscObj.GetMachineId(cmachineID);

    char buff[4096];
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "rm %s", FileName);
    system(buff);

    QFile file(FileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug()<< FileName << " file Open failed";
        return -1;
    }

    QTextStream out(&file);

    memset(buff, 0, sizeof(buff));

    /*RD 2.0 subbu
    qDebug()<<"#################1############";


    if(UID_AuthenticationFusion(AuthUid, b64Templ1, b64Templ2) < 0)
    {
        qDebug() << "In prepareAuthReq File() -> UID_Authentication Failed";
        return -1;
    }
    qDebug()<<"#################2############";
    */

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0)
    {
        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }
    char szShopId[16];
    memset(szShopId, 0, 16);
    getShopId(szShopId);
    char authMode[4];
    memset(authMode, 0, 4);

    char cDealMembuff[32];
    memset(cDealMembuff, 0, sizeof(cDealMembuff));
    if(iDealMemSelected == DEALER_SELECTED)
    {
        strcpy(cDealMembuff, ui->ldtShpId->text().toAscii().data());
        strcpy(authMode, "D");
    }
    else if(iDealMemSelected == MEMBER_SELECTED)
    {
        strcpy(cDealMembuff, ui->ldtRationCard->text().toAscii().data());
        strcpy(authMode, "B");
    }else if(iDealMemSelected == INSPECTOR_SELECTED)
    {
        strcpy(cDealMembuff, ui->ldtShpId->text().toAscii().data());
        strcpy(authMode, "V");
    }
    else
    {
        strcpy(cDealMembuff, ui->ldtShpId->text().toAscii().data());
        strcpy(authMode, "D");
    }

    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL)
    {
        qDebug() << "File Not Found" << endl;
        return -1;
    }

    //RD 2.0 inspector subbu

    fprintf(fp,"%s","<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    fprintf(fp,"%s","<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://www.uidai.gov.in/authentication/uid-auth-request/2.0\" xmlns:ns2=\"http://service.fetch.rationcard/\">");
    fprintf(fp,"%s","<SOAP-ENV:Body>");
    fprintf(fp,"%s","<ns2:getAuthenticateNICAuaInspectionRD2>");
    fprintf(fp,"<fps_seesion_Id>%s</fps_seesion_Id>",SessionId);
    fprintf(fp,"<Shop_no>%s</Shop_no>",szShopId);
    fprintf(fp,"<User_Id>%s</User_Id>",cDealMembuff);
    fprintf(fp,"<uidNumber>%s</uidNumber>",AuthUid);
    fprintf(fp,"<udc>%s</udc>",cmachineID);
    fprintf(fp,"<authMode>%s</authMode>",authMode);
    fprintf(fp,"%s","<auth_packet>");
    fprintf(fp,"<ns1:certificateIdentifier>%s</ns1:certificateIdentifier>",pid_info.ci);
    fprintf(fp,"<ns1:dataType>%s</ns1:dataType>","X");
    fprintf(fp,"<ns1:dc>%s</ns1:dc>",pid_info.dev_data.dc);
    fprintf(fp,"<ns1:dpId>%s</ns1:dpId>",pid_info.dev_data.dpId);
    fprintf(fp,"<ns1:encHmac>%s</ns1:encHmac>",pid_info.Hmac);
    fprintf(fp,"<ns1:mc>%s</ns1:mc>",pid_info.dev_data.mc);
    fprintf(fp,"<ns1:mid>%s</ns1:mid>",pid_info.dev_data.mi);
    fprintf(fp,"<ns1:rdId>%s</ns1:rdId>",pid_info.dev_data.rdsId);
    fprintf(fp,"<ns1:rdVer>%s</ns1:rdVer>",pid_info.dev_data.rdsVer);
    fprintf(fp,"<ns1:secure_pid>%s</ns1:secure_pid>",pid_info.pid);
    fprintf(fp,"<ns1:sessionKey>%s</ns1:sessionKey>",pid_info.Skey);
    fprintf(fp,"%s","</auth_packet>");
    fprintf(fp,"<password>%s</password>",HD_PASSWORD);
    fprintf(fp,"<Resp><errCode>%s</errCode><errInfo>%s</errInfo><nmPoints>%s</nmPoints><fCount>%s</fCount><fType>%s</fType><iCount>%s</iCount><iType>%s</iType><pCount>%s</pCount><pType>%s</pType><qScore>%s</qScore></Resp>",pid_info.err_code,pid_info.err_info,pid_info.nmPoints,pid_info.fCount,pid_info.fType,pid_info.iCount,pid_info.iType,pid_info.pCount,pid_info.pType,pid_info.qScore);;
    fprintf(fp,"%s","</ns2:getAuthenticateNICAuaInspectionRD2>");
    fprintf(fp,"%s","</SOAP-ENV:Body>");
    fprintf(fp,"%s","</SOAP-ENV:Envelope>");
    fclose(fp);


    free(pid_info.pid);
    pid_info.pid = NULL;

    return 0;
}

int Widget::parseAuthResponse_Insp(char *filename)
{
    qDebug() << "inside parseAuthResponse_Insp()";
    QSqlQuery query;
    query.exec("DELETE FROM InspDetails");
    query.clear();

    QString cInspanme, cInspDegisnation,cauth_transaction_code;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In ParsingFirstCondition = " << filename;
        return -1;
    }

    if(file.size() == 0)
        return -1;

    QXmlStreamReader xml(&file);

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            /* Inspection New Tag 01072017*/
            if(xml.name() == "auth_transaction_code")
            {
                xml.readNext();
                cauth_transaction_code = xml.text().toString();
                qDebug() << "auth_transaction_code : " << cauth_transaction_code;
            }
            if(xml.name() == "inspectorDesignation")
            {
                xml.readNext();
                cInspDegisnation = xml.text().toString();
                qDebug() << "inspectorDesignation : " << cInspDegisnation;
            }
            if(xml.name() == "inspectorName")
            {
                xml.readNext();
                cInspanme = xml.text().toString();
                qDebug() << "inspectorName : " << cInspanme;
                query.clear();
                query.prepare("INSERT INTO InspDetails(inspectorName, inspectorDesignation) VALUES(:inspectorName, :inspectorDesignation)");
                query.bindValue(":inspectorName",cInspanme);
                query.bindValue(":inspectorDesignation", cInspDegisnation);
                if(!query.exec())
                    qDebug()<<"InspDetails :" << query.lastError();
            }
        }
    }

    file.close();

    return 0;
}

int Widget::Submit_Inspector_Verfication()
{
    char cReqfilename[50]="", cRespfilename[50]="",curlcommand[1000]="";
    int ret = 0;
    GPRS objGprs;

    if(objGprs.Check_Network_Available() < 0)
    {
        return -1;
    }
    popup_msg *dlgPopup = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup->ui->lblPopup->setText("Please Wait... \n Uploading Observation Details to the Server...");
    else if(LangMode == 2)
        dlgPopup->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.. \n सर्वर पर निगरानी जानकारी अपलोड करें"));

    dlgPopup->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    strcpy(cReqfilename,"/root/inspPushCBData_req.xml");
    strcpy(cRespfilename,"/root/inspPushCBData_resp.xml");

    system("rm /root/inspPushCBData_req.xml");
    system("rm /root/inspPushCBData_resp.xml");

    ret = prepareinspPushCBDataReq(cReqfilename);
    qDebug()<<"ret form Prepare Out File : "<<ret;

    char catCmd[128];
    memset(catCmd, 0, sizeof(catCmd));
    sprintf(catCmd, "cat %s", cReqfilename);
    system(catCmd);

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
            strcpy(cUrlName, query.value(0).toString().toAscii().data()); //Production URL Vivek
        }
    }
    sprintf(curlcommand,"curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", cReqfilename, cUrlName ,cRespfilename,60);
    qDebug() << "\nCurl Cmd : " << curlcommand;

    int sysret = system(curlcommand);
    qDebug() << "sysret = " << sysret;

    if(SystemCurlResponse(sysret, EPDS_SERVER) < 0)
    {
        dlgPopup->close();
        dlgPopup->deleteLater();
        return -2;
    }

    dlgPopup->close();
    dlgPopup->deleteLater();

    if(ParsingFirstCondition(cRespfilename) != 0)
    {
        return -3;
    }

}

int Widget::prepareinspPushCBDataReq(char *FileName)
{
    qDebug()<<"**** Inside prepareinspPushCBDataReq ************";
    QFile file(FileName);
    char cAuthId[50]="";
    int irowcount = 0;
    char capprovalStatus[10]="";
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug()<< FileName << " file Open failed";
        return -1;
    }

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0)
    {
        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }
    char szShopId[16];
    memset(szShopId, 0, 16);
    getShopId(szShopId);

    //Get Selected Approval Value from Radio Buttons
    memset(capprovalStatus, 0, sizeof(capprovalStatus));
    memset(cCurrentApprovalValue, 0, sizeof(cCurrentApprovalValue));
    if(ui->radioButton_OK_inspcVerify->isChecked())
    {
        strcpy(cCurrentApprovalValue, ui->radioButton_OK_inspcVerify->text().toAscii().data());
        qDebug()<<"Selected Approval Value" <<cCurrentApprovalValue;
    }
    else if(ui->radioButton_seized_inspVer->isChecked())
    {
        strcpy(cCurrentApprovalValue, ui->radioButton_seized_inspVer->text().toAscii().data());
        qDebug()<<"Selected Approval Value" <<cCurrentApprovalValue;
    }
    getApprovestatus(cCurrentApprovalValue, capprovalStatus);
    qDebug()<<"capprovalStatus :"<<capprovalStatus;

    //Fetch Value From TableWidget
    irowcount = ui->tableWidget_InspectorVerificationn->rowCount();
    qDebug()<< "irowCount="<<irowcount;

    //UID //added by vivek 30062017
    memset(cAuthId,0,sizeof(cAuthId));
    strcpy(cAuthId,ui->lEdit_InpsAuth->text().toAscii().data());
    qDebug()<<"Inspector UID :" << cAuthId;

    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL)
    {
        qDebug() << "File Not Found" << endl;
        return -1;
    }

    fprintf(fp, "%s","<?xml version=\"1.0\" encoding=\"UTF-8\"?> ");
    fprintf(fp, "%s","<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://service.fetch.rationcard/\"> ");
    fprintf(fp, "<%s>", "SOAP-ENV:Body");
    fprintf(fp, "<%s>", "ns1:inspPushCBData");
    fprintf(fp, "<fpsId>%s</fpsId>",szShopId); // Hardcoded "107000100114"
    fprintf(fp, "<fpsSessionId>%s</fpsSessionId>",SessionId );//Hardcoded "07020170529141323"
    fprintf(fp, "<password>%s</password>", HD_PASSWORD);
    fprintf(fp, "<approvalStatus>%s</approvalStatus>", capprovalStatus);
    for(int i = 0; i < irowcount ; i++) /* Commodity List Tag start */
    {
        fprintf(fp,"<%s>","inspCBUpdate");
        fprintf(fp, "<closingBalance>%s</closingBalance>",ui->tableWidget_InspectorVerificationn->item(i,1)->text().toAscii().data());
        fprintf(fp, "<commCode>%s</commCode>", ui->tableWidget_InspectorVerificationn->item(i,4)->text().toAscii().data());
        fprintf(fp, "<observedClosingBalance>%s</observedClosingBalance>", ui->tableWidget_InspectorVerificationn->item(i,2)->text().toAscii().data()); //EDitable
        // fprintf(fp,"<receiptId>%s</receiptId>",cInspcTxnId); //Tag Removed by Vivek 01012017
        fprintf(fp,"<variation>%s</variation>",ui->tableWidget_InspectorVerificationn->item(i,3)->text().toAscii().data());
        fprintf(fp,"</%s>","inspCBUpdate");
    } /* Commodity List Tag End */
    fprintf(fp,"<inspUid>%s</inspUid>",cAuthId); //Added by vivek 30062017
    fprintf(fp, "</%s>", "ns1:inspPushCBData");
    fprintf(fp, "</%s>", "SOAP-ENV:Body");
    fprintf(fp, "</%s>", "SOAP-ENV:Envelope");
    fclose(fp);
    return 0;
}

int Widget::getApprovestatus(char *approveValue, char *approveKey)
{
    qDebug()<<"Inside getApprovestatus" << endl << "approveValue :" <<approveValue;
    QSqlQuery query;
    QString buff;
    memset(approveKey, 0, sizeof(approveKey));
    query.prepare("SELECT approveKey FROM InspApprovalsDtls WHERE approveValue = ?");
    query.addBindValue(approveValue);
    if(!query.exec())
    {
        qDebug()<<"getApprovestatus :" <<query.lastError();
        return -1;
    }
    else
    {
        while(query.next())
        {
            buff = query.value(0).toString();
        }

        strcpy(approveKey,buff.toAscii().data());
        qDebug()<<"approveKey :" <<approveKey;
    }
    return 0;

}

int Widget::PrintTxnReceiptInspection()
{
    char prn[256];
    LogoPrint logo;
    logo.PrintLogo();

    int currentRow = ui->tableWidet_InspectorDtls->currentRow(); //Inspector Details

    QString str= "";

    /*Get Inspectorname And Degisnation*/
    QString cinspectorName, cinspectorDegisnation;
    QSqlQuery qry;
    qry.prepare("SELECT inspectorName, inspectorDesignation FROM InspDetails");
    if(!qry.exec())
        qDebug()<<"PrintTxnReceiptInspection() :" << qry.lastError();
    else
    {
        while(qry.next())
        {
            cinspectorName = qry.value(0).toString();
            cinspectorDegisnation = qry.value(1).toString();

            qDebug()<<"cinspectorName :" <<cinspectorName;
            qDebug()<<"cinspectorDegisnation" <<cinspectorDegisnation;
        }

    }
    /*******************************/

    qDebug()<<"cinspectorName :" <<cinspectorName;
    qDebug()<<"cinspectorDegisnation" <<cinspectorDegisnation;

    str.clear();
    str= QString::fromUtf8("Department of Food, Civil Supplies and Consumer Affairs ");
    prepareBmp(QString::fromUtf8(str.toUtf8()),22,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER);
    str.clear();

    str= QString::fromUtf8("Government of MP");
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );

    str.clear();

    str= QString::fromUtf8("INSEPECTION RECEIPT");
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
    str.clear();
    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    QSqlQuery query;
    QString fps_id;
    query.prepare("SELECT ShopID from ShopOwnerDetails");
    if(!query.exec())
    {
        qDebug()<< "at print out fps id"<<query.lastError() ;
    }
    else
    {
        while (query.next())
        {
            fps_id = query.value(0).toString();

        }
    }
    str= QString::fromUtf8("FPS Id :") + fps_id;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);
    str.clear();

    str= QString::fromUtf8("Transaction ID :") + inspreceiptId;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);
    str.clear();

    str.clear();
    str= QString::fromUtf8("Inspected By  :") + cinspectorName; //hardcoded
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);


    str.clear();
    str= QString::fromUtf8("Degisnation   :") + cinspectorDegisnation; //Hardcoded
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

    str= QString::fromUtf8("Date: ") + ui->labelDate->text() +"  "+ ui->labelTime->text();
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);
    str.clear();

    memset(prn, 0, sizeof(prn));
    sprintf(prn,"Status : %s" ,cCurrentApprovalValue);
    prn_write_text((unsigned char *)prn, strlen(prn), 2);

    memset(prn, 0, sizeof(prn));
    sprintf(prn,"%s","Commodity    CB     Observation Variation");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);
    int iRow = ui->tableWidget_InspectorVerificationn->rowCount();
    int i=0, SlNo=0;

    while(i<iRow)
    {

        ++SlNo;

        memset(prn,0,sizeof(prn));

        sprintf(prn,"%-12s  %-0.2f   %-0.2f   %-0.2f",ui->tableWidget_InspectorVerificationn->item(i,0)->text().toAscii().data(), atof(ui->tableWidget_InspectorVerificationn->item(i,1)->text().toAscii().data()), atof(ui->tableWidget_InspectorVerificationn->item(i,2)->text().toAscii().data()), atof(ui->tableWidget_InspectorVerificationn->item(i,3)->text().toAscii().data()));
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        i++;
    }
    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "\n\n");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    prn_paper_feed(4);

    return 0;
}

int Widget :: PrintTxnReceiptInspectionHin()
{
    char prn[256];
    LogoPrint logo;
    logo.PrintLogo();

    int currentRow = ui->tableWidet_InspectorDtls->currentRow(); //Inspector Details

    QString str= "";
    /*Get Inspectorname And Degisnation*/
    QString cinspectorName, cinspectorDegisnation;
    QSqlQuery qry;
    qry.prepare("SELECT inspectorName, inspectorDesignation FROM InspDetails");
    if(!qry.exec())
        qDebug()<<"PrintTxnReceiptInspection() :" << qry.lastError();
    else
    {
        while(qry.next())
        {
            cinspectorName = qry.value(0).toString();
            cinspectorDegisnation = qry.value(1).toString();
        }

    }
    /*******************************/
    str.clear();
    str= QString::fromUtf8("खाद्य विभाग, नागरिक आपूर्ति, उपभोक्ता मामले");
    prepareBmp(QString::fromUtf8(str.toUtf8()),22,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER);
    str.clear();

    str= QString::fromUtf8("बिहार सरकार  ");
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );

    str.clear();

    str= QString::fromUtf8("रसीद की जांच करें");
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
    str.clear();
    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    QSqlQuery query;
    QString fps_id;

    query.prepare("SELECT ShopID from ShopOwnerDetails");
    if(!query.exec())
    {
        qDebug()<< "at print out fps id"<<query.lastError() ;
    }
    else
    {
        while (query.next())
        {
            fps_id = query.value(0).toString();

        }
    }
    str= QString::fromUtf8("एफपीएस आईडी:") + fps_id;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);
    str.clear();

    str= QString::fromUtf8("लेनदेन आईडी :") + inspreceiptId;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);
    str.clear();

    str.clear();
    str= QString::fromUtf8("चेक करके:") + cinspectorName;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

    //01072017
    str.clear();
    str= QString::fromUtf8("स्थिति         :") + cinspectorDegisnation;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

    str= QString::fromUtf8("तारीख: ") + ui->labelDate->text() +"  "+ ui->labelTime->text();
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);
    str.clear();

    str= QString::fromUtf8("स्थिति : ") +"  "+cCurrentApprovalValue;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);
    str.clear();

    str= QString::fromUtf8("वस्तु ") + QString::fromUtf8("   सीबी ") + QString::fromUtf8("  पर्यवेक्षण") +   QString::fromUtf8(" परिवर्तन")+"\n"
            "------------------------------------------";
    prepareBmp(QString::fromUtf8(str.toUtf8()),22,QString::fromUtf8(str.toUtf8()),18,ALIGN_LEFT);

    str.clear();
    int iRow = ui->tableWidget_InspectorVerificationn->rowCount();
    int i=0, SlNo=0;

    while(i<iRow)
    {
        ++SlNo;

        char Entitlement[120]="";
        QString QcommNamellalign;
        QcommNamellalign.clear();
        QcommNamellalign = QString(ui->tableWidget_InspectorVerificationn->item(i,0)->text()).leftJustified(15, ' ');

        memset(Entitlement,0,sizeof(Entitlement));

        sprintf(Entitlement,"%-0.2f   %-0.2f     %-0.2f ", atof(ui->tableWidget_InspectorVerificationn->item(i,1)->text().toAscii().data()) ,atof(ui->tableWidget_InspectorVerificationn->item(i,2)->text().toAscii().data()),atof(ui->tableWidget_InspectorVerificationn->item(i,3)->text().toAscii().data()));

        str=QcommNamellalign +" "+Entitlement;

        prepareBmp(QString::fromUtf8(str.toUtf8()),22,QString::fromUtf8(str.toUtf8()),18,ALIGN_LEFT);

        i++;
    }
    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "\n\n");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    prn_paper_feed(4);

    return 0;
}

int Widget::Get_transactionID(char *filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }

    QXmlStreamReader xml_bal(&file);

    inspreceiptId.clear();

    while(!xml_bal.atEnd() && !xml_bal.hasError())
    {
        QXmlStreamReader::TokenType token = xml_bal.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;


        if(token == QXmlStreamReader::StartElement)
        {

            if(xml_bal.name() == "receiptId")
            {
                xml_bal.readNext();
                inspreceiptId = xml_bal.text().toString();
                qDebug() << "inspreceiptId : " <<inspreceiptId;
            }

        }
    }
    return 0;
}
