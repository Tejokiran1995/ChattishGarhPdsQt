#include "widget.h"
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
extern char  HD_HTS[8];
extern char  HD_HTS[8];

extern QString Qwadh_global;

extern "C"
{
#include"include/gl11.h"
int validate_uid( char *);
}

void Widget::on_btn_BenVerify_clicked()
{
    cidType="R";

    if(LangMode == 1)
        ui->lblRationCard->setText("Card No.");
    else
        ui->lblRationCard->setText(QString::fromUtf8("कार्ड नंबर"));

    ui->btn_RCNOFLAGBenverify->setChecked(true);
    ui->ldtBenVerifyCardNo->clear();
    ui->ldtBenVerifyCardNo->setFocus();
    ui->stackedWidget->setCurrentIndex(49);

}

void Widget::on_btnBack_BenVerify_clicked()
{
    ui->stackedWidget->setCurrentIndex(40);
}

void Widget::on_btn_RCNOFLAGBenverify_clicked(bool checked)
{
    ui->ldtBenVerifyCardNo->clear();
    ui->ldtBenVerifyCardNo->setReadOnly(false);
    ui->ldtBenVerifyCardNo->setEchoMode(QLineEdit::Normal);

    if(checked == true)
    {
        if(LangMode==1)
            ui->lblEkycCardNo_2->setText("Card no");
        else if(LangMode == 2 || LangMode == 5)
            ui->lblEkycCardNo_2->setText(trUtf8("कार्ड नंबर"));;
        cidType = "R";
        ui->ldtBenVerifyCardNo->setFocus();
    }
}

void Widget::on_btn_UIDFLAGBenverify_clicked(bool checked)
{
    ui->ldtBenVerifyCardNo->clear();
    ui->ldtBenVerifyCardNo->setReadOnly(false);
    ui->ldtBenVerifyCardNo->setEchoMode(QLineEdit::Password);

    if(checked == true)
    {
        if(LangMode==1)
            ui->lblEkycCardNo_2->setText("Aadhaar No.");
        else if(LangMode == 2 || LangMode == 5)
            ui->lblEkycCardNo_2->setText(trUtf8("आधार नं"));

        cidType = "U";
        ui->ldtBenVerifyCardNo->setFocus();
    }

}

void Widget::on_btnOK_BenVerify_clicked()
{
    if(ui->btn_RCNOFLAGBenverify->isChecked())
    {
        cidType="R";
        if(ui->ldtBenVerifyCardNo->text().length() == 0)
        {
            if(LangMode == 1)
            {
                miscObj.DisplayWideMsgBox("Please Enter RC Number");
            }
            else if (LangMode == 2)
                miscObj.tel_DisplayMsgBox("कृपया आरसी नंबर दर्ज करें");

            ui->ldtBenVerifyCardNo->clear();
            return;

        }
    }
    if(ui->btn_UIDFLAGBenverify->isChecked())
    {
        cidType="U";
        if(ui->ldtBenVerifyCardNo->text().length() < 12)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox("Please enter 12 digit uid number ");
            else
                miscObj.tel_DisplayMsgBox("कृपया 12 अंक यूआईडी नंबर दर्ज करें ");

            ui->ldtBenVerifyCardNo->clear();
            return;

        }
        CMisc obj;
        if(obj.validate_uid(ui->ldtBenVerifyCardNo->text().toAscii().data()) < 0)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox("Invalid UID");
            else
                miscObj.tel_DisplayMsgBox("अमान्य यू आई डी ");

            ui->ldtBenVerifyCardNo->clear();
            return;
        }

        rsaEncryption(ui->ldtBenVerifyCardNo->text().toAscii().data(),skey);
    }

    if(Get_BenVerify_CardHolderDetails()< 0)
        return;

    int row=0,count=0;
    char buff[160]="";
    memset(buff,0,sizeof(buff));

    QSqlQuery query1;
    query1.prepare("SELECT COUNT(MemberNameEn) FROM CustomerFamily where RationCardId=?");
    query1.addBindValue(RationCardId);
    if(!query1.exec())
        qDebug() << query1.lastError();
    else
    {
        while(query1.next())
            count = query1.value(0).toInt(0);
    }
    qDebug() <<"count -" <<count;

    if(count == 0)
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Members Details Not Found For The Card.");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox("सदस्य विवरण इस कार्ड के लिए उपलब्ध नहीं हैं");
        else if(LangMode == 5)
            miscObj.tel_DisplayWideMsgBox("सदस्यांसाठी सदस्यांचा\nतपशील सापडला नाही.");

        return;
    }
    ui->tableWidgetBenVerify->setRowCount(count);

    QSqlQuery query;
    query.prepare("select MemberNameEn,MemberUID,UID_Status,MemberId,bfd_1 from CustomerFamily where RationCardId=?");

    query.addBindValue(RationCardId);
    query.exec();


    while(query.next())
    {

        memset(buff,0,sizeof(buff));
        strcpy(buff, query.value(0).toString().toAscii().data());
        qDebug()<<"0 MemberNameEn = "<<buff;

        ui->tableWidgetBenVerify->setItem(row, 0, new QTableWidgetItem(buff));
        ui->tableWidgetBenVerify->resizeColumnToContents(0);
        ui->tableWidgetBenVerify->item(row, 0)->setFlags( ui->tableWidgetBenVerify->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
        ui->tableWidgetBenVerify->item(row, 0)->setFlags( ui->tableWidgetBenVerify->item(row,0)->flags() | Qt::ItemIsSelectable);

        memset(buff,0,sizeof(buff));
        strcpy(buff, query.value(1).toString().toAscii().data());

        ui->tableWidgetBenVerify->setItem(row, 1, new QTableWidgetItem(buff));
        ui->tableWidgetBenVerify->item(row, 1)->setFlags(ui->tableWidgetBenVerify->item(row, 1)->flags() ^ Qt::ItemIsEditable);
        ui->tableWidgetBenVerify->item(row,1)->setFlags( ui->tableWidgetBenVerify->item(row,1)->flags() | Qt::ItemIsSelectable);


        memset(buff,0,sizeof(buff));
        strcpy(buff, query.value(2).toString().toAscii().data());

        ui->tableWidgetBenVerify->setItem(row, 2, new QTableWidgetItem(buff));
        ui->tableWidgetBenVerify->item(row, 2)->setFlags(ui->tableWidgetBenVerify->item(row, 2)->flags() ^ Qt::ItemIsEditable);
        ui->tableWidgetBenVerify->item(row, 2)->setFlags( ui->tableWidgetBenVerify->item(row,2)->flags() | Qt::ItemIsSelectable);


        memset(buff,0,sizeof(buff));
        strcpy(buff, query.value(3).toString().toAscii().data());

        ui->tableWidgetBenVerify->setItem(row, 3, new QTableWidgetItem(buff));
        ui->tableWidgetBenVerify->item(row, 3)->setFlags(ui->tableWidgetBenVerify->item(row, 3)->flags() ^ Qt::ItemIsEditable);
        ui->tableWidgetBenVerify->item(row, 3)->setFlags( ui->tableWidgetBenVerify->item(row,3)->flags() | Qt::ItemIsSelectable);


        ui->tableWidgetBenVerify->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));
        ui->tableWidgetBenVerify->item(row, 4)->setFlags(ui->tableWidgetBenVerify->item(row, 4)->flags() ^ Qt::ItemIsEditable);
        ui->tableWidgetBenVerify->item(row, 4)->setFlags( ui->tableWidgetBenVerify->item(row,4)->flags() | Qt::ItemIsSelectable);
        // ui->tableWidgetBenVerify->item(row,3)->setData(Qt::BackgroundColorRole, QVariant(QColor(Qt::green)));

        ui->tableWidgetBenVerify->setRowHeight(row,20);

        row++;
    }

    //qDebug()<<"Column 1 Width = "<<ui->tableWidgetBenVerify->columnWidth(1);
    ui->tableWidgetBenVerify->hideColumn(3);
    ui->tableWidgetBenVerify->hideColumn(4);

    ui->tableWidgetBenVerify->setColumnWidth(0,200); //160
    ui->tableWidgetBenVerify->setColumnWidth(1,150); //110
    ui->tableWidgetBenVerify->setColumnWidth(2,150); //110



    //ui->tableWidgetBenVerify->item(row2, 3)->setData(Qt::BackgroundColorRole, QVariant(QColor(Qt::green)));

    ui->tableWidgetBenVerify->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetBenVerify->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetBenVerify->selectRow(0);
    ui->tableWidgetBenVerify->verticalHeader()->hide();

    iDealMemSelected = BEN_VERIFICATION;
    ui->ConsentFormcheckBox_8->setChecked(true);
    ui->stackedWidget->setCurrentIndex(50);

}


int Widget::Get_BenVerify_CardHolderDetails()
{
    qDebug()<<"Inside GetCardHolderDetails";
    int ret = 0;

    miscObj.AudioTest((char *)"183");

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == ENGLISH)
        dlgPopup1->ui->lblPopup->setText("Please Wait...\nDownloading Members Details...");
    else if(LangMode == HINDI)
        dlgPopup1->ui->lblPopup->setText(trUtf8("परिवार का सदस्य विवरण \n डाउनलोड हो रहा है"));

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    GPRS objGprs;
    if(objGprs.Check_Network_Available() < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return -1;
    }

    char FileName[64];
    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/BenVerify_CardHolder.xml");

    system("rm /mnt/jffs2/BenVerify_CardHolder.xml");
    system("rm /mnt/jffs2/BenVerify_CardHolderDetails.xml");

    ret = prepareCardHolderReqFile_BenVerify(FileName);

    system("cat /mnt/jffs2/BenVerify_CardHolder.xml");

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
            //strcpy(cUrlName,"http://164.100.137.92/ePosServiceBR1_0Test/breposServiceRes?wsdl");
        }
    }

    char cmdCurl[1024];
    memset(cmdCurl, 0, sizeof(cmdCurl));

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/BenVerify_CardHolderDetails.xml", responseTimedOutTimeInSec.toInt());


    qDebug()<<"\nCurl Cmd : "<<cmdCurl;
    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();

        return -1;
    }

    qDebug() << "************BenVerify_CardHolderDetails.xml**********";
    system("cat /mnt/jffs2/BenVerify_CardHolderDetails.xml");
    qDebug() << "*******************************************";

    dlgPopup1->close();
    dlgPopup1->deleteLater();


    if(ParsingFirstCondition("/mnt/jffs2/BenVerify_CardHolderDetails.xml") < 0)
        return -1;

    query.clear();
    query.exec("DELETE FROM CustomerFamily");

    parseBenVerify_CardHolderDetails("/mnt/jffs2/BenVerify_CardHolderDetails.xml");

    return 0;
}

int Widget::prepareCardHolderReqFile_BenVerify(char *FileName)
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

    database_queries obj;
    if(obj.getSessionId(SessionId) < 0)
    {
        miscObj.DisplayMsgBox("Session Id not found\nPlease try again");
        return -1;
    }
    char szShopId[16];
    memset(szShopId, 0, sizeof(szShopId));
    getShopId(szShopId);

    if(ui->btn_UIDFLAGBenverify->isChecked())
        sprintf(buff,"<?xml version=\"1.0\" encoding=\"UTF-8\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:beneficiaryVerificationDetails><fpsSessionId>%s</fpsSessionId><stateCode>%s</stateCode><id>%s</id><idType>%s</idType><fpsID>%s</fpsID><password>%s</password><hts>%s</hts></ns1:beneficiaryVerificationDetails></SOAP-ENV:Body></SOAP-ENV:Envelope>",SessionId,stateCode.toStdString().c_str(),asciiBase64Enc,cidType.toAscii().data(),szShopId,HD_PASSWORD, HD_HTS);
    else
        sprintf(buff,"<?xml version=\"1.0\" encoding=\"UTF-8\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:beneficiaryVerificationDetails><fpsSessionId>%s</fpsSessionId><stateCode>%s</stateCode><id>%s</id><idType>%s</idType><fpsID>%s</fpsID><password>%s</password><hts>%s</hts></ns1:beneficiaryVerificationDetails></SOAP-ENV:Body></SOAP-ENV:Envelope>",SessionId,stateCode.toStdString().c_str(),ui->ldtBenVerifyCardNo->text().toAscii().data(),cidType.toAscii().data(),szShopId,HD_PASSWORD, HD_HTS);

    out<<buff;

    file.close();

    return 0;
}

int Widget::parseBenVerify_CardHolderDetails(char *filename)
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

            if(xml.name() == "wadh")
            {
                xml.readNext();
                Qwadh = xml.text().toString();
                qDebug() << " >>>>>>wadh : " << Qwadh;
                // Qwadh_auth=Qwadh;
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
    strcpy(RationCardId,RCDetails.at(0).childNodes().at(0).toElement().text().toAscii().data());



    ChildCount = RCDetails.at(0).childNodes().count();
    qDebug() << "Child Count = " << ChildCount;

    int k = 0;
    for(i=0, j=0; i< ChildCount; i++)
    {
        qDebug()<<RCDetails.at(0).childNodes().at(i).nodeName().toAscii().data();

        if(RCDetails.at(0).childNodes().at(i).nodeName() == "rcMemberDetVerify")
        {
            list = document.elementsByTagName("rcMemberDetVerify");

            QString MemberId  = list.at(k).childNodes().at(0).toElement().text();
            QString memberName = list.at(k).childNodes().at(1).toElement().text();
            QString MemberNamell  = list.at(k).childNodes().at(2).toElement().text();
            QString memberFusion  = list.at(k).childNodes().at(3).toElement().text();

            QString uid         = list.at(k).childNodes().at(4).toElement().text();
            QString VeriftSts = list.at(k).childNodes().at(5).toElement().text();
            QString uidStatus   = list.at(k).childNodes().at(6).toElement().text();

            qDebug()<< "RCID   : "<<RationCardId;
            qDebug() <<"memberName   : "<<memberName;
            qDebug() <<"memberID   : "<<MemberId;
            qDebug() <<"memberFusion   : "<<memberFusion;
            qDebug() <<"UID          : "<<uid<<"#########";
            qDebug() <<"UID Status   : "<<uidStatus<<"#########";

            QSqlQuery query;
            query.prepare("INSERT INTO CustomerFamily (RationCardId, MemberNameEn, MemberUID, UID, bfd_1, bfd_2, bfd_3, MemberFusion, UID_Status, MemberId, MemberNamell) values(:RationCardId, :MemberNameEn, :MemberUID, :UID, :bfd_1, :bfd_2, :bfd_3, :MemberFusion, :UID_Status, :MemberId, :MemberNamell)");
            query.bindValue(":RationCardId",RationCardId);
            query.bindValue(":MemberNameEn",memberName);
            query.bindValue(":MemberUID", uid);
            query.bindValue(":UID","");
            query.bindValue(":bfd_1", VeriftSts);
            query.bindValue(":bfd_2", "");
            query.bindValue(":bfd_3", "");
            query.bindValue(":MemberFusion", memberFusion);
            query.bindValue(":UID_Status", uidStatus);
            query.bindValue(":MemberId", MemberId);
            query.bindValue(":MemberNamell",MemberNamell);

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

void Widget::on_btn_BenVerifyBack__clicked()
{
    ui->ldtBenVerifyCardNo->clear();
    ui->stackedWidget->setCurrentIndex(49);
}

void Widget::on_BenVerifyScanFP_clicked()
{
    iConsentFormBackFlag=6;
    ui->ConsentCheckBox->setChecked(true);
    ConsentForm();
    ui->stackedWidget->setCurrentIndex(21);
    return;
}

//void Widget::on_BenVerifyScanFP_clicked()
//{
//    iDealMemSelected = BEN_VERIFICATION;
//    char UIDStatus[160]="";
//    memset(UIDStatus,0,sizeof(UIDStatus));

//    int currentrow = ui->tableWidgetBenVerify->currentRow();

//    strcpy(UIDStatus,ui->tableWidgetBenVerify->item(currentrow,2)->text().toAscii().data());

//    qDebug()<<"UIDStatus"<<UIDStatus<<endl;

//    //if(strcmp(UIDStatus,"Verified")==0)
//    if(ui->tableWidgetBenVerify->item(currentrow,4)->text().toUpper() == "Y")
//    {
//        ui->ldtBenVerifyCardNo->clear();

//        if(LangMode == 1)
//            miscObj.DisplayWideMsgBox("UID is Already Verified");
//        else if(LangMode == 2)
//            miscObj.tel_DisplayMsgBox("UID पहले से सत्यापित है");

//        sleep(2);
//        return;
//    }

//    system("rm -rf /mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml");

//    if(EKYC_Calling(2) < 0)
//    {
////        ui->ldtBenVerifyCardNo->clear();
////        ui->stackedWidget->setCurrentIndex(49);
//        return; //doubt
//    }
//    else
//    {
//        Print_Beneficiarydetails("/mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml");
//        ui->ldtBenVerifyCardNo->clear();
//        ui->stackedWidget->setCurrentIndex(1);
//    }
//}

int Widget::Print_Beneficiarydetails(char *filename)
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
    QDomNodeList list;
    QDomNodeList RCDetails;

    RCDetails = document.elementsByTagName("return");
    ChildCount = RCDetails.at(0).childNodes().count();
    qDebug() << "Child Count = " << ChildCount;
    int k = 0;

    list = document.elementsByTagName("return");

    QString eKYCGeneder = list.at(k).childNodes().at(5).toElement().text();
    QString eKYCMemberName = list.at(k).childNodes().at(6).toElement().text();

    qDebug() << "eKYCGeneder    : " << eKYCGeneder;
    qDebug() << "eKYCMemberName : " << eKYCMemberName;

    if(eKYCGeneder.length()==0)
        return -1;

    prn_open();
    if(prn_paperstatus() < 0)
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया कागज डालें");
        else if(LangMode == 5)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया पेपर ठेवा");
        prn_close();

        return -1;
    }
    if(miscObj.Battery_check() != 0) //01072017
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद प्रिंट करने में असमर्थ. \n कृपया बैटरी चार्ज करें");
        else if(LangMode == 5)
            miscObj.tel_DisplayWideMsgBox((char *)"कमी बॅटरीमुळे प्रवेश मुद्रित करण्यात अक्षम. \n कृपया बॅटरी चार्ज करा");
        prn_close();

        return -1;

    }

    popup_msg *dlgPopup3 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup3->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup3->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें।.."));
    else if(LangMode == 5)
        dlgPopup3->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करा"));


    dlgPopup3->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();


    LogoPrint objLogo;
    objLogo.PrintLogo(stateCode.toInt());

    database_queries obj;
    LangMode = obj.getLangMode();

    QString str;
    str.clear();

    str= stateReceiptHeaderEn;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
    str.clear();

    char prn[120]="";

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "%s" ,"    Verification Receipt");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    char RTC[32], DATE[16], TIME[16], JULIAN_DAYS[8];

    char szDashedDt[16],szDashedTm[16];

    memset(szDashedDt, 0, sizeof(szDashedDt));
    memset(szDashedTm, 0, sizeof(szDashedTm));

    memset(RTC, 0, sizeof(RTC));
    memset(DATE, 0, sizeof(DATE));
    memset(TIME, 0, sizeof(TIME));
    memset(JULIAN_DAYS, 0, sizeof(JULIAN_DAYS));

    GetRTC_Params(RTC, DATE, TIME, JULIAN_DAYS);

    miscObj.DDMMYYYY2DD_MM_YYYY(DATE, szDashedDt);
    miscObj.HHMMSS2HH_MM_SS(TIME, szDashedTm);

    memset(prn, 0, sizeof(prn));
    sprintf(prn, " Date: %s        Time: %s", szDashedDt, szDashedTm);
    prn_write_text((unsigned char *)prn, strlen(prn), 0);


    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "FPS ID : %s",ui->label_shopid->text().toAscii().data());
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "Name : %s",eKYCMemberName.toAscii().data());
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "Gender : %s",eKYCGeneder.toAscii().data());
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "Ration Card Number : %s",RationCardId);
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "Status : %s","Verified");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    prn_paper_feed(5);

    dlgPopup3->close();
    dlgPopup3->deleteLater();

    prn_close();

    return 0;
}
