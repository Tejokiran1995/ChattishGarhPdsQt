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

extern char HD_PASSWORD[64];
extern char  HD_HTS[8];
extern QString Qwadh_global;
extern int EKYC_TEST_FLAG;
extern pData pid_info;
extern QString Servicename;

void Widget::on_btnAadhaarServices_clicked()
{
    ui->BtnBenIRIS->hide();
    ui->stackedWidget->setCurrentIndex(40);
}

void Widget::on_btnBackAadhaarServices_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Widget::on_btnEKYC_Update_clicked()
{
    qDebug()<<"PAGE 10";
    ui->ldtEkycCardNo->setEchoMode(QLineEdit::Normal);

    ui->radioButton_RCNOFLAGEKYC->setChecked(true);
    cidType = "R";

    if(LangMode == 1)
        ui->lblEkycCardNo->setText("Card No.");
    else
        ui->lblEkycCardNo->setText(QString::fromUtf8("कार्ड नंबर"));;
    ui->ldtEkycCardNo->clear();

    char Distofficecode[16]="";
    memset(Distofficecode,0,sizeof(Distofficecode));
    database_queries qry;
    qry.getdistofficecode(Distofficecode);

    ui->ldtEkycCardNo->setFocus();
    iMainTransaction = EKYC_TRANSACTION;
    ui->stackedWidget->setCurrentIndex(10);

}

void Widget::on_btnBack_EKYC_clicked()
{
    ui->stackedWidget->setCurrentIndex(40);

}

void Widget::on_radioButton_RCNOFLAGEKYC_clicked(bool checked)
{
    ui->ldtEkycCardNo->clear();

    char Distofficecode[16]="";
    memset(Distofficecode,0,sizeof(Distofficecode));

    database_queries qry;
    qry.getdistofficecode(Distofficecode);

    ui->ldtEkycCardNo->setFocus();
    ui->ldtEkycCardNo->setEchoMode(QLineEdit::Normal);

    if(LangMode==1)
        ui->lblEkycCardNo->setText("Card No.");
    else if(LangMode == 2)
        ui->lblEkycCardNo->setText(trUtf8("कार्ड नंबर")); // for ekyc

    /*cidType Initialization*/
    cidType = "R"; // mode sending R
    ui->radioButton_UIDFLAGEKYC->setChecked(false);
}

void Widget::on_radioButton_UIDFLAGEKYC_clicked()
{
    ui->ldtEkycCardNo->clear();
    ui->ldtEkycCardNo->setFocus();
    ui->ldtEkycCardNo->setEchoMode(QLineEdit::Password);

    if(LangMode==1)
        ui->lblEkycCardNo->setText("Aadhaar NO");
    else if(LangMode == 2)
        ui->lblEkycCardNo->setText(trUtf8("आधार संख्या")); // for ekyc
    /*cidType Initialization*/
    cidType = "U"; // mode sending U
}

void Widget::on_btnOK_EKYC_clicked()
{
    ui->ldtEKYC_UID_Entered->clear();
    iMainTransaction = EKYC_TRANSACTION;

    if(ui->radioButton_RCNOFLAGEKYC->isChecked()){
        if(ui->ldtEkycCardNo->text().length()< 3){
            if(LangMode==1)
                miscObj.DisplayWideMsgBox((char *)"Enter correct RC number");
            else if(LangMode==2)
                miscObj.tel_DisplayWideMsgBox((char *)"कृपया कार्ड नंबर दर्ज करें");
            return;
        }
    }
    if(ui->radioButton_UIDFLAGEKYC->isChecked()){
        if(ui->ldtEkycCardNo->text().length() < 12){
            if(LangMode==1)
                miscObj.DisplayWideMsgBox((char *)"Please enter 12 digit uid number ");
            else if(LangMode==2)
                miscObj.tel_DisplayWideMsgBox((char *)"कृपया 12 अंक यूआईडी नंबर दर्ज करें.");

            return;
        }
        if(miscObj.validate_uid(ui->ldtEkycCardNo->text().toAscii().data()) < 0){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Invalid UID");
            else
                miscObj.tel_DisplayMsgBox((char *)"अमान्य यूआईडी");

            ui->ldtEkycCardNo->clear();
            return;
        }
        rsaEncryption(ui->ldtEkycCardNo->text().toAscii().data(),skey); //RSA encryption
    }
    MemberDetails();
}

void Widget::MemberDetails()
{
    miscObj.Check_mplayer_Exists();
    char buff[128]="";
    int row=0, count=0;

    iModuleType = DUMMY_TXN;

    iFestivalFlag = 0;
    Finger_Iris_Selection = 0;
    strcpy((char *)CardType, "NA");

    /******** TIME LAP START************/
    QTime tim;
    tim.restart();
    tim.start();
    /******************************/
    int iRationType = 0;

    qDebug()<<"iRation Type = "<<iRationType;
    if(GetEkycCardHolderDetails(iRationType) < 0){
        return;
    }

    ui->lblRCNO_Val->setText(RationCardId);
    ui->lblRCNO_Val_2->setText(RationCardId);

    QSqlQuery query1;
    query1.prepare("SELECT COUNT(MemberNameEn) FROM CustomerFamily where RationCardId=?");
    query1.addBindValue(RationCardId);

    if(!query1.exec())
        qDebug() << query1.lastError();
    else{
        while(query1.next())
            count = query1.value(0).toInt(0);
    }
    qDebug() <<"count -" <<count;

    if(count == 0){
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Members Details Not Found For The Card.");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कार्ड के लिए सदस्यों का विवरण नहीं मिला.");
        return;
    }
    ui->twdgtEntitlement->setRowCount(count);

    QSqlQuery query;
    query.prepare("select MemberNameEn, MemberUID, bfd_1, bfd_2, bfd_3,MemberId,MemberNamell from CustomerFamily where RationCardId=?");

    query.addBindValue(RationCardId);
    query.exec();

    qDebug() << ui->ldtEkycCardNo->text().toAscii().data() << "###";
    qDebug() << "RationCardId :" << RationCardId;

    while(query.next()){
        memset(buff,0,sizeof(buff));
        if(LangMode==1){
            strcpy(buff, query.value(0).toString().toAscii().data());
            ui->twdgtEntitlement->setItem(row, 0, new QTableWidgetItem(buff));
        }else if(LangMode==2)
            ui->twdgtEntitlement->setItem(row, 0, new QTableWidgetItem(query.value(6).toString()));
        qDebug()<<"0 MemberName = "<<buff;

        ui->twdgtEntitlement->resizeColumnToContents(0);
        ui->twdgtEntitlement->item(row, 0)->setFlags( ui->twdgtEntitlement->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
        ui->twdgtEntitlement->item(row, 0)->setFlags( ui->twdgtEntitlement->item(row,0)->flags() | Qt::ItemIsSelectable);

        memset(buff,0,sizeof(buff));
        strcpy(buff, query.value(1).toString().toAscii().data());

        qDebug()<<"Masked Aadhaar No : "<<buff<<"#####";
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
        qDebug()<<"5 member id = "<<buff;

        ui->twdgtEntitlement->setItem(row, 6, new QTableWidgetItem(buff));
        ui->twdgtEntitlement->item(row, 6)->setFlags(ui->twdgtEntitlement->item(row, 6)->flags() ^ Qt::ItemIsEditable);
        ui->twdgtEntitlement->item(row,6)->setFlags( ui->twdgtEntitlement->item(row,6)->flags() | Qt::ItemIsSelectable);

        ui->twdgtEntitlement->setRowHeight(row,20);
        row++;
    }
    qDebug()<<"Column 1 Width = "<<ui->twdgtEntitlement->columnWidth(1);

    ui->twdgtEntitlement->setColumnWidth(0,170); //200
    ui->twdgtEntitlement->setColumnWidth(1,200); //150

    ui->twdgtEntitlement->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->twdgtEntitlement->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twdgtEntitlement->selectRow(0);
    ui->twdgtEntitlement->verticalHeader()->hide();

    ui->twdgtEntitlement->setColumnHidden(2,true);
    ui->twdgtEntitlement->setColumnHidden(3,true);
    ui->twdgtEntitlement->setColumnHidden(4,true);
    ui->twdgtEntitlement->setColumnHidden(5, true);
    ui->twdgtEntitlement->setColumnHidden(6, true);
    ui->twdgtEntitlement->setColumnHidden(7, true);

    QString Qtype_card;
    query.clear();
    query.prepare("SELECT type_card FROM CustomerMaster");
    if(!query.exec())
        qDebug() << query.lastError();
    else{
        while(query.next()){
            Qtype_card = query.value(0).toString();
        }
    }
    ui->label_type_card->clear();
    ui->label_type_card->setText(Qtype_card);

    if(LangMode == 1){
        ui->lblRCNO->setText("Card No: ");
        ui->pushButton_2->setText("EKYC");
    }else if(LangMode == 2){
        ui->lblRCNO->setText(trUtf8("कार्ड नंबर:"));
        ui->pushButton_2->setText(trUtf8("आधार\nसंबंध"));
    }
    ui->ConsentFormcheckBox->setChecked(true);
    ui->stackedWidget->setCurrentIndex(5);

    return;
}

int Widget::GetEkycCardHolderDetails(int iRationType)
{
    int ret = 0;
    GPRS objGprs;

    miscObj.AudioTest((char *)"183");

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == ENGLISH)
        dlgPopup1->ui->lblPopup->setText("Please Wait...\nDownloading Members Details...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.\nसदस्यों विवरण डाउनलोड हो रहा है."));

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    if(objGprs.Check_Network_Available() < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();

        return -1;
    }
    char FileName[64];
    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/EkycCardHolder.xml");

    system("rm /mnt/jffs2/EkycCardHolder.xml");
    system("rm /mnt/jffs2/EkycCardHolderDetails.xml");

    ret = prepareEkycCardHolderReqFile(FileName);

    system("cat /mnt/jffs2/EkycCardHolder.xml");

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

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/EkycCardHolderDetails.xml", responseTimedOutTimeInSec.toInt());

    qDebug()<<"\nCurl Cmd : "<<cmdCurl;
    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();

        return -1;
    }

    qDebug() << "************EkycCardHolderDetails.xml**********";
    system("cat /mnt/jffs2/EkycCardHolderDetails.xml");
    qDebug() << "*******************************************";

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    if(ParsingFirstCondition((char *)"/mnt/jffs2/EkycCardHolderDetails.xml") < 0)
        return -1;

    query.clear();
    query.exec("DELETE FROM CustomerFamily");
    query.clear();
    query.exec("DELETE FROM CustomerMaster");

    parseEkycCardHolderDetails("/mnt/jffs2/EkycCardHolderDetails.xml");
    return 0;
}

int Widget::prepareEkycCardHolderReqFile(char *FileName)
{
    qDebug()<<"Out File (" <<FileName << ") Preparation ....";
    QFile file(FileName);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug()<< FileName << " file Open failed";
        return -1;
    }

    QTextStream out(&file);

    char buff[2048];
    memset(buff, 0, sizeof(buff));

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0)
    {
        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }
    char szShopId[16];
    memset(szShopId, 0, sizeof(szShopId));
    getShopId(szShopId);

    //RD-2.0 subbu 07/11/2017

    if(ui->radioButton_UIDFLAGEKYC->isChecked())
        sprintf(buff, "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getEKYCMemberDetailsRD><fpsSessionId>%s</fpsSessionId><stateCode>%s</stateCode><id>%s</id><idType>%s</idType><fpsID>%s</fpsID><password>%s</password><hts>%s</hts></ns1:getEKYCMemberDetailsRD></SOAP-ENV:Body></SOAP-ENV:Envelope>",SessionId,stateCode.toStdString().c_str(),asciiBase64Enc, cidType.toAscii().data() ,szShopId, HD_PASSWORD, HD_HTS);
    else
        sprintf(buff, "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getEKYCMemberDetailsRD><fpsSessionId>%s</fpsSessionId><stateCode>%s</stateCode><id>%s</id><idType>%s</idType><fpsID>%s</fpsID><password>%s</password><hts>%s</hts></ns1:getEKYCMemberDetailsRD></SOAP-ENV:Body></SOAP-ENV:Envelope>",SessionId,stateCode.toStdString().c_str(), ui->ldtEkycCardNo->text().toAscii().data(), cidType.toAscii().data() ,szShopId, HD_PASSWORD, HD_HTS);

    out<<buff;
    file.close();

    return 0;
}

int Widget::parseEkycCardHolderDetails(char *filename)
{
    QString Qwadh;

    QFile file1(filename);
    if(!file1.exists())
    {
        qDebug() << "File Not Found";
        return -1;
    }
    if(!file1.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error Reading File" << endl;
        return -2;
    }
    ///////////////////RD 2.0///////////////////////////////////////

    QXmlStreamReader xml(&file1);
    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "zwadh")
            {
                xml.readNext();
                Qwadh = xml.text().toString();
                qDebug() << " >>>>>>wadh : " << Qwadh;
            }
        }
    }

    file1.close();

    Qwadh_global.clear();
    Qwadh_global=Qwadh;

    //////////////////  RD 2.0 //////
    QFile file(filename);
    if(!file.exists())
    {
        qDebug() << "File Not Found";
        return -1;
    }
    if(file.size() == 0)
        return -1;

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

    int ChildCount=0,i = 0, j=0;
    QDomNodeList list;
    QDomNodeList RCDetails;

    RCDetails = document.elementsByTagName("return");
    strcpy(RationCardId,RCDetails.at(0).childNodes().at(0).toElement().text().toAscii().data()); //Added by Vivek 12062017

    ChildCount = RCDetails.at(0).childNodes().count();
    qDebug() << "Child Count = " << ChildCount;

    int k = 0;
    for(i=0, j=0; i< ChildCount; i++)
    {
        qDebug()<<RCDetails.at(0).childNodes().at(i).nodeName().toAscii().data();

        if(RCDetails.at(0).childNodes().at(i).nodeName() == "rcMemberDet")
        {
            list = document.elementsByTagName("rcMemberDet");

            QString bfd_1 = list.at(k).childNodes().at(0).toElement().text();
            QString bfd_2 = list.at(k).childNodes().at(1).toElement().text();
            QString bfd_3 = list.at(k).childNodes().at(2).toElement().text();
            QString MemberId  = list.at(k).childNodes().at(3).toElement().text();
            QString memberName = list.at(k).childNodes().at(4).toElement().text();      // New Eelement added 06052017
            QString memberNamell = list.at(k).childNodes().at(5).toElement().text();      // New Eelement added 06052017

            QString memberFusion  = list.at(k).childNodes().at(6).toElement().text();
            QString uid         = list.at(k).childNodes().at(7).toElement().text();
            QString uidStatus   = list.at(k).childNodes().at(8).toElement().text();

            qDebug()<< "RCID   : "<<RationCardId;
            qDebug() << "bfd_1 : " << bfd_1;
            qDebug() << "bfd_2 : " << bfd_2;
            qDebug() << "bfd_3 : " << bfd_3;
            qDebug() <<"memberName   : "<<memberName;
            qDebug() <<"memberID   : "<<MemberId;              // New Eelement added 06052017
            qDebug() <<"memberFusion   : "<<memberFusion;
            qDebug() <<"UID          : "<<uid<<"#########";
            qDebug() <<"UID Status   : "<<uidStatus<<"#########";

            QSqlQuery query;
            query.prepare("INSERT INTO CustomerFamily (RationCardId, MemberNameEn, MemberUID, UID, bfd_1, bfd_2, bfd_3, MemberFusion, UID_Status, MemberId, MemberNamell) values(:RationCardId, :MemberNameEn, :MemberUID, :UID, :bfd_1, :bfd_2, :bfd_3, :MemberFusion, :UID_Status, :MemberId, :MemberNamell)");
            query.bindValue(":RationCardId",RationCardId);
            query.bindValue(":MemberNameEn",memberName);
            query.bindValue(":MemberUID", uid);
            query.bindValue(":UID","");
            query.bindValue(":bfd_1", bfd_1);
            query.bindValue(":bfd_2", bfd_2);
            query.bindValue(":bfd_3", bfd_3);
            query.bindValue(":MemberFusion", memberFusion);
            query.bindValue(":UID_Status", uidStatus);
            query.bindValue(":MemberId", MemberId);
            query.bindValue(":MemberNamell",memberNamell);

            int ret = query.exec();
            if(ret == false)
            {
                qDebug()<<"ERROR :::: "<<query.lastError();
                return -1;
            }

            k++;
        }


    }
    return 0;
}

void Widget::EKYC_FP_Submit()
{
    QString QDealerUID;
    QSqlQuery query;
    char cmemberid[64]="";

    int currentrow = ui->twdgtEntitlement->currentRow();
    strcpy(cmemberid,ui->twdgtEntitlement->item(currentrow,6)->text().toAscii().data());

    query.prepare("SELECT UID FROM ShopOwnerDetails");
    if(!query.exec())
        qDebug() << query.lastError();
    else{
        while(query.next()){
            QDealerUID = query.value(0).toString();
        }
    }
    qDebug() << "EKYC_FP_Submit cmemberid = " << cmemberid;
    qDebug() << "wadh_value = " << Qwadh_global.toAscii().data();

    if(rdCallinit_ekyc(1,Qwadh_global.toAscii().data())< 0){
        qDebug()<<"rdCallinit_ekyc Failed"<<endl;
        return;
    }

    char cUID_Dummy[16];
    char b64Templ[1024];
    memset(cUID_Dummy, 0, sizeof(cUID_Dummy));
    memset(b64Templ, 0, sizeof(b64Templ));

    strcpy(cUID_Dummy, ui->ldtEKYC_UID_Entered->text().toAscii().data());
    char cCust_Mob_Hmac[512];
    char cCust_Mob_Skey[512];
    char cCust_Mob_Pload[1024];

    memset(cCust_Mob_Hmac, 0, sizeof(cCust_Mob_Hmac));
    memset(cCust_Mob_Skey, 0, sizeof(cCust_Mob_Skey));
    memset(cCust_Mob_Pload, 0, sizeof(cCust_Mob_Pload));

    popup_msg *dlgPopup3 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup3->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup3->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें."));
    dlgPopup3->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    system("rm /mnt/jffs2/EKYCAuthenticateReq.xml");
    system("rm /mnt/jffs2/EKYCAuthenticateResp.xml");

    FILE *fp;
    fp = fopen("/mnt/jffs2/EKYCAuthenticateReq.xml","w");
    if(fp == NULL){
        qDebug() << "File Not Found" << endl;
        dlgPopup3->close();
        dlgPopup3->deleteLater();
        return;
    }

    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));

    miscObj.GetMachineId(cmachineID);

    char POI_POA[8];
    char Mob[8];

    memset(POI_POA, 0, sizeof(POI_POA));
    memset(Mob, 0, sizeof(Mob));

    if(ui->ChkBox_Y_POI_POA->isChecked() == true)
        strcpy(POI_POA, "Y");
    else
        strcpy(POI_POA, "N");
    qDebug()<<"POI_ POA = "<<POI_POA;

    if(ui->ChkBox_Y_Mob_eMail->isChecked() == true)
        strcpy(Mob, "Y");
    else
        strcpy(Mob, "N");
    qDebug()<<"Mob Email = "<<Mob;

    fprintf(fp,"%s","<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    fprintf(fp,"%s","<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://service.fetch.rationcard/\">");
    fprintf(fp,"%s","<SOAP-ENV:Body>");
    fprintf(fp,"%s","<ns1:getEKYCAuthenticateRD>");
    fprintf(fp,"<fpsSessionId>%s</fpsSessionId>",SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp,"<Shop_no>%s</Shop_no>",ui->ldtShpId->text().toAscii().data());
    fprintf(fp,"<terminal_id>%s</terminal_id>",cmachineID);
    fprintf(fp,"<existingRCNumber>%s</existingRCNumber>",ui->lblEKYC_RCNO_Val->text().toAscii().data());
    fprintf(fp,"<rcMemberName>%s</rcMemberName>",MemberName);
    fprintf(fp,"<rcUid>%s</rcUid>","0");//MemberUID
    //fprintf(fp,"<rcUid>%s</rcUid>",ui->ldtEKYC_UID_Entered->text().toAscii().data());//MemberUID
    fprintf(fp,"<memberId>%s</memberId>",cmemberid);
    fprintf(fp,"%s","<ekycresAuth>");
    fprintf(fp,"<dc>%s</dc>",pid_info.dev_data.dc);
    fprintf(fp,"<dpId>%s</dpId>",pid_info.dev_data.dpId);
    fprintf(fp,"<mc>%s</mc>",pid_info.dev_data.mc);
    fprintf(fp,"<mid>%s</mid>",pid_info.dev_data.mi);
    fprintf(fp,"<rdId>%s</rdId>",pid_info.dev_data.rdsId);
    fprintf(fp,"<rdVer>%s</rdVer>",pid_info.dev_data.rdsVer);
    fprintf(fp,"<res_Consent_POIandPOA>%s</res_Consent_POIandPOA>","Y");
    fprintf(fp,"<res_Consent_mobileOREmail>%s</res_Consent_mobileOREmail>","Y");
    fprintf(fp,"<res_certificateIdentifier>%s</res_certificateIdentifier>",pid_info.ci);
    fprintf(fp,"<res_encHmac>%s</res_encHmac>",pid_info.Hmac);
    fprintf(fp,"<res_secure_pid>%s</res_secure_pid>",pid_info.pid);
    fprintf(fp,"<res_sessionKey>%s</res_sessionKey>",pid_info.Skey);
    fprintf(fp,"<res_uid>%s</res_uid>",asciiBase64Enc);
    fprintf(fp,"%s","</ekycresAuth>");
    fprintf(fp,"<password>%s</password>",HD_PASSWORD);
    fprintf(fp,"<eKYCType>%s</eKYCType>","eKYCS");
    fprintf(fp,"<Resp><errCode>%s</errCode><errInfo>%s</errInfo><nmPoints>%s</nmPoints><fCount>%s</fCount><fType>%s</fType><iCount>%s</iCount><iType>%s</iType><pCount>%s</pCount><pType>%s</pType><qScore>%s</qScore></Resp>",pid_info.err_code,pid_info.err_info,pid_info.nmPoints,pid_info.fCount,pid_info.fType,pid_info.iCount,pid_info.iType,pid_info.pCount,pid_info.pType,pid_info.qScore);;
    fprintf(fp,"%s","</ns1:getEKYCAuthenticateRD>");
    fprintf(fp,"%s","</SOAP-ENV:Body>");
    fprintf(fp,"%s","</SOAP-ENV:Envelope>");
    fclose(fp);

    free(pid_info.pid);
    pid_info.pid = NULL;

    qDebug() << "*********/mnt/jffs2/EKYCAuthenticateReq.xml *********";
    system("cat /mnt/jffs2/EKYCAuthenticateReq.xml");
    qDebug();
    qDebug() << "***************************************************";

    char cUrlName[256];
    query.clear();
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

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/EKYCAuthenticateReq.xml", cUrlName,  "/mnt/jffs2/EKYCAuthenticateResp.xml", responseTimedOutTimeInSec.toInt());

    qDebug()<<"\nCurl Cmd : "<<cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0){
        dlgPopup3->close();
        dlgPopup3->deleteLater();
        return;
    }
    qDebug() << "***********/mnt/jffs2/EKYCAuthenticateResp.xml************";
    system("cat /mnt/jffs2/EKYCAuthenticateResp.xml");
    qDebug();
    qDebug() << "***************************************************";

    dlgPopup3->close();
    dlgPopup3->deleteLater();

    if(ParsingFirstCondition((char *)"/mnt/jffs2/EKYCAuthenticateResp.xml") != 0){
        qDebug()<<"ParsingFirstCondition!=0";
        ui->ConsentFormcheckBox->setChecked(true);
        ui->stackedWidget->setCurrentIndex(5);
        return;
    }
    qDebug()<<"parseSeedingcCardHolderDetails"<<endl;
    //    parseSeedingcCardHolderDetails("/mnt/jffs2/EKYCAuthenticateResp.xml");
    if(parseSeedingcCardHolderDetails("/mnt/jffs2/EKYCAuthenticateResp.xml") < 0)
        return;

    ui->ldtUID_EKYC->clear();
    ui->ldtEKYC_UID_Entered->clear();

    ui->ConsentFormcheckBox->setChecked(true);
    ui->stackedWidget->setCurrentIndex(5);
    return;
}

int Widget::parseSeedingcCardHolderDetails(char *filename)
{
    QFile file(filename);
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
    QDomDocument document;
    if(!document.setContent( &file ) )
    {
        qDebug( "Failed to parse the file into a DOM tree." );
        file.close();
        return -3;
    }
    file.close();

    int ChildCount=0;
    int k = 0;
    QDomNodeList list;
    //    QDomNodeList RCDetails;
    //    RCDetails = document.elementsByTagName("return");

    list = document.elementsByTagName("return");

    ChildCount = list.at(0).childNodes().count();
    qDebug() << "Child Count = " << ChildCount;

    QString respCode = list.at(k).childNodes().at(0).toElement().text();

    memset(gauth_transaction_code, 0, sizeof(gauth_transaction_code));
    strcpy(gauth_transaction_code, list.at(k).childNodes().at(3).toElement().text().toAscii().data());
    qDebug()<<"gauth_transaction_code:"<<gauth_transaction_code;

    QString eKYCDOB = list.at(k).childNodes().at(4).toElement().text();
    QString eKYCGeneder = list.at(k).childNodes().at(5).toElement().text();
    QString eKYCMemberName = list.at(k).childNodes().at(6).toElement().text();
    QString eKYCPindCode  = list.at(k).childNodes().at(7).toElement().text();

    qDebug() << "respCode       : " << respCode;
    qDebug() << "eKYCDOB        : " << eKYCDOB;
    qDebug() << "eKYCGeneder    : " << eKYCGeneder;
    qDebug() << "eKYCMemberName : " << eKYCMemberName;
    qDebug() << "eKYCPindCode   : " << eKYCPindCode;

    char buff[512];
    memset(buff, 0, sizeof(buff));

    if(respCode == "E00")
    {
        if(LangMode==1)
        {
            sprintf(buff, "Name :%s\nDOB  :%s\nGender:%s\nPIN  :%s", eKYCMemberName.toAscii().data(), eKYCDOB.toAscii().data(), eKYCGeneder.toAscii().data(), eKYCPindCode.toAscii().data());
            miscObj.DisplayWideMsgBox(buff);
        }
        else if(LangMode==2)
        {
            sprintf(buff, "नाम :%s\nजन्म तारीख  :%s\nलिंग:%s\nपिन नं  :%s", eKYCMemberName.toAscii().data(), eKYCDOB.toAscii().data(), eKYCGeneder.toAscii().data(), eKYCPindCode.toAscii().data());
            miscObj.tel_DisplayWideMsgBox(buff);
        }
    }

    return 0;
}
