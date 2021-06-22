#include "ui_popup_msg.h"
#include"ui_widget.h"
#include<widget.h>
#include<popup_msg.h>
#include<QDateTime>
#include<stdio.h>
#include<stdlib.h>
extern char  HD_PASSWORD[64];
void Widget::on_btnMDM_clicked()
{
    iMainTransaction = MDM_TRANSACTION;

    Finger_Iris_Selection = 0;
    memset(gauth_transaction_code, 0, sizeof(gauth_transaction_code));

    ui->label_type_card->clear();
    ui->lineEditAnganwadiCardNo->clear();

    //UidEncode_b64_flag = 1;

    LangMode = getLangMode();
    if(LangMode == 1)
    {
        ui->label_85->setText("M D M");
        //ui->label_82->setText("M D M");
        ui->label_87->setText("School Id:");
        ui->BtnBack_13->setText("Back");
        ui->BtnAnganwadiMemDetails->setText("School\nDetails");
    }
    else
    {
        ui->label_85->setText(trUtf8("एम डी एम"));
        ui->BtnBack_13->setText(trUtf8("वापस"));
        ui->label_87->setText(trUtf8("स्कूल आईडी:"));
        ui->BtnAnganwadiMemDetails->setText(trUtf8("स्कूल\nविवरण")); //ఏడబ్ల్యుసి
    }

    ui->lineEditAnganwadiCardNo->setFocus();
    ui->stackedWidget->setCurrentIndex(54);
    return;
}
int Widget::GetSchoolMemberDetails()
{
    popup_msg *dlgPopup1 = new popup_msg(this);

    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...\nDownloading School Members Details.");
    else
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें ...\n विद्यालय के सदस्यों का विवरण डाउनलोड करना"));

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    system("rm /mnt/jffs2/geticdsdetailsreq.xml");
    system("rm /mnt/jffs2/geticdsdetailsresp.xml");

    QSqlQuery query;
    char cShopID[16];
    CMisc miscObj;

    query.clear();
    query.prepare("SELECT ShopID FROM ShopOwnerDetails");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            memset(cShopID, 0, sizeof(cShopID));
            strcpy(cShopID, query.value(0).toString().toAscii().data());
        }
    }

    //memset(SessionId, 0, sizeof(SessionId));
    //QSqlQuery query;
    query.clear();
    query.prepare("select fpsSessionId from Dealer");
    query.exec();query.next();
    strcpy(SessionId,query.value(0).toString().toAscii().data());

    QFile file("/mnt/jffs2/geticdsdetailsreq.xml");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug() << "/mnt/jffs2/geticdsdetailsreq.xml File Open Failed";

        dlgPopup1->close();
        dlgPopup1->deleteLater();

        return -1;
    }

    char cmachineID[16];
    memset(cmachineID, 0x00, sizeof(cmachineID));

    miscObj.GetMachineId(cmachineID);
    QTextStream out(&file);

    char Reqbuff[2048];
    memset(Reqbuff, 0, sizeof(Reqbuff));

    sprintf(Reqbuff, "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getSchoolsDetails><fpsSessionId>%s</fpsSessionId><schoolId>%s</schoolId><fpsId>%s</fpsId><password>%s</password><deviceId>%s</deviceId></ns1:getSchoolsDetails></SOAP-ENV:Body></SOAP-ENV:Envelope>", SessionId, ui->lineEditAnganwadiCardNo->text().toAscii().data(), cShopID, HD_PASSWORD,cmachineID);
    out << Reqbuff;
    file.close();

    qDebug() << "************/mnt/jffs2/geticdsdetailsreq.xml****************";
    system("cat /mnt/jffs2/geticdsdetailsreq.xml");
    qDebug();
    qDebug() << "************************************************************";

    char cUrlName[256];
    memset(cUrlName, 0, sizeof(cUrlName));
    /*if(GeetingUrl(cUrlName))
    {
        miscObj.DisplayMsgBox((char *)"URL Not Found");
        return -1;
    }*/
    //QSqlQuery query;
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

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/geticdsdetailsreq.xml", cUrlName,  "/mnt/jffs2/geticdsdetailsresp.xml", responseTimedOutTimeInSec.toInt());

    qDebug()<<"\nCurl Cmd : "<<cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();

        return -1;
    }

    qDebug() << "************/mnt/jffs2/geticdsdetailsresp.xml***************";
    system("cat /mnt/jffs2/geticdsdetailsresp.xml");
    qDebug() << "************************************************************";

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    if(ParsingFirstCondition("/mnt/jffs2/geticdsdetailsresp.xml") != 0)
    {
        return -1;
    }

    if(parseICDSMemberDetails("/mnt/jffs2/geticdsdetailsresp.xml") < 0)
    {
        miscObj.DisplayWideMsgBox((char *)"Incorrect XML");
        return -1;
    }
    return 0;
}
void Widget::on_BtnBack_13_clicked()
{
    ui->stackedWidget->setCurrentIndex(28);
}
void Widget::on_BtnAnganwadiMemDetails_clicked()
{
    CMisc miscObj;

    /*if(iMainTransaction == ICDS_TRANSACTION)
    {
        if(ui->lineEditAnganwadiCardNo->text().length() == 0)
        {

            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Please Enter ICDS Card No");
            else
                miscObj.tel_DisplayWideMsgBox("దయచేసి ఐసీడీఎస్ కార్డ్ సంఖ్య ను ఎంటర్ చేయండి");
            return;
        }
        if(GetAnganwadiMemberDetails() < 0)
            return;

    }*/
    qDebug()<<"In School Id";

    if(iMainTransaction == MDM_TRANSACTION)
    {
        if(ui->lineEditAnganwadiCardNo->text().length() == 0)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Please Enter School Id");
            else
                miscObj.tel_DisplayWideMsgBox("कृपया स्कूल आईडी दर्ज करें");
            return;
        }
        if(GetSchoolMemberDetails() < 0)
            return;
    }

    int count = 0;
    CMisc objMisc;

    ui->lblRCNO_Val->setText(ui->lineEditAnganwadiCardNo->text());
    ui->lblRCNO_Val_2->setText(ui->lineEditAnganwadiCardNo->text());
    ui->label_9->setText(ui->lineEditAnganwadiCardNo->text());


    QSqlQuery query1;
    query1.prepare("SELECT COUNT(MemberNameEn) FROM CustomerFamily");
    if(!query1.exec())
        qDebug() << query1.lastError();
    else
    {
        while(query1.next())
            count = query1.value(0).toInt(0);
    }

    if(count == 0)
    {
        if(LangMode == 1)
            objMisc.DisplayWideMsgBox((char *)"Members Details Not Found For The Card");
        else
            objMisc.tel_DisplayWideMsgBox("सदस्यों का विवरण कार्ड के लिए नहीं मिला");

        return;
    }

    ui->twdgtEntitlement->setRowCount(count);

    char buff[300];
    QSqlQuery query;
    query.prepare("SELECT MemberNameEn, MemberUID, bfd_1, bfd_2, bfd_3,MemberId FROM CustomerFamily where RationCardId=?");
    qDebug()<<"lineEditAnganwadiCardNo :"<<ui->lineEditAnganwadiCardNo->text().data();
    query.addBindValue(ui->lineEditAnganwadiCardNo->text().toAscii().data());
    //sprintf(buff,"SELECT MemberNameEn, MemberUID, bfd_1, bfd_2, bfd_3,encuid FROM CustomerFamily where RationCardId='%s'",ui->lineEditAnganwadiCardNo->text().toAscii().data());
    //query.exec();

    if(!query.exec())
        qDebug() <<"In MDM Get Details "<< query.lastError();
    else
    {
        int row = 0;
        while(query.next())
        {
            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(0).toString().toAscii().data());
            //strcpy(buff, QMemName.toAscii().data());
            qDebug()<<"0 MemberNameEn = "<<buff;

            ui->twdgtEntitlement->setItem(row, 0, new QTableWidgetItem(buff));
            //ui->twdgtEntitlement->setItem(row, 0, new QTableWidgetItem(QMemName));
            ui->twdgtEntitlement->resizeColumnToContents(0);
            ui->twdgtEntitlement->item(row, 0)->setFlags( ui->twdgtEntitlement->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->twdgtEntitlement->item(row, 0)->setFlags( ui->twdgtEntitlement->item(row,0)->flags() | Qt::ItemIsSelectable);


            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(1).toString().toAscii().data());
            qDebug()<<"1 MemberUID = "<<buff;

            ui->twdgtEntitlement->setItem(row, 1, new QTableWidgetItem(buff));
            ui->twdgtEntitlement->item(row, 1)->setFlags(ui->twdgtEntitlement->item(row, 1)->flags() ^ Qt::ItemIsEditable);
            ui->twdgtEntitlement->item(row,1)->setFlags( ui->twdgtEntitlement->item(row,1)->flags() | Qt::ItemIsSelectable);


            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(2).toString().toAscii().data());
            qDebug()<<"2 BFD1 = "<<buff;

            ui->twdgtEntitlement->setItem(row, 2, new QTableWidgetItem(buff));
            ui->twdgtEntitlement->item(row, 2)->setFlags(ui->twdgtEntitlement->item(row, 2)->flags() ^ Qt::ItemIsEditable);
            ui->twdgtEntitlement->item(row, 2)->setFlags( ui->twdgtEntitlement->item(row,2)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(3).toString().toAscii().data());
            qDebug()<<"3 BFD2 = "<<buff;

            ui->twdgtEntitlement->setItem(row, 3, new QTableWidgetItem(buff));
            ui->twdgtEntitlement->item(row, 3)->setFlags(ui->twdgtEntitlement->item(row, 3)->flags() ^ Qt::ItemIsEditable);
            ui->twdgtEntitlement->item(row, 3)->setFlags( ui->twdgtEntitlement->item(row,3)->flags() | Qt::ItemIsSelectable);


            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(4).toString().toAscii().data());
            qDebug()<<"4 BFD3 = "<<buff;

            ui->twdgtEntitlement->setItem(row, 4, new QTableWidgetItem(buff));
            ui->twdgtEntitlement->item(row, 4)->setFlags(ui->twdgtEntitlement->item(row, 4)->flags() ^ Qt::ItemIsEditable);
            ui->twdgtEntitlement->item(row, 4)->setFlags( ui->twdgtEntitlement->item(row,4)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(1).toString().toAscii().data());
            qDebug()<<"5 Real MemberUID = "<<buff;

            ui->twdgtEntitlement->setItem(row, 5, new QTableWidgetItem(buff));
            ui->twdgtEntitlement->item(row, 5)->setFlags(ui->twdgtEntitlement->item(row, 1)->flags() ^ Qt::ItemIsEditable);
            ui->twdgtEntitlement->item(row,5)->setFlags( ui->twdgtEntitlement->item(row,1)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(5).toString().toAscii().data());
            qDebug()<<"6 Real Member ID = "<<buff;

            ui->twdgtEntitlement->setItem(row, 6, new QTableWidgetItem(buff));
            ui->twdgtEntitlement->item(row, 6)->setFlags(ui->twdgtEntitlement->item(row, 1)->flags() ^ Qt::ItemIsEditable);
            ui->twdgtEntitlement->item(row,6)->setFlags( ui->twdgtEntitlement->item(row,1)->flags() | Qt::ItemIsSelectable);


            ui->twdgtEntitlement->setRowHeight(row,20);
            row++;
        }
    }
    qDebug()<<"Column 1 Width = "<<ui->twdgtEntitlement->columnWidth(1);
    //ui->twdgtEntitlement->setColumnHidden(2, true);
    ui->twdgtEntitlement->setColumnWidth(0,200); //120
    ui->twdgtEntitlement->setColumnWidth(1,110); //105
    //ui->twdgtEntitlement->setColumnWidth(2,30); //105
    //ui->twdgtEntitlement->setColumnWidth(3,30); //105
    //ui->twdgtEntitlement->setColumnWidth(4,30); //105

    ui->twdgtEntitlement->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->twdgtEntitlement->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twdgtEntitlement->selectRow(0);
    //    ui->twdgtEntitlement->resizeColumnsToContents();
    //    ui->twdgtEntitlement->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ui->twdgtEntitlement->verticalHeader()->hide();
    ui->twdgtEntitlement->setColumnHidden(2, true);
    ui->twdgtEntitlement->setColumnHidden(3, true);
    ui->twdgtEntitlement->setColumnHidden(4, true);

    ui->twdgtEntitlement->setColumnHidden(5, true);
    ui->twdgtEntitlement->setColumnHidden(6, true);

    if(iMainTransaction == MDM_TRANSACTION)
    {
        if(LangMode == 1)
            ui->lblRCNO->setText("School No:"); //ఏడబ్ల్యుసి
        else
            ui->lblRCNO->setText(trUtf8("स्कूल नं"));
    }


    ui->ConsentFormcheckBox->setChecked(true);
    ui->stackedWidget->setCurrentIndex(5);
    return;
}

int Widget::parseICDSMemberDetails(char *filename)
{
    QFile file(filename);
    int Handleflag=2;
    if(!file.exists())
    {
        qDebug() << "File Not Found";
        return -1;
    }

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error Reading File" << endl;
        return -2;
    }
    QString Qekycprompt;
    MinQtyStr.clear();
    QXmlStreamReader xml(&file);

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "ekycPrompt")
            {
                xml.readNext();
                Qekycprompt.clear();
                Qekycprompt = xml.text().toString();
                eKYCPrompt = Qekycprompt;
                qDebug() << " :Qekycprompt------------------------------>" << Qekycprompt;
            }

        }
    }
    file.close();

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error Reading File" << endl;
        return -2;
    }

    QDomDocument document;
    if(!document.setContent( &file ) )
    {
        qDebug( "Failed to parse the file into a DOM tree." );
        file.close();
        return -3;
    }
    file.close();
    QStringList Qstrlistweighing;
    Qstrlistweighing.clear();
    MinQtyStr.clear();
    QSqlQuery query;

    query.clear();
    query.exec("DELETE FROM CustomerFamily");
    query.clear();
    query.exec("DELETE FROM CustomerMaster");
    query.clear();
    query.exec("DELETE FROM BenQuotaTable");
    query.clear();
    query.exec("DELETE FROM ItemMaster");
    query.clear();
    query.exec("DELETE FROM ICDSCustomerFamily");



    int ChildCount=0,i = 0, j=0;
    QDomNodeList list;
    QDomNodeList Qawdetails;

    if(iMainTransaction == ICDS_TRANSACTION)
        Qawdetails = document.elementsByTagName("awdetails");
    else if (iMainTransaction == MDM_TRANSACTION)
        Qawdetails = document.elementsByTagName("schoolDetails");

    ChildCount = Qawdetails.at(0).childNodes().count();
    qDebug() << "Child Count = " << ChildCount;
    int k = 0;
    for(i=0, j=0; i< ChildCount; i++)
    {
        qDebug()<<Qawdetails.at(0).childNodes().at(i).nodeName().toAscii().data();

        if(Qawdetails.at(0).childNodes().at(i).nodeName() == "commDetails")
        {
            list = document.elementsByTagName("commDetails");

            QString availedQty  = list.at(j).childNodes().at(0).toElement().text();
            QString balQty      = list.at(j).childNodes().at(1).toElement().text();
            QString closingBal  = list.at(j).childNodes().at(2).toElement().text();
            QString commName    = list.at(j).childNodes().at(3).toElement().text();
            QString commNamell  = list.at(j).childNodes().at(4).toElement().text();
            QString commcode    = list.at(j).childNodes().at(5).toElement().text();
            QString measureUnit = list.at(j).childNodes().at(6).toElement().text();
            MinQtyStr.push_back(list.at(j).childNodes().at(7).toElement().text());

            QString price       = list.at(j).childNodes().at(8).toElement().text();
            QString requiredQty = list.at(j).childNodes().at(9).toElement().text();
            QString totQty      = list.at(j).childNodes().at(10).toElement().text();
            Qstrlistweighing.push_back(list.at(j).childNodes().at(11).toElement().text());



            qDebug()<<"availedQty   : "<<availedQty;
            qDebug()<<"balQty       : "<<balQty;
            qDebug()<<"closingBal   : "<<closingBal;
            qDebug()<<"commName     : "<<commName;
            qDebug()<<"commcode     : "<<commcode;
            qDebug()<<"measureUnit  : "<<measureUnit;
            qDebug()<<"price        : "<<price;
            qDebug()<<"requiredQty  : "<<requiredQty;
            qDebug()<<"totQty       : "<<totQty<<"#########";
            qDebug()<<"MinQtyStr.at(j)"<<MinQtyStr.at(j);
            qDebug()<<"Qstrlistweighing :"<<Qstrlistweighing.at(j);


            query.clear();
            query.prepare("INSERT INTO BenQuotaTable (RationCardId, CardHolderName, BalQty, MonthlyQuota, ItemCode, ItemType, ItemPrice, AvailedQty, MeasureUnit, RequiredQty, closingBal,commNamell,weighingFlag,minimumqty) VALUES( :RationCardId, :CardHolderName, :BalQty, :MonthlyQuota, :ItemCode, :ItemType, :ItemPrice, :AvailedQty, :MeasureUnit, :RequiredQty, :closingBal,:commNamell,:weighingFlag,:minimumqty)");
            query.bindValue(":RationCardId", ui->lineEditAnganwadiCardNo->text().toAscii().data());
            query.bindValue(":CardHolderName","");
            query.bindValue(":BalQty",  balQty.trimmed());
            query.bindValue(":MonthlyQuota", totQty.trimmed());
            query.bindValue(":ItemCode",commcode.trimmed());
            query.bindValue(":ItemType",commName.trimmed());
            query.bindValue(":ItemPrice",price.trimmed());
            query.bindValue(":AvailedQty",availedQty.trimmed());
            query.bindValue(":MeasureUnit", measureUnit.trimmed());
            query.bindValue(":RequiredQty", requiredQty.trimmed());
            query.bindValue(":closingBal", closingBal.trimmed());
            query.bindValue(":commNamell", commNamell.trimmed());
            query.bindValue(":weighingFlag", Qstrlistweighing.at(j));
            query.bindValue(":minimumqty", MinQtyStr.at(j));

            int ret = query.exec();
            if(ret == false){ qDebug()<<"BenQuotaTable :"<<query.lastError();return -1;}

            query.clear();
            query.prepare("INSERT INTO ItemMaster (ItemCode, ItemType, ItemNameEn, ItemNameTel, ItemPrice, Month, Year) VALUES( :ItemCode, :ItemType, :ItemNameEn, :ItemNameTel, :ItemPrice, :Month, :Year)");
            query.bindValue(":ItemCode",    commcode.trimmed());
            query.bindValue(":ItemType",    commName.trimmed());
            query.bindValue(":ItemNameEn",  commName.trimmed());
            query.bindValue(":ItemNameTel", commNamell);
            query.bindValue(":ItemPrice",   price.trimmed());
            query.bindValue(":Month",       "");
            query.bindValue(":Year",        "");

            ret = query.exec();
            if(ret == false)
            {
                qDebug()<<"ItemMaster :"<<query.lastError();

                CMisc miscObj;
                miscObj.DisplayWideMsgBox((char *)"Item Name Should be Unique");

                return -1;
            }

            query.clear();
            query.prepare("UPDATE ItemMaster SET ItemPrice=? WHERE ItemNameEn = ?");
            query.addBindValue(price.trimmed());
            query.addBindValue(commName.trimmed());
            if(!query.exec())
            {
                qDebug() <<"UPDATE ItemMaster" <<query.lastError();
                return -1;
            }

            j++;
        }
        else if(Qawdetails.at(0).childNodes().at(i).nodeName() == "memberdetails")
        {
            QString memberName;
            memberName.clear();

            list = document.elementsByTagName("memberdetails");
            if(Handleflag == 1)
                memberName = list.at(k).childNodes().at(2).toElement().text();
            else
                memberName = list.at(k).childNodes().at(0).toElement().text();

            QString member_id   = list.at(k).childNodes().at(1).toElement().text();

            QString uid         = list.at(k).childNodes().at(3).toElement().text();

            qDebug() << "member_id   : "<<member_id.trimmed();
            qDebug() <<"memberName   : "<<memberName.trimmed();
            qDebug() <<"UID          : "<<uid.trimmed()<<"#########";

            QSqlQuery query;
            /*query.prepare("INSERT INTO CustomerFamily (RationCardId, MemberNameEn, MemberUID, UID, bfd_1, bfd_2, bfd_3, MemberFusion,UID_Status,encuid,MemberNamell,Memberauth_finger,Memberauth_iris,Memberauth_manual,Memberauth_otp) values(:RationCardId, :MemberNameEn, :MemberUID, :UID, :bfd_1, :bfd_2, :bfd_3, :MemberFusion,:UID_Status,:encuid,:MemberNamell,:Memberauth_finger,:Memberauth_iris,:Memberauth_manual,:Memberauth_otp)");
            query.bindValue(":RationCardId",ui->lineEditAnganwadiCardNo->text().trimmed());
            query.bindValue(":MemberNameEn",memberName.trimmed());
            query.bindValue(":MemberUID", uid);
            query.bindValue(":UID","");
            query.bindValue(":bfd_1", "");
            query.bindValue(":bfd_2", "");
            query.bindValue(":bfd_3", "");
            query.bindValue(":MemberFusion", "");
            query.bindValue(":UID_Status", "");
            query.bindValue(":encuid",member_id);
            query.bindValue(":MemberNamell", "");
            query.bindValue(":Memberauth_finger", "");
            query.bindValue(":Memberauth_iris", "");
            query.bindValue(":Memberauth_manual", "");
            query.bindValue(":Memberauth_otp", "");*/
            char TmpStr[200];
            sprintf(TmpStr,"INSERT INTO CustomerFamily values('%s','%s','%s','','','','','','','%s','','','','','')",ui->lineEditAnganwadiCardNo->text().trimmed().toAscii().data()
                    ,memberName.trimmed().toAscii().data(),uid.toAscii().data(),member_id.toAscii().data());
            qDebug()<<"TmpStr :"<<TmpStr;

            int ret = query.exec(TmpStr);
            if(ret == false){
                qDebug()<<"In Customer Details :"<<query.lastError();return -1;}

            query.clear();

            memset(TmpStr,0x00,sizeof(TmpStr));
            sprintf(TmpStr,"INSERT INTO ICDSCustomerFamily values('%s','%s','%s')",member_id.trimmed().toAscii().data(),
                    memberName.trimmed().toAscii().data(),uid.trimmed().toAscii().data());
            /*query.prepare("INSERT INTO ICDSCustomerFamily (member_id, member_name, member_uid) VALUES(:member_id, :member_name, :member_uid)");
            query.bindValue(":member_id",member_id.trimmed());
            query.bindValue(":member_name",memberName.trimmed());
            query.bindValue(":member_uid", uid.trimmed());*/
            qDebug()<<"TmpStr :"<<TmpStr;
            ret = query.exec(TmpStr);
            if(ret == false){ qDebug()<<"ICDSCustomerFamily Error:"<<query.lastError();return -1;}

            k++;
        }
    }
    qDebug()<<"Returned";

    return 0;
}


int Widget::MdmTxnRcpt()
{
    char prn[512];
    LogoPrint logo;
    logo.PrintLogo(stateCode.toInt());
    QSqlQuery query;
    QString TmpString,str;
    if(LangMode == 1)
    {

        str= stateReceiptHeaderEn;
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
        str.clear();

        str= "M D M RECEIPT";
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
        str.clear();
    }
    else
    {
        TmpString= stateReceiptHeaderLl;
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_CENTER );
        TmpString.clear();

        TmpString.clear();
        TmpString= QString::fromUtf8("एम डी एम ")+QString::fromUtf8(Qpds_tran_tel.toAscii().data());
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_CENTER );
    }
    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn,0x00,sizeof(prn));
    sprintf(prn,"select ShopID,Name from ShopOwnerDetails");
    query.clear();
    if(!query.exec(prn))
    {
    }
    query.next();
    if(LangMode == 1)
    {
            memset(prn, 0, sizeof(prn));
            sprintf(prn,"Owner Name   : %s",query.value(1).toString().toAscii().data());
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));
            sprintf(prn,"FPS ID       : %s",query.value(0).toString().toAscii().data());
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));
            sprintf(prn,"Consumer Name: %s",ui->twdgtEntitlement->item(ui->twdgtEntitlement->currentRow(),0)->text().toAscii().data());
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));
            sprintf(prn,"SCHOOL ID    : %s",ui->lineEditAnganwadiCardNo->text().toAscii().data());//,ui->label_146->text().toAscii().data());
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));
            sprintf(prn,"Transaction ID:%s",receiptid.toAscii().data());//query.value(2).toString().toAscii().data());
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            TmpString.clear();
            TmpString= QString::fromUtf8("Date: ") + ui->labelDate->text() +"  "+ ui->labelTime->text();
            prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

            memset(prn, 0, sizeof(prn));
            strcpy(prn, "------------------------------------------");
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));
            //sprintf(prn,"Commodity    Lifted  B.Qty  Rate  Price");
            sprintf(prn,"Commodity    Lifted  Rate  Price");
            prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }
    else
    {
            TmpString.clear();
            TmpString= QString::fromUtf8("एफपीएस का नाम :") + query.value(1).toString();
            prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

            TmpString.clear();
            TmpString= QString::fromUtf8("एफपीएस आईडी  :") + query.value(0).toString();
            prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

            TmpString.clear();
            TmpString= QString::fromUtf8("ग्राहक    :") + ui->twdgtEntitlement->item(ui->twdgtEntitlement->currentRow(),0)->text();
            prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

            TmpString.clear();
            TmpString= QString::fromUtf8("स्कूल आईडी  :") + ui->lineEditAnganwadiCardNo->text();// +"/"+ui->label_146->text().toAscii().data();
            prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

            TmpString.clear();
            TmpString= QString::fromUtf8("लेनदेन आईडी:") + receiptid;//query.value(2).toString().toAscii().data();
            prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

            TmpString.clear();
            TmpString= QString::fromUtf8("तारीख: ") + ui->labelDate->text() +"  "+ ui->labelTime->text();
            prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

            memset(prn, 0, sizeof(prn));
            strcpy(prn, "------------------------------------------");
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            TmpString.clear();
            //TmpString= QString::fromUtf8(" वस्तु   ")+ QString::fromUtf8("मात्रा(Kg)")+QString::fromUtf8(" शेष राशि ") + QString::fromUtf8("की दर ")+QString::fromUtf8("कीमत");
            TmpString= QString::fromUtf8(" वस्तु   ")+ QString::fromUtf8("मात्रा(Kg)")+ QString::fromUtf8("की दर ")+QString::fromUtf8("कीमत");
            prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);
    }
    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    for(int i=0;i<ui->tblCommIssue->rowCount();i++)
    {
            if(ui->tblCommIssue->item(i,3)->text().toFloat()>0.00)
            {
                    if(LangMode == 1)
                    {
                            memset(prn,0x00,sizeof(prn));
                            //sprintf(prn,"%-12s %-4.3f  %-5.2f  %-4.2f  %-7.2f",ui->tblCommIssue->item(i,0)->text().toAscii().data(),ui->tblCommIssue->item(i,3)->text().toFloat(),atof(ui->tblCommIssue->item(i,1)->text().toAscii().data())-atof(ui->tblCommIssue->item(i,3)->text().toAscii().data()),ui->tblCommIssue->item(i,2)->text().toFloat(),ui->tblCommIssue->item(i,4)->text().toFloat());
                            sprintf(prn,"%-12s %-4.3f  %-4.2f  %-7.2f",ui->tblCommIssue->item(i,0)->text().toAscii().data(),ui->tblCommIssue->item(i,3)->text().toFloat(),ui->tblCommIssue->item(i,2)->text().toFloat(),ui->tblCommIssue->item(i,4)->text().toFloat());
                            prn_write_text((unsigned char *)prn, strlen(prn), 0);
                    }
                    else
                    {
                            TmpString.clear();
                            TmpString = ui->tblCommIssue->item(i,0)->text();
                            if(TmpString.size()<12)
                            {
                                    for(int j=TmpString.size();j<12;j++)
                                            TmpString += " ";
                            }
                            sprintf(prn," %-3.3f %-3.2f %-4.2f",ui->tblCommIssue->item(i,3)->text().toFloat(),ui->tblCommIssue->item(i,2)->text().toFloat(),ui->tblCommIssue->item(i,4)->text().toFloat());
                            TmpString += prn;
                            qDebug()<<"TmpString :"<<TmpString;
                            prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);
                    }
            }

    }
    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    if(LangMode == 1)
    {
            memset(prn,0x00,sizeof(prn));
            sprintf(prn,"Total Amount : %7.2f",ui->lblTotalAmtVal_2->text().toFloat());
            prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }
    else
    {
            TmpString.clear();
            memset(prn,0x00,sizeof(prn));
            sprintf(prn,"%7.2f",ui->lblTotalAmtVal_2->text().toFloat());
            TmpString= QString::fromUtf8("कुल राशि  : ") + prn;
            prepareBmp(QString::fromUtf8(TmpString.toUtf8()),22,QString::fromUtf8(TmpString.toUtf8()),19,ALIGN_LEFT);
    }
    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
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
            TmpString.clear();
            TmpString= QString::fromUtf8("    सार्वजनिक वितरण विभाग:    ");
            prepareBmp(QString::fromUtf8(TmpString.toUtf8()),22,QString::fromUtf8(TmpString.toUtf8()),18,ALIGN_CENTER);

            TmpString.clear();
            TmpString= QString::fromUtf8("    (नोट: किग्रा/लीटर में मात्रा)    ");
            prepareBmp(QString::fromUtf8(TmpString.toUtf8()),22,QString::fromUtf8(TmpString.toUtf8()),18,ALIGN_CENTER);
    }
    prn_paper_feed(6);
}
