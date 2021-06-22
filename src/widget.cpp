//1512130273
#include "include/widget.h"
#include "ui_widget.h"
#include "iris.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<QStringList>
#include<QDialog>
//background-color: rgb(255, 85, 0);l
char  HD_PASSWORD[64];
char  HD_HTS[8];

QString Qtxnflow;
int receipt_type;
TxnUpload UploadData;
QString DeviceMacId;
fp_capture capture;
info dev_info;
pData pid_info;
rd_status rdStatus;
device_data dev_details;
fp_capture_ekyc capture_ekyc;
QString Qwadh_global;
QString Qwadh_Member_global;
QString zcommboCommCode_global;
QString zcommboStatus_global;
int memekycflag = 0;
int memmobotpflag = 0;
int OtppopTimeOut = 0;

QString Qwadh_Dealer_global,QwadhStatus,QwadhStatus_D,QwadhStatus_N1,QwadhStatus_N2,member_wadhstatus;

QString Qwadh_auth;

discoveryiris * BiomatiquesRDS_Discovery();
dev_infoiris * BiomatiquesRDS_DeviceInfo(discoveryiris * );
captureiris * BiomatiquesRDS_Capture(discoveryiris * ,pidopts );

iriTech_info1 iriTech_Info;
iriTech_rd_status iriTech_rdStatus1;
iriTech_eye_capture eye_Capture1;
iriTech_pData iriTech_pid_Info;

int Iritech_rd_service_discovery(iriTech_rd_status *iriTech_rdStatus1);
int Iritech_rd_capture(iriTech_pData *iriTech_pidInfo,iriTech_eye_capture *eye_Capture1);


extern "C"
{
int validate_uid( char *);
int usb_node_check(char *);
int usb_node_check_Essae(char *);
int data_weight(char *, char *);
int bt_connect(char *);
int bt_scan(char *);
int bt_node_chk();
int bt_pwr_on();
int bt_pwroff();
int bt_paring(char *, char *);
int bt_reconnect();
int data_weight(char *,char *);
int scanning(char bt_names[40][256]);
int getweight(char *, char *);
char bufExpiryStr[12];
char *hmac,*pload;
char *pidb, *encryptedSessKey;
int aes_enc_data_len,enc_sessn_key_len,hmac_len;
int UidEncode_b64_flag;
int EKYC_TEST_FLAG;

discoveryiris  temp;
discoveryiris *t;
dev_infoiris temp_dev_info;
dev_infoiris *tt;
captureiris temp_capture;
captureiris *ttt;

int device_search();
}

extern char gRes_Pid_TimeStamp[32];
extern char AuthCodeResp[1024];
extern char RRNresp[128];
extern char cAccBal[1024];
extern char cCustName[128];

int iCommMode;
struct TransactionLog TxnEJLog;

struct BIL_Header
{
    char protoVersion[8];
    char date[16];
    char time[16];
    char Vendor_ID[16];
    char TerminalID[16];
    char Agent_ID[16];
    char AgentCity[64];
    char AgentState[64];
    char AgentPincode[16];
    TransactionLog transactionLog;
};

typedef struct _BFD_RESP_CODE_MSG
{
    char aucRespCode[4];
    char aucRespMessage[128];
}tdstBfdRespCodeAndMessage;

tdstBfdRespCodeAndMessage gstRespCodeAndMessage[RESP_CODE_INDEX]=
{
    {"200", "Successfully Authenticated    "},
    {"202", "Rejected                      "},
    {"203", "Invalid UID                   "},
    {"204", "UID Is Already Authenticated  "},
    {"222", "Unknown Error                 "},
    {"100", "Successfully Authenticated   "},
    {"101", "Rejected                     "},
    {"103", "Invalid UID                  "},
    {"104", "UID Is Already Authenticated "},
    {"111", "UnKnown Error                "},
};


Widget::Widget(QWidget *parent):QWidget(parent),ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
    setWindowState(Qt::WindowFullScreen);
    //    setWindowTitle("NAGALAND PDS");
    setWindowTitle("PDS");
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFocusPolicy(Qt::NoFocus);

    QApplication::setOverrideCursor(Qt::BlankCursor);

    set_RDIcon();
    QTimer *rdtimer = new QTimer(this);
    connect(rdtimer, SIGNAL(timeout()), this, SLOT(set_RDIcon()));
    rdtimer->start(2000);

    setsignalStrength();
    QTimer *signaltimer = new QTimer(this);
    connect(signaltimer, SIGNAL(timeout()), this, SLOT(setsignalStrength()));
    signaltimer->start(12000);

    movie = NULL;
    batBuzzStatus = false;

    updateBatStatus();

    QTimer *batTimer = new QTimer(this);
    connect(batTimer, SIGNAL(timeout()), this, SLOT(updateBatStatus()));
    batTimer->start(10000);

    //    ui->label_141->setAttribute(Qt::WA_TranslucentBackground);
    //    ui->lblAP_Logo1->setAttribute(Qt::WA_TranslucentBackground);
    //    ui->lblAP_Logo1_3->setAttribute(Qt::WA_TranslucentBackground);

    ui->radioButton_scanbarcode->hide();
    ui->btnMobile_Seeding->hide();

    QFile filename;
    filename.setFileName("/mnt/jffs2/db_commonpds_2_3.db");
    if(filename.exists() == false){
        if(dbaccess.openDB()){
            if(dbaccess.createTable() < 0){
                qDebug()<<"DB Creation Failed.";
                QMessageBox::critical(0,"Database Issue","Unable to establish a eatabase",QMessageBox::Cancel);
                return;
            }
        }
        else{
            QMessageBox::critical(0,"Cannot Open Database","Unable to establish a database connection",QMessageBox::Cancel);
            return;
        }
    }
    else{
        dbaccess.openDB();

        //        QSqlQuery query;
        //        query.clear();
        //        if(!query.exec("CREATE TABLE IF NOT EXISTS OFFLINE_AND_RECEIVE_STOCK_URL(Offline_ReceiveStock_URL TEXT)"))
        //        {
        //            qDebug() << "OFFLINE_AND_RECEIVE_STOCK_URL : " << query.lastError();
        //            return;
        //        }
    }

    DeviceMacId.clear();
    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));
    miscObj.GetMachineId(cmachineID);
    DeviceMacId = cmachineID;

    QSqlQuery AlterQuery;
    AlterQuery.clear();
    /*if(!AlterQuery.exec("select (select count() from sqlite_master where type = 'table' and name='KeyRegister'),(select count() from sqlite_master where type = 'table' and name='BenfiaryTxn')"))
    {
            qDebug()<<"Alter1 error:"<<AlterQuery.lastError();
    }
    AlterQuery.next();
    qDebug()<<"AlterQuery.value(0).toInt() :"<<AlterQuery.value(0).toInt();
    qDebug()<<"AlterQuery.value(0).toInt() :"<<AlterQuery.value(1).toInt();
    if((AlterQuery.value(0).toInt()!=0)&&(AlterQuery.value(1).toInt()!=0))
    {
            AlterQuery.clear();
            if(!AlterQuery.exec("ALTER TABLE KeyRegister ADD AllotMonth text DEFAULT \"6\""))
                    qDebug()<<"Alter1 error:"<<AlterQuery.lastError();
            if(!AlterQuery.exec("ALTER TABLE KeyRegister ADD AllotYear text DEFAULT \"2019\""))
                    qDebug()<<"Alter1 error:"<<AlterQuery.lastError();
            if(!AlterQuery.exec("ALTER TABLE BenfiaryTxn ADD AllotMonth text DEFAULT \"6\""))
                    qDebug()<<"Alter1 error:"<<AlterQuery.lastError();
            if(!AlterQuery.exec("ALTER TABLE BenfiaryTxn ADD AllotYear text DEFAULT \"2019\""))
                    qDebug()<<"Alter1 error:"<<AlterQuery.lastError();
    }*//*
    if(!AlterQuery.exec("ALTER TABLE PartialOnlineData ADD pOfflineStoppedDate text"))
        qDebug()<<"Alter1 error:"<<AlterQuery.lastError();*/
    QSqlQuery query;
    query.clear();
    int StateInfoCnt = 0;
    query.prepare("SELECT count(*) FROM StateInfo");
    if(!query.exec())
        qDebug() << "In get StateInfo::" << query.lastError();
    else{
        while(query.next()){
            StateInfoCnt = query.value(0).toInt();
        }
    }
    if (StateInfoCnt > 0)
    {
        query.prepare("SELECT * FROM StateInfo");
        if(!query.exec())
            qDebug() << "In get StateInfo::" << query.lastError();
        else
        {
            while(query.next()){
                stateCode = query.value(0).toString();
                qDebug()<<"stateCode ==== "<< stateCode;
            }
        }
    }

    ui->comboTypeOfCard->hide();
    ui->lblTypeOfCard->hide();
    ui->lblIssueQty->hide();
    ui->ldtIssueQty->hide();
    ui->btnOK_Settings->hide();
    ui->ldtWM_MAC_ID->hide();
    ui->lblWMC_MAC_ID->hide();
    ui->radioButtonLeftIndex->hide();
    ui->radioButtonLeftLittle->hide();
    ui->radioButtonLeftMiddle->hide();
    ui->radioButtonLeftRing->hide();
    ui->radioButtonLeftThumb->hide();
    ui->radioButtonRightIndex->hide();
    ui->radioButtonRightLittle->hide();
    ui->radioButtonRightMiddle->hide();
    ui->radioButtonRightRing->hide();
    ui->radioButtonRightThumb->hide();
    //ui->lblDueAmt->hide();
    //    ui->comboCommMode->hide();
    //    ui->btnGetWt->hide();
    ui->btnKerosene->hide();
    ui->ldtShpId->hide();
    ui->lblShopId->hide();
    ui->btnLogin->hide();
    //    ui->comboCommMode->hide();
    ui->BtnRouteOfficer->hide();
    /*ui->PartialCurrStkRptBtn->hide();
    ui->Partial_DaDyRptBtn->hide();
    ui->PartialStkRptLbl->hide();
    ui->LblPartDayRpt->hide();*/

    //    ui->btnImpds->hide();
    ui->tableWidgetStockTable->hide();
    ui->BtnHawker->hide();
    ui->BtnPreview->hide();
    HideConsent();

    ui->tableWidgetRecieveGoodsAuth->setColumnHidden(3,true);

    QRegExp rx("\\d+");
    QRegExpValidator *valptr = new QRegExpValidator(rx, this);
    ui->lineEditAadharNo->setValidator(valptr);
    ui->lineEditCust_Mobile->setValidator(valptr);
    ui->lineEditOTP->setValidator(valptr);

    ui->ldtEkycCardNo->setValidator(valptr);
    ui->ldtRationCard->setValidator(valptr);
    ui->ldtShpId->setValidator(new QRegExpValidator( QRegExp("[A-Za-z0-9_]+"), this ));

    ui->leditcustrcno->setValidator(new QRegExpValidator( QRegExp("[0-9]+"), this ));
    ui->leditcustmobno->setValidator(new QRegExpValidator( QRegExp("[0-9]+"), this ));
    ui->leditmobupdateotp->setValidator(new QRegExpValidator( QRegExp("[0-9]+"), this ));

    SetLanguage();
    SetAppType(stateCode.toInt());//changing application type according to state code

    iCommMode = getCommMode();
    if(iCommMode == 1){
        iCommMode = 0;
    }

    system("mkdir -p /mnt/jffs2/bir");
    system("mkdir -p /mnt/jffs2/raw_images");
    system("mkdir -p /mnt/jffs2/bmp_images");
    system("mkdir -p /mnt/jffs2/b64_files");

    char Verbuff[64];
    memset(Verbuff, 0, sizeof(Verbuff));;
    sprintf(Verbuff,"Ver : PDS-P%s%s", NEW_VERSION,SUB_VERSION);
    ui->labelAppVer_2->setText(Verbuff);

    memset(Verbuff, 0, sizeof(Verbuff));
    sprintf(Verbuff, "Old Ver - %s", OLD_VERSION);
    ui->ldtShpId->setFocus();

    iFestivalFlag = 0;
    app_ver();

    database_queries qry;
    qry.DeleteTxnsDB();

    displayDate();

    displayTime();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(displayTime()));
    timer->start(1000);

    nLogout =0;
    qApp->installEventFilter(this);

    miscObj.Check_mplayer_Exists();
    miscObj.AudioTest((char *)"41");

    system("rm /mnt/jffs2/md5checksum.txt");

    //    QSqlQuery query;
    query.clear();
    query.prepare("SELECT ShopID FROM  ShopOwnerDetails");
    if(!query.exec())
        qDebug() << "SELECT ShopID FROM  ShopOwnerDetails = " << query.lastError();
    else{
        while(query.next()){
            ui->ldtShpId->setText(query.value(0).toString());
        }
    }
    UidEncode_b64_flag = 1;

    memset(szHelpDeskNo, 0, sizeof(szHelpDeskNo));
    miscObj.getHelpDeskNo(szHelpDeskNo);
    if(strncmp(szHelpDeskNo,"NA",2)==0){
        ui->lblHelpDeskNo->hide();
        ui->lblHelpDesk->hide();
    }else
        ui->lblHelpDeskNo->setText(szHelpDeskNo);;

    gl11_lcdbklt_on();
    gl11_lcdbklt_time_duration(90);

    SetLineEditValidation();

    /*Initialize Global Vaiable*/
    memset(gRoute_Off_auth_code,0,sizeof(gRoute_Off_auth_code));
    memset(gRoute_Off_UID,0,sizeof(gRoute_Off_UID));

    //    ui->pushButton_3->hide();
    ui->BtnScanIris->hide();
    ui->label_129->hide();

    DYN_StockUpdate_TxnreceiptId = "";
    //    ui->label_140->hide();
    ui->btnMobileUpdate->hide();
    ui->label_55->hide();
    ui->labelRO->hide();
    ui->tableWidgetStockTable->hide();
    ui->pushButton_7->hide();
    ui->BtnDealerIris->hide();
    ui->radioButton_scanbarcode->hide();
    //    ui->BtnBenIRIS->hide();
    ui->btnMDM->hide();
    ui->stackedWidget->setCurrentIndex(0);
}

Widget::~Widget()
{
    delete ui;
}

int Widget::getShopId(char *ShopId)
{
    database_queries dbquery;
    dbquery.getShopId_fromdb(ShopId);
    ui->ldtShpId->setText(ShopId);
}

void Widget::displayDate()
{
    QDateTime *DateTime = new QDateTime;
    QString Date;
    Date = DateTime->currentDateTime().date().toString("dd/MM/yyyy");
    qDebug() << "Date = " << Date << endl;

    ui->labelDate->setText(Date);
    ui->labelDate_2->setText(Date);
}

void Widget::displayTime()
{
    ui->labelDate->setText(QDateTime::currentDateTime().date().toString("dd/MM/yyyy"));
    ui->labelDate_2->setText(QDateTime::currentDateTime().date().toString("dd/MM/yyyy"));

    ui->labelTime->setText(QTime::currentTime().toString("hh:mm:ss "));
    ui->labelTime_2->setText(QTime::currentTime().toString("hh:mm:ss "));
}

void Widget::SetLanguage()
{
    database_queries dbqry;
    LangMode = dbqry.getLangMode();

    if(LangMode == 1){
        ui->comboLangSel->setCurrentIndex(0);
        ChangeUI_to_Eng();
    }else if(LangMode == 2){
        ui->comboLangSel->setCurrentIndex(1);
        ChangeUI_to_HINDI();
    }
    return;
}

void Widget::SetAppType(int StateCode)
{
    QString LogoStr;
    switch(StateCode)
    {
    case 1 :
        LogoStr = "/rsc/images/JKLogo.png";
        SetLogo(LogoStr);
        break;
    case 5 :
        LogoStr = "/rsc/images/uk_logo.png";
        SetLogo(LogoStr);
        break;
    case 10 :
        LogoStr = "/rsc/images/bihar.png";
        SetLogo(LogoStr);
        break;
    case 12 :
        LogoStr = "/rsc/images/MizoramLogo.png";
        SetLogo(LogoStr);
        break;
    case 13 :
        LogoStr = "/rsc/images/NagalandGovtLogo.png";
        SetLogo(LogoStr);
        break;
    case 14 :
        LogoStr = "/rsc/images/ManipurGovtLogo.png";
        SetLogo(LogoStr);
        break;
    case 15 :
        LogoStr = "/rsc/images/MizoramLogo.png";
        SetLogo(LogoStr);
        break;
    case 17 :
        LogoStr = "/rsc/images/MeghalayaLogo.png";
        SetLogo(LogoStr);
        break;
    case 25 :
    case 26 :
        LogoStr = "/rsc/images/DamanDiuLogo.png";
        SetLogo(LogoStr);
        break;
    case 31 :
        LogoStr = "/rsc/images/lakshadweep.png"; //lakshadweep
        SetLogo(LogoStr);
        break;
    case 35 :
        LogoStr = "/rsc/images/MizoramLogo.png"; //Andaman & Nicobar
        SetLogo(LogoStr);
        break;
    case 37 :
        LogoStr = "/rsc/images/MizoramLogo.png"; //Ladakh
        SetLogo(LogoStr);
        break;
    default:
        qDebug() << "Invalid State Code...";
    }
}

void Widget::SetAppType1(int StateCode)
{
    if(StateCode == 13) //nagaland
    {
        ui->lblAP_Logo1->setStyleSheet("border-image: url(:/rsc/images/NagalandGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_2->setStyleSheet("border-image: url(:/rsc/images/NagalandGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_3->setStyleSheet("border-image: url(:/rsc/images/NagalandGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_4->setStyleSheet("border-image: url(:/rsc/images/NagalandGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_9->setStyleSheet("border-image: url(:/rsc/images/NagalandGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_11->setStyleSheet("border-image: url(:/rsc/images/NagalandGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_25->setStyleSheet("border-image: url(:/rsc/images/NagalandGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_44->setStyleSheet("border-image: url(:/rsc/images/NagalandGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_49->setStyleSheet("border-image: url(:/rsc/images/NagalandGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_79->setStyleSheet("border-image: url(:/rsc/images/NagalandGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_40->setStyleSheet("border-image: url(:/rsc/images/NagalandGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
    }
    else if(StateCode == 14) //manipur
    {
        ui->lblAP_Logo1->setStyleSheet("border-image: url(:/rsc/images/ManipurGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_2->setStyleSheet("border-image: url(:/rsc/images/ManipurGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_3->setStyleSheet("border-image: url(:/rsc/images/ManipurGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_4->setStyleSheet("border-image: url(:/rsc/images/ManipurGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_9->setStyleSheet("border-image: url(:/rsc/images/ManipurGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_11->setStyleSheet("border-image: url(:/rsc/images/ManipurGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_25->setStyleSheet("border-image: url(:/rsc/images/ManipurGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_44->setStyleSheet("border-image: url(:/rsc/images/ManipurGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_49->setStyleSheet("border-image: url(:/rsc/images/ManipurGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_79->setStyleSheet("border-image: url(:/rsc/images/ManipurGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_40->setStyleSheet("border-image: url(:/rsc/images/ManipurGovtLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
    }
    else if(StateCode == 15) //mizoram
    {
        ui->lblAP_Logo1->setStyleSheet("border-image: url(:/rsc/images/MizoramLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_2->setStyleSheet("border-image: url(:/rsc/images/MizoramLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_3->setStyleSheet("border-image: url(:/rsc/images/MizoramLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_4->setStyleSheet("border-image: url(:/rsc/images/MizoramLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_9->setStyleSheet("border-image: url(:/rsc/images/MizoramLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_11->setStyleSheet("border-image: url(:/rsc/images/MizoramLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_25->setStyleSheet("border-image: url(:/rsc/images/MizoramLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_44->setStyleSheet("border-image: url(:/rsc/images/MizoramLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_49->setStyleSheet("border-image: url(:/rsc/images/MizoramLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_79->setStyleSheet("border-image: url(:/rsc/images/MizoramLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_40->setStyleSheet("border-image: url(:/rsc/images/MizoramLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
    }
    else if(StateCode == 17) //meghalaya
    {
        ui->lblAP_Logo1->setStyleSheet("border-image: url(:/rsc/images/MeghalayaLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_2->setStyleSheet("border-image: url(:/rsc/images/MeghalayaLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_3->setStyleSheet("border-image: url(:/rsc/images/MeghalayaLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_4->setStyleSheet("border-image: url(:/rsc/images/MeghalayaLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_9->setStyleSheet("border-image: url(:/rsc/images/MeghalayaLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_11->setStyleSheet("border-image: url(:/rsc/images/MeghalayaLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_25->setStyleSheet("border-image: url(:/rsc/images/MeghalayaLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_44->setStyleSheet("border-image: url(:/rsc/images/MeghalayaLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_49->setStyleSheet("border-image: url(:/rsc/images/MeghalayaLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_79->setStyleSheet("border-image: url(:/rsc/images/MeghalayaLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_40->setStyleSheet("border-image: url(:/rsc/images/MeghalayaLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
    }
    else if((StateCode == 25) || (StateCode == 26)) //Daman & Diu
    {
        ui->lblAP_Logo1->setStyleSheet("border-image: url(:/rsc/images/DamanDiuLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_2->setStyleSheet("border-image: url(:/rsc/images/DamanDiuLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_3->setStyleSheet("border-image: url(:/rsc/images/DamanDiuLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_4->setStyleSheet("border-image: url(:/rsc/images/DamanDiuLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_9->setStyleSheet("border-image: url(:/rsc/images/DamanDiuLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_11->setStyleSheet("border-image: url(:/rsc/images/DamanDiuLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_25->setStyleSheet("border-image: url(:/rsc/images/DamanDiuLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_44->setStyleSheet("border-image: url(:/rsc/images/DamanDiuLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_49->setStyleSheet("border-image: url(:/rsc/images/DamanDiuLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->label_79->setStyleSheet("border-image: url(:/rsc/images/DamanDiuLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
        ui->lblAP_Logo1_40->setStyleSheet("border-image: url(:/rsc/images/DamanDiuLogo.png); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));");
    }
}

void Widget::SetLogo(QString Logo)
{
    QString StyleSheetStr;

    StyleSheetStr = "border-image: url(:" + Logo + "); background-color: qlineargradient(spread:pad, x1:0.97005, y1:0, x2:0.960199, y2:0.972, stop:0 rgba(80, 191, 229, 255), stop:1 rgba(255, 255, 255, 255));";

    qDebug() << "Style sheet::"  <<StyleSheetStr;

    ui->lblAP_Logo1->setStyleSheet(StyleSheetStr);
    ui->lblAP_Logo1_2->setStyleSheet(StyleSheetStr);
    ui->lblAP_Logo1_3->setStyleSheet(StyleSheetStr);
    ui->lblAP_Logo1_4->setStyleSheet(StyleSheetStr);
    ui->lblAP_Logo1_9->setStyleSheet(StyleSheetStr);
    ui->lblAP_Logo1_11->setStyleSheet(StyleSheetStr);
    ui->label_25->setStyleSheet(StyleSheetStr);
    ui->label_44->setStyleSheet(StyleSheetStr);
    ui->label_49->setStyleSheet(StyleSheetStr);
    ui->label_79->setStyleSheet(StyleSheetStr);
    ui->lblAP_Logo1_40->setStyleSheet(StyleSheetStr);
}

void Widget::keyPressEvent(QKeyEvent* evt)
{
    //16777216-cancel
    qDebug() << evt->key() << "%%%%%%%";

    if((evt->key() == 49) || (evt->key() == 50) || (evt->key() == 51) || (evt->key() == 52) || (evt->key() == 53) || (evt->key() == 54) || (evt->key() == 55))
    {
        if(ui->stackedWidget->currentIndex() == 1){
            qDebug() << "In CurrentIndex 1";
            if(evt->key() == 49){
                qDebug() << "Button 1 Pressed";
                ui->btnAuthenticate->click();
            }else if(evt->key() == 50){
                qDebug() << "Button 2 Pressed";
                ui->btnAadhaarServices->click();
            }else if(evt->key() == 51){
                qDebug() << "Button 3 Pressed";
                ui->btnReports->click();
            }else if(evt->key() == 52){
                qDebug() << "Button 4 Pressed";
                ui->btn_inspector->click();
            }else if(evt->key() == 53){
                qDebug() << "Button 5 Pressed";
                ui->btnReceiveGoods->click();
            }
        }else if(ui->stackedWidget->currentIndex() == 2){
            qDebug() << "In CurrentIndex 2";
            if(evt->key() == 49){
                qDebug() << "Button 1 Pressed";
                ui->btnDailySales->click();
            }else if(evt->key() == 50){
                qDebug() << "Button 2 Pressed";
                ui->btnStockRegister->click();
            }else if(evt->key() == 51){
                qDebug() << "Button 3 Pressed";
            }
        }else if(ui->stackedWidget->currentIndex() == 17){
            qDebug() << "In CurrentIndex 17";
            if(evt->key() == 49){
                qDebug() << "Button 1 Pressed";
                ui->BtnUSBUpdateApp->click();
            }else if(evt->key() == 50){
                qDebug() << "Button 2 Pressed";
                ui->BtnGPRSUpdateApp->click();
            }
        }else if(ui->stackedWidget->currentIndex() == 18){
            qDebug() << "In CurrentIndex 18";
            if(evt->key() == 49){
                qDebug() << "Button 1 Pressed";
                ui->BtnDeviceSettings->click();
            }else if(evt->key() == 50){
                qDebug() << "Button 2 Pressed";
                ui->btnUpdateSW->click();
            }else if(evt->key() == 51){
                qDebug() << "Button 3 Pressed";
            }else if(evt->key() == 52){
                qDebug() << "Button 4 Pressed";
            }
        }else if(ui->stackedWidget->currentIndex() == 23){
            qDebug() << "In CurrentIndex 23";
            if(evt->key() == 49){
                qDebug() << "Button 1 Pressed";
                ui->BtnRouteOfficer->click();
            }else if(evt->key() == 50){
                qDebug() << "Button 2 Pressed";
                ui->BtnDealerRecGoods->click();
            }else if(evt->key() == 51){
                qDebug() << "Button 3 Pressed";
                ui->BtnHawker->click();
            }
        }else if(ui->stackedWidget->currentIndex() == 28){
            qDebug() << "In CurrentIndex 28";
            if(evt->key() == 49){
                qDebug() << "Button 1 Pressed";
                ui->BtnPDS->click();
            }else if(evt->key() == 50){
                qDebug() << "Button 2 Pressed";
                on_btnMDM_clicked();
            }else if(evt->key() == 51){
                qDebug() << "Button 3 Pressed";
            }else if(evt->key() == 52){
                qDebug() << "Button 4 Pressed";
            }else if(evt->key() == 53){
                qDebug() << "Button 5 Pressed";
            }else if(evt->key() == 54){
                qDebug() << "Button 6 Pressed";
            }else if(evt->key() == 55){
                qDebug() << "Button 7 Pressed";
            }
        }else if(ui->stackedWidget->currentIndex() == 30){
            qDebug() << "In CurrentIndex 30";
            if(evt->key() == 49){
                qDebug() << "Button 1 Pressed";;
            }else if(evt->key() == 50){
                qDebug() << "Button 2 Pressed";
            }else if(evt->key() == 51){
                qDebug() << "Button 3 Pressed";
            }else if(evt->key() == 52){
                qDebug() << "Button 4 Pressed";
            }
        }
        else if(ui->stackedWidget->currentIndex() == 31)
        {
            qDebug() << "In CurrentIndex 31";
            if(evt->key() == 49)
            {
                qDebug() << "Button 1 Pressed";
                //ui->BtnBankMaster->click();
            }
            else if(evt->key() == 50)
            {
                qDebug() << "Button 2 Pressed";
                //ui->BtnBenMaster->click();
            }
            else if(evt->key() == 51)
            {
                qDebug() << "Button 3 Pressed";
                //ui->btnHttpsCert->click();
            }
        }
        else if(ui->stackedWidget->currentIndex() == 40)
        {
            qDebug() << "In CurrentIndex 40";
            if(evt->key() == 49)
            {
                qDebug() << "Button 1 Pressed"  ;

                ui->btnEKYC_Update->click();
                //ui->btnBFD->click();
            }
            else if(evt->key() == 50)
            {
                qDebug() << "Button 2 Pressed";
                //ui->btnMobileUpdate->click();
            }
            else if(evt->key() == 51)
            {
                qDebug() << "Button 3 Pressed";
                //ui->btnEKYC_Update->click();
            }
        }
    }
    else if(evt->key() == Qt::Key_Backspace)    //YELLOW
    {
        if(ui->stackedWidget->currentIndex() == 0)
        {
            ui->btnSettings_2->click();
        }
        else if(ui->stackedWidget->currentIndex() == 3)
        {
            //ui->BtnConsentForm_3->click();
        }
        else if(ui->stackedWidget->currentIndex() == 4)
        {
            ui->BtnLastReciept->click();
        }
        else if(ui->stackedWidget->currentIndex() == 5)
        {
            ui->pushButton_2->click();
        }
        else if(ui->stackedWidget->currentIndex() == 6)
        {
            ui->btnConfirmCommodity->click();
        }
        else if(ui->stackedWidget->currentIndex() == 15)
        {
            //ui->btnCapture->click();
        }
        else if(ui->stackedWidget->currentIndex() == 20)
        {
            // ui->BtnDealerScanFP->click();
        }
        else if(ui->stackedWidget->currentIndex() == 23)
        {
            ui->BtnScanFinger->click();
        }
        else if(ui->stackedWidget->currentIndex() == 26)
        {
            //ui->pushButtonstatus->click();
        }
        else if(ui->stackedWidget->currentIndex() == 39)
        {
            //ui->btnNCR_Submit->click();
        }
    }
    else if(evt->key() == Qt::Key_Return)   //GREEN
    {
        if(ui->stackedWidget->currentIndex() == 0)
        {
            ui->ldtGetOwnDetails->click();
        }
        else if(ui->stackedWidget->currentIndex() == 20)
        {
            //            ui->btnLogin->click();//Commented for HPDS
            ui->BtnDealerScanFP->click();
        }
        else if(ui->stackedWidget->currentIndex() == 4)
        {
            ui->btnOK_Entitle->click();
        }
        else if(ui->stackedWidget->currentIndex() == 5)
        {
            ui->pushButton_2->click();

            //  ui->pushButton_3->click();//cmt 4 iris auth
        }
        else if(ui->stackedWidget->currentIndex() == 6)
        {
            ui->btnConfirmCommodity->click();

            //            ui->btnGetWt->click();
        }
        else if(ui->stackedWidget->currentIndex() == 7)
        {
            ui->pushButton_6->click();
        }
        else if(ui->stackedWidget->currentIndex() == 3)
        {
            //ui->BtnNext->click();
        }
        else if(ui->stackedWidget->currentIndex() == 11)
        {
            ui->btnOkDt->click();
        }
        else if(ui->stackedWidget->currentIndex() == 15)
        {
            //ui->BtnNext_2->click();
        }
        else if(ui->stackedWidget->currentIndex() == 16)
        {
            //ui->BtnSubmit->click();
        }
        else if(ui->stackedWidget->currentIndex() == 19)
        {
            //ui->BtnOK->click();
        }
        else if(ui->stackedWidget->currentIndex() == 21)
        {
            ui->BtnOK_2->click();
        }
        else if(ui->stackedWidget->currentIndex() == 22)
        {
            ui->BtnNext_3->click();
        }
        else if(ui->stackedWidget->currentIndex() == 23)
        {
            //ui->BtnScanIris->click();
        }
        else if(ui->stackedWidget->currentIndex() == 24)
        {
            //ui->BtnOK_3->click();
        }
        else if(ui->stackedWidget->currentIndex() == 25)
        {
            //ui->BtnOK_4->click();
        }
        else if(ui->stackedWidget->currentIndex() == 26)
        {
            //ui->BtnGrievSubmit->click();
        }
        else if(ui->stackedWidget->currentIndex() == 27)
        {
            //ui->BtnSubmit_2->click();
        }
        else if(ui->stackedWidget->currentIndex() == 29)
        {
            //ui->BtnAnganwadiMemDetails->click();
        }
        else if(ui->stackedWidget->currentIndex() == 32)
        {
            //ui->BtnCashlessScanFP->click();
        }
        else if(ui->stackedWidget->currentIndex() == 33)
        {
            //ui->pushButton->click();
        }
        else if(ui->stackedWidget->currentIndex() == 35)
        {
            ui->BtnOTP->click();
        }
        else if(ui->stackedWidget->currentIndex() == 36)
        {
            ui->BtnSubmit_3->click();
        }
        else if(ui->stackedWidget->currentIndex() == 39)
        {
            //ui->btnNCR_Iris->click();
        }
    }
    else if(evt->key() == Qt::Key_Escape)   //RED
    {
        qDebug() << "In Escape Event";

        if(ui->stackedWidget->currentIndex() == 0)
        {
            ui->btnQuitApp->click();
        }
        else if(ui->stackedWidget->currentIndex() == 11)
        {
            ui->btnBackDt->click();
        }
        else if(ui->stackedWidget->currentIndex() == 17)
        {
            ui->BtnBack_4->click();
        }
        else if(ui->stackedWidget->currentIndex() == 18)
        {
            ui->BtnBack_5->click();
        }
        else if(ui->stackedWidget->currentIndex() == 19)
        {
            //ui->BtnBack_6->click();
        }
        else if(ui->stackedWidget->currentIndex() == 20)
        {
            ui->btnBack->click();
        }
        else if(ui->stackedWidget->currentIndex() == 1)
        {
            ui->btnLogout->click();
        }
        else if(ui->stackedWidget->currentIndex() == 2)
        {
            ui->btnGotoHome->click();
        }
        else if(ui->stackedWidget->currentIndex() == 4)
        {
            ui->btnBackEntitle->click();
        }
        else if(ui->stackedWidget->currentIndex() == 5)
        {
            // evt->ignore();//popup cancel button
            ui->pushButton_4->click();
        }
        else if(ui->stackedWidget->currentIndex() == 6)
        {
            ui->btnCancelCommodity->click();
        }
        else if(ui->stackedWidget->currentIndex() == 7)
        {
            ui->pushButton_5->click();
        }
        else if(ui->stackedWidget->currentIndex() == 9)
        {
            ui->btnBackSettings->click();
        }
        else if(ui->stackedWidget->currentIndex() == 3)
        {
            //ui->BtnBack->click();
        }
        else if(ui->stackedWidget->currentIndex() == 15)
        {
            //ui->BtnBack_2->click();
        }
        else if(ui->stackedWidget->currentIndex() == 16)
        {
            //ui->BtnBack_3->click();
        }
        else if(ui->stackedWidget->currentIndex() == 17)
        {
            ui->BtnBack_4->click();
        }
        else if(ui->stackedWidget->currentIndex() == 18)
        {
            ui->BtnBack_5->click();
        }
        else if(ui->stackedWidget->currentIndex() == 19)
        {
            ///ui->BtnBack_6->click();
        }
        else if(ui->stackedWidget->currentIndex() == 22)
        {
            ui->BtnBack_7->click();
        }
        else if(ui->stackedWidget->currentIndex() == 23)
        {
            ui->BtnBack_8->click();
        }
        else if(ui->stackedWidget->currentIndex() == 25)
        {
            //ui->BtnBack_9->click();
        }
        else if(ui->stackedWidget->currentIndex() == 26)
        {
            //ui->BtnBack_10->click();
        }
        else if(ui->stackedWidget->currentIndex() == 27)
        {
            //ui->BtnBack_11->click();
        }
        else if(ui->stackedWidget->currentIndex() == 28)
        {
            ui->BtnBack_12->click();
        }
        else if(ui->stackedWidget->currentIndex() == 28)
        {
            ui->BtnBack_12->click();
        }
        else if(ui->stackedWidget->currentIndex() == 29)
        {
            //ui->BtnBack_13->click();
        }
        else if(ui->stackedWidget->currentIndex() == 30)
        {
            //ui->BtnBack_14->click();
        }
        else if(ui->stackedWidget->currentIndex() == 31)
        {
            //ui->BtnBack_15->click();
        }
        else if(ui->stackedWidget->currentIndex() == 32)
        {
            //ui->BtnBack_16->click();
        }
        else if(ui->stackedWidget->currentIndex() == 34)
        {
            //ui->BtnBack_17->click();
        }
        else if(ui->stackedWidget->currentIndex() == 35)
        {
            ui->BtnBack_18->click();
        }
        else if(ui->stackedWidget->currentIndex() == 36)
        {
            ui->BtnBack_19->click();
        }
        else if(ui->stackedWidget->currentIndex() == 39)
        {
            //ui->btnBackNCR->click();
        }
        else if(ui->stackedWidget->currentIndex() == 40)
        {
            ui->btnBackAadhaarServices->click();
        }
    }
    evt->ignore();
}

void Widget::mousePressEvent(QMouseEvent *evt)
{
    if(evt->NonClientAreaMouseButtonPress == 174){
        evt->ignore();
    }
}

void Widget::mouseDoubleClickEvent(QMouseEvent *evt)
{
    qDebug()<<"Ignoring Mouse Double Click Event";
    if(evt->NonClientAreaMouseButtonDblClick == 176){
    }
}

void Widget::on_btnAuthenticate_clicked()
{
    //    if(CheckTxnDate()<0)
    //        return;
    if(rtcDateValidation() < 0)
        return;

    receipt_type=0;
    //    ui->BtnBenIRIS->show(); //hiding IRIS-BSK.

    if(DealerLoginBy != 2){
        ui->BtnLastReciept->show();
        if(partialOnlineOfflineStatus.toUpper() == "Y")
            ui->radioButton_UIDFLAG->hide();
        else
            ui->radioButton_UIDFLAG->show();
    }
    miscObj.Check_mplayer_Exists();

    ui->radioButton_RCNOFLAG->setChecked(true);
    ui->ldtRationCard->clear();
    ui->ldtRationCard->setFocus();

    Finger_Iris_Selection = 0;
    memset(gauth_transaction_code, 0, sizeof(gauth_transaction_code));

    prn_open();

    int Ret = GetPrintPaperSts(paperRequiredFlag);
    qDebug() << "GetPrintPaperSts::" << Ret <<"\t"<< paperRequiredFlag;

    if((prn_paperstatus() < 0) && (paperRequiredFlag.toUpper() != "N")){
        prn_close();
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया कागज डालें.");
        return;
    }
    prn_close();

//    ui->BtnSwavalamban->hide();
    ui->ldtRationCard->clear();
    ui->comboTypeOfCard->setCurrentIndex(0);
    ui->stackedWidget->setCurrentIndex(28);
    return;
}

void Widget::on_btnBackEntitle_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Widget::on_btnOK_Entitle_clicked()
{   
    PartialTxn = 0;

    ui->btnGetWt->hide();
    ui->comboCommMode->hide(); // initially weigh scale hide it will show depends on flag -BSK.

    if(TxnAllotedBetweenTime()<0)
        return;
    if(CheckTxnDate()<0)
        return;
    if(DealerLoginBy==2)
    {
        GetOfflineCommDetails();
        return;
    }
    if(LangMode == 1)
        pop_up("Please Wait....");
    else if(LangMode == 2)
        pop_up("कृपया प्रतीक्षा करें....");

    miscObj.Check_mplayer_Exists();

    char buff[128]="";
    int row=0, count=0;
    CMisc miscObj;
    QString membername;

    iModuleType = DUMMY_TXN;

    iFestivalFlag = 0;
    Finger_Iris_Selection = 0;

    if(ui->radioButton_RCNOFLAG->isChecked())
    {
        cidType = "R";

        if(ui->ldtRationCard->text().length() == 0)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Please Enter RC Number");
            else
                miscObj.tel_DisplayMsgBox((char *)"कृपया कार्ड नंबर दर्ज करें.");

            ui->ldtRationCard->clear();
            return;
        }
    }
    if(ui->radioButton_UIDFLAG->isChecked())
    {
        cidType = "U";

        if(ui->ldtRationCard->text().length() < 12){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Please enter 12 digit uid number ");
            else
                miscObj.tel_DisplayMsgBox((char *)"कृपया 12 अंक यूआईडी नंबर दर्ज करें.");

            ui->ldtRationCard->clear();
            return;
        }
        if(miscObj.validate_uid(ui->ldtRationCard->text().toAscii().data()) < 0){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Invalid UID");
            else
                miscObj.tel_DisplayMsgBox((char *)"अमान्य यूआईडी.");

            ui->ldtRationCard->clear();
            return;
        }
        rsaEncryption(ui->ldtRationCard->text().toAscii().data(),skey);
    }
    strcpy((char *)RationCardId, ui->ldtRationCard->text().toAscii().data());
    strcpy((char *)CardType, "NA");

    ///******** TIME LAP START************
    QTime tim;
    tim.restart();
    tim.start();
    //******************************
    int iRationType = 0;
    qDebug()<<"iMainTransaction = "<<iMainTransaction;
    if(iMainTransaction == PDS_TRANSACTION){
        iRationType = 1;

        if(partialOnlineOfflineStatus.toUpper() == "Y")
        {
            GPRS objGprs;
            if(objGprs.Check_GPRS_Available() == 0)
            {
                popup_msg *dlgPopup1 = new popup_msg(this);
                if(LangMode == ENGLISH)
                    dlgPopup1->ui->lblPopup->setText("Please Wait...\nUploading Pending CB...");
                else if(LangMode == 2)
                    dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.\nलंबित अभिलेख अपलोड हो रहा है. ."));
                dlgPopup1->show();
                QCoreApplication::processEvents();
                QCoreApplication::processEvents();

                int RetVal = ManualReceiveGdsUpload();

                dlgPopup1->close();
                dlgPopup1->deleteLater();
                if(RetVal != 0)
                {
                    if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Offline Data Uploading Failed\nPlease Try again ..");
                    else
                        miscObj.tel_DisplayWideMsgBox((char *)"ऑफ़लाइन डेटा अपलोडिंग विफल हुई \n कृपया पुनः प्रयास करें ..");
                    return;
                }

                if(LangMode == ENGLISH)
                    dlgPopup1->ui->lblPopup->setText("Please Wait...\nUploading Pending Records...");
                else if(LangMode == 2)
                    dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.\nलंबित अभिलेख अपलोड हो रहा है. ."));
                dlgPopup1->show();
                QCoreApplication::processEvents();
                QCoreApplication::processEvents();

                //uploading Pending Transaction foe that Particular Card Number
                RetVal = CheckingPofflineTrans();

                dlgPopup1->close();
                dlgPopup1->deleteLater();
                if(RetVal != 0)
                {
                    if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Offline Data Uploading Failed\nPlease Try again ..");
                    else
                        miscObj.tel_DisplayWideMsgBox((char *)"ऑफ़लाइन डेटा अपलोडिंग विफल हुई \n कृपया पुनः प्रयास करें ..");
                    return;
                }
                //                if(CheckingPofflineTrans() < 0)
                //                {
                //                    dlgPopup1->close();
                //                    dlgPopup1->deleteLater();
                //                    miscObj.DisplayWideMsgBox((char *)"Uplading pending records failed \nPlease try again");
                //                    return;
                //                }
                //                dlgPopup1->close();
                //                dlgPopup1->deleteLater();
            }
        }
    }

    if(GetCardHolderDetails(iRationType) < 0){
        if(PartialTxn!=1){
            if(ui->radioButton_RCNOFLAG->isChecked())
                ui->ldtRationCard->clear();
        }
        return;
    }
    qDebug()<<"GetCardHolderDetails success"<<endl;
    ui->lblRCNO_Val->setText(Rcno);
    ui->lblRCNO_Val_2->setText(Rcno);

    QSqlQuery query1;
    query1.prepare("SELECT COUNT(MemberNameEn) FROM CustomerFamily where RationCardId=?");
    query1.addBindValue(Rcno.toAscii().data());
    if(!query1.exec())
        qDebug() << query1.lastError();
    else{
        while(query1.next())
            count = query1.value(0).toInt(0);
    }
    if(count == 0){
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Members Details Not Found For The Card");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कार्ड के लिए सदस्यों का विवरण नहीं मिला.");
        return;
    }
    ui->twdgtEntitlement->setRowCount(count);

    QSqlQuery query;
    query.prepare("select MemberNameEn, MemberUID, bfd_1, bfd_2, bfd_3, MemberId,MemberNamell,Memberauth_manual,uid from CustomerFamily where RationCardId=?");
    //query.prepare("select MemberNameEn, MemberUID, bfd_1, bfd_2, bfd_3, MemberId,MemberNamell,DealerFlag from CustomerFamily where RationCardId=?");//DealerFlag added
    query.addBindValue(Rcno.toAscii().data());
    query.exec();

    qDebug() << ui->ldtRationCard->text().toAscii().data() << "###";
    while(query.next())
    {

        if(LangMode == 1){
            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(0).toString().toAscii().data());
            qDebug()<<"0 MemberNameEn = "<<buff;
            ui->twdgtEntitlement->setItem(row, 0, new QTableWidgetItem(buff));
        }else if(LangMode == 2){
            membername.clear();
            membername=query.value(6).toString();
            ui->twdgtEntitlement->setItem(row, 0, new QTableWidgetItem(membername));
        }
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

        // added by vivek new column memberId 20062017
        memset(buff,0,sizeof(buff));
        strcpy(buff, query.value(5).toString().toAscii().data());
        qDebug()<<"6 Member id = "<<buff;

        ui->twdgtEntitlement->setItem(row, 6, new QTableWidgetItem(buff));
        ui->twdgtEntitlement->item(row, 6)->setFlags(ui->twdgtEntitlement->item(row, 6)->flags() ^ Qt::ItemIsEditable);
        ui->twdgtEntitlement->item(row,6)->setFlags( ui->twdgtEntitlement->item(row,6)->flags() | Qt::ItemIsSelectable);

        memset(buff,0,sizeof(buff));
        strcpy(buff, query.value(7).toString().toAscii().data());
        qDebug()<<"7 Manual/dealerFlag = "<<buff;

        ui->twdgtEntitlement->setItem(row, 7, new QTableWidgetItem(buff));
        ui->twdgtEntitlement->item(row, 7)->setFlags(ui->twdgtEntitlement->item(row, 7)->flags() ^ Qt::ItemIsEditable);
        ui->twdgtEntitlement->item(row,7)->setFlags( ui->twdgtEntitlement->item(row,7)->flags() | Qt::ItemIsSelectable);

        memset(buff,0,sizeof(buff));
        strcpy(buff, query.value(8).toString().toAscii().data());
        qDebug()<<"8 Wadh Status = "<<buff;

        ui->twdgtEntitlement->setItem(row, 8, new QTableWidgetItem(buff));
        ui->twdgtEntitlement->item(row, 8)->setFlags(ui->twdgtEntitlement->item(row, 8)->flags() ^ Qt::ItemIsEditable);
        ui->twdgtEntitlement->item(row,8)->setFlags( ui->twdgtEntitlement->item(row,8)->flags() | Qt::ItemIsSelectable);


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

    ui->twdgtEntitlement->setColumnHidden(2, true);
    ui->twdgtEntitlement->setColumnHidden(3, true);
    ui->twdgtEntitlement->setColumnHidden(4, true);
    ui->twdgtEntitlement->setColumnHidden(5, true);
    ui->twdgtEntitlement->setColumnHidden(6, true);
    ui->twdgtEntitlement->setColumnHidden(7, true); //zdealerFlag
    ui->twdgtEntitlement->setColumnHidden(8, true);

    /******* TIME LAP STOP **********/
    char lapbuff[32];
    memset(lapbuff, 0, sizeof(lapbuff));
    sprintf(lapbuff,"Lap Time : %d", tim.elapsed());
    tim.restart();
    /********************************/

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
    ui->label_146->clear();
    ui->label_146->setText(Qtype_card);

    if(LangMode == 2)
        ui->lblRCNO->setText(trUtf8("कार्ड नंबर"));
    else  if(LangMode == 1)
        ui->lblRCNO->setText("Card No.");

    if(iMainTransaction == PDS_TRANSACTION || iMainTransaction == FEST_TRANSACTION){
        if(LangMode == 1)
            ui->pushButton_2->setText(trUtf8("Scan FP"));
        else if(LangMode == 2)
            ui->pushButton_2->setText(trUtf8(" उंगली\n स्कैन"));//subrata

        if(irisStatus == "Y")
        {
            ui->BtnBenIRIS->show();
        }
        else
        {
            ui->BtnBenIRIS->hide();
        }
        //        ui->BtnBenIRIS->show();
    }
    else if(iMainTransaction == PDS_CASHLESS_TRANSACTION){
        if(LangMode == 1)
            ui->pushButton_2->setText("Next");
    }
    ui->ConsentFormcheckBox->setChecked(true);
    ui->stackedWidget->setCurrentIndex(5);
    return;
}

int Widget::PerformEKYC()
{
    iModuleType = DUMMY_TXN;
    miscObj.Check_mplayer_Exists();
    int iRowCount = 0;
    int i = 0;

    QTime tim;
    tim.restart();
    tim.start();
    /******************************/
    iDealMemSelected = MEMBER_SELECTED;
    iRowCount = ui->twdgtEntitlement->rowCount();

    qDebug()<<"No Of Row in twdgtEntitlement = "<<iRowCount;
    for(i = 0; i < iRowCount; i++)
    {
        qDebug()<< i;
        if(ui->twdgtEntitlement->item(i,0)->isSelected() == true){
            break;
        }
    }
    qDebug()<< i <<"<=====>"<<iRowCount;
    if(i == iRowCount)
    {
        qDebug()<<"Please Select Benficiar";
        if(LangMode==1)
            miscObj.DisplayMsgBox((char *)"Please Select Member");
        if(LangMode==2)
            miscObj.tel_DisplayMsgBox((char *)"सदस्य का चयन करें.");//subrata
        return -1;
    }

    int row = 0;
    row = ui->twdgtEntitlement->currentRow();

    qDebug()<<"***********Current Row No = "<<row;
    qDebug()<<"Member Name = "<<ui->twdgtEntitlement->item(row, 0)->text().toAscii().data();
    qDebug()<<"Member UID =  "<<ui->twdgtEntitlement->item(row, 1)->text().toAscii().data();
    qDebug()<<"Real Member UID =  "<<ui->twdgtEntitlement->item(row, 5)->text().toAscii().data();
    qDebug()<<"Owner's UID = "<<ui->twdgtEntitlement->item(row, 2)->text().toAscii().data();

    memset(MemberName, 0, sizeof(MemberName));
    memset(MemberUID, 0, sizeof(MemberUID));
    memset(UID, 0, sizeof(UID));

    strcpy(MemberName,  ui->twdgtEntitlement->item(row, 0)->text().toAscii().data());
    strcpy(MemberUID,   ui->twdgtEntitlement->item(row, 5)->text().toAscii().data());
    strcpy(UID,         ui->twdgtEntitlement->item(row, 2)->text().toAscii().data());

    if(LangMode==1)
        ui->lblEKYC_NameVal->setText(MemberName);
    if(LangMode==2)//subrata
        ui->lblEKYC_NameVal->setText(ui->twdgtEntitlement->item(row, 0)->text());

    if(miscObj.validate_uid(MemberUID) == 0)
    {
        if(LangMode==1)
            miscObj.DisplayWideMsgBox((char *)"UID Already Seeded");
        else if(LangMode==2)
            miscObj.tel_DisplayWideMsgBox((char *)"आधार कार्ड से पहले ही जोड़ा गया है.");//subrata

        ui->ldtEkycCardNo->clear();
        return -1;
    }
    ui->lblEKYC_RCNO_Val->setText(RationCardId);

    ui->ldtEKYC_UID_Entered->setFocus();

    ui->ConsentFormcheckBox_5->setChecked(true);
    ui->stackedWidget->setCurrentIndex(42);
}

//void Widget::on_pushButton_2_clicked()
//{
//    DealerTxnFlag = 0; //initialising dealer txn flag
//    BenAuthSUCCESS = 0;

//    if(!ui->ConsentFormcheckBox->isChecked())
//    {
//        //        miscObj.DisplayWideMsgBox((char *)"Please accept consent form");
//        if(LangMode == 1)
//            miscObj.DisplayWideMsgBox((char *)"Please accept consent form");
//        else if(LangMode == 2)
//            miscObj.tel_DisplayWideMsgBox("कृपया सहमति प्रपत्र स्वीकार करें");
//        QString ConsentReasonStr = "0-N";
//        if(SendTxnCancelReason(ConsentReasonStr,'C') < 0)
//            return;
//        return;
//    }

//    ui->lblRC_NO_Summary->setText(ui->lblRCNO_Val->text().toAscii().data());
//    if(TxnAllotedBetweenTime()<0)
//        return;
//    Qtxnflow.clear();

//    if(LangMode == 1)
//        pop_up("Please Wait....");
//    else if(LangMode == 2)
//        pop_up("कृपया प्रतीक्षा करें....");

//    ui->ldtEKYC_UID_Entered->clear();

//    miscObj.Check_mplayer_Exists();
//    //    Finger_Iris_Selection= 0; //commented for IRIS purpose -BSK.
//    int row = 0;
//    row = ui->twdgtEntitlement->currentRow();

//    if(iMainTransaction == EKYC_TRANSACTION){

//        qDebug()<<"***********Current Row No = "<<row;
//        qDebug()<<"Member Name = "<<ui->twdgtEntitlement->item(row, 0)->text().toAscii().data();
//        qDebug()<<"Member UID =  "<<ui->twdgtEntitlement->item(row, 1)->text().toAscii().data();

//        if(ui->twdgtEntitlement->item(row, 2)->text().toUpper()=="N"){
//            if(LangMode==1)
//                miscObj.DisplayWideMsgBox((char *)"UID Already Seeded");
//            else if(LangMode==2)
//                miscObj.tel_DisplayWideMsgBox((char *)"आधार कार्ड से पहले ही जोड़ा गया है.");//subrata
//            return;
//        }
//        PerformEKYC();
//        return;
//    }

//    ui->pushButton_2->setDisabled(1);

//    qDebug() << "Dealer Transaction flag:" << ui->twdgtEntitlement->item(row, 7)->text();
//    qDebug() << " UID at Dealer Transaction flag:" << ui->twdgtEntitlement->item(row, 1)->text();

//    if((ui->twdgtEntitlement->item(row, 7)->text().toUpper()=="D") && (ui->twdgtEntitlement->item(row, 1)->text().toUpper()=="NA") )// Benificiary Dealer Txn
//    {
//        miscObj.DisplayWideMsgBox((char *)"Transaction linked to dealer");
//        DealerTxnFlag = 1; // Benificiary Dealer Txn
//        DisplayDealerDetails();
//        ui->ConsentFormcheckBox_3->setChecked(true);
//        ui->stackedWidget->setCurrentIndex(20);
//    }else
//        BenficiaryAuth();

//    ui->pushButton_2->setEnabled(1);
//    return;
//}

void Widget::on_pushButton_2_clicked()
{
    if(iMainTransaction == EKYC_TRANSACTION)
    {
        iConsentFormBackFlag = 2;
        ui->ConsentCheckBox->setChecked(true);
        on_BtnOK_2_clicked();
    }
    else
    {
        iConsentFormBackFlag = 2;
        ui->ConsentCheckBox->setChecked(true);
        ConsentForm();
        ui->stackedWidget->setCurrentIndex(21);
    }
    return;
}

void Widget::on_pushButton_4_clicked()
{
    if((iMainTransaction == PDS_TRANSACTION) || (iMainTransaction == PDS_CASHLESS_TRANSACTION) || (iMainTransaction == FEST_TRANSACTION) || (iMainTransaction == HAWKER_TRANSACTION))
    {
        ui->ldtRationCard->clear();
        ui->stackedWidget->setCurrentIndex(4);
    }
    else if(iMainTransaction == EKYC_TRANSACTION)
    {
        ui->ldtEkycCardNo->clear();     //01072017
        ui->stackedWidget->setCurrentIndex(40);
    }
    else if(iMainTransaction == MDM_TRANSACTION)
    {
        ui->lineEditAnganwadiCardNo->setText("");
        ui->stackedWidget->setCurrentIndex(54);
    }
    else
        ui->stackedWidget->setCurrentIndex(29);
}

void Widget::BenficiaryAuth_BKP()
{
    iModuleType = DUMMY_TXN;
    QString ccheckMobileUpdate;
    miscObj.Check_mplayer_Exists();
    int ret = 0;
    int iRowCount = 0;
    int i = 0;

    //    QTime tim;
    //    tim.restart();
    //    tim.start();

    /******************************/
    if(iMainTransaction == PDS_TRANSACTION)
        iDealMemSelected = MEMBER_SELECTED;
    else if(iMainTransaction == MDM_TRANSACTION)
        iDealMemSelected = MDM_TRANSACTION;

    iRowCount = ui->twdgtEntitlement->rowCount();

    qDebug()<<"No Of Row in twdgtEntitlement = "<<iRowCount;
    for(i = 0; i < iRowCount; i++)
    {
        qDebug()<< i;
        if(ui->twdgtEntitlement->item(i,0)->isSelected() == true)
        {
            break;
        }
    }
    qDebug()<< i <<"<=====>"<<iRowCount;
    if(i == iRowCount)
    {
        qDebug()<<"Please Select Benficiar";
        if(LangMode==1)
            miscObj.DisplayMsgBox((char *)"Please Select Member");
        if(LangMode==2)
            miscObj.tel_DisplayMsgBox((char *)"सदस्य का चयन करें.");
        return;
    }

    int row = 0;
    row = ui->twdgtEntitlement->currentRow();
    qDebug()<<"***********Current Row No = "<<row;

    qDebug()<<"Member Name = "<<ui->twdgtEntitlement->item(row, 0)->text().toAscii().data();
    qDebug()<<"Member UID =  "<<ui->twdgtEntitlement->item(row, 1)->text().toAscii().data();

    //    char buff[32];
    memset(MemberName, 0, sizeof(MemberName));
    memset(MemberUID, 0, sizeof(MemberUID));
    memset(UID, 0, sizeof(UID));

    strcpy(MemberName,  ui->twdgtEntitlement->item(row, 0)->text().toAscii().data());
    strcpy(MemberUID,   ui->twdgtEntitlement->item(row, 5)->text().toAscii().data());
    strcpy(UID,         ui->twdgtEntitlement->item(row, 2)->text().toAscii().data());

    if((strcmp(ui->twdgtEntitlement->item(row, 1)->text().toAscii().data(),"NA") == 0) && (strcmp(BenAuthMode_Manual,"N") == 0))
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"UID is Not Present...\nPlease seed the UID using UID seeding...");
        else if(LangMode== 2)
            miscObj.tel_DisplayWideMsgBox((char *)"UID मौजूद नहीं है ... \nकृपया UID सीडिंग का उपयोग करके UID को सीड करें ...");

        return;

    }

    if(QString(MemberUID).isEmpty() || QString(MemberUID).isNull())
    {
        if(LangMode==1)
            miscObj.DisplayWideMsgBox((char *)"UID is Not Present...\nPlease Select Other Member");
        else if(LangMode==2)
            miscObj.tel_DisplayWideMsgBox((char *)"UID मौजूद नहीं है ... \n कृपया अन्य सदस्य का चयन करें");

        return;
    }

    getBeneficiary_AuthTypeANDFusion(row,ui->twdgtEntitlement->item(row,1)->text().toAscii().data());

    qDebug()<<"=========Beneficiary Flags================="<<endl;

    qDebug()<<"FusionStatus"<<FusionStatus;
    qDebug()<<"BenAuthMode_Finger="<<BenAuthMode_Finger;
    qDebug()<<"BenAuthMode_Iris"<<BenAuthMode_Iris;
    qDebug()<<"BenAuthMode_Manual"<<BenAuthMode_Manual;
    qDebug()<<"BenAuthMode_Otp"<<BenAuthMode_Otp;
    qDebug()<<"Main Transaction : "<<iMainTransaction;
    memekycflag=0;

    if((iMainTransaction == PDS_TRANSACTION)||((iMainTransaction == MDM_TRANSACTION)))// || (iMainTransaction == FEST_TRANSACTION) || iMainTransaction == ICDS_TRANSACTION || (iMainTransaction == MDM_TRANSACTION) || (iMainTransaction == HAWKER_TRANSACTION))
    {
        if(Finger_Iris_Selection == 0 && ((strcmp(BenAuthMode_Finger,"Y") == 0)||(iMainTransaction == MDM_TRANSACTION)))
        {
            memekycflag=0;
            ret = AuthBen();
            qDebug()<<"VVVV ====VVVV Ret Auth Ben : "<<ret;
            if(ret < 0)
            {
                if (ret == -2)
                {
                    ui->stackedWidget->setCurrentIndex(4);
                    ui->ldtRationCard->clear();
                }
                return;
            }
            else
                getcomTxn_type = "F"; //doubt
        }
        else if(Finger_Iris_Selection == 1 && strcmp(BenAuthMode_Iris,"Y")== 0)
        {
            int i = 1;
            while(1)
            {
                if((i == 1) || (i == 3) || (i == 5))
                {
                    int ret = Capture_Iris(LEFT_EYE, MemberUID);
                    if(ret == 0)
                    {
                        getcomTxn_type="I";
                        break;
                    }
                    else if(ret == -2)
                        return;
                }
                else if((i == 2) || (i == 4) || (i == 6))
                {
                    int ret = Capture_Iris(RIGHT_EYE, MemberUID);
                    if(ret == 0)
                    {
                        getcomTxn_type="I";
                        break;
                    }
                    else if(ret == -2)
                        return;
                }
                if(i == 2)
                {
                    if(LangMode == 1){
                        miscObj.DisplayWideMsgBox((char *)"Exceeded Attempts for Iris Authentication...");
                    }else if(LangMode == 2){
                        miscObj.tel_DisplayWideMsgBox((char *)"ഐറിസ് ഓതന്റിക്കേഷനുള്ള പരിധി കഴിഞ്ഞിരിക്കുന്നു.");
                    }
                    /*
                    EKYC_TEST_FLAG = 0;
                    system("rm -rf /mnt/jffs2/PWDACKrequest.xml");
                    if(PwdackRequest_Response("/mnt/jffs2/PWDACKrequest.xml") < 0)
                        return;
                    */
                    getcomTxn_type="M";
                    if(strcmp(BenAuthMode_Manual,"Y")== 0)
                        memekycflag =1;

                    break;
                }
                i++;
            }
        }
        else if(strcmp(BenAuthMode_Manual,"N")== 0)//NA CASE and Manul -N
        {
            if(iMainTransaction == PDS_TRANSACTION)
            {
                qDebug()<<"BenAuthMode_Manual ---"<<BenAuthMode_Manual;
                return;
            }
        }
        else if(strcmp(BenAuthMode_Manual,"Y")== 0)//NA CASE only for FP
        {
            if(iMainTransaction == PDS_TRANSACTION)
                memekycflag=1;
        }
        else if(strcmp(BenAuthMode_Manual,"M")== 0)//Manual Txn flow //by santosh
        {
            if(iMainTransaction == PDS_TRANSACTION)
            {
                miscObj.DisplayWideMsgBox((char *)"Transaction linked to Manual");
                system("rm -rf /mnt/jffs2/PWDACKrequest.xml");
                int AckRet = PwdackRequest_Response("/mnt/jffs2/PWDACKrequest.xml");
                if(AckRet < 0)
                {
                    return;
                }
                else
                {
                    getcomTxn_type="M";
                    DisplayCommDetails();
                    return;
                }
            }
        }
    }

    if((memekycflag == 1))//-Manual authentication-doing ekyc //changed by santosh
    {
        if (eKYCPrompt != "Y")
            return;
        //        getcomTxn_type="M";
        getcomTxn_type="E";

        memekycflag=0;
        qDebug()<<"aadhaar -Manual authentication-doing ekyc";
        char EkycUid[16]="";
        int ekycflag=0;
        memset(EkycUid,0,sizeof(EkycUid));

        while(1)
        {
            ekycflag=0;
            ret=Ekyc_uid_popup(EkycUid);
            qDebug()<<"UId="<<EkycUid;
            if(ret == -1)
                return;
            if(ret==-2)
            {
                continue;
            }
            else if(ret == 0)
            {
                ekycflag = 1;
                memset(memekycuid,0,sizeof(memekycuid));
                strcpy(memekycuid,EkycUid);
                break;
            }
            else if(ret == -1 && (strcmp(BenAuthMode_Manual,"N")== 0))
            {
                qDebug()<<"Manual flag"<<BenAuthMode_Manual;
                return;
            }
            /*  else if(ret == -1 && (strcmp(BenAuthMode_Manual,"Y")== 0))
            {
                qDebug()<<"Manual flag"<<BenAuthMode_Manual;
                ekycflag=0;
                system("rm -rf /mnt/jffs2/PWDACKrequest.xml");
                if(PwdackRequest_Response("/mnt/jffs2/PWDACKrequest.xml")< 0)
                    return;

                break;
            }*/
        }
        if(ekycflag == 1)
        {
            EKYC_TEST_FLAG = 1;

            int RetSts = Member_EKYC_Status(1);
            if(RetSts == -1)
                return ;

            //if((Qtxnflow == "B")||strcmp(BenAuthMode_Manual,"N")== 0)
            if(Qtxnflow == "B")
            {
                qDebug()<<"Qtxnflow="<<Qtxnflow<<"BenAuthMode_Manual"<<BenAuthMode_Manual<<endl;
                ui->stackedWidget->setCurrentIndex(4);
                ui->ldtRationCard->clear();
                return;
            }
            ///////////////////
            /// New Txn Flow///
            ///////////////////
            if(Qtxnflow == "D")
            {
                getcomTxn_type = "D";
                DealerTxnFlag = 1; // Benificiary Dealer Txn
                DisplayDealerDetails();
                ui->ConsentFormcheckBox_3->setChecked(true);
                ui->stackedWidget->setCurrentIndex(20);
                return;
            }
            //            else if(strcmp(BenAuthMode_Manual,"M")== 0)//Manual Txn flow //by santosh
            if(Qtxnflow == "M")
            {
                getcomTxn_type="M";
                DisplayCommDetails();
                return;
            }

            EKYC_TEST_FLAG = 0;
            //commented by santosh
            //            if(Qtxnflow != "F")
            //            {
            //                system("rm -rf /mnt/jffs2/PWDACKrequest.xml");
            //                if(PwdackRequest_Response("/mnt/jffs2/PWDACKrequest.xml")< 0)
            //                    return;
            //            }
        }
    }

    qDebug()<<"???? getcomTxn_type ????"<<getcomTxn_type;
    if(iMainTransaction == PDS_TRANSACTION)
    {
        /************ Mobile Update ****************/

        ccheckMobileUpdate = checkMobileUpdate("/mnt/jffs2/CardHolderDetails.xml");
        if(ccheckMobileUpdate == "Y" || ccheckMobileUpdate == "y")
        {

            QString QMobileNo;
            QMobileNo.clear();
            qDebug() << "************************************";
            qDebug() << " Updatng MobileNumber";
            qDebug() << "************************************";

            bool ok;


MOBILE :
            if(LangMode == 1)
                QMobileNo = QInputDialog::getText(this, tr("Update Mobile Number"),tr("<b>Would You Like To Update Mobile No.? <br> Enter Mobile Number </b>"), QLineEdit::Normal," ", &ok);
            else if(LangMode == 2)
                QMobileNo = QInputDialog::getText(this, tr("Update Mobile Number"), trUtf8("<b>क्या आप अपना मोबाइल नंबर अपडेट करना चाहते हैं? <br> अपना मोबाइल नंबर दर्ज करें.</b>"), QLineEdit::Normal,"", &ok);

            QRegExp re("\\d*");  // a digit (\d), zero or more times (*)

            if(QMobileNo.isEmpty() && ok == true)
            {
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox((char *)"please enter 10 Digit Mobile Number only");
                else if(LangMode == 2)
                    miscObj.tel_DisplayWideMsgBox((char *)"कृपया केवल 10 अंकों का\n मोबाइल नंबर दर्ज करें.");
                goto MOBILE;

            }
            if(!QMobileNo.isEmpty())
            {
                if (!re.exactMatch(QMobileNo))
                {
                    if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"please enter numeric value only");
                    else if(LangMode == 2)
                        miscObj.tel_DisplayWideMsgBox((char *)"कृपया केवल संख्यात्मक मूल्य दर्ज करें.");
                    goto MOBILE;
                }

                if(QMobileNo.length() < 10 || QMobileNo.length() > 10)
                {

                    if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"please enter 10 Digit Mobile Number only");
                    else if(LangMode == 2)
                        miscObj.tel_DisplayWideMsgBox((char *)"कृपया केवल 10 अंकों का\n मोबाइल नंबर दर्ज करें.");
                    goto MOBILE;

                }

                char cMobileNo[15]="";
                memset(cMobileNo, 0, sizeof(cMobileNo));
                sprintf(cMobileNo,"%s",QMobileNo.toAscii().data());

                UpdateMobileNumber(cMobileNo);
            }

        }


        /************ Mobile Update End *************/
    }


    {
        //        QSqlQuery query;
        //        query.prepare("SELECT BalQty FROM BenQuotaTable");
        //        if(!query.exec())
        //            qDebug() << "BenQuotaTable Bal Qty:"<<query.lastError();
        //        else
        //        {
        //            while(query.next())
        //            {
        //                if(query.value(0).toFloat(0) < 0)
        //                {
        //                    if(LangMode == 1)
        //                        miscObj.DisplayWideMsgBox((char *)"Recieved Invalid Quantity From ePDS Server");
        //                    else if(LangMode == 2)
        //                        miscObj.tel_DisplayWideMsgBox((char *)"ईपीडीएस सर्वर से अमान्य मात्रा प्राप्त हुई.");

        //                    return;
        //                }
        //            }
        //        }
        //        int count = 0;
        //        QSqlQuery query1;
        //        query1.prepare("SELECT COUNT(ItemType) FROM BenQuotaTable where RationCardId=?");
        //        if(iMainTransaction == MDM_TRANSACTION)
        //            query1.addBindValue(ui->lineEditAnganwadiCardNo->text().toAscii().data());
        //        else
        //            query1.addBindValue(Rcno.toAscii().data());
        //        if(!query1.exec())
        //        {
        //            qDebug() << "BenQuotaTable COUNT:"<< query1.lastError();
        //            return;
        //        }
        //        else
        //        {
        //            while(query1.next())
        //                count = query1.value(0).toInt(0);
        //        }
        //        ui->tblCommIssue->setRowCount(count);

        //        int row2 = 0;
        //        qDebug()<<" ?????????????????????????? ++++++Rcno"<<Rcno<<endl;


        //        query.clear();
        //        query.prepare("select ItemType, MonthlyQuota, ItemPrice, ItemCode, BalQty, AvailedQty, MeasureUnit, RequiredQty, closingBal,commNamell,weighingFlag,minimumqty from BenQuotaTable where RationCardId=? ");
        //        if(iMainTransaction == PDS_TRANSACTION)
        //            query.addBindValue(Rcno.toAscii().data());
        //        else
        //            query.addBindValue(ui->lineEditAnganwadiCardNo->text().toAscii().data());
        //        query.exec();

        //        while(query.next())
        //        {
        //            if(LangMode == 1)
        //            {
        //                if(iMainTransaction == PDS_TRANSACTION)
        //                {
        //                    memset(buff,0,sizeof(buff));
        //                    sprintf(buff, "%s(%s)", query.value(0).toString().toAscii().data(), query.value(1).toString().toAscii().data());
        //                    qDebug()<<"0 Item Type  - Total Qty (Monthly Quota) = "<<buff;

        //                    ui->tblCommIssue->setItem(row2, 0, new QTableWidgetItem(buff));
        //                }
        //                else
        //                {
        //                    QString commidityname;
        //                    QString s1,s2;
        //                    s1="(";
        //                    s2=")";
        //                    commidityname.clear();

        //                    memset(buff,0,sizeof(buff));
        //                    commidityname=query.value(9).toString()+s1+query.value(1).toString()+s2;

        //                    qDebug()<<"commidityname"<<commidityname;
        //                    ui->tblCommIssue->setItem(row2, 0, new QTableWidgetItem(commidityname));
        //                }
        //            }
        //            else if(LangMode == 2)
        //            {
        //                QString commidityname;
        //                QString s1,s2;
        //                s1="(";
        //                s2=")";
        //                commidityname.clear();

        //                memset(buff,0,sizeof(buff));
        //                commidityname=query.value(9).toString()+s1+query.value(1).toString()+s2;

        //                qDebug()<<"commidityname"<<commidityname;
        //                ui->tblCommIssue->setItem(row2, 0, new QTableWidgetItem(commidityname));

        //            }
        //            ui->tblCommIssue->item(row2, 0)->setFlags( ui->tblCommIssue->item(row2,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
        //            ui->tblCommIssue->item(row2, 0)->setFlags(ui->tblCommIssue->item(row2, 0)->flags() ^ Qt::ItemIsSelectable);

        //            char TmpString[300];
        //            if(iMainTransaction == PDS_TRANSACTION)
        //            {
        //                query1.clear();
        //                memset(TmpString,0x00,sizeof(TmpString));
        //                sprintf(TmpString,"select (select count() from sqlite_master where type='table' and name = 'KeyRegister'),(select count() from KeyRegister where RcId = '%s' and CommCode ='%s')",Rcno.toAscii().data(),query.value(3).toString().toAscii().data());
        //                qDebug()<<"TmpString:"<<TmpString;
        //                if(!query1.exec(TmpString))
        //                {
        //                    qDebug()<<"No table found KeyRegister "<<endl;
        //                }
        //                query1.next();
        //                if((query1.value(0).toString().toInt()!=0)&&(query1.value(1).toString().toInt()!=0))
        //                {
        //                    query1.clear();
        //                    memset(TmpString,0x00,sizeof(TmpString));
        //                    sprintf(TmpString,"select balanceEntitlement from KeyRegister where RcId = '%s' and CommCode ='%s'",Rcno.toAscii().data(),query.value(3).toString().toAscii().data());
        //                    qDebug()<<"TmpString:"<<TmpString;
        //                    if(query1.exec(TmpString))
        //                    {

        //                    }
        //                    query1.next();

        //                    qDebug()<<"Local  Issue Qty offline:"<<atof(query1.value(0).toString().toAscii().data());
        //                    qDebug()<<"Total Qty :"<<atof(query.value(1).toString().toAscii().data());
        //                    qDebug()<<"Balance  Qty from Server:"<<atof(query.value(4).toString().toAscii().data());

        //                    if(query1.value(0).toString().toFloat() < query.value(4).toString().toFloat())
        //                    {
        //                        memset(buff,0,sizeof(buff));
        //                        sprintf(buff,"%s",query1.value(0).toString().toAscii().data());
        //                        qDebug()<<"1 Bal Qty = "<<buff;
        //                    }
        //                    else
        //                    {
        //                        //Bal Qty
        //                        memset(buff,0,sizeof(buff));
        //                        strcpy(buff, query.value(4).toString().toAscii().data());
        //                        qDebug()<<"In else Bal Qty = "<<buff;
        //                    }
        //                }
        //                else
        //                {
        //                    //Key register tables not found

        //                    //Bal Qty
        //                    memset(buff,0,sizeof(buff));
        //                    strcpy(buff, query.value(4).toString().toAscii().data());
        //                    qDebug()<<"In else Bal Qty = "<<buff;
        //                }
        //            }
        //            else
        //            {
        //                memset(buff,0,sizeof(buff));
        //                strcpy(buff, query.value(4).toString().toAscii().data());
        //                qDebug()<<"In else Bal Qty = "<<buff;
        //            }
        //            ui->tblCommIssue->setItem(row2, 1, new QTableWidgetItem(buff));
        //            ui->tblCommIssue->item(row2, 1)->setFlags(ui->tblCommIssue->item(row2, 1)->flags() ^ Qt::ItemIsEditable);
        //            ui->tblCommIssue->item(row2, 1)->setFlags(ui->tblCommIssue->item(row2, 1)->flags() ^ Qt::ItemIsSelectable);


        //            char iqty[32],prate[32]="";
        //            memset(iqty,0,sizeof(iqty));
        //            memset(prate,0,sizeof(prate));

        //            //Item Price
        //            memset(buff,0,sizeof(buff));
        //            strcpy(buff, query.value(2).toString().toAscii().data());
        //            qDebug()<<"2 Item Price = "<<buff;
        //            strcpy(iqty,buff);

        //            ui->tblCommIssue->setItem(row2, 2, new QTableWidgetItem(buff));
        //            ui->tblCommIssue->item(row2, 2)->setFlags(ui->tblCommIssue->item(row2, 2)->flags() ^ Qt::ItemIsEditable);
        //            ui->tblCommIssue->item(row2, 2)->setFlags(ui->tblCommIssue->item(row2, 2)->flags() ^ Qt::ItemIsSelectable);

        //            memset(buff,0,sizeof(buff));
        //            strcpy(buff, query.value(4).toString().toAscii().data());
        //            qDebug()<<"1 Bal Qty = "<<buff;
        //            strcpy(prate,buff);

        //            ui->tblCommIssue->setItem(row2, 3, new QTableWidgetItem(buff));
        //            ui->tblCommIssue->item(row2, 3)->setData(Qt::BackgroundColorRole, QVariant(QColor(Qt::green)));
        //            //        ui->tblCommIssue->item(0,3)

        //            //amt
        //            memset(buff,0,sizeof(buff));
        //            sprintf(buff,"%0.2f",atof(iqty)*atof(prate));
        //            qDebug()<<iqty<<"*"<<prate <<" = Amt Bufff::: "<<buff;

        //            ui->tblCommIssue->setItem(row2, 4, new QTableWidgetItem(buff));

        //            if(strcmp(query.value(10).toString().toAscii().data(),"Y")==0)
        //            {
        //                qDebug()<<"&&&&&&&&&&&&&&&&& get weight from weighing scale";

        //                ui->tblCommIssue->item(row2, 3)->setFlags(ui->tblCommIssue->item(row2, 3)->flags() ^ Qt::ItemIsEditable);
        //                ui->tblCommIssue->item(row2, 3)->setFlags(ui->tblCommIssue->item(row2, 3)->flags() ^ Qt::ItemIsSelectable);
        //            }
        //            else
        //            {
        //                qDebug()<<"&&&&&&&&&&&&&&&&& Enter weight Manully";

        //                ui->tblCommIssue->item(row2, 3)->setFlags(ui->tblCommIssue->item(row2, 3)->flags() | Qt::ItemIsEditable);
        //                ui->tblCommIssue->item(row2, 3)->setFlags(ui->tblCommIssue->item(row2, 3)->flags() ^ Qt::ItemIsSelectable);
        //            }

        //            //        ui->tblCommIssue->setItem(row2, 4, new QTableWidgetItem("0.00"));

        //            //Item Code
        //            memset(buff,0,sizeof(buff));
        //            strcpy(buff, query.value(3).toString().toAscii().data());

        //            ui->tblCommIssue->item(row2, 4)->setFlags( ui->tblCommIssue->item(row2,4)->flags() ^ Qt::ItemIsEditable);
        //            ui->tblCommIssue->item(row2, 4)->setFlags(ui->tblCommIssue->item(row2, 4)->flags() ^ Qt::ItemIsSelectable);

        //            qDebug()<<"5 Item Code = "<<buff;

        //            ui->tblCommIssue->setItem(row2, 5, new QTableWidgetItem(buff));
        //            ui->tblCommIssue->item(row2, 5)->setFlags( ui->tblCommIssue->item(row2,5)->flags() ^ Qt::ItemIsEditable);
        //            ui->tblCommIssue->item(row2, 5)->setFlags(ui->tblCommIssue->item(row2, 5)->flags() ^ Qt::ItemIsSelectable);


        //            if(iMainTransaction == PDS_TRANSACTION)
        //            {
        //                query1.clear();
        //                memset(TmpString,0x00,sizeof(TmpString));
        //                sprintf(TmpString,"select (select count() from sqlite_master where type='table' and name = 'Pos_Ob'),(select count() from Pos_Ob where CommCode ='%s')",query.value(3).toString().toAscii().data());
        //                qDebug()<<"TmpString :"<<TmpString;
        //                if(!query1.exec(TmpString))
        //                {
        //                    qDebug()<<"No table found Cb "<<endl;
        //                }
        //                query1.next();
        //                if((query1.value(0).toString().toInt()!=0)&&(query1.value(1).toString().toInt()!=0))
        //                {
        //                    query1.clear();
        //                    query1.prepare("select closingBalance from Pos_Ob where CommCode =?");
        //                    query1.bindValue(":CommCode",query.value(3).toString().toAscii().data());
        //                    if(query1.exec())
        //                    {

        //                    }
        //                    query1.next();
        //                    qDebug()<<"Local Db CB :"<<query1.value(0).toString().toFloat();
        //                    qDebug()<<"Server Db CB :"<<query.value(8).toString().toFloat();
        //                    if(query1.value(0).toString().toFloat()<query.value(8).toString().toFloat())
        //                    {
        //                        ui->tblCommIssue->setItem(row2, 6, new QTableWidgetItem(query1.value(0).toString()));
        //                    }
        //                    else
        //                    {
        //                        ui->tblCommIssue->setItem(row2, 6, new QTableWidgetItem(query.value(8).toString()));
        //                    }
        //                }
        //                else
        //                {
        //                    ui->tblCommIssue->setItem(row2, 6, new QTableWidgetItem(query.value(8).toString()));
        //                }
        //            }
        //            else
        //            {
        //                ui->tblCommIssue->setItem(row2, 6, new QTableWidgetItem(query.value(8).toString()));
        //            }
        //            //ui->tblCommIssue->setItem(row2, 6, new QTableWidgetItem(query.value(8).toString()));
        //            ui->tblCommIssue->item(row2, 6)->setFlags( ui->tblCommIssue->item(row2,6)->flags() ^ Qt::ItemIsEditable);
        //            ui->tblCommIssue->item(row2, 6)->setFlags(ui->tblCommIssue->item(row2, 6)->flags() ^ Qt::ItemIsSelectable);

        //            //by subbu-29122017
        //            memset(buff,0,sizeof(buff));
        //            sprintf(buff, "%s", query.value(3).toString().toAscii().data());
        //            ui->tblCommIssue->setItem(row2, 7, new QTableWidgetItem(buff));
        //            ui->tblCommIssue->item(row2, 7)->setFlags(ui->tblCommIssue->item(row2, 7)->flags() ^ Qt::ItemIsEditable);
        //            ui->tblCommIssue->item(row2, 7)->setFlags(ui->tblCommIssue->item(row2, 7)->flags() ^ Qt::ItemIsSelectable);

        //            ui->tblCommIssue->setItem(row2, 8, new QTableWidgetItem(query.value(10).toString().toAscii().data()));
        //            //for minimum qty
        //            ui->tblCommIssue->setItem(row2, 9, new QTableWidgetItem(query.value(11).toString().toAscii().data()));

        //            ui->tblCommIssue->setRowHeight(row2,20);

        //            row2++;
        //        }

        //        if(row2 == 0)
        //        {
        //            qDebug()<<"No Data in BenQuota Table";
        //            if(LangMode == 1)
        //                miscObj.DisplayMsgBox((char *)"No Commodities For This Card");
        //            if(LangMode == 2)
        //                miscObj.tel_DisplayWideMsgBox((char *)"इस कार्ड के लिए कोई वस्तु नहीं है.");

        //            return;
        //        }
        //        qDebug()<<"Column(1) Width = "<<ui->tblCommIssue->columnWidth(1);

        //        ui->tblCommIssue->setColumnWidth(0,120);
        //        ui->tblCommIssue->setColumnWidth(1,60);
        //        ui->tblCommIssue->setColumnWidth(2,60);
        //        ui->tblCommIssue->setColumnWidth(3,70);
        //        ui->tblCommIssue->setColumnWidth(4,60);
        //        ui->tblCommIssue->setColumnWidth(6,60);

        //        ui->tblCommIssue->setSelectionMode(QAbstractItemView::SingleSelection);
        //        ui->tblCommIssue->verticalHeader()->hide();
        //        ui->tblCommIssue->setColumnHidden(5, true);
        //        ui->tblCommIssue->setColumnHidden(7, true);//29122017 by subbu
        //        ui->tblCommIssue->setColumnHidden(8, true);
        //        ui->tblCommIssue->setColumnHidden(9, true);
        //        ui->tblCommIssue->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        //        ui->tblCommIssue->horizontalHeader()->setMinimumWidth(0);

        //        //Go to next page to display the items in table widget
        //        iCommMode = getCommMode();

        //        qDebug()<<"Communication Mode To Weing Machine = "<<iCommMode;

        //        fTotalAmt = 0.00;
        //        fTotalQty = 0.00;
        //        ui->lblTotalAmtVal->setText("0.00");

        //        /******* TIME LAP STOP **********/

        //        char lapbuff[32];
        //        memset(lapbuff, 0, sizeof(lapbuff));
        //        sprintf(lapbuff,"Lap Time : %d", tim.elapsed());
        //        tim.restart();

        //        /********************************/

        //        Finger_Iris_Selection = 0;

        //        if(iMainTransaction == MDM_TRANSACTION || iMainTransaction == ICDS_TRANSACTION)
        //        {
        //            qDebug()<<"DISABLING DYNAMIC";
        //            ui->btnGetWt->hide();
        //            ui->comboCommMode->hide();

        //        }
        //        else
        //        {
        //            qDebug()<<"ENABLING DYNAMIC";
        //        }
        //        iModuleType = COMM_TXN;
        //        fTotalAmt = 0.00;
        //        fTotalQty = 0.00;
        //        int iNoOfRows = 0;

        //        iNoOfRows = ui->tblCommIssue->rowCount();
        //        for(i=0; i<iNoOfRows;i++)
        //        {
        //            if(ui->tblCommIssue->item(i, 4) != NULL)
        //            {
        //                fTotalAmt += atof(ui->tblCommIssue->item(i, 4)->text().toAscii().data());
        //            }
        //        }
        //        memset(buff, 0, sizeof(buff));
        //        sprintf(buff,"%0.2f",fTotalAmt);
        //        ui->lblTotalAmtVal->setText(buff);
        //        ui->stackedWidget->setCurrentIndex(6);
    }

    //////////////////////////////////////////////////////////
    DisplayCommDetails();
    return;
}

void Widget::BenficiaryAuth()
{
    iModuleType = DUMMY_TXN;
    QString ccheckMobileUpdate;
    miscObj.Check_mplayer_Exists();
    int ret = 0;
    int iRowCount = 0;
    int i = 0;

    /******************************/
    if(iMainTransaction == PDS_TRANSACTION)
        iDealMemSelected = MEMBER_SELECTED;
    else if(iMainTransaction == MDM_TRANSACTION)
        iDealMemSelected = MDM_TRANSACTION;

    iRowCount = ui->twdgtEntitlement->rowCount();

    qDebug()<<"No Of Row in twdgtEntitlement = "<<iRowCount;
    for(i = 0; i < iRowCount; i++)
    {
        qDebug()<< i;
        if(ui->twdgtEntitlement->item(i,0)->isSelected() == true)
        {
            break;
        }
    }
    qDebug()<< i <<"<=====>"<<iRowCount;
    if(i == iRowCount)
    {
        qDebug()<<"Please Select Benficiar";
        if(LangMode==1)
            miscObj.DisplayMsgBox((char *)"Please Select Member");
        if(LangMode==2)
            miscObj.tel_DisplayMsgBox((char *)"सदस्य का चयन करें.");
        return;
    }

    int row = 0;
    row = ui->twdgtEntitlement->currentRow();
    qDebug()<<"***********Current Row No = "<<row;

    qDebug()<<"Member Name = "<<ui->twdgtEntitlement->item(row, 0)->text().toAscii().data();
    qDebug()<<"Member UID =  "<<ui->twdgtEntitlement->item(row, 1)->text().toAscii().data();

    memset(MemberName, 0, sizeof(MemberName));
    memset(MemberUID, 0, sizeof(MemberUID));
    memset(UID, 0, sizeof(UID));

    strcpy(MemberName,  ui->twdgtEntitlement->item(row, 0)->text().toAscii().data());
    strcpy(MemberUID,   ui->twdgtEntitlement->item(row, 5)->text().toAscii().data());
    strcpy(UID,         ui->twdgtEntitlement->item(row, 2)->text().toAscii().data());

    if(QString(MemberUID).isEmpty() || QString(MemberUID).isNull())
    {
        if(LangMode==1)
            miscObj.DisplayWideMsgBox((char *)"UID is Not Present...\nPlease Select Other Member");
        else if(LangMode==2)
            miscObj.tel_DisplayWideMsgBox((char *)"UID मौजूद नहीं है ... \n कृपया अन्य सदस्य का चयन करें");

        return;
    }

    getBeneficiary_AuthTypeANDFusion(row,ui->twdgtEntitlement->item(row,1)->text().toAscii().data());

    qDebug()<<"=========Beneficiary Flags================="<<endl;

    qDebug()<<"FusionStatus"<<FusionStatus;
    qDebug()<<"BenAuthMode_Finger="<<BenAuthMode_Finger;
    qDebug()<<"BenAuthMode_Iris"<<BenAuthMode_Iris;
    qDebug()<<"BenAuthMode_Manual"<<BenAuthMode_Manual;
    qDebug()<<"BenAuthMode_Otp"<<BenAuthMode_Otp;
    qDebug()<<"Main Transaction : "<<iMainTransaction;
    memekycflag=0;

    /*
    strncpy(BenAuthMode_Manual,"D",1);//Hard corded
    BenAuthMode_Manual[1]='\0';
    eKYCPrompt="Y";
    */

    if((strcmp(ui->twdgtEntitlement->item(row, 1)->text().toAscii().data(),"NA") == 0) && (strcmp(BenAuthMode_Manual,"N") == 0))
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"UID is Not Present...\nPlease seed the UID using UID seeding...");
        else if(LangMode== 2)
            miscObj.tel_DisplayWideMsgBox((char *)"UID मौजूद नहीं है ... \nकृपया UID सीडिंग का उपयोग करके UID को सीड करें ...");

        return;

    }

    if((iMainTransaction == PDS_TRANSACTION)||((iMainTransaction == MDM_TRANSACTION)))// || (iMainTransaction == FEST_TRANSACTION) || iMainTransaction == ICDS_TRANSACTION || (iMainTransaction == MDM_TRANSACTION) || (iMainTransaction == HAWKER_TRANSACTION))
    {
        if(Finger_Iris_Selection == 0 && ((strcmp(BenAuthMode_Finger,"Y") == 0)||(iMainTransaction == MDM_TRANSACTION)))
        {
            memekycflag=0;
            ret = AuthBen();
            qDebug()<<"VVVV ====VVVV Ret Auth Ben : "<<ret;
            if(ret < 0)
            {
                if (ret == -2)
                {
                    ui->stackedWidget->setCurrentIndex(4);
                    ui->ldtRationCard->clear();
                }
                return;
            }
            else
                getcomTxn_type = "F"; //doubt
        }
        else if(Finger_Iris_Selection == 1 && strcmp(BenAuthMode_Iris,"Y")== 0)
        {
            int i = 1;
            while(1)
            {
                if((i == 1) || (i == 3) || (i == 5))
                {
                    int ret = Capture_Iris(LEFT_EYE, MemberUID);
                    if(ret == 0)
                    {
                        getcomTxn_type="I";
                        break;
                    }
                    else if(ret == -2)
                        return;
                }
                else if((i == 2) || (i == 4) || (i == 6))
                {
                    int ret = Capture_Iris(RIGHT_EYE, MemberUID);
                    if(ret == 0)
                    {
                        getcomTxn_type="I";
                        break;
                    }
                    else if(ret == -2)
                        return;
                }
                if(i == 2)
                {
                    if(LangMode == 1){
                        miscObj.DisplayWideMsgBox((char *)"Exceeded Attempts for IRIS Authentication...");
                    }else if(LangMode == 2){
                        miscObj.tel_DisplayWideMsgBox((char *)"IRIS प्रमाणीकरण के लिए अधिक प्रयास");
                    }
                    memekycflag=1;

                    Finger_Iris_Selection = 0;

                    break;
                }
                i++;
            }
        }
        else if(strcmp(BenAuthMode_Manual,"M")== 0)//Manual Txn flow
        {
            if(iMainTransaction == PDS_TRANSACTION)
            {
                miscObj.DisplayWideMsgBox((char *)"Transaction linked to Manual");
                system("rm -rf /mnt/jffs2/PWDACKrequest.xml");
                int AckRet = PwdackRequest_Response("/mnt/jffs2/PWDACKrequest.xml");
                if(AckRet < 0)
                {
                    return;
                }
                else
                {
                    getcomTxn_type="M";
                    DisplayCommDetails();
                    return;
                }
            }
        }
        else if(strcmp(BenAuthMode_Manual,"D")== 0)//Manual Txn flow
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Transaction linked to dealer");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"यूआईडी उपलब्ध नहीं है \n लेन-देन डीलर से जुड़ा है ");

            //            getcomTxn_type = "D";
            DealerTxnFlag = 1; // Benificiary Dealer Txn

            DisplayDealerDetails();

            ui->ConsentFormcheckBox_3->setChecked(true);
            ui->stackedWidget->setCurrentIndex(20);

            Finger_Iris_Selection = 0;

            return;
        }

    }

    qDebug()<<"1.memekycflag"<<memekycflag<<endl;
    qDebug()<<"2.BenAuthMode_Manual"<<BenAuthMode_Manual<<endl;
    qDebug()<<"3.eKYCPrompt"<<eKYCPrompt<<endl;
    qDebug()<<"4.BenAuthSUCCESS"<<BenAuthSUCCESS<<endl;

    if(eKYCPrompt == "Y" && BenAuthSUCCESS != 1)
    {
        getcomTxn_type="E";

        memekycflag=0;
        qDebug()<<"aadhaar -Manual authentication-doing ekyc";
        char EkycUid[16]="";
        int ekycflag=0;
        memset(EkycUid,0,sizeof(EkycUid));

        while(1)
        {
            ekycflag=0;
            ret=Ekyc_uid_popup(EkycUid);
            qDebug()<<"UId="<<EkycUid;
            if(ret == -1)
                return;
            if(ret==-2)
            {
                continue;
            }
            else if(ret == 0)
            {
                ekycflag = 1;
                memset(memekycuid,0,sizeof(memekycuid));
                strcpy(memekycuid,EkycUid);
                break;
            }
            else if(ret == -1 && (strcmp(BenAuthMode_Manual,"N")== 0))
            {
                qDebug()<<"Manual flag"<<BenAuthMode_Manual;
                return;
            }

        }
        if(ekycflag == 1)
        {
            EKYC_TEST_FLAG = 1;
            int RetSts = Member_EKYC_Status(1);
            if(RetSts == -1)
                return ;

            qDebug()<<"1.Qtxnflow="<<Qtxnflow<<"BenAuthMode_Manual"<<BenAuthMode_Manual<<endl;
            if((Qtxnflow.isNull()) || ((Qtxnflow != "F") && (Qtxnflow != "B") && (Qtxnflow != "M") && (Qtxnflow != "D")))
            {
                return;
            }

            if(Qtxnflow == "B")
            {
                qDebug()<<"2.Qtxnflow="<<Qtxnflow<<"BenAuthMode_Manual"<<BenAuthMode_Manual<<endl;
                ui->stackedWidget->setCurrentIndex(4);
                ui->ldtRationCard->clear();
                return;
            }
            ///////////////////
            /// New Txn Flow///
            ///////////////////
            if(Qtxnflow == "D")
            {
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox((char *)"Transaction linked to dealer");
                else if(LangMode == 2)
                    miscObj.tel_DisplayWideMsgBox((char *)"यूआईडी उपलब्ध नहीं है \n लेन-देन डीलर से जुड़ा है ");

                //                getcomTxn_type = "D";
                DealerTxnFlag = 1; // Benificiary Dealer Txn
                DisplayDealerDetails();
                ui->ConsentFormcheckBox_3->setChecked(true);
                ui->stackedWidget->setCurrentIndex(20);
                return;
            }
            if(Qtxnflow == "M")
            {
                getcomTxn_type="M";
                DisplayCommDetails();
                return;
            }

            EKYC_TEST_FLAG = 0;
        }
    }
    else if((strcmp(BenAuthMode_Manual,"M")== 0) && eKYCPrompt == "N" && BenAuthSUCCESS != 1)
    {
        qDebug()<<"BenAuth==Y and ekycpromt==N\n";

        system("rm -rf /mnt/jffs2/PWDACKrequest.xml");
        if(PwdackRequest_Response("/mnt/jffs2/PWDACKrequest.xml")< 0)
            return;

        popup_msg *dlgPopup1 = new popup_msg(this);
        if (LangMode == ENGLISH)
            dlgPopup1->ui->lblPopup->setText("Going for Manual Transaction ...");
        else if(LangMode == HINDI)
            dlgPopup1->ui->lblPopup->setText(trUtf8("मैनुअल लेनदेन के लिए जा रहा है ..."));

        dlgPopup1->show();
        QCoreApplication::processEvents();
        QCoreApplication::processEvents();
        sleep(3);
        dlgPopup1->close();
        dlgPopup1->deleteLater();
    }
    else if((strcmp(BenAuthMode_Manual,"N")== 0) && BenAuthSUCCESS != 1 && getcomTxn_type !="E")
    {
        qDebug()<<" ???? >>>BenAuthMode_Manual ---"<<BenAuthMode_Manual;
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Manual Transaction not allowd for this Beneficiary");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"इस लाभार्थी के लिए मैनुअल लेनदेन की अनुमति नहीं है");
        return;
    }
    else if((strcmp(BenAuthMode_Manual,"D")== 0) && eKYCPrompt == "N" && BenAuthSUCCESS != 1)
    {
        qDebug()<<"I am going for Dealer Auth for Benficiary"<<endl;

        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Transaction linked to dealer");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"यूआईडी उपलब्ध नहीं है \n लेन-देन डीलर से जुड़ा है ");

        //        getcomTxn_type = "D";
        DealerTxnFlag = 1; // Benificiary Dealer Txn
        DisplayDealerDetails();
        ui->ConsentFormcheckBox_3->setChecked(true);
        ui->stackedWidget->setCurrentIndex(20);
        return;
    }

    qDebug()<<"???? getcomTxn_type ????"<<getcomTxn_type;

    if(iMainTransaction == PDS_TRANSACTION)
    {
        /************ Mobile Update ****************/

        ccheckMobileUpdate = checkMobileUpdate("/mnt/jffs2/CardHolderDetails.xml");
        if(ccheckMobileUpdate == "Y" || ccheckMobileUpdate == "y")
        {

            QString QMobileNo;
            QMobileNo.clear();
            qDebug() << "************************************";
            qDebug() << " Updatng MobileNumber";
            qDebug() << "************************************";

            bool ok;

MOBILE :
            if(LangMode == 1)
                QMobileNo = QInputDialog::getText(this, tr("Update Mobile Number"),tr("<b>Would You Like To Update Mobile No.? <br> Enter Mobile Number </b>"), QLineEdit::Normal," ", &ok);
            else if(LangMode == 2)
                QMobileNo = QInputDialog::getText(this, tr("Update Mobile Number"), trUtf8("<b>क्या आप अपना मोबाइल नंबर अपडेट करना चाहते हैं? <br> अपना मोबाइल नंबर दर्ज करें.</b>"), QLineEdit::Normal,"", &ok);

            QRegExp re("\\d*");  // a digit (\d), zero or more times (*)

            if(QMobileNo.isEmpty() && ok == true)
            {
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox((char *)"please enter 10 Digit Mobile Number only");
                else if(LangMode == 2)
                    miscObj.tel_DisplayWideMsgBox((char *)"कृपया केवल 10 अंकों का\n मोबाइल नंबर दर्ज करें.");
                goto MOBILE;

            }
            if(!QMobileNo.isEmpty())
            {
                if (!re.exactMatch(QMobileNo))
                {
                    if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"please enter numeric value only");
                    else if(LangMode == 2)
                        miscObj.tel_DisplayWideMsgBox((char *)"कृपया केवल संख्यात्मक मूल्य दर्ज करें.");
                    goto MOBILE;
                }

                if(QMobileNo.length() < 10 || QMobileNo.length() > 10)
                {

                    if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"please enter 10 Digit Mobile Number only");
                    else if(LangMode == 2)
                        miscObj.tel_DisplayWideMsgBox((char *)"कृपया केवल 10 अंकों का\n मोबाइल नंबर दर्ज करें.");
                    goto MOBILE;

                }

                char cMobileNo[15]="";
                memset(cMobileNo, 0, sizeof(cMobileNo));
                sprintf(cMobileNo,"%s",QMobileNo.toAscii().data());

                UpdateMobileNumber(cMobileNo);
            }

        }

        /************ Mobile Update End *************/
    }
    //////////////////////////////////////////////////////////

    DisplayCommDetails();
    return;
}

void Widget::setvalue1(QString str)
{
    qDebug()<<"Value = "<<str;
}

void Widget::on_btnCancelCommodity_clicked()
{
    //        if(PartialTxn != 1)
    //        {
    //            if(iMainTransaction == NCR_TRANSACTION)
    //                ui->stackedWidget->setCurrentIndex(39);
    //            else{
    //                ui->ConsentFormcheckBox->setChecked(true);
    //                ui->stackedWidget->setCurrentIndex(5);
    //                }
    //        }
    //        else
    //        {
    //            ui->stackedWidget->setCurrentIndex(4);
    //        }
    //    Reasons_Vec.clear();

    Finger_Iris_Selection = 0;
    if(DealerLoginBy == 2)
    {
        ui->stackedWidget->setCurrentIndex(4);
    }
    if(PartialTxn != 1) //online
    {
        if(Reasons_Vec.size() > 0)
        {
            QInputDialog qDialog;
            QStringList items;
            QString Item_Str;

            QList <QString> RecordList;
            for(int i =0 ; i < Reasons_Vec.size() ; i++)
            {
                RecordList.clear();
                RecordList =  Reasons_Vec[i];
                Item_Str = RecordList[0];
                Item_Str.append("-");
                Item_Str.append(RecordList[1]);
                items << Item_Str;
            }

            //    qDialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
            qDialog.setComboBoxItems(items);
            qDialog.setWindowTitle("Select reason");

            QString text;

            if (qDialog.exec() == QDialog::Accepted)
            {
                text = qDialog.textValue();
                qDebug() << "Accepetedd:" << text;
                /////Send Request here//////////
                if(SendTxnCancelReason(text,'R') < 0)
                    return;
                ////////////////////////////////

                if(iMainTransaction == NCR_TRANSACTION)
                    ui->stackedWidget->setCurrentIndex(39);
                else
                {
                    ui->ConsentFormcheckBox->setChecked(true);
                    ui->stackedWidget->setCurrentIndex(5);
                }
            }
            else
            {
                return;
            }
        }
        else
        {
            if(iMainTransaction == NCR_TRANSACTION)
                ui->stackedWidget->setCurrentIndex(39);
            else
            {
                ui->ConsentFormcheckBox->setChecked(true);
                ui->stackedWidget->setCurrentIndex(5);
            }
        }
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(4);
    }
}

int Widget::CheckingMandatoryFestComm()
{
    qDebug() << "In CheckingMandatoryFestComm()";

    int rowcount = ui->tblCommIssue->rowCount();

    for(int i=0;i<rowcount;i++)
    {
        QStringList list = ui->tblCommIssue->item(i,0)->text().split('(', QString::SkipEmptyParts);
        QString QCommName = list.at(0).trimmed();

        int ItemCode = 0;
        QSqlQuery query;
        query.prepare("SELECT ItemCode FROM ItemMaster WHERE ItemType=?");
        query.addBindValue(QCommName);
        if(!query.exec())
            qDebug() << query.lastError();
        else
        {
            while(query.next())
            {
                ItemCode = query.value(0).toInt(0);
            }
        }

        if((ItemCode == 8) || (ItemCode == 9) || (ItemCode == 14) || (ItemCode == 15) || (ItemCode == 16))//check festval code
        {
            if((ui->tblCommIssue->item(i,3)->text().toFloat() == 0) || (ui->tblCommIssue->item(i,3)->text().toFloat() == 0.00))
            {
                return -1;
            }
        }
    }

    return 0;
}

int Widget::dotremoval(char *Qty)
{
    int i = 0,count = 0;
    int len = 0,dotPos = 0;

    for(i = 0; i < strlen(Qty); i++)
    {
        if(Qty[0] == '.')
            return -1;
        if(Qty[strlen(Qty)-1] == '.')
            return -1;
        if(Qty[i] == '.')
        {
            dotPos = i;
            count++;
        }
    }

    if(count > 1)
        return -1;
    else
    {
        if(dotPos == 0)
            return 0;
        else{
            len  = strlen(Qty) - (dotPos+1);
            if(len > 3)
                return -2;
        }
    }

    return 0;
}

void Widget::on_btnConfirmCommodity_clicked()
{
    //if(TxnAllotedBetweenTime()<0)
    //    return;
    if(LangMode == 1)
        pop_up("Please Wait....");
    else if(LangMode == 2)
        pop_up("कृपया प्रतीक्षा करें....");

    ui->tblCommIssue->setFocus();

    for(int i = 0;i < ui->tblCommIssue->rowCount(); i++){
        int res_sys = dotremoval(ui->tblCommIssue->item(i,3)->text().toAscii().data());
        if(res_sys != 0){
            qDebug()<<"Please Enter Valid Quantity";
            if(LangMode==1)
                pop_up((char *)"Please Enter Valid Quantity");
            if(LangMode==2)
                pop_up((char *)"कृपया मात्रा को \nठीक से दर्ज कीजिये");
            sleep(2);
            return;
        }
    }
    if(iFestivalFlag == 1){
        if(CheckingMandatoryFestComm() != 0){
            if(LangMode == 1)
                miscObj.DisplayMsgBox((char *)"Please Select Festival Commodity For Issuance");
            else if(LangMode==2)
                miscObj.tel_DisplayMsgBox((char *)"कृपया जारी करने के लिए महोत्सव कमोडिटी का चयन करें");
            return;
        }
    }

    iFestivalFlag = 0;
    ui->pushButton_6->setEnabled(true);
    ui->btnConfirmCommodity->setFocus();

    if((iMainTransaction == PDS_TRANSACTION) || (iMainTransaction == PDS_CASHLESS_TRANSACTION)|| (iMainTransaction == FEST_TRANSACTION) || (iMainTransaction == NCR_TRANSACTION))
    {
        int iRowCount = ui->tblCommIssue->rowCount();
        float ftotqty = 0.00;

        for(int i = 0; i < iRowCount; i++)
            ftotqty = ftotqty + ui->tblCommIssue->item(i, 3)->text().toFloat(0);

        miscObj.Check_mplayer_Exists();
        qDebug()<<"Total Amt : "<<fTotalAmt;
        qDebug()<<"Total Qty : "<<ftotqty;
        if(ftotqty <= 0.00){
//        if((fTotalAmt < 0.00) || (ftotqty <= 0.00)){
//        if((fTotalAmt == 0.00) && (fTotalQty == 0.00)){
            qDebug()<<"No Items are issued";
            miscObj.AudioTest((char *)"189");
            if(LangMode == 1)
                miscObj.DisplayMsgBox((char *)"Please Select Any Commodity For Issuance");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"वितरित किए जाने वाले \nकिसी भी उत्पाद का चयन करें.");
            return;
        }
        if(fTotalAmt < 0.00){
            qDebug()<<"Negetive amount";
            if(LangMode == 1)
                miscObj.DisplayMsgBox((char *)"Invalid total amount");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"अमान्य कुल नकद");
            return;
        }

    }

    //    int z = ui->tblCommIssue->rowCount();
    //    QString TmpString;
    //    char TmpBuff[128];
    //    if(PartialTxn ==1){
    //        for(int y = 0; y <z ; y++){
    //            if((ui->tblCommIssue->item(y, 3)->text().toFloat(0)!=0)&&(ui->tblCommIssue->item(y, 1)->text().toFloat(0) != ui->tblCommIssue->item(y, 3)->text().toFloat(0))){
    //                if(LangMode == 1){
    //                    memset(TmpBuff,0,sizeof(TmpBuff));
    //                    //sprintf(buff,"Pleae lift Full Bal Qty in Partial Txn for all Commodities");//\n
    //                    sprintf(TmpBuff,"Please lift full Bal.Qty(%0.3f) for %s commodity",ui->tblCommIssue->item(y, 1)->text().toFloat(0),ui->tblCommIssue->item(y, 0)->text().toAscii().data());
    //                    miscObj.DisplayWideMsgBox(TmpBuff);
    //                }else{
    //                    TmpString.clear();
    //                    TmpString = trUtf8("कृपया पूरा कमोडिटी शेष मात्रा(");
    //                    TmpString += ui->tblCommIssue->item(y, 1)->text();
    //                    TmpString += trUtf8("के बराबर");
    //                    TmpString += ui->tblCommIssue->item(y, 0)->text();
    //                    TmpString += trUtf8("के लिए  उठाएं");
    //                    miscObj.tel_DisplayWideMsgBox((char *)TmpString.toAscii().data());
    //                    //miscObj.tel_DisplayWideMsgBox((char *)"कृपया सभी कमोडिटी के लिए आंशिक लेनदेन में पूर्ण बाल मात्रा उठाएँ.");
    //                }
    //                return;
    //            }
    //        }
    //    }  //commented under partial lifting in offline -BSK.

    if(iMainTransaction == ICDS_TRANSACTION || iMainTransaction == MDM_TRANSACTION){
        int iRowCount = ui->tblCommIssue->rowCount();
        float ftotqty = 0.00;

        for(int i = 0; i < iRowCount; i++)
            ftotqty = ftotqty + ui->tblCommIssue->item(i, 3)->text().toFloat(0);

        if(ftotqty <= 0.00){
            miscObj.AudioTest((char *)"189");
            if(LangMode == 1)
                miscObj.DisplayMsgBox((char *)"Please Select Any Commodity For Issuance");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"वितरित किए जाने वाले किसी\n भी वस्तु का चयन करें");
            return;
        }
    }
    if(iMainTransaction == PDS_TRANSACTION || (iMainTransaction == FEST_TRANSACTION))
    {
        ui->lblRC_NO_Summary->setText(Rcno);
        if(LangMode == 1){
            ui->label_4->setText("Card No.");
            ui->pushButton_6->setText("PRINT");
        }
        else{
            ui->lblRationCard->setText(QString::fromUtf8("कार्ड नंबर"));
            ui->pushButton_6->setText(QString::fromUtf8("प्रिंट"));
        }
    }
    else if(iMainTransaction == PDS_CASHLESS_TRANSACTION){
        //        ui->lblRC_NO_Summary->setText(ui->ldtRationCard->text());
        if(LangMode == 1)
            ui->pushButton_6->setText("Pay");
        else if(LangMode ==2)
            ui->pushButton_6->setText(trUtf8("भुगतान"));
    }
    else if(iMainTransaction == MDM_TRANSACTION){
        ui->lblRC_NO_Summary->setText(ui->lineEditAnganwadiCardNo->text());
        //        ui->label_4->setText("School No:");
        //        ui->pushButton_6->setText("PRINT");
        if(LangMode == 1){
            ui->label_4->setText("School No.");
            ui->pushButton_6->setText("PRINT");
        }
        else{
            ui->lblRationCard->setText(QString::fromUtf8("स्कूल नंबर"));
            ui->pushButton_6->setText(QString::fromUtf8("प्रिंट"));
        }
    }

    float TotalAmtWithDue = 0.00;
    if(iMainTransaction == PDS_CASHLESS_TRANSACTION || iMainTransaction == PDS_TRANSACTION){
        TotalAmtWithDue = fTotalAmt + previous_amount.toFloat(0);
    }else{
        TotalAmtWithDue = fTotalAmt;
    }

    char szTotoalAmtWithDue[16];
    miscObj.Round_TotalAmount(TotalAmtWithDue,szTotoalAmtWithDue);
    ui->lblTotalAmtVal_2->setText(szTotoalAmtWithDue);

    ui->lblTotalAmtVal_2->setText(szTotoalAmtWithDue);
    if(iMainTransaction == PDS_TRANSACTION || (iMainTransaction == PDS_CASHLESS_TRANSACTION)){
        //ui->lblDueAmtVal->setText(previous_amount.toAscii().data());
    }else{
        //ui->lblDueAmtVal->setText("0");
        //ui->lblDueAmtVal->hide();
        //ui->lblDueAmt->hide();
    }

    popup_msg *dlgPopup1 = new popup_msg(this);

    if(iMainTransaction == PDS_TRANSACTION || (iMainTransaction == FEST_TRANSACTION)){
        miscObj.AudioTest((char *)"188");

        if(LangMode == 1)
            dlgPopup1->ui->lblPopup->setText("Reciept/Bill Amount is...");
        else if(LangMode == 2)
            dlgPopup1->ui->lblPopup->setText(trUtf8("रसीद /विधेयक राशि.."));
        dlgPopup1->show();
        QCoreApplication::processEvents();
        QCoreApplication::processEvents();

        int iDBFlag = miscObj.GetAudioFlag();

        if(iDBFlag == 1)
            sleep(4);
        else
            sleep(2);
        PlayRupeesPaisa();
        miscObj.AudioTest((char *)"66");
    }

    int iRow = ui->tblCommIssue->rowCount();
    qDebug()<<"No Of Rows = "<<iRow;
    int i=0;
    char buff[256];

    qDebug()<<"Commodity   Qty   Price";

    ui->listWidgetCommClaimed->clear();
    while(i<iRow)
    {
//        if((ui->tblCommIssue->item(i,3) != NULL) && ((strcmp(ui->tblCommIssue->item(i,3)->text().toAscii().constData(), "0" ) != 0) && (strcmp(ui->tblCommIssue->item(i,3)->text().toAscii().constData(), "0.00" ) != 0)))
        if((ui->tblCommIssue->item(i,3) != NULL) && (ui->tblCommIssue->item(i,3)->text().toFloat(0) > 0.00))
        {
            if(LangMode==1){
                memset(buff, 0, sizeof(buff));
                sprintf(buff, "%-16s        %-8s           %-8s       %-8s", ui->tblCommIssue->item(i,0)->text().toAscii().data(), ui->tblCommIssue->item(i,1)->text().toAscii().data(), ui->tblCommIssue->item(i,3)->text().toAscii().data(), ui->tblCommIssue->item(i,4)->text().toAscii().data());
                qDebug() << buff << "**********";

                ui->listWidgetCommClaimed->addItem(new QListWidgetItem(buff));
            }
            if(LangMode==2){
                QString str;
                str.clear();
                memset(buff, 0, sizeof(buff));
                sprintf(buff, "%-10s         %-10s     %-10s", ui->tblCommIssue->item(i,1)->text().toAscii().data(), ui->tblCommIssue->item(i,3)->text().toAscii().data(), ui->tblCommIssue->item(i,4)->text().toAscii().data());
                qDebug() << buff << "**********";

                QStringList list = ui->tblCommIssue->item(i,0)->text().split('(', QString::SkipEmptyParts);
                QString QCommNamell = list.at(0).trimmed();
                str = QCommNamell +"     "+QString(buff);
                ui->listWidgetCommClaimed->addItem(new QListWidgetItem(str));
            }
        }
        i++;
    }
    //    ui->lblRC_NO_Summary->setText(ui->ldtRationCard->text());
    if((DealerLoginBy == 2)||(partialOnlineOfflineStatus.toUpper()=="Y"))
    {
        ui->lblRC_NO_Summary->setText(ui->ldtRationCard->text());
        ui->lblRCNO_Val_2->setText(ui->ldtRationCard->text());
    }
    else
    {
        ui->lblRC_NO_Summary->setText(ui->lblRCNO_Val->text().toAscii().data());
        ui->lblRCNO_Val_2->setText(ui->lblRCNO_Val->text().toAscii().data());
    }
    QSqlQuery query;
    query.prepare("select CustomerName, UID from CustomerMaster where RationCardId=?");
    query.addBindValue(ui->ldtRationCard->text().toAscii().data());
    query.exec();

    while(query.next()){
        memset(buff,0,sizeof(buff));
        strcpy(buff, query.value(0).toString().toAscii().data());
        qDebug()<<"1 Buff = "<<buff;
    }
    if(iMainTransaction == PDS_TRANSACTION || (iMainTransaction == FEST_TRANSACTION)){
        dlgPopup1->close();
        dlgPopup1->deleteLater();
    }
    memset(TxnId, 0, sizeof(TxnId));
    miscObj.GetTransactionId(TxnId);
    qDebug()<<"Txn Id = "<<TxnId;
    ui->stackedWidget->setCurrentIndex(7);

    return;
}

void Widget::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}

QString Widget::GetSurveyWt()
{
    bool ok;
    QString text;
    char Buf[10];
    int i;
    QString TmpString;
    TmpString.clear();
    TmpString = "";
    if(LangMode == 1)
    {
        for(int i=0;i<SurveyMsgEn.size();i++)
        {
            TmpString+= SurveyMsgEn.at(i);
            if(i!=0)
                if(i%42==0)
                    TmpString+='\n';
        }
    }
    else
    {
        for(int i=0;i<SurveyMsgLl.size();i++)
        {
            TmpString+= SurveyMsgLl.at(i);
            if(i!=0)
                if(i%42==0)
                    TmpString+='\n';
        }
    }
    while(1)
    {

        if(LangMode==1)
            text = QInputDialog::getText(this, tr("Required Qty"),TmpString, QLineEdit::Normal,"", &ok);
        else if(LangMode==2)
            text = QInputDialog::getText(this, tr("Required Qty"),TmpString, QLineEdit::Normal,"", &ok);

        if(ok && !text.isEmpty())
        {
            memset(Buf,0x00,sizeof(Buf));
            sprintf(Buf,"%s",text.toAscii().data());
            qDebug()<<"";
            for(i=0;i<strlen(Buf);i++)
            {
                if((Buf[i]!='.')&&(Buf[i]<'0'||(Buf[i]>'9')))
                {
                    break;
                }
            }
            if(i!=strlen(Buf))
            {
                continue;
            }else{
                if(atof(Buf)>SurveyMinQty.toFloat())
                {
                    //if(LangMode==1)
                    {
                        char TmpStr[200];
                        sprintf(TmpStr,"Please Enter required Qty less than or equal to %s",SurveyMinQty.toAscii().data());
                        miscObj.DisplayWideMsgBox((char *)TmpStr);
                    }
                    /*else
                                {
                                        QString TmpString1;
                                        TmpString1.clear();
                                        TmpString1 =  "कृपया आवश्यक मात्रा";
                                        TmpString1 += SurveyMinQty;
                                        TmpString1 += "से कम या इसके बराबर दर्ज करें";
                                        miscObj.tel_DisplayWideMsgBox((char *)trUtf8(TmpString1.toAscii().data()));
                                }*/
                    continue;
                }
                return Buf;
            }
        }
        else if(ok && text.isEmpty())
        {
            //return "0";
            continue;
        }
        else
        {
            qDebug()<<"We enter cancel";
            SurveyStsFlag="Q";
            return "0";
        }
    }
}

void Widget::on_pushButton_6_clicked()
{    
    if(TxnAllotedBetweenTime() < 0)
        return;
    if(DealerLoginBy == 2)
    {
        PartialOfflineTxnUpload();
        return;
    }
    receipt_type=2;
    char RCID[32]="";

    memset(RCID,0,sizeof(RCID));

    if(LangMode == 1)
        pop_up("Please Wait....");
    else if(LangMode == 2)
        pop_up("कृपया प्रतीक्षा करें....");

    prn_open();
//    if(prn_paperstatus() < 0)
    if((prn_paperstatus() < 0) && (paperRequiredFlag.toUpper() != "N"))
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
            miscObj.tel_DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद को प्रिंट करने में असमर्थ. \n अपनी बैटरी चार्ज करें");

        prn_close();

        return;

    }
    if(PartialTxn==1)
    {
        PartialOfflineTxnUpload();
        return;
    }
    ui->pushButton_6->setDisabled(true);

    //Check_mplayer_Exists();
    miscObj.Check_mplayer_Exists();
    /******** TIME LAP START************/
    QTime tim;
    tim.restart();
    tim.start();
    /******************************/

    GetCustomerDetails();

    prepareInsertUpdateCommFile();

    char TmpStr[1024];
    QSqlQuery query;
    /*if(partialOnlineOfflineStatus.toUpper() == "Y")
    {
            popup_msg *dlgPopup1 = new popup_msg(this);
            if(LangMode == 1)
                dlgPopup1->ui->lblPopup->setText("Please Wait...\nCommodity Qty Uploading to Server");
            else
                dlgPopup1->ui->lblPopup->setText(trUtf8("सर्वर पर कमोडिटी विवरण भेज रहा है"));
            dlgPopup1->show();
            QCoreApplication::processEvents();
            QCoreApplication::processEvents();

            memset(TmpStr,0x00,sizeof(TmpStr));
            FILE *fp = popen("ping google.com -c 5|grep packets|awk {'print $7'}","r");
            fgets(TmpStr,sizeof(TmpStr),fp);
            pclose(fp);
            qDebug()<<"Ping TmpStr:"<<TmpStr;
            if(strncmp(TmpStr,"0%",2)==0)
            {
                    dlgPopup1->close();
                    dlgPopup1->deleteLater();
            }
            else
            {
                //PartialTxn =1;
                if(ui->radioButton_RCNOFLAG->isChecked()==true)
                {
                }
                else
                {
                    ui->ldtRationCard->setText(ui->lblRCNO_Val->text());;
                }
                dlgPopup1->close();
                dlgPopup1->deleteLater();
                PartialOfflineTxnUpload();
                return;
            }
    }*/
    //Send Details to Server
    int RetVal = SendTxnToServer();
    if(iMainTransaction == PDS_TRANSACTION)
    {
        query.clear();
        if(!query.exec("select (select count() from sqlite_master where type='table' and name = 'KeyRegister'),(select count() from sqlite_master where type='table' and name = 'Pos_Ob'),(select count() from sqlite_master where type='table' and name = 'BenfiaryTxn')"))
        {

        }
        query.next();
        if((RetVal==0)&&(query.value(0).toString().toInt()==1)&&(query.value(1).toString().toInt()==1)&&(query.value(2).toString().toInt()==1))
        {
            QString RcNumTmp;
            RcNumTmp.clear();
            if(ui->radioButton_RCNOFLAG->isChecked()==true)
            {
                qDebug()<<"RcNumTmp1 :"<<RcNumTmp;
                RcNumTmp = ui->ldtRationCard->text();
            }
            else
            {
                qDebug()<<"RcNumTmp2 :"<<RcNumTmp;
                RcNumTmp = ui->lblRCNO_Val->text();
            }
            qDebug()<<"RcNumTmp :"<<RcNumTmp;
            memset(TmpStr,0x00,sizeof(TmpStr));
            for(int i=0;i<ui->tblCommIssue->rowCount();i++)
            {
                if(ui->tblCommIssue->item(i,3)->text().toFloat() > 0.00)
                {
                    memset(TmpStr,0x00,sizeof(TmpStr));
//                    sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement-%0.3f where rcId='%s' and commCode='%s'",ui->tblCommIssue->item(i,3)->text().toFloat(),RcNumTmp.toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data());
                    sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement-%0.3f where rcId='%s' and commCode='%s' and allocationType='%s' and allotedMonth='%s' and allotedYear='%s'",ui->tblCommIssue->item(i,3)->text().toFloat(),RcNumTmp.toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->tblCommIssue->item(i,11)->text().toAscii().data(),ui->tblCommIssue->item(i,12)->text().toAscii().data(),ui->tblCommIssue->item(i,13)->text().toAscii().data());
                    qDebug()<<"TmpStr  :"<<TmpStr;
                    if(!query.exec(TmpStr))
                    {
                        for(int j=0;j<i;j++)
                        {
                            if(ui->tblCommIssue->item(j,3)->text().toFloat()>0.00)
                            {
                                memset(TmpStr,0x00,sizeof(TmpStr));
//                                sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement+%0.3f where rcId='%s' and commCode='%s'",ui->tblCommIssue->item(j,3)->text().toFloat(),RcNumTmp.toAscii().data(),ui->tblCommIssue->item(j,5)->text().toAscii().data());
                                sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement+%0.3f where rcId='%s' and commCode='%s' and allocationType='%s' and allotedMonth='%s' and allotedYear='%s'",ui->tblCommIssue->item(i,3)->text().toFloat(),RcNumTmp.toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->tblCommIssue->item(i,11)->text().toAscii().data(),ui->tblCommIssue->item(i,12)->text().toAscii().data(),ui->tblCommIssue->item(i,13)->text().toAscii().data());
                                if(!query.exec(TmpStr))
                                {

                                }
                            }
                        }
                        qDebug()<<"Balance Updation Failed Query";
                        return ;
                    }
                }
            }
            for(int i=0;i<ui->tblCommIssue->rowCount();i++)
            {
                if(ui->tblCommIssue->item(i,3)->text().toFloat()>0.00)
                {
                    memset(TmpStr,0x00,sizeof(TmpStr));

                    //sprintf(TmpStr,"update Pos_Ob set closingBalance = closingBalance-%0.3f where commCode='%s'",ui->tblCommIssue->item(i,3)->text().toFloat(),ui->tblCommIssue->item(i,5)->text().toAscii().data());

                    sprintf(TmpStr,"update Pos_Ob set  closingBalance = closingBalance-%0.3f where commCode=(select commonCommCode from commodityMaster where commCode='%s')",ui->tblCommIssue->item(i,3)->text().toFloat(),ui->tblCommIssue->item(i,5)->text().toAscii().data());
                    qDebug()<<"TmpStr  :"<<TmpStr;

                    if(!query.exec(TmpStr))
                    {
                        for(int j=0;j<ui->tblCommIssue->rowCount();j++)
                        {
                            if(ui->tblCommIssue->item(j,3)->text().toFloat()>0.00)
                            {
                                memset(TmpStr,0x00,sizeof(TmpStr));
//                                sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement+%0.3f where rcId='%s' and commCode='%s'",ui->tblCommIssue->item(j,3)->text().toFloat(),RcNumTmp.toAscii().data(),ui->tblCommIssue->item(j,5)->text().toFloat());
                                sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement+%0.3f where rcId='%s' and commCode='%s' and allocationType='%s' and allotedMonth='%s' and allotedYear='%s'",ui->tblCommIssue->item(i,3)->text().toFloat(),RcNumTmp.toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->tblCommIssue->item(i,11)->text().toAscii().data(),ui->tblCommIssue->item(i,12)->text().toAscii().data(),ui->tblCommIssue->item(i,13)->text().toAscii().data());
                                if(!query.exec(TmpStr))
                                {

                                }
                            }
                        }
                        for(int j=0;j<i;j++)
                        {
                            if(ui->tblCommIssue->item(j,3)->text().toFloat() > 0.00)
                            {
                                memset(TmpStr,0x00,sizeof(TmpStr));
                                //sprintf(TmpStr,"update Pos_Ob set  closingBalance = closingBalance+%0.3f where commCode='%s'",ui->tblCommIssue->item(j,3)->text().toFloat(),ui->tblCommIssue->item(j,5)->text().toAscii().data());
                                sprintf(TmpStr,"update Pos_Ob set  closingBalance = closingBalance+%0.3f where commCode=(select commonCommCode from commodityMaster where commCode='%s')",ui->tblCommIssue->item(j,3)->text().toFloat(),ui->tblCommIssue->item(j,5)->text().toAscii().data());
                                if(!query.exec(TmpStr))
                                {

                                }
                            }
                        }
                        qDebug()<<"Closing Balanace Updation Failed Query";
                        return;
                    }
                }
            }

            displayTime();
            displayDate();

            char FpsId[30];
            memset(FpsId,0x00,sizeof(FpsId));
            getShopId(FpsId);

            for(int i=0;i<ui->tblCommIssue->rowCount();i++)
            {
                if(ui->tblCommIssue->item(i,3)->text().toFloat()>0.00)
                {
                    memset(TmpStr,0x00,sizeof(TmpStr));

//                    sprintf(TmpStr,"insert into BenfiaryTxn values('%s','%s',(select schemeId from KeyRegister where rcId = '%s' limit 1),'%s',(select totalEntitlement from KeyRegister where rcId = '%s' and commCode='%s'),(select balanceEntitlement from KeyRegister where rcId = '%s' and commCode='%s'),'%s','%s','%s','%s','%s',(select memberNameLl from KeyRegister where rcId = '%s' limit 1),'%s %s','Y','O',(select AllotMonth from KeyRegister where rcId = '%s' limit 1),(select AllotYear from KeyRegister where rcId = '%s' limit 1),(select allocationType from KeyRegister where rcId = '%s' limit 1))",FpsId,RcNumTmp.toAscii().data(),RcNumTmp.toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),RcNumTmp.toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),RcNumTmp.toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->tblCommIssue->item(i,3)->text().toAscii().data(),ui->tblCommIssue->item(i,2)->text().toAscii().data(),ui->tblCommIssue->item(i,4)->text().toAscii().data(),ui->lblTotalAmtVal->text().toAscii().data(),TxnId,RcNumTmp.toAscii().data(),ui->labelDate->text().toAscii().data(),ui->labelTime->text().toAscii().data(),RcNumTmp.toAscii().data(),RcNumTmp.toAscii().data(),RcNumTmp.toAscii().data());

                    sprintf(TmpStr,"insert into BenfiaryTxn values('%s','%s',(select schemeId from KeyRegister where rcId = '%s' limit 1),'%s',(select totalEntitlement from KeyRegister where rcId = '%s' and commCode='%s'),(select balanceEntitlement from KeyRegister where rcId = '%s' and commCode='%s'),'%s','%s','%s','%s','%s',(select memberNameEn from KeyRegister where rcId = '%s' limit 1),'%s %s','Y','O','%s','%s','%s')",FpsId,RcNumTmp.toAscii().data(),RcNumTmp.toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),RcNumTmp.toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),RcNumTmp.toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->tblCommIssue->item(i,3)->text().toAscii().data(),ui->tblCommIssue->item(i,2)->text().toAscii().data(),ui->tblCommIssue->item(i,4)->text().toAscii().data(),ui->lblTotalAmtVal->text().toAscii().data(),TxnId,RcNumTmp.toAscii().data(),ui->labelDate->text().toAscii().data(),ui->labelTime->text().toAscii().data(),ui->tblCommIssue->item(i,12)->text().toAscii().data(),ui->tblCommIssue->item(i,13)->text().toAscii().data(),ui->tblCommIssue->item(i,11)->text().toAscii().data());

                    qDebug()<<"TmpStr : "<<TmpStr;
                    query.clear();
                    if(!query.exec(TmpStr))
                    {
                        qDebug()<<"Upload Insert Failed";
                        query.clear();
                        query.prepare("delete from BenfiaryTxn where RecptId=?");
                        query.addBindValue(TxnId);
                        if(!query.exec(TmpStr))
                        {

                        }
                        return ;
                    }
                }
            }
        }
    }
    if(RetVal == 0)
    {
        miscObj.AudioTest((char *)"191");
        char buff[64];
        memset(buff, 0, sizeof(buff));

        if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"लेनदेन सफल था ...");
        else
            miscObj.DisplayWideMsgBox((char *)"Transaction Successfull");

//        sprintf(buff, "Transaction Successfull...\nPrinting, Please Wait...");
        sprintf(buff, "Printing, Please Wait...");

        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);

        popup_msg *dlgPopup2 = new popup_msg(this);

        if(LangMode == 2)
//            dlgPopup2->ui->lblPopup->setText(trUtf8("लेनदेन सफल था ... \nकृपया प्रतीक्षा करें,रसीद छपाई हो रही है"));
            dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें,रसीद छपाई हो रही है"));
        else
            dlgPopup2->ui->lblPopup->setText(buff);

        dlgPopup2->show();
        QCoreApplication::processEvents();
        QCoreApplication::processEvents();

        getHeadings();

        if((iMainTransaction == PDS_TRANSACTION)||(iMainTransaction == MDM_TRANSACTION))
        {
            if(ParsePrintLastRecptResponse((char *)"/mnt/jffs2/UpdateInsertService_Resp.xml",RCID) < 0)
            {
                miscObj.DisplayWideMsgBox((char *)"InvalID Response Parsing");
                return;
            }
            ParsePrintLastRecptResponse_balance((char *)"/mnt/jffs2/UpdateInsertService_Resp.xml");

            if(iMainTransaction == PDS_TRANSACTION)
                PrintLastReciptResponse(RCID);
            else if(iMainTransaction == MDM_TRANSACTION)
                MdmTxnRcpt();
        }

        prn_close();

        dlgPopup2->close();
        dlgPopup2->deleteLater();
    }

    ui->twdgtEntitlement->clearContents();
    ui->tblCommIssue->clearContents();
    ui->lblTotalAmtVal->clear();
    ui->ldtIssueQty->clear();

    if(ui->radioButton_UIDFLAG->isChecked())
        ui->ldtRationCard->clear(); //01072017

    /******* TIME LAP STOP **********/
    char lapbuff[32];
    memset(lapbuff, 0, sizeof(lapbuff));
    sprintf(lapbuff,"Lap Time : %d", tim.elapsed());
    tim.restart();
    /********************************/

    if(iMainTransaction == PDS_TRANSACTION)
    {
        iMainTransaction = PDS_TRANSACTION;

        Finger_Iris_Selection = 0;
        memset(gauth_transaction_code, 0, sizeof(gauth_transaction_code));

        ui->ldtRationCard->clear();
        ui->stackedWidget->setCurrentIndex(4);
    }
    else if(iMainTransaction == MDM_TRANSACTION)
    {
        iMainTransaction = MDM_TRANSACTION;

        ui->label_type_card->clear();
        ui->lineEditAnganwadiCardNo->clear();

        Finger_Iris_Selection = 0;
        memset(gauth_transaction_code, 0, sizeof(gauth_transaction_code));

        ui->stackedWidget->setCurrentIndex(54);
        return;
    }

    return;
}

int Widget::SendTxnToServer()
{
    GPRS objGprs;

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...\nCommodity Qty Uploading to Server");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.\nकमोडिटी मात्रा अपलोड हो रहा है. "));

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    if(objGprs.Check_Network_Available() < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        CheckOfflineTxnUpload();
        return -1;
    }
    memset(gRTC, 0, sizeof(gRTC));
    memset(gDATE, 0, sizeof(gDATE));
    memset(gTIME, 0, sizeof(gTIME));
    memset(gJULIAN_DAYS, 0, sizeof(gJULIAN_DAYS));

    GetRTC_Params(gRTC, gDATE, gTIME, gJULIAN_DAYS);

    memset(gszDashedDt, 0, sizeof(gszDashedDt));
    memset(gszDashedTm, 0, sizeof(gszDashedTm));

    miscObj.DDMMYYYY2DD_MM_YYYY(gDATE, gszDashedDt);
    miscObj.HHMMSS2HH_MM_SS(gTIME, gszDashedTm);

    char Filename[64];
    memset(Filename, 0, sizeof(Filename));
    strcpy(Filename, "/mnt/jffs2/UpdateInsertService.xml");
    system("cat /mnt/jffs2/UpdateInsertService.xml");
    system("rm /mnt/jffs2/UpdateInsertService_Resp.xml");

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

    if(iMainTransaction == PDS_TRANSACTION || iMainTransaction == FEST_TRANSACTION)
    {
//        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/UpdateInsertService.xml", cUrlName,  "/mnt/jffs2/UpdateInsertService_Resp.xml", SERVER_TIME_OUT);
        int ret_http_code = UrlPingCheck(cUrlName);

        if(ret_http_code == 1)
        {
            qDebug()<<"URL IS LIVE WORKING";
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/UpdateInsertService.xml", cUrlName,  "/mnt/jffs2/UpdateInsertService_Resp.xml", responseTimedOutTimeInSec.toInt());
        }
        else
        {
            qDebug()<<"URL IS NOT WORKING Switching to other url";
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/UpdateInsertService.xml", PDS_URL2,  "/mnt/jffs2/UpdateInsertService_Resp.xml", responseTimedOutTimeInSec.toInt());
        }
    }
    if(iMainTransaction == MDM_TRANSACTION)
    {
//        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/UpdateInsertService.xml", cUrlName,  "/mnt/jffs2/UpdateInsertService_Resp.xml", SERVER_TIME_OUT);

        int ret_http_code = UrlPingCheck(cUrlName);

        if(ret_http_code == 1)
        {
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/UpdateInsertService.xml", cUrlName,  "/mnt/jffs2/UpdateInsertService_Resp.xml", responseTimedOutTimeInSec.toInt());
        }
        else {
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/UpdateInsertService.xml", PDS_URL2,  "/mnt/jffs2/UpdateInsertService_Resp.xml", responseTimedOutTimeInSec.toInt());
        }

    }
    qDebug()<<"\nCurl Cmd : "<<cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        CheckOfflineTxnUpload();
        return -1;
    }

    qDebug() << "**********UpdateInsertService_Resp.xml*********";
    system("cat /mnt/jffs2/UpdateInsertService_Resp.xml");
    qDebug() << "***********************************************";

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    if(ParsingFirstCondition((char *)"/mnt/jffs2/UpdateInsertService_Resp.xml") < 0)
        return -1;

    parseCardsInfoDetails("/mnt/jffs2/UpdateInsertService_Resp.xml");

    return 0;
}

int Widget::parseCardsInfoDetails(char *filename)
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
    list = document.elementsByTagName("return");
    AvailedCards         = list.at(0).childNodes().at(0).toElement().text();
    qDebug()<<"@@@@@@@@@@@ Availed Cards : "<<AvailedCards;
    TotalCards         = list.at(0).childNodes().at(3).toElement().text();
    if(iMainTransaction == PDS_TRANSACTION)
        receiptid=list.at(0).childNodes().at(4).toElement().text();
    else if(iMainTransaction == MDM_TRANSACTION)
        receiptid=list.at(0).childNodes().at(0).toElement().text();

    qDebug()<<"Availed Cards : "<<AvailedCards;
    qDebug()<<"Total Cards   : "<<TotalCards;
    qDebug()<<"receiptid     : "<<receiptid;
}

int Widget::prepareInsertUpdateCommFile()
{
    int i =0, iCount = 0 ;
    char RcId[36], cSchemeID[8];

    QSqlQuery query;
    query.prepare("select RcId, schemeId from CustomerMaster where RationCardId=?");
    query.addBindValue(Rcno.toAscii().data());
    query.exec();

    qDebug()<<"Ration Card Id  : "<<RationCardId;
    qDebug()<<"ldt Ration Card : "<<ui->ldtRationCard->text().toAscii().data();

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0)
    {
        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }
    while(query.next())
    {
        memset(RcId,0,sizeof(RcId));
        strcpy(RcId, query.value(0).toString().toAscii().data());
        qDebug()<<"RcId = "<<RcId;

        memset(cSchemeID, 0, sizeof(cSchemeID));
        strcpy(cSchemeID, query.value(1).toString().toAscii().data());
    }

    QString QShop_no;
    query.clear();
    query.prepare("SELECT ShopID FROM Shopownerdetails");
    if(!query.exec())
        qDebug() << "Unable to get shopid = " << query.lastError();
    else
    {
        while(query.next())
        {
            QShop_no = query.value(0).toString();
        }
    }
    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));

    miscObj.GetMachineId(cmachineID);

    system("rm /mnt/jffs2/UpdateInsertService.xml");
    FILE *fp;
    fp = fopen("/mnt/jffs2/UpdateInsertService.xml","w");
    if(fp == NULL)
    {
        qDebug() << "File Not Found" << endl;
        return -1;
    }

    QString QCardType;
    if((ui->label_type_card->text() == "NFSA-AAY") || (ui->label_type_card->text() == "NFSA-PHH"))
    {
        QCardType = "N";
    }
    else if(ui->label_type_card->text() == "WHITE")
    {
        QCardType = "S";
    }

    ui->twdgtEntitlement->item(ui->twdgtEntitlement->currentRow(),0)->text().toAscii().data();
    ui->twdgtEntitlement->item(ui->twdgtEntitlement->currentRow(),6)->text();

    if(iMainTransaction == PDS_TRANSACTION)
    {
        fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\">");
        fprintf(fp, "<%s>", "SOAP-ENV:Body");
        fprintf(fp, "<%s>", "ns1:getCommodityTransaction");
        fprintf(fp, "<fpsSessionId>%s</fpsSessionId>", SessionId);
        fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
        fprintf(fp, "<exRCNumber>%s</exRCNumber>",Rcno.toAscii().data());
        fprintf(fp, "<shop_no>%s</shop_no>", QShop_no.toAscii().data());
        fprintf(fp, "<deviceId>%s</deviceId>", cmachineID);
        fprintf(fp, "<rationCardId>%s</rationCardId>", RcId);
        fprintf(fp, "<schemeId>%s</schemeId>", cSchemeID);

        iCount = ui->tblCommIssue->rowCount();

        for(i = 0; i < iCount; i++)
        {

            char buf1[128], buf2[128];
            memset(buf1, 0, sizeof(buf1));
            memset(buf2, 0, sizeof(buf2));

            if((ui->tblCommIssue->item(i,3)->text().toInt(0) > 0) || (ui->tblCommIssue->item(i,3)->text().toFloat(0) > 0.00))
            {
                fprintf(fp, "<%s>", "commodityDetail");

                QStringList list;
                list = ui->tblCommIssue->item(i,0)->text().split('(', QString::SkipEmptyParts);

                strcpy(buf1, list.at(0).trimmed().toAscii().data());


                query.clear();
                QString itemname;

                query.prepare("SELECT ItemNameEn FROM ItemMaster WHERE ItemCode=?");
                query.addBindValue(ui->tblCommIssue->item(i,5)->text().toAscii().data());

                if(!query.exec())
                    qDebug() << "Unable to get ItemNameEn = " << query.lastError();
                else
                {
                    while(query.next())
                    {
                        itemname = query.value(0).toString();
                    }
                }

                fprintf(fp, "<commCode>%s</commCode>", ui->tblCommIssue->item(i,5)->text().toAscii().data());
                if(LangMode == 1)
                    fprintf(fp, "<commName>%s</commName>", buf1);
                if(LangMode == 2)
                {
                    fprintf(fp, "<commName>%s</commName>",itemname.toAscii().data());
                }
                fprintf(fp, "<requiredQuantity>%s</requiredQuantity>", ui->tblCommIssue->item(i,3)->text().toAscii().data());

                fprintf(fp, "<commodityAmount>%s</commodityAmount>", ui->tblCommIssue->item(i,4)->text().toAscii().data());

                fprintf(fp, "<price>%s</price>", ui->tblCommIssue->item(i,2)->text().toAscii().data());

                fprintf(fp, "<allocationType>%s</allocationType>", ui->tblCommIssue->item(i,11)->text().toAscii().data());
                fprintf(fp, "<allotedMonth>%s</allotedMonth>", ui->tblCommIssue->item(i,12)->text().toAscii().data());
                fprintf(fp, "<allotedYear>%s</allotedYear>", ui->tblCommIssue->item(i,13)->text().toAscii().data());


                fprintf(fp, "</%s>", "commodityDetail");

            }
        }

        fprintf(fp, "<recieptId>%s</recieptId>", TxnId);    //newly
        fprintf(fp, "<totAmount>%s</totAmount>", ui->lblTotalAmtVal_2->text().toAscii().data());
        fprintf(fp, "<uid_no>%s</uid_no>", MemberUID);
        fprintf(fp, "<uid_ref_no>%s</uid_ref_no>", gauth_transaction_code);

        fprintf(fp, "<card_type>%s</card_type>", QCardType.toAscii().data());
        fprintf(fp, "<password>%s</password>", HD_PASSWORD);
        fprintf(fp, "<memberId>%s</memberId>", ui->twdgtEntitlement->item(ui->twdgtEntitlement->currentRow(),6)->text().toAscii().data()); //HD_HTS
        if(SurveyStsFlag.toUpper() =="Y")
            fprintf(fp,"<surveyEntryQuantity>%s</surveyEntryQuantity><surveyStatus>%s</surveyStatus>",(GetSurveyWt()).toAscii().data(),SurveyStsFlag.toAscii().data());
        else
        {

            fprintf(fp,"<surveyEntryQuantity>0.0</surveyEntryQuantity><surveyStatus>%s</surveyStatus>",SurveyStsFlag.toAscii().data());
        }

        //finger=F,Manual=M,OTP=O,IRIS=I

        fprintf(fp,"<trans_type>%s</trans_type>",getcomTxn_type.toAscii().data());
        fprintf(fp,"<availedBenfName>%s</availedBenfName>", ui->twdgtEntitlement->item(ui->twdgtEntitlement->currentRow(),0)->text().toAscii().data());

        fprintf(fp, "</%s>", "ns1:getCommodityTransaction");
        fprintf(fp, "</%s>", "SOAP-ENV:Body");
        fprintf(fp, "</%s>", "SOAP-ENV:Envelope");
    }
    else if(iMainTransaction == MDM_TRANSACTION)
    {
        char icds_mem_id[16];

        query.clear();
        query.prepare("SELECT member_id FROM ICDSCustomerFamily WHERE member_uid=?");
        query.addBindValue(MemberUID);
        if(!query.exec())
            qDebug() << query.lastError();
        else
        {
            while(query.next())
            {
                memset(icds_mem_id, 0, sizeof(icds_mem_id));
                strcpy(icds_mem_id, query.value(0).toString().toAscii().data());
            }
        }

        fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\">");
        fprintf(fp, "<%s>", "SOAP-ENV:Body");
        fprintf(fp, "<%s>", "ns1:getSchoolTransaction");
        fprintf(fp, "<fpsSessionId>%s</fpsSessionId>", SessionId);
        fprintf(fp, "<schoolId>%s</schoolId>", ui->lineEditAnganwadiCardNo->text().toAscii().data());
        fprintf(fp, "<memberId>%s</memberId>", icds_mem_id);
        fprintf(fp, "<fpsId>%s</fpsId>", QShop_no.toAscii().data());
        fprintf(fp, "<deviceId>%s</deviceId>", cmachineID);

        iCount = ui->tblCommIssue->rowCount();

        for(i = 0; i < iCount; i++)
        {
            char buf1[128], buf2[128];

            memset(buf1, 0, sizeof(buf1));
            memset(buf2, 0, sizeof(buf2));

            if((ui->tblCommIssue->item(i,3)->text().toInt(0) > 0) || (ui->tblCommIssue->item(i,3)->text().toFloat(0) > 0.00))
            {
                fprintf(fp, "<commodityDetail>");
                QStringList list;
                list = ui->tblCommIssue->item(i,0)->text().split('(', QString::SkipEmptyParts);

                strcpy(buf1, list.at(0).trimmed().toAscii().data());

                fprintf(fp, "<commCode>%s</commCode>", ui->tblCommIssue->item(i,5)->text().toAscii().data());
                fprintf(fp, "<commName>%s</commName>", buf1);
                fprintf(fp, "<requiredQuantity>%s</requiredQuantity>", ui->tblCommIssue->item(i,3)->text().toAscii().data());
                fprintf(fp, "</commodityDetail>");
            }
        }


        fprintf(fp, "<totAmount>%s</totAmount>", ui->lblTotalAmtVal->text().toAscii().data()); //newly
        fprintf(fp, "<uidNo>%s</uidNo>", MemberUID);
        fprintf(fp, "<uidRefNo>%s</uidRefNo>", gauth_transaction_code);
        fprintf(fp, "<password>%s</password>", HD_PASSWORD);//"11169d3abc20724bb857cc5ea59e3c42" );
        fprintf(fp, "</%s>", "ns1:getSchoolTransaction");
        fprintf(fp, "</%s>", "SOAP-ENV:Body");
        fprintf(fp, "</%s>", "SOAP-ENV:Envelope");
    }

    fclose(fp);

    qDebug() << "******** /mnt/jffs2/UpdateInsertService.xml *******";
    system("cat /mnt/jffs2/UpdateInsertService.xml");
    qDebug() << "***************************************************";

    return 0;
}

void Widget::on_btnGetWt_clicked()
{
    qDebug()<<"Communication Mode : "<<iCommMode;

    if(ui->comboCommMode->currentIndex() == 0)
    {
        qDebug()<<"Select Comm Mode";
        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"Please Select Comm Mode");
        else
            miscObj.tel_DisplayMsgBox((char *)"कृपया कम्युनिकेशन मोड चुनें");
        return;
    }

    qDebug()<<"Mode Selected";
    int iRowCount = 0;
    int i = 0;

    iRowCount = ui->tblCommIssue->rowCount();

    qDebug()<<"No Of Row in tblCommIssue = "<<iRowCount;

    for(i = 0; i < iRowCount; i++)
    {
        qDebug()<< i;
        if(ui->tblCommIssue->item(i,0)->isSelected() == true)
        {
            break;
        }
    }
    qDebug()<< i <<"<=====>"<<iRowCount;
    if(i == iRowCount)
    {
        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"Please Select Commodity");
        else
            miscObj.tel_DisplayMsgBox((char *)"कृपया वस्तु का चयन करे");
        return;
    }

    if(iCommMode == 1)
    {
        char bl_weight[32];

        if(getweight(buff_btId, bl_weight) < 0)
        {
            qDebug() << "Getting Bluetooth Weight Failed";
            system("rfcomm release 0");
            sleep(1);

            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Please Select Other Mode of Communication");
            else
                miscObj.tel_DisplayWideMsgBox((char *)"कृपया कम्युनिकेशन के अन्य मोड का चयन करें");

            ui->comboCommMode->setCurrentIndex(0);

            return;
        }

        modem objM;
        char BUFF[16];
        memset(BUFF, 0, sizeof(BUFF));
        int ret = objM.Parse(bl_weight, BUFF);
        if(ret < 0)
        {
            qDebug() << "Parsing Failed";
            if(ret == -2)
                miscObj.DisplayWideMsgBox((char *)"Invalid Weight...");

            return;
        }
        qDebug() << "BUFF = " << BUFF;
        insertWt(BUFF);

    }/*
    else if(iCommMode == 2)
    {
        qDebug()<<"Getting Weignt through RS232";
        OprRS232();
    }*/
    else if(iCommMode == 2)
    {
        qDebug()<<"Getting Weignt through USB";
        OprUSB3();
    }

    return;
}


void Widget::OprRS232()
{
    int i = 0;
    int iRowCount = 0;

    iRowCount = ui->tblCommIssue->rowCount();

    qDebug()<<"No Of Row in tblCommIssue = "<<iRowCount;

    for(i = 0; i < iRowCount; i++)
    {
        qDebug() << i << "####";
        if(ui->tblCommIssue->item(i,0)->isSelected() == true)
        {
            break;
        }
    }
    qDebug()<< i <<"<=====>"<<iRowCount;

    if(i == iRowCount)
    {
        qDebug()<<"Please Select Commodity";
        miscObj.AudioTest((char *)"51");

        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"Please Select Commodity");
        else
            miscObj.tel_DisplayMsgBox((char *)"कृपया वस्तु का चयन करे");
        return;
    }

    char weight[16];
    memset(weight, 0, sizeof(weight));

    modem objModem;
    char Wt_buff[16];
    memset(Wt_buff, 0, sizeof(Wt_buff));

    data_weight("/dev/ttymxc0", Wt_buff);
    qDebug() << "Weight = " << Wt_buff;

    int ret = objModem.Parse(Wt_buff, weight);
    qDebug() << "Weight1 = " << weight;
    if(ret == -2)
        return;

    ui->ldtIssueQty->setText(weight);

    char buff[8];
    memset(buff, 0, sizeof(buff));
    strcpy(buff, ui->ldtIssueQty->text().toAscii().data());
    int iCurrentRow = ui->tblCommIssue->currentRow();
    ui->tblCommIssue->setItem(iCurrentRow, 3, new QTableWidgetItem(buff));

    char price[8];
    memset(price, 0, sizeof(price));
    strcpy(price , ui->tblCommIssue->item(iCurrentRow, 2)->text().toAscii().data());

    char Amt[8];
    memset(Amt, 0, sizeof(Amt));
    sprintf(Amt, "%.2f", atof(buff) * atof(price));
    qDebug()<<"AMT = "<<Amt;


    if(((strcmp(ui->tblCommIssue->item(iCurrentRow,3)->text().toAscii().constData(), "0" ) == 0) || (strcmp(ui->tblCommIssue->item(iCurrentRow,3)->text().toAscii().constData(), "0.00" ) == 0)))
    {
        qDebug() << "Amount should be Zero!";
        memset(Amt, 0, sizeof(Amt));
        strcpy(Amt, "0");
    }

    ui->tblCommIssue->setItem(iCurrentRow, 4, new QTableWidgetItem(Amt));

    QSqlQuery query;
    query.prepare("SELECT ItemCode from BenQuotaTable");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        int row = 0;
        while(query.next())
        {
            qDebug() << "*****row = " << row;
            switch(query.value(0).toInt(0))
            {
            case 1:
            case 2:
            case 3:
            case 5:
            case 7:
            case 10:
            case 11:
            case 12:
            case 13:
                ui->tblCommIssue->item(row, 3)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                break;
            default:
                break;
            }

            row++;
        }
    }

    return;
}


void Widget::OprUSB3()
{
    int iRowCount = 0;

    iRowCount = ui->tblCommIssue->rowCount();
    int i = 0;
    qDebug()<<"No Of Row in tblCommIssue = "<<iRowCount;
    for(i = 0; i < iRowCount; i++)
    {
        if(ui->tblCommIssue->item(i,0)->isSelected() == true)
        {
            break;
        }
    }
    qDebug()<< i <<"<=====>"<<iRowCount;
    if(i == iRowCount)
    {
        qDebug()<<"Please Select Commodity";

        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"Please Select Commodity");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया वस्तु का चयन करे");

        return;
    }
    system("rmmod /vision/cp210x.ko");
    system("rmmod /vision/ftdi_sio.ko");

    system("insmod /vision/usbserial.ko");  //added By Praveen
    system("insmod /vision/cp210x.ko");
    system("dmesg");


    char weight[16];
    memset(weight, 0, sizeof(weight));

    modem objModem;
    char usb_node[16];
    memset(usb_node, 0, sizeof(usb_node));
    int ret = usb_node_check(usb_node); //Calon
    if(ret < 0)
    {
        ret = 0;
        system("rmmod /vision/cp210x.ko");
        sleep(1);
        system("insmod /vision/ftdi_sio.ko");
        sleep(1);

        memset(usb_node, 0, sizeof(usb_node));
        ret = usb_node_check_Essae(usb_node);   //Essae
        if(ret < 0)
        {
            qDebug() << "Getting USB Port Failed";
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"USB Port Opening Failed...\nPlease Select Other Mode of Communication...");
            else
                miscObj.tel_DisplayWideMsgBox((char *)"यूएसबी पोर्ट ओपनिंग विफल...\nकृपया कम्युनिकेशन के अन्य मोड का चयन करें");
            return;
        }

    }

    char Wt_buff[16];
    memset(Wt_buff, 0, sizeof(Wt_buff));

    data_weight(usb_node, Wt_buff);
    qDebug() << "Weight = " << Wt_buff;

    ret = objModem.Parse(Wt_buff, weight);
    qDebug() << "Weight1 = " << weight;
    if(ret == -2)
        return;


    system("rmmod /home/cp210x.ko");


    ui->ldtIssueQty->setText(weight);


    char buff[8];
    memset(buff, 0, sizeof(buff));
    strcpy(buff, ui->ldtIssueQty->text().toAscii().data());
    int iCurrentRow = ui->tblCommIssue->currentRow();
    ui->tblCommIssue->setItem(iCurrentRow, 3, new QTableWidgetItem(buff));


    char price[8];
    memset(price, 0, sizeof(price));
    strcpy(price , ui->tblCommIssue->item(iCurrentRow, 2)->text().toAscii().data());

    char Amt[8];
    memset(Amt, 0, sizeof(Amt));
    sprintf(Amt, "%.2f", atof(buff) * atof(price));
    qDebug()<<"AMT = "<<Amt;

    if(((strcmp(ui->tblCommIssue->item(iCurrentRow,3)->text().toAscii().constData(), "0" ) == 0) || (strcmp(ui->tblCommIssue->item(iCurrentRow,3)->text().toAscii().constData(), "0.00" ) == 0)))
    {
        qDebug() << "Amount should be Zero!";
        memset(Amt, 0, sizeof(Amt));
        strcpy(Amt, "0");
    }

    ui->tblCommIssue->setItem(iCurrentRow, 4, new QTableWidgetItem(Amt));
    ui->tblCommIssue->item(iCurrentRow, 3)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    //    QSqlQuery query;
    //    query.prepare("SELECT ItemCode from BenQuotaTable");
    //    if(!query.exec())
    //        qDebug() << query.lastError();
    //    else
    //    {
    //        int row = 0;
    //        while(query.next())
    //        {
    //            switch(query.value(0).toInt(0))
    //            {
    //            case 1:
    //            case 2:
    //            case 3:
    //            case 5:
    //            case 7:
    //            case 10:
    //            case 11:
    //            case 12:
    //            case 13:
    //                ui->tblCommIssue->item(row, 3)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    //                break;
    //            default:
    //                break;
    //            }
    //            row++;
    //        }
    //    }
    return;
}

void Widget::insertWt(char *weight)
{
    qDebug() << "iN insertWt(char *weight)";
    ui->ldtIssueQty->setText(weight);

    char buff[16];
    memset(buff, 0, sizeof(buff));
    strcpy(buff, ui->ldtIssueQty->text().toAscii().data());
    int iCurrentRow = ui->tblCommIssue->currentRow();
    ui->tblCommIssue->setItem(iCurrentRow, 3, new QTableWidgetItem(buff));

    char price[8];
    memset(price, 0, sizeof(price));
    strcpy(price , ui->tblCommIssue->item(iCurrentRow, 2)->text().toAscii().data());

    char Amt[8];
    memset(Amt, 0, sizeof(Amt));
    sprintf(Amt, "%.2f", atof(buff) * atof(price));
    qDebug()<<"AMT = "<<Amt;


    if(((strcmp(ui->tblCommIssue->item(iCurrentRow,3)->text().toAscii().constData(), "0" ) == 0) || (strcmp(ui->tblCommIssue->item(iCurrentRow,3)->text().toAscii().constData(), "0.00" ) == 0)))
    {
        qDebug() << "Amount should be Zero!";
        memset(Amt, 0, sizeof(Amt));
        strcpy(Amt, "0");
    }

    ui->tblCommIssue->setItem(iCurrentRow, 4, new QTableWidgetItem(Amt));
    ui->tblCommIssue->item(iCurrentRow, 3)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    //    QSqlQuery query;
    //    query.prepare("SELECT ItemCode from BenQuotaTable");
    //    if(!query.exec())
    //        qDebug() << query.lastError();
    //    else
    //    {
    //        int row = 0;
    //        while(query.next())
    //        {
    //            switch(query.value(0).toInt(0))
    //            {
    //            case 1:
    //            case 2:
    //            case 3:
    //            case 5:
    //            case 7:
    //            case 10:
    //            case 11:
    //            case 12:
    //            case 13:
    //                ui->tblCommIssue->item(row, 3)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    //                break;
    //            default:
    //                break;
    //            }
    //            row++;
    //        }
    //    }
    return;
}

/************************ added by kvcs *****************************/

int Widget::GetCustomerDetails()
{
    QSqlQuery query;

    query.prepare("select RationCardId, CustomerName, LpgGasStatus, LpgType, CardType, MemberCount, OfficeName, RcId from CustomerMaster where RationCardId=?");
    query.addBindValue(RationCardId);
    query.exec();

    while(query.next())
    {
        memset(CustomerName,0,sizeof(CustomerName));
        strcpy(CustomerName, query.value(1).toString().toAscii().data());
        qDebug()<<"CustomerName = "<<CustomerName;

        memset(MemberCount,0,sizeof(MemberCount));
        strcpy(MemberCount, query.value(5).toString().toAscii().data());
        qDebug()<<"MemberCount = "<<MemberCount;

    }
    return 0;
}


void Widget::GetRTC_Params(char *RTC, char *DATE, char *TIME, char *JULIAN_DAYS)//IND
{
    struct tm *tt;
    time_t  t;
    time(&t);
    tt=localtime(&t);

    sprintf(RTC,"%02d%02d%04d%02d%02d%02d",tt->tm_mday,(tt->tm_mon+1),(tt->tm_year+1900), tt->tm_hour, tt->tm_min, tt->tm_sec);
    qDebug()<<"RTC = "<<RTC;

    sprintf(DATE, "%02d%02d%04d", tt->tm_mday,tt->tm_mon+1, tt->tm_year+1900);
    qDebug()<<"DATE = "<<DATE;

    sprintf(JULIAN_DAYS, "%03d", tt->tm_yday);
    qDebug()<<"JULIAN_DAYS = "<<JULIAN_DAYS;

    sprintf(TIME, "%02d%02d%02d", tt->tm_hour, tt->tm_min, tt->tm_sec);
    qDebug()<<"TIME = "<<TIME;


}

void Widget::on_ldtIssueQty_editingFinished()
{

}

int Widget::parseList()
{
    return 0;
}


void Widget::on_btnBackSettings_clicked()
{
    ui->stackedWidget->setCurrentIndex(18);
}

void Widget::on_comboCommMode_activated(int index)
{
    qDebug()<<"Current Index = "<<index;

    switch(index)
    {
    case 0:
        qDebug()<<"Please Select Comm Mode to get Data";
        break;
    case 1:
        qDebug() << "Bluetooth Selected...";
        InitialiseBluetooth();
        iCommMode = 1;
        break;
    case 2:
        qDebug() << "RS232 Selected ...";
        iCommMode = 2;
        break;
    case 3:
        qDebug() << "USB Selected ...";
        iCommMode = 3;
        break;
    default:
        iCommMode = 0;
        break;
    }
    char szCommMode[4];
    memset(szCommMode, 0, sizeof(szCommMode));
    sprintf(szCommMode, "%d", iCommMode);
    QSqlQuery query;
    query.prepare("UPDATE Settings SET CommMode=?");
    query.addBindValue(szCommMode);
    if(!query.exec()){
        qDebug() << query.lastError();
    }
}

void Widget::on_comboLangSel_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        qDebug()<<"Lang Mode = Eng(1)";
        ChangeUI_to_Eng();
        LangMode = 1;
        setLangMode(1);
        ui->comboLangSel->setCurrentIndex(0);
        break;
    case 1:
        LangMode = 2;
        qDebug()<<"Lang Mode = HINDI(2)";
        ChangeUI_to_HINDI();

        setLangMode(2);
        ui->comboLangSel->setCurrentIndex(1);
        break;
    default:
        LangMode = 1;
        setLangMode(1);
        break;
    }
}


void Widget::ChangeUI_to_HINDI()
{
    /***************** PAGE-0 *********************/
    if(stateCode.toInt() == 1) //Uttarakhand
    {
        ui->lblHome_3->setText(QString::fromUtf8("जम्मू और कश्मीर पी डी एस"));
    }
    else if(stateCode.toInt() == 5) //Uttarakhand
    {
        ui->lblHome_3->setText(QString::fromUtf8("उत्तराखण्ड पी डी एस"));
    }
    else if(stateCode == "10") //Bihar
    {
        ui->lblHome_3->setText(QString::fromUtf8("बिहार पी डी एस"));
    }
    else if(stateCode == "12") //Arunachal Pradesh
    {
        ui->lblHome_3->setText(QString::fromUtf8("अरुणाचल प्रदेश पी डी एस"));
    }
    else if(stateCode == "13") //Nagaland
    {
        ui->lblHome_3->setText(QString::fromUtf8("नगालैंड पी डी एस"));
    }else if(stateCode == "14") //Manipur
    {
        ui->lblHome_3->setText(QString::fromUtf8("मणिपुर पी डी एस"));
    }else if(stateCode == "15") //Mizoram
    {
        ui->lblHome_3->setText(QString::fromUtf8("मिजोरम पी डी एस"));
    }else if(stateCode == "17") //Meghalaya
    {
        ui->lblHome_3->setText(QString::fromUtf8("मेघालय पी डी एस"));
    }
    else if((stateCode == "25") || (stateCode == "26"))
    {
        ui->lblHome_3->setText(QString::fromUtf8("दादरा-नागर हवेली और दमन-दीव पी डी एस"));
    }
    else if(stateCode.toInt() == 31) //Lakshadweep
    {
        ui->lblHome_3->setText(QString::fromUtf8("लक्षद्वीप पी डी एस"));
    }
    else if(stateCode.toInt() == 35) //Andaman & Nicobar
    {
        ui->lblHome_3->setText(QString::fromUtf8("अण्डमान और निकोबार पी डी एस"));
    }
    else if(stateCode.toInt() == 37) //Ladakh
    {
        ui->lblHome_3->setText(QString::fromUtf8("लेह - लद्दाख पी डी एस"));
    }

    ui->ldtGetOwnDetails->setText(QString::fromUtf8("प्रारंभ"));
    ui->lblShopId->setText(QString::fromUtf8("दुकान नंबर"));
    ui->btnQuitApp->setText(QString::fromUtf8("निकास"));
    ui->btnSettings_2->setText(QString::fromUtf8("सेटिंग्स"));
    //ui->btnpoweroff->setText(QString::fromUtf8("बिजली बंद"));
    ui->lblHelpDesk->setText(QString::fromUtf8("टोल फ्री नंबर"));

    /**********************************************/

    /***************** PAGE-1 *********************/
    ui->lblHome->setText(QString::fromUtf8("होम"));
    ui->btnAuthenticate->setText(QString::fromUtf8("विक्रय"));
    ui->btnReceiveGoods->setText(QString::fromUtf8("ऑनलाइन स्टॉक\nप्राप्ति"));
    ui->OfflineReceiveGoodsButton->setText(QString::fromUtf8("ऑफ़लाइन स्टॉक\nप्राप्ति"));
    ui->btnReports->setText(QString::fromUtf8("रिपोर्ट"));
    ui->btnUpdateSW->setText(QString::fromUtf8("सॉफ्टवेर अपडेट"));
    ui->btnAadhaarServices->setText(QString::fromUtf8("आधार सेवाएं"));
    ui->btn_inspector->setText(QString::fromUtf8("इंस्पेक्शन"));
    ui->UploadDataButton->setText(QString::fromUtf8("डेटा अपलोड करें"));

    /**********************************************/

    /***************** PAGE-2 *********************/
    ui->lblH_Reports->setText(QString::fromUtf8("रिपोर्ट"));
    ui->btnDailySales->setText(QString::fromUtf8("प्रतिदिन रिपोर्ट"));
    ui->btnStockRegister->setText(QString::fromUtf8("स्टॉक रिपोर्ट"));

    //    ui->btnCashLessEOD->setText("JAN-DHAN\nEJ UPLOAD");
    //ui->btnCashLessEOD->setText("CASH LESS\nEJ UPLOAD");

    ui->btnGotoHome->setText(QString::fromUtf8("होम"));
    // ui->btnJanDhanDayReport->setText("CASHLESS\nDAY REPORT");
    /***********************************************/

    /***************** PAGE-3 *********************/
    /**********************************************/

    /***************** PAGE-4 *********************/
    ui->lblHome_2->setText(QString::fromUtf8("नकद पीडीएस"));
    ui->btnImpds->setText(QString::fromUtf8("आई एम पी डी एस"));
    ui->lblTypeOfCard->setText(QString::fromUtf8("कार्ड टाइप"));
    ui->lblRationCard->setText(QString::fromUtf8("कार्ड नंबर"));
    ui->btnBackEntitle->setText(QString::fromUtf8("होम"));
    ui->btnOK_Entitle->setText(QString::fromUtf8("सदस्य विवरण"));
    ui->BtnLastReciept->setText(QString::fromUtf8("पिछली रसीद"));
    /**********************************************/

    /***************** PAGE-5 *********************/
    ui->lblRCNO->setText(QString::fromUtf8("कार्ड नंबर"));
    ui->pushButton_2->setText(QString::fromUtf8("अंगुली की छाप \n लगाए"));
    ui->BtnBenIRIS->setText(QString::fromUtf8("आँख\nस्कैन करें"));
    ui->pushButton_4->setText(QString::fromUtf8("पीछे"));
    ui->BtnConsentForm_2->setText(QString::fromUtf8("सहमति पत्र"));
    //    ui->BtnConsentForm_3->setText(QString::fromUtf8("सहमति पत्र"));
    ui->BtnConsentForm_4->setText(QString::fromUtf8("सहमति पत्र"));
    ui->BtnConsentForm_5->setText(QString::fromUtf8("सहमति पत्र"));
    ui->BtnConsentForm_6->setText(QString::fromUtf8("सहमति पत्र"));
    ui->BtnConsentForm_7->setText(QString::fromUtf8("सहमति पत्र"));
    ui->BtnConsentForm_8->setText(QString::fromUtf8("सहमति पत्र"));
    /**********************************************/

    /***************** PAGE-6 *********************/
    //    ui->label_9->setText(QString::fromUtf8("वस्तु"));
    ui->label_9->setText(QString::fromUtf8("कार्ड नंबर"));
    ui->lblTotalAmt->setText(QString::fromUtf8("कुल कीमत"));
    ui->btnConfirmCommodity->setText(QString::fromUtf8("पुष्टि करें"));
    ui->btnCancelCommodity->setText(QString::fromUtf8("पीछे"));
    ui->btnGetWt->setText(QString::fromUtf8("वजन करे"));
    /**********************************************/

    /***************** PAGE-7 *********************/
    ui->label_4->setText(QString::fromUtf8("कार्ड नंबर"));
    ui->label_6->setText(QString::fromUtf8("   वस्तु"));
    ui->label_43->setText(QString::fromUtf8("पिछला बैलेंस"));
    ui->label_7->setText(QString::fromUtf8("आवंटित मात्रा"));
    ui->label_8->setText(QString::fromUtf8("मूल्य"));
    ui->pushButton_5->setText(QString::fromUtf8("पीछे"));
    ui->pushButton_6->setText(QString::fromUtf8("प्रिंट"));
    /**********************************************/

    /***************** PAGE-9 *********************/
    ui->lblSettings->setText(QString::fromUtf8("सेटिंग्स"));
    ui->lblLangSel->setText(QString::fromUtf8("भाषा"));
    ui->label_10->setText(QString::fromUtf8("मशीन आईडी"));
    ui->label_144->setText(QString::fromUtf8("RD संस्करण"));
    ui->btnBackSettings->setText(QString::fromUtf8("पीछे"));

    /***********************************************/

    /***************** PAGE-10 *********************/

    ui->lblEkycCardNo->setText(QString::fromUtf8("कार्ड नंबर"));
    ui->btnBack_EKYC->setText(QString::fromUtf8("पीछे"));
    ui->btnOK_EKYC->setText(QString::fromUtf8("ठीक"));

    /***********************************************/

    /***************** PAGE-11 *********************/
    ui->lblSettings_2->setText(QString::fromUtf8("रिपोर्ट"));
    ui->lblPopupDate->setText(QString::fromUtf8("तारीख"));
    ui->btnBackDt->setText(QString::fromUtf8("पीछे"));
    ui->pushButton_7->setText(QString::fromUtf8("होम"));
    ui->btnOkDt->setText(QString::fromUtf8("प्रिंट"));
    ui->Partial_DaDyRptBtn->setText(QString::fromUtf8("ऑफ़लाइन\nप्रतिदिन रिपोर्ट"));
    ui->PartialCurrStkRptBtn->setText(QString::fromUtf8("ऑफ़लाइन\nस्टॉक रिपोर्ट"));
    /***********************************************/

    /***************** PAGE-15 *********************/

    /***********************************************/

    /***************** PAGE-16 *********************/

    /***********************************************/

    /***************** PAGE-17 *********************/
    ui->label_21->setText(QString::fromUtf8("अपडेट सॉफ्टवेर "));
    ui->BtnUSBUpdateApp->setText(QString::fromUtf8("पेन ड्राइव"));
    ui->BtnGPRSUpdateApp->setText(QString::fromUtf8("जीपीआरएस"));
    ui->BtnBack_4->setText(QString::fromUtf8("पीछे"));
    /***********************************************/

    /***************** PAGE-18 *********************/
    ui->label_24->setText(QString::fromUtf8("सेटिंग्स"));
    ui->BtnDeviceSettings->setText(QString::fromUtf8("मशीन"));
    ui->BtnBack_5->setText(QString::fromUtf8("पीछे"));
    /***********************************************/

    /***************** PAGE-19 *********************/
    /***********************************************/

    /***************** PAGE-20 *********************/
    ui->btnBack->setText(QString::fromUtf8("पीछे"));
    ui->btnLogin->setText(QString::fromUtf8("आँख स्कैन करें"));
    ui->BtnDealerScanFP->setText(QString::fromUtf8("स्कैन उंगली"));
    ui->BtnConsentForm->setText(QString::fromUtf8("सहमति पत्र"));
    ui->label_147->setText(QString::fromUtf8("एफपीएस आईडी"));
    /***********************************************/

    /***************** PAGE-21 *********************/
    ui->label_45->setText(QString::fromUtf8("सहमति पत्र"));
    ui->BtnOK_2->setText(QString::fromUtf8("सहमत"));
    ui->BtnDisAgree->setText(QString::fromUtf8("असहमत"));
    ui->BtnDealerScanFP->setText(QString::fromUtf8("स्कैन उंगली"));
    /***********************************************/


    /***************** PAGE-22 *********************/
    ui->label_55->setText(QString::fromUtf8("र ओ नंबर"));
    ui->label_50->setText(QString::fromUtf8("स्टॉक प्राप्त"));
    ui->label_56->setText(QString::fromUtf8("ट्रक नंबर"));
    ui->BtnBack_7->setText(QString::fromUtf8("पीछे"));
    ui->BtnNext_3->setText(QString::fromUtf8("ठीक"));
    ui->BtnPreview->setText(QString::fromUtf8("प्रीव्यू"));
    /***********************************************/

    /***************** PAGE-23 *********************/
    ui->label_52->setText(QString::fromUtf8("प्रमाणीकरण"));
    ui->BtnDealerRecGoods->setText(QString::fromUtf8("2. मालिक"));
    ui->BtnRouteOfficer->setText(QString::fromUtf8("1. रूट अफसर"));
    ui->BtnBack_8->setText(QString::fromUtf8("पीछे"));
    ui->BtnScanIris->setText(QString::fromUtf8("आँख स्कैन करें"));
    ui->BtnScanFinger->setText(QString::fromUtf8("अंगुली की छाप \n लगाए"));
    // ui->BtnHawker->setText("3. Hawker");
    /***********************************************/

    /***************** PAGE-24 *********************/
    ui->label_62->setText(QString::fromUtf8("वितरित माल"));
    //ui->label_65->setText(QString::fromUtf8("ट्रक चित नंबर"));
    //ui->label_59->setText(QString::fromUtf8("र ओ नंबर"));
    //ui->label_60->setText(QString::fromUtf8("ट्रक नंबर"));
    //ui->BtnOK_3->setText(QString::fromUtf8("ठीक"));
    /***********************************************/

    /***************** PAGE-25 *********************/
    /***********************************************/

    /***************** PAGE-26 *********************/

    /***********************************************/

    /***************** PAGE-27 *********************/

    /***********************************************/

    /***************** PAGE-28 *********************/
    ui->lblIssueHeading->setText(QString::fromUtf8("वितरित"));
    ui->BtnPDS->setText(QString::fromUtf8("पी डी एस"));
    ui->btnMDM->setText(QString::fromUtf8("एम डी एम"));
    ui->BtnBack_12->setText(QString::fromUtf8("पीछे"));
    /***********************************************/

    /***************** PAGE-29 *********************/

    /***********************************************/

    /***************** PAGE-30 *********************/


    /***********************************************/

    /***************** PAGE-31 *********************/

    /***********************************************/


    /***************** PAGE-32 *********************/
    /***********************************************/

    /***************** PAGE-34 *********************/

    /***********************************************/

    /***************** PAGE-35 *********************/
    ui->label_122->setText(QString::fromUtf8("उपभोक्ता आधार नंबर "));
    ui->label_123->setText(QString::fromUtf8("उपभोक्ता मोबाइल नंबर"));
    ui->BtnBack_18->setText(QString::fromUtf8("पीछे"));
    ui->BtnOTP->setText(QString::fromUtf8("ओ.टी.पी "));
    /***********************************************/

    /***************** PAGE-36 *********************/
    ui->label_127->setText(QString::fromUtf8("ओ.टी.पी"));
    ui->BtnBack_19->setText(QString::fromUtf8("पीछे"));
    ui->BtnSubmit_3->setText(QString::fromUtf8("सबमिट"));
    /***********************************************/

    /***************** PAGE-37 *********************/

    /***********************************************/

    /***************** PAGE-38 *********************/

    /***********************************************/

    /***************** PAGE-39 *********************/

    /**********************************************/

    /***************** PAGE-40 *********************/
    ui->btnEKYC_Update->setText(QString::fromUtf8("आधार सीडिंग"));
    ui->btnMobileUpdate->setText(QString::fromUtf8("मोबाइल नंबर\nअपडेट"));
    ui->lblAadhaarServices->setText(QString::fromUtf8("आधार सेवाएं"));
    ui->btnBackAadhaarServices->setText(trUtf8("पीछे"));
    /**********************************************/

    /***************** PAGE-42 *********************/
    ui->lblHome_4->setText(QString::fromUtf8("आधार सीडिंग"));
    ui->lblEKYC_Name->setText(QString::fromUtf8("नाम"));
    ui->lblEKYC_RCNO->setText(QString::fromUtf8("कार्ड नंबर"));
    ui->lblEKYC_UID_Promt->setText(QString::fromUtf8("आधार नंबर"));
    ui->btnEKYC_FP->setText(QString::fromUtf8("स्कैन उंगली "));
    ui->btnEKYC_Back->setText(QString::fromUtf8("पीछे"));
    /**********************************************/


    /***************** PAGE-44 *********************/
    ui->lbl_inspectorVerification->setText(QString::fromUtf8("निरीक्षण"));
    ui->btn_Submit_InspectorVerify->setText(QString::fromUtf8("आगे जाये"));
    ui->btn_Back_inspectorVerify->setText(QString::fromUtf8("पीछे जाये"));

    /**********************************************/

    /***************** PAGE-45 *********************/
    ui->lbl_InspAuth->setText(QString::fromUtf8("निरीक्षक प्रमाणीकरण"));
    ui->lbl_EnteUIDInspAuth->setText(QString::fromUtf8("यूआईडी दर्ज करें"));
    ui->btnBack_InspAuth->setText(QString::fromUtf8("पीछे जाये"));
    ui->btnScanFpInspAuth->setText(QString::fromUtf8("स्कैन-उंगली"));

    /**********************************************/


    /********Offline Receive Goods***************/
    ui->RecvGoodsHead->setText(QString::fromUtf8("सामान प्राप्त करें"));
    ui->RecvGoodsTruckChitNoLbl->setText(QString::fromUtf8("ट्रक का नंबर   :"));
    ui->RecvGoodsVechNoLbl->setText(QString::fromUtf8("वाहन नंबर  :"));
    ui->RecvGoodsChallanNoLbl->setText(QString::fromUtf8("चालान नंबर  :"));
    ui->RecvGoodsBackBtn->setText(QString::fromUtf8("वापस"));
    ui->RecvGoodsSubmitBtn->setText(QString::fromUtf8("जमा करें"));
    ui->RecvGdsCommEntryHead->setText(QString::fromUtf8("सामान प्राप्त करें"));
    ui->RecvGoodsCommEntBack->setText(QString::fromUtf8("वापस"));
    ui->RecvGoodsCommEntAdd->setText(QString::fromUtf8("+जोड़ना"));
    ui->RecvGoodsCommEntSubmit->setText(QString::fromUtf8("जमा करें"));
    ui->RecvGdsConfirmHead->setText(QString::fromUtf8("सामान प्राप्त करें"));
    ui->RecvGdsConfirmBack->setText(QString::fromUtf8("वापस"));
    ui->RecvGdsConfirmSubmit->setText(QString::fromUtf8("पुष्टि करें"));
    /********Offline Receive Goods***************/

    ui->radioButton_OK_inspcVerify->setText(QString::fromUtf8("हाँ"));
    ui->radioButton_seized_inspVer->setText(QString::fromUtf8("जब्त"));
    ui->radioButton_RCNOFLAG->setText(QString::fromUtf8("कार्ड नंबर"));
    ui->radioButton_UIDFLAG->setText(QString::fromUtf8("आधार नंबर"));
    ui->radioButton_RCNOFLAGEKYC->setText(QString::fromUtf8("कार्ड नंबर"));
    ui->radioButton_UIDFLAGEKYC->setText(QString::fromUtf8("आधार नंबर"));
    ui->lblHome_5->setText(QString::fromUtf8("आधार सीडिंग"));
    ui->label_42->setText(QString::fromUtf8("कुल कीमत"));
    ui->btncurrstkback->setText(QString::fromUtf8("पीछे"));
    ui->btncurrstkprint->setText(QString::fromUtf8("प्रिंट"));
    ui->label_23->setText(QString::fromUtf8("वर्तमान स्टॉक\n रिपोर्ट"));
    ui->btnDtSalesView->setText(QString::fromUtf8("प्रीव्यू"));

    /*********Mobile seeding and OTP**********/

    ui->btnMobile_Seeding->setText(QString::fromUtf8("मोबाइल नंबर \n अपडेट"));
    ui->label_29->setText(QString::fromUtf8("ग्राहक \nकार्ड नंबर"));
    ui->label_51->setText(QString::fromUtf8("नया ग्राहक \n मोबाइल नंबर"));
    ui->btnmobupdateback->setText(QString::fromUtf8("वापस"));
    ui->btnmobupdatenext->setText(QString::fromUtf8("ओ.टी.पी"));
    ui->label_61->setText(QString::fromUtf8("ओ.टी.पी दर्ज करे"));
    ui->leditmobupdatebk->setText(QString::fromUtf8("वापस"));
    ui->label_26->setText(QString::fromUtf8("मोबाइल नंबर अपडेट"));
    ui->label_53->setText(QString::fromUtf8("मोबाइल नंबर अपडेट"));
    ui->leditmobupdatesub->setText(QString::fromUtf8("जमा करें"));
    ui->radioButton_scanbarcode->setText(QString::fromUtf8("स्कैन आरसी \n बारकोड "));
    //    ui->tblSalesDaily->setHorizontalHeaderLabels(QString::fromUtf8("वस्तु;योजना;बिक्री;पीएचएच;कुल").split(";"));
    ui->tblSalesDaily->setHorizontalHeaderLabels(QString::fromUtf8("वस्तु;योजना;बिक्री;").split(";"));
    ui->tableWidgetStockreport->setHorizontalHeaderLabels(QString::fromUtf8("वस्तु;योजना;भण्डार;जारी;सीबी").split(";"));
    ui->tableWidgetDealNomDetails->setHorizontalHeaderLabels(QString::fromUtf8("नाम;डीलर प्रकार;बी1;बी2;बी3;वास्तविक यूआईडी;डीलर प्रकार").split(";"));
    ui->twdgtEntitlement->setHorizontalHeaderLabels(QString::fromUtf8("नाम; सदस्य आईडी;बी1;बी2;बी3").split(";"));
    ui->tblCommIssue->setHorizontalHeaderLabels(QString::fromUtf8("वस्तु(कुल);शेष;दर;जारी;राशि; ;सीबी").split(";"));
    ui->tableWidget_InspectorVerificationn->setHorizontalHeaderLabels(QString::fromUtf8("वस्तु;सीबी;पर्यवेक्षण;परिवर्तन").split(";"));
    ui->tableWidgetStockTable_3->setHorizontalHeaderLabels(QString::fromUtf8("वस्तु;योजना;आवंटन;आरओ;भेजे;प्राप्त;commcode;scheme id").split(";"));
    ui->tableWidgetRecieveGoodsAuth->setHorizontalHeaderLabels(QString::fromUtf8("नाम;यूआईडी;मोबाइल;वास्तविक यूआईडी").split(";"));
    ui->label_70->setText(QString::fromUtf8("लाभार्थी का \nसत्यापन"));
    ui->btn_BenVerify->setText(QString::fromUtf8("लाभार्थी का \nसत्यापन"));
    ui->btn_RCNOFLAGBenverify->setText(QString::fromUtf8("कार्ड नंबर"));
    ui->btn_UIDFLAGBenverify->setText(QString::fromUtf8("आधार"));
    ui->btnBack_BenVerify->setText(QString::fromUtf8("पीठ"));
    ui->btnOK_BenVerify->setText(QString::fromUtf8("ठीक"));
    ui->label_81->setText(QString::fromUtf8("मित्र विवरण"));
    ui->btn_BenVerifyBack_->setText(QString::fromUtf8("पीठ"));
    ui->BenVerifyScanFP->setText(QString::fromUtf8("फिंगरप्रिंट \nस्कैन करें"));
    ui->tableWidgetBenVerify->setHorizontalHeaderLabels(QString::fromUtf8("नाम;सदस्य आईडी;स्थिति").split(";"));
    return;
}

void Widget::ChangeUI_to_Eng()
{
    /***************** PAGE-0 *********************/
    if(stateCode.toInt() == 1) //Jammu & Kashmir
    {
        ui->lblHome_3->setText("Jammu & Kashmir PDS");
    }
    else if(stateCode.toInt() == 5) //Uttarakhand
    {
        ui->lblHome_3->setText("Uttarakhand PDS");
    }
    else if(stateCode == "10") //Bihar PDS
    {
        ui->lblHome_3->setText("BIHAR PDS");
    }
    else if(stateCode == "12") //Arunachal Pradesh
    {
        ui->lblHome_3->setText("Arunachal Pradesh PDS");
    }
    else if(stateCode == "13") //Nagaland
    {
        ui->lblHome_3->setText("Nagaland PDS");
    }
    else if(stateCode == "14") //Manipur
    {
        ui->lblHome_3->setText("Manipur PDS");
    }
    else if(stateCode == "15") //Mizoram
    {
        ui->lblHome_3->setText("Mizoram PDS");
    }
    else if(stateCode == "17") //Meghalaya
    {
        ui->lblHome_3->setText("Meghalaya PDS");
    }
    else if((stateCode == "25") || (stateCode == "26")) //Daman
    {
        ui->lblHome_3->setText("U.T OF DADRA  AND NAGAR HAVELI\nAND DAMAN AND DIU PDS");
    }
    else if(stateCode.toInt() == 31)
    {
        ui->lblHome_3->setText("UT of Lakshadweep PDS");
    }
    else if(stateCode.toInt() == 35)
    {
        ui->lblHome_3->setText("Andaman and Nicobar PDS");
    }
    else if(stateCode.toInt() == 37)
    {
        ui->lblHome_3->setText("Leh-Ladakh PDS");
    }

    ui->ldtGetOwnDetails->setText("START");
    ui->lblShopId->setText("Shop Id");
    ui->btnQuitApp->setText("Quit");
    ui->btnSettings_2->setText("Settings");
    //ui->btnpoweroff->setText("Power off");
    ui->lblHelpDesk->setText("TOLL FREE NUMBER:");
    /**********************************************/

    /***************** PAGE-1 *********************/
    ui->lblHome->setText("HOME");
    ui->btnAuthenticate->setText("Issue");
    ui->btnReceiveGoods->setText("Online Receive\nGoods");
    ui->OfflineReceiveGoodsButton->setText("Offline Receive\nGoods");
    ui->btnReports->setText("Reports");
    ui->btnUpdateSW->setText("Update\nSoftware");
    ui->btnKerosene->setText("Kerosene");
    ui->btnAadhaarServices->setText("Aadhaar\nServices");
    ui->btn_inspector->setText("Inspection");
    ui->UploadDataButton->setText("Upload Data");
    /**********************************************/

    /***************** PAGE-2 *********************/
    ui->lblH_Reports->setText("REPORTS");
    ui->btnDailySales->setText("Daily Sales\nReport");
    ui->btnStockRegister->setText("Stock\nReport");
    ui->btnGotoHome->setText("Home");
    /***********************************************/

    /***************** PAGE-4 *********************/
    ui->lblHome_2->setText("Cash PDS");
    ui->btnImpds->setText("IMPDS");
    ui->lblTypeOfCard->setText("Card Type");
    ui->lblRationCard->setText("Card No.");
    ui->btnBackEntitle->setText("Home");
    ui->btnOK_Entitle->setText("Get \n Details");
    ui->BtnLastReciept->setText("Last Reciept");;
    /**********************************************/

    /***************** PAGE-5 *********************/
    ui->lblRCNO->setText("Card No.");
    ui->pushButton_2->setText("Scan-FP");
    ui->BtnBenIRIS->setText("Scan\nEye");
    ui->pushButton_4->setText("Back");
    ui->BtnConsentForm_2->setText("Consent\nForm");
    //    ui->BtnConsentForm_3->setText("Consent\nForm");
    ui->BtnConsentForm_4->setText("Consent\nForm");
    ui->BtnConsentForm_5->setText("Consent\nForm");
    ui->BtnConsentForm_6->setText("Consent\nForm");
    ui->BtnConsentForm_7->setText("Consent\nForm");
    ui->BtnConsentForm_8->setText("Consent\nForm");
    /**********************************************/

    /***************** PAGE-6 *********************/
    //    ui->label_9->setText("COMMODITIES");
    ui->label_9->setText("Card No. ");
    ui->lblTotalAmt->setText("TOTAL Amt");
    ui->btnConfirmCommodity->setText("Confirm");
    ui->btnCancelCommodity->setText("Back");
    ui->btnGetWt->setText("Get Wt");
    /**********************************************/

    /***************** PAGE-7 *********************/
    ui->label_4->setText("Card No");
    ui->label_6->setText("Comm(Tot)");
    ui->label_43->setText("Prev-Bal");
    ui->label_7->setText("Alloc-Qty");
    ui->label_8->setText("Price");
    ui->pushButton_5->setText("Back");
    ui->pushButton_6->setText("Print");
    /**********************************************/

    /***************** PAGE-9 *********************/
    ui->lblSettings->setText("SETTINGS");
    ui->lblLangSel->setText("Language");
    ui->label_10->setText("Device ID");
    ui->label_144->setText("RD Version");
    ui->btnBackSettings->setText("Back");
    /***********************************************/

    /***************** PAGE-10 *********************/
    ui->lblEkycCardNo->setText("Card No.");
    ui->btnBack_EKYC->setText("Back");
    ui->btnOK_EKYC->setText("Ok");
    /***********************************************/

    /***************** PAGE-11 *********************/
    ui->lblSettings_2->setText("REPORTS");
    ui->lblPopupDate->setText("Date");
    ui->Partial_DaDyRptBtn->setText("Offline Day\nReport");
    ui->PartialCurrStkRptBtn->setText("Offline\nCurrent Stock\nReport");
    ui->btnBackDt->setText("Back");
    ui->btnOkDt->setText("Print");
    ui->pushButton_7->setText("Home");
    /***********************************************/

    /***************** PAGE-15 *********************/

    /***********************************************/

    /***************** PAGE-16 *********************/

    /***********************************************/

    /***************** PAGE-17 *********************/
    ui->label_21->setText("UPDATE SOFTWARE");
    ui->BtnUSBUpdateApp->setText("USB");
    ui->BtnGPRSUpdateApp->setText("GPRS");
    ui->BtnBack_4->setText("Back");
    /***********************************************/

    /***************** PAGE-18 *********************/
    ui->label_24->setText("SETTINGS");
    ui->BtnDeviceSettings->setText("Device");
    ui->BtnBack_5->setText("Back");
    /***********************************************/

    /***************** PAGE-19 *********************/
    ui->label_28->setText("SERVER");
    /***********************************************/

    /***************** PAGE-20 *********************/
    ui->btnBack->setText("Back");
    ui->btnLogin->setText("Scan Eye");
    ui->BtnDealerScanFP->setText("Scan-FP");
    ui->BtnConsentForm->setText("Consent\nForm");
    ui->label_147->setText("FPS ID :");
    /***********************************************/

    /***************** PAGE-21 *********************/
    ui->label_45->setText("Consent \n Form");
    ui->BtnOK_2->setText("Agree");
    ui->BtnDisAgree->setText("Disagree");
    ui->BtnDealerScanFP->setText("Scan-FP");
    /***********************************************/


    /***************** PAGE-22 *********************/
    ui->label_55->setText("RO No:");
    ui->label_50->setText("RECEIVE GOODS");
    ui->label_56->setText("Truck No");
    ui->BtnBack_7->setText("Back");
    ui->BtnNext_3->setText("Next");
    ui->BtnPreview->setText("Preview");
    /***********************************************/

    /***************** PAGE-23 *********************/
    ui->label_52->setText("Authentication");
    ui->BtnDealerRecGoods->setText("1. Dealer");
    ui->BtnRouteOfficer->setText(("2. Route Officer"));
    ui->BtnBack_8->setText("Back");
    ui->BtnScanIris->setText("Scan Eye");
    ui->BtnScanFinger->setText("Scan FP");
    ui->BtnHawker->setText("3. Hawker");
    /***********************************************/

    /***************** PAGE-24 *********************/
    ui->label_62->setText("Delivered Goods");
    //ui->label_65->setText("Truck Chit No");
    //ui->label_59->setText("RO No");
    //ui->label_60->setText("Truck No");
    //ui->BtnOK_3->setText("OK");
    /***********************************************/

    /***************** PAGE-25 *********************/
    /***********************************************/

    /***************** PAGE-26 *********************/
    /***********************************************/

    /***************** PAGE-27 *********************/

    /***********************************************/

    /***************** PAGE-28 *********************/
    ui->lblIssueHeading->setText("ISSUE");
    ui->BtnPDS->setText("Cash PDS");
    ui->BtnBack_12->setText("Back");
    ui->btnMDM->setText("M D M");
    /***********************************************/

    /***************** PAGE-29 *********************/

    /***********************************************/

    /***************** PAGE-30 *********************/

    /***********************************************/

    /***************** PAGE-31 *********************/
    /***********************************************/

    /***************** PAGE-32 *********************/

    /***********************************************/

    /***************** PAGE-34 *********************/

    /***********************************************/

    /***************** PAGE-35 *********************/
    ui->label_122->setText("Customer\nAadhaar No");
    ui->label_123->setText("Customer\nMobile No");
    ui->BtnBack_18->setText("Back");
    ui->BtnOTP->setText("OTP");
    /***********************************************/

    /***************** PAGE-36 *********************/
    ui->label_127->setText("OTP");
    ui->BtnBack_19->setText("Back");
    ui->BtnSubmit_3->setText("Submit");
    /***********************************************/

    /***************** PAGE-37 *********************/
    /***********************************************/

    /***************** PAGE-38 *********************/

    /***********************************************/

    /***************** PAGE-39 *********************/
    ui->lblNCR_Heading->setText("Natural Calamity");

    /**********************************************/

    /***************** PAGE-40 *********************/
    ui->btnEKYC_Update->setText("UID Seeding");
    //ui->btnBFD->setText("BFD");
    ui->btnMobileUpdate->setText("UID Mobile \n Update");
    ui->lblAadhaarServices->setText("Aadhaar Services");
    /**********************************************/

    /***************** PAGE-42 *********************/
    ui->lblHome_4->setText("UID Seeding");
    ui->lblEKYC_Name->setText("Name");
    ui->lblEKYC_RCNO->setText("Card No.");
    ui->lblEKYC_UID_Promt->setText("Enter UID");
    ui->btnEKYC_FP->setText("Scan-FP");
    ui->btnEKYC_Back->setText("Back");
    /**********************************************/

    /***************** PAGE-44 *********************/
    ui->lbl_inspectorVerification->setText("INSPECTION");
    ui->btn_Submit_InspectorVerify->setText("Next");
    ui->btn_Back_inspectorVerify->setText("Back");
    ui->radioButton_OK_inspcVerify->setText("Ok");
    ui->radioButton_seized_inspVer->setText("Seized");
    /**********************************************/

    /***************** PAGE-45 *********************/
    ui->lbl_InspAuth->setText("INSPECTOR AUTHENTICATION");
    ui->lbl_EnteUIDInspAuth->setText("Enter UID");
    ui->btnBack_InspAuth->setText("Back");
    ui->btnScanFpInspAuth->setText("Scan-FP");

    /**********************************************/

    ui->radioButton_OK_inspcVerify->setText("Ok");
    ui->radioButton_seized_inspVer->setText("Seized");
    ui->radioButton_RCNOFLAG->setText("RC No.");
    ui->radioButton_UIDFLAG->setText("Aadhaar");
    ui->radioButton_RCNOFLAGEKYC->setText("Card No.");
    ui->radioButton_UIDFLAGEKYC->setText("Aadhaar");
    ui->lblHome_5->setText("UID SEEDING");
    ui->label_42->setText("Total Amt");
    ui->btncurrstkback->setText("Back");
    ui->btncurrstkprint->setText("Print");
    ui->label_23->setText("CURRENT STOCK REPORT");

    /*************Mobile seeding and OTP********/
    ui->btnMobile_Seeding->setText("Mobile\n Seeding");
    ui->label_29->setText("Customer \nCard No.");
    ui->label_51->setText("Customer \n Mobile Number");
    ui->btnmobupdateback->setText("Back");
    ui->btnmobupdatenext->setText("OTP");
    ui->label_61->setText("Enter OTP");
    ui->leditmobupdatebk->setText("Back");
    ui->label_26->setText("Mobile Number Seeding");
    ui->label_53->setText("Mobile Number Seeding");
    ui->leditmobupdatesub->setText("Submit");
    ui->radioButton_scanbarcode->setText("Scan RC\n Barcode");

    /********Offline Receive Goods***************/
    ui->RecvGoodsHead->setText(QString("RECEIVE GOODS").toUpper());
    ui->RecvGoodsTruckChitNoLbl->setText(QString("Truck Chit No   :").toUpper());//new
    ui->RecvGoodsVechNoLbl->setText(QString("Vechile No     :").toUpper());
    ui->RecvGoodsChallanNoLbl->setText(QString("Challan Number:").toUpper());
    ui->RecvGoodsBackBtn->setText(QString("Back").toUpper());
    ui->RecvGoodsSubmitBtn->setText(QString("Submit").toUpper());
    ui->RecvGdsCommEntryHead->setText(QString("Receive Goods").toUpper());
    ui->RecvGoodsCommEntBack->setText(QString("Back").toUpper());
    ui->RecvGoodsCommEntAdd->setText(QString("+Add").toUpper());
    ui->RecvGoodsCommEntSubmit->setText(QString("Submit").toUpper());
    ui->RecvGdsConfirmHead->setText(QString("Receive Goods").toUpper());
    ui->RecvGdsConfirmBack->setText(QString("Back").toUpper());
    ui->RecvGdsConfirmSubmit->setText(QString("Confirm").toUpper());
    /********Offline Receive Goods***************/


    ui->btnDtSalesView->setText("View");
    //    ui->tblSalesDaily->setHorizontalHeaderLabels(QString::fromUtf8("Commodity;Scheme;Sale;PHH;TOTAL").split(";"));
    ui->tblSalesDaily->setHorizontalHeaderLabels(QString::fromUtf8("Commodity;Scheme;Sale").split(";"));
    ui->tableWidgetStockreport->setHorizontalHeaderLabels(QString::fromUtf8("Commodity;Scheme;Stock;Issued;CB").split(";"));
    ui->tableWidgetDealNomDetails->setHorizontalHeaderLabels(QString::fromUtf8("Name;Dealer Type;B1;B2;B3;Actual UID;Dealer Type").split(";"));
    ui->twdgtEntitlement->setHorizontalHeaderLabels(QString::fromUtf8("Name;MEMBER ID;B1;B2;B3").split(";"));
    ui->tblCommIssue->setHorizontalHeaderLabels(QString("Item(TotQty);BalQty;Rate;IssueQty;Amt; ;CB").split(";"));
    ui->tableWidget_InspectorVerificationn->setHorizontalHeaderLabels(QString("commodity;CB;OBSERVATIONS;VARIATIONS").split(";"));
    ui->tableWidgetStockTable_3->setHorizontalHeaderLabels(QString("Comm;scheme;Allot;ROQty;dispatch;Receive;commcode;scheme id").split(";"));
    ui->tableWidgetRecieveGoodsAuth->setHorizontalHeaderLabels(QString("Name;UID;Mobile;Actual_UID").split(";"));
    ui->label_70->setText("BENEFICIARY\nVERIFY");
    ui->btn_BenVerify->setText("Beneficiary \n Verify");
    ui->btn_RCNOFLAGBenverify->setText("Card No.");
    ui->btn_UIDFLAGBenverify->setText("Aadhaar");
    ui->btnBack_BenVerify->setText("Back");
    ui->btnOK_BenVerify->setText("OK");
    ui->label_81->setText("MEMBER DETAILS");
    ui->btn_BenVerifyBack_->setText("Back");
    ui->BenVerifyScanFP->setText("Scan-FP");
    ui->tableWidgetBenVerify->setHorizontalHeaderLabels(QString::fromUtf8("Name;Member Id;Status").split(";"));
    ui->btnBackAadhaarServices->setText("Back");

    return;
}

void Widget::setLangMode(int iLang)
{
    char szLang[4];
    memset(szLang, 0, sizeof(szLang));
    sprintf(szLang, "%d", iLang);

    QSqlQuery query;
    query.prepare("UPDATE Settings SET LangMode=?");
    query.addBindValue(szLang);
    if(!query.exec()){
        qDebug() << query.lastError();
    }
    return;
}

int Widget::getCommMode()
{
    char szCommMode[4];
    memset(szCommMode, 0, sizeof(szCommMode));

    QSqlQuery query;
    query.prepare("SELECT CommMode from Settings");
    if(!query.exec()){
        qDebug() << query.lastError();
        return -1;
    }else{
        while(query.next()){
            strcpy(szCommMode, query.value(0).toString().toAscii().data());
        }
    }
    qDebug()<<"Comm Mode : "<<szCommMode;
    return atoi(szCommMode);
}

void Widget::setWM_MacId(char *szMacId)
{
    QSqlQuery query;
    query.prepare("UPDATE Settings SET WM_MacId=?");
    query.addBindValue(szMacId);
    if(!query.exec())
        qDebug() << query.lastError();
    ui->ldtWM_MAC_ID->setText(szMacId);
}

int Widget::getWM_MacId(char *szMacId)
{
    memset(szMacId, 0, sizeof(szMacId));

    QSqlQuery query;
    query.prepare("SELECT WM_MacId from Settings");
    if(!query.exec()){
        qDebug() << query.lastError();
        return -1;
    }else{
        while(query.next()){
            strcpy(szMacId, query.value(0).toString().toAscii().data());
        }
    }
    qDebug()<<"MAC ID : "<<szMacId;
    return 0;
}

void Widget::setSessionId(char *szSId)
{
    QSqlQuery query;
    query.prepare("UPDATE SESSION SET SessionId=?");
    query.addBindValue(szSId);
    if(!query.exec())
        qDebug() << query.lastError();
}

void Widget::on_tblCommIssue_cellChanged(int row, int column)
{
    float ItemPrice =0.00;
    char szItemPrice[32];
    char buf1[256]="";
    memset(buf1,0,sizeof(buf1));
    QSqlQuery query;
    //    float znpnsqty=0,znpnsqtylocal=0;
    if(ui->stackedWidget->currentIndex() == 6){
        if(column == 3){
            memset(buf1,0,sizeof(buf1));
            strcpy(buf1,ui->tblCommIssue->item(row,column)->text().toAscii().data());
            qDebug()<<" ENTERD qty"<<buf1;

            for(int i=0;i<strlen(buf1);i++){
                if(buf1[i] != '.' && buf1[i] != '0' && buf1[i] != '1' && buf1[i] != '2' && buf1[i] != '3' && buf1[i] != '4' && buf1[i] != '5' && buf1[i] != '6' && buf1[i] != '7' && buf1[i] != '8' && buf1[i] != '9'){
                    if(LangMode==1)
                        pop_up((char *)"Please Enter Valid Quantity");
                    if(LangMode==2)
                        pop_up((char *)"कृपया मात्रा को \nठीक से दर्ज कीजिये");
                    sleep(2);
                    ui->tblCommIssue->item(row, column)->setText("0");
                    return;
                }
            }
            ///////////////////////////////

            qDebug()<<"Required Qty :"<<ui->tblCommIssue->item(row, column)->text().toFloat();
            if(PartialTxn!=1){
                qDebug()<<"Min Qty      :"<<MinQtyStr.at(row).toFloat();
                // if(ui->tblCommIssue->item(row, column)->text().toFloat() < MinQtyStr.at(row).toFloat())
                //                if(ui->tblCommIssue->item(row,3)->text() == "0" || (atof(ui->tblCommIssue->item(row, column)->text().toAscii().data()) > 0) )

                if(atof(ui->tblCommIssue->item(row, column)->text().toAscii().data()) > 0)
                    if(BenEnterQtyChecking(ui->tblCommIssue->item(row, 1)->text(),ui->tblCommIssue->item(row, column)->text().toAscii().data(),MinQtyStr.at(row).toAscii().data())){
                        char TmpStr[300];
                        memset(TmpStr,0x00,sizeof(TmpStr));
                        QString name = ui->tblCommIssue->item(row, 0)->text().split("(").at(0);
                        sprintf(TmpStr,"Please Enter the %s Qty greater than or equal to %s",name.toAscii().data(),MinQtyStr.at(row).toAscii().data());
                        objMisc.DisplayWideMsgBox((char *)TmpStr);
                        ui->tblCommIssue->item(row, column)->setText("0");
                        ui->tblCommIssue->item(row, column+1)->setText("0");
                        return;
                    }
            }
            int res_sys = dotremoval(buf1);
            if(res_sys != 0){
                if(LangMode==1)
                    pop_up((char *)"Please Enter Valid Quantity");
                if(LangMode==2)
                    pop_up((char *)"कृपया मात्रा को \nठीक से दर्ज कीजिये");
                sleep(2);
                ui->tblCommIssue->item(row, column)->setText("0");
                return;
            }
            query.clear();
            QStringList list = ui->tblCommIssue->item(row, 0)->text().split('(', QString::SkipEmptyParts);
            QString QCommName = list.at(0).trimmed();
            QString QDummyQty = list.at(1);
            QStringList dummylist = QDummyQty.split(')', QString::SkipEmptyParts);
            QString QTotQty = dummylist.at(0);

            qDebug()<<" ......QCommName"<<QCommName;
            qDebug()<<" --------QDummyQty"<<QDummyQty;
            qDebug()<<" =======QTotQty"<<QTotQty;

            int iFestCommCode = 0;
            QString itemmesure;

            if(LangMode == 1){
                query.prepare("SELECT ItemCode,MeasureUnit FROM ItemMaster WHERE ItemType=?");
                query.addBindValue(QCommName);
            }
            if(LangMode==2){
                query.prepare("SELECT ItemCode,MeasureUnit FROM ItemMaster WHERE ItemNameTel=?");
                query.addBindValue(QCommName);
            }
            if(!query.exec())
                qDebug() << query.lastError();
            else{
                while(query.next()){
                    iFestCommCode = query.value(0).toInt(0);
                    itemmesure = query.value(1).toString();//05012018-subbu
                    qDebug()<< "iFestCommCode =" << iFestCommCode;
                    qDebug()<<"itemmesure"<<itemmesure;
                }
            }
            if(ui->tblCommIssue->item(row, column)->text().toFloat(0) < 0){
                if(LangMode==1)
                    miscObj.DisplayWideMsgBox((char *)"Enter the Quantity greater than zero");
                else if(LangMode==2)
                    miscObj.tel_DisplayWideMsgBox((char *)"कृपया शून्य से बड़ा दर्ज करें.");
                return;
            }
            if(iFestCommCode == 6 && itemmesure.toUpper() =="PKT"){
                qDebug()<<"atta entered wrong";
                if(((ui->tblCommIssue->item(row,column)->text().toFloat(0)) - (ui->tblCommIssue->item(row,column)->text().toInt(0))) > 0){
                    //ui->tblCommIssue->item(row,column)->setText("0.00");
                    if(LangMode==1)
                        pop_up((char *)"Please Enter Valid Quantity");
                    if(LangMode==2)
                        pop_up((char *)"कृपया मात्रा को \nठीक से दर्ज कीजिये");
                    sleep(2);
                    ui->tblCommIssue->item(row, column)->setText("0");
                    return;
                }
            }
            //else if(atof(ui->tblCommIssue->item(row, column)->text().toAscii().data()) > (atof(ui->tblCommIssue->item(row, column-2)->text().toAscii().data()) + 0.005))

            if(atof(ui->tblCommIssue->item(row, column)->text().toAscii().data()) > (atof(ui->tblCommIssue->item(row, 1)->text().toAscii().data()))){
                qDebug()<<"weight exeeding Bal Qty";
                if(LangMode ==1)
                    miscObj.DisplayMsgBox((char *)"Please Issue Commodity upto Bal Qty only");
                else if(LangMode ==2)
                    miscObj.tel_DisplayWideMsgBox((char *)"कृपया केवल शेष राशि \nतक कमोडिटी जारी करें");
                ui->tblCommIssue->item(row, column)->setText("0");
                ui->tblCommIssue->item(row, column+1)->setText("0");
                return;
            }

            if(PartialTxn != 1){
                //                if(ui->tblCommIssue->item(row,3)->text() == "0" || (atof(ui->tblCommIssue->item(row, column)->text().toAscii().data()) > 0))
                if(atof(ui->tblCommIssue->item(row, column)->text().toAscii().data()) > 0){
                    //                    if((atof(ui->tblCommIssue->item(row,3)->text().toAscii().data()) < (atof(ui->tblCommIssue->item(row,9)->text().toAscii().data()))) && QminQtyflag == "Y") //commented -BSK.
                    if((atof(ui->tblCommIssue->item(row,3)->text().toAscii().data()) < (atof(ui->tblCommIssue->item(row,9)->text().toAscii().data()))) && minqty_flag == "Y" && (atof(ui->tblCommIssue->item(row, 1)->text().toAscii().data()) >= atof(ui->tblCommIssue->item(row,9)->text().toAscii().data()))){
                        qDebug()<<"minimum qty should greater than or equal entry qantity";
                        char dispbuff[256];
                        memset(dispbuff, 0, sizeof(dispbuff));

                        if(LangMode ==1){
                            sprintf(dispbuff, "Issue quantity should more than or equal to Minimum quantity : %s", ui->tblCommIssue->item(row,9)->text().toAscii().data());
                            miscObj.DisplayMsgBox((char *)dispbuff);
                        }else if(LangMode ==2){
                            sprintf(dispbuff, "इश्यू की मात्रा न्यूनतम मात्रा से अधिक या उसके बराबर होनी चाहिए: %s", ui->tblCommIssue->item(row,9)->text().toAscii().data());
                            miscObj.tel_DisplayWideMsgBox((char *)dispbuff);
                        }
                        ui->tblCommIssue->item(row, column)->setText("0");
                        ui->tblCommIssue->item(row, column+1)->setText("0");
                        return;
                    }
                }
            }

            //            if(iModuleType == COMM_TXN){
            //                if(PartialTxn == 1){
            //                    if(column == 3){
            //                        if(ui->tblCommIssue->item(row,3)->text().toFloat()>0){
            //                            qDebug()<<"Bal :"<<ui->tblCommIssue->item(row, 1)->text().toFloat();
            //                            qDebug()<<"Lifted :"<<ui->tblCommIssue->item(row, 3)->text().toFloat();
            //                            if(ui->tblCommIssue->item(row, 1)->text().toFloat() != ui->tblCommIssue->item(row, 3)->text().toFloat()){
            //                                char buff[128];
            //                                memset(buff,0,sizeof(buff));
            //                                sprintf(buff,"Please enter Qty equals %0.3f Kg",ui->tblCommIssue->item(row, 1)->text().toFloat(0));
            //                                miscObj.DisplayWideMsgBox(buff);
            //                                ui->tblCommIssue->item(row, column)->setText("0");
            //                                ui->tblCommIssue->item(row, column+1)->setText("0");
            //                                return;
            //                            }
            //                        }
            //                    }
            //                }
            //            } //commented under partial lifting in offline -BSK.

            /*{
                    if(MinQtyChecking(MinQtyStr.at(row),ui->tblCommIssue->item(row, column)->text().toAscii().data()))
                    {
                        char TmpStr[300];
                        memset(TmpStr,0x00,sizeof(TmpStr));
                        sprintf(TmpStr,"Please Enter the Minimum Qty(%s) \n              (OR)\n Minimum Qty X Minimum Qty (%sx%sx..)  ",MinQtyStr.at(row).toAscii().data(),MinQtyStr.at(row).toAscii().data(),MinQtyStr.at(row).toAscii().data());
                        objMisc.DisplayWideMsgBox((char *)TmpStr);
                        ui->tblCommIssue->item(row, column)->setText("0");
                        ui->tblCommIssue->item(row, column+1)->setText("0");
                        return;
                    }
            }*/

            ItemPrice = 0.00;
            ItemPrice = atof(ui->tblCommIssue->item(row, column-1)->text().toAscii().data() ) * atof(ui->tblCommIssue->item(row, column)->text().toAscii().data());
            memset(szItemPrice, 0, sizeof(szItemPrice));
            sprintf(szItemPrice, "%0.2f", ItemPrice);

            ui->tblCommIssue->setItem(row, column+1, new QTableWidgetItem(szItemPrice));

            ui->tblCommIssue->item(row, 4)->setFlags( ui->tblCommIssue->item(row,4)->flags() ^ Qt::ItemIsEditable);
            ui->tblCommIssue->item(row, 4)->setFlags(ui->tblCommIssue->item(row, 4)->flags() ^ Qt::ItemIsSelectable);
            if(iMainTransaction == MDM_TRANSACTION){
                if(ui->tblCommIssue->item(row,3)->text().toFloat() != 0.0){
                    float issuedquantity = ui->tblCommIssue->item(row,1)->text().toFloat();
                    float liftedquantity = ui->tblCommIssue->item(row,3)->text().toFloat();

                    qDebug()<<"issued quantity"<<issuedquantity;
                    qDebug()<<"lifted quantity"<<liftedquantity;

                    if(liftedquantity != issuedquantity){
                        objMisc.DisplayWideMsgBox("Please take total quantity");
                        ui->tblCommIssue->item(row,3)->setText("0");
                        return;
                    }
                }
            }

            if(DealerLoginBy != 2){
                float IssueQty = atof(ui->tblCommIssue->item(row, 3)->text().toAscii().data());
                if(IssueQty > 0.00)
                {
                    if(iModuleType == COMM_TXN){
                        float fdb_ClosingBal = 0.0;
                        QSqlQuery query;
                        if(PartialTxn!=1){
                            if(LangMode == 1)
                                query.prepare("SELECT closingBal FROM BenQuotaTable WHERE ItemType=?");
                            else if(LangMode == 2)
                                query.prepare("SELECT closingBal FROM BenQuotaTable WHERE commNamell=?");
                            query.addBindValue(ui->tblCommIssue->item(row, 0)->text().split("(").at(0));
                            if(!query.exec())
                                qDebug() << query.lastError();
                            else{
                                while(query.next()){
                                    fdb_ClosingBal = query.value(0).toString().toFloat(0);
                                }
                            }
                        }
                        else
                            fdb_ClosingBal =  ui->tblCommIssue->item(row, 6)->text().toFloat();

                        qDebug()<<"ItemType"<<ui->tblCommIssue->item(row, 0)->text().split("(").at(0);
                        qDebug()<<"fdb_ClosingBal"<<fdb_ClosingBal;

                        if(atof(ui->tblCommIssue->item(row, column)->text().toAscii().data()) > fdb_ClosingBal){
                            qDebug() << "Exceeding Closing Balance";
                            if(LangMode ==1){
                                char dispbuff[256];
                                memset(dispbuff, 0, sizeof(dispbuff));
                                sprintf(dispbuff, "Please Issue Commodity upto Closing Balance only\nClosing Balance : %0.2f", fdb_ClosingBal);

                                miscObj.DisplayWideMsgBox((char *)dispbuff);
                                qDebug()<<"**********"<<dispbuff;
                            }
                            else if(LangMode==2){
                                char dispbuff[256];
                                memset(dispbuff, 0, sizeof(dispbuff));
                                sprintf(dispbuff, "कृपया केवल समापन शेष तक अंक कमोडिटी जारी करें: \n शेष राशि: %0.2f", fdb_ClosingBal);
                                miscObj.tel_DisplayWideMsgBox((char *)dispbuff);
                                qDebug()<<"**********"<<dispbuff;
                            }
                            ui->tblCommIssue->item(row, column)->setText("0");
                            ui->tblCommIssue->item(row, column+1)->setText("0");
                            return;
                        }
                    }

                    if(atof(ui->tblCommIssue->item(row, column)->text().toAscii().data()) > ui->tblCommIssue->item(row, 6)->text().toFloat()){
                        qDebug() << "2.Exceeding Closing Balance";
                        if(LangMode ==1){
                            char dispbuff[256];
                            memset(dispbuff, 0, sizeof(dispbuff));
                            sprintf(dispbuff, "Please Issue Commodity upto Closing Balance only");

                            miscObj.DisplayWideMsgBox((char *)dispbuff);
                            qDebug()<<"**********"<<dispbuff;
                        }
                        else if(LangMode==2){
                            char dispbuff[256];
                            memset(dispbuff, 0, sizeof(dispbuff));
                            sprintf(dispbuff, "कृपया केवल समापन शेष तक अंक कमोडिटी जारी करें");
                            miscObj.tel_DisplayWideMsgBox((char *)dispbuff);
                            qDebug()<<"**********"<<dispbuff;
                        }
                        ui->tblCommIssue->item(row, column)->setText("0");
                        ui->tblCommIssue->item(row, column+1)->setText("0");
                        return;
                    }

                    if(PartialTxn != 1) //online
                    {
                        //                        float a = atof(ui->tblCommIssue->item(row, 3)->text().toAscii().data());//Entered Qty
                        //                        float b = atof(ui->tblCommIssue->item(row, 9)->text().toAscii().data());//min Qty
                        //                        float c = atof(ui->tblCommIssue->item(row, 1)->text().toAscii().data());//Bal Qty

                        //                        if(c < b) //if Balance qty less than Min qty then txn go on Bal qty-BSK.
                        //                            b = c;

                        //                        char buff[20];
                        //                        float answer = remainder(a, b);
                        //                        cout << "Remainder of " << a << "/" << b << " = " << answer << endl;

                        //                        memset(buff,0,sizeof(buff));
                        //                        sprintf(buff,"%0.3f",answer);

                        //                        if((strcmp(buff,"0.000") == 0) || (strcmp(buff,"-0.005") == 0) || (strcmp(buff,"0.005") == 0)){
                        //                            qDebug()<<"Issue Qty Should Be Multiple By Minimum Qty validation Successfull";
                        //                        }else{
                        //                            if(LangMode == 1){
                        //                                char dispbuff[256];
                        //                                memset(dispbuff, 0, sizeof(dispbuff));
                        //                                sprintf(dispbuff,"Issue Qty Should Be Multiple By Minimum Qty - %0.3f",b);
                        //                                miscObj.DisplayWideMsgBox((char *)dispbuff);
                        //                                qDebug()<<"**********"<<dispbuff;
                        //                            }else if(LangMode == 2){
                        //                                char dispbuff[256];
                        //                                memset(dispbuff, 0, sizeof(dispbuff));
                        //                                //                                sprintf(dispbuff,"कृपया शून्य से अधिक मात्रा दर्ज करें - %s",ui->tblCommIssue->item(row, 9)->text().toAscii().data());
                        //                                sprintf(dispbuff,"कृपया शून्य से अधिक मात्रा दर्ज करें - %0.3f",b);
                        //                                miscObj.tel_DisplayWideMsgBox(dispbuff);
                        //                            }

                        //                            ui->tblCommIssue->item(row, column)->setText("0");
                        //                            ui->tblCommIssue->item(row, column+1)->setText("0");
                        //                            return;
                        //                        }

                        QString BalQty      = ui->tblCommIssue->item(row, 1)->text().toAscii().data();
                        QString IssueQty    = ui->tblCommIssue->item(row, 3)->text().toAscii().data();
                        QString MinQty      = ui->tblCommIssue->item(row, 9)->text().toAscii().data();
                        QString IssueQtyOut;

                        if(minqty_flag == "Y")
                        {
                            char ConvBalQty[20],ConvIssueQty[20];
                            memset(ConvBalQty,0,sizeof(ConvBalQty));
                            memset(ConvIssueQty,0,sizeof(ConvIssueQty));

                            sprintf(ConvBalQty,"%0.3f",atof(BalQty.toAscii().data()));
                            sprintf(ConvIssueQty,"%0.3f",atof(IssueQty.toAscii().data()));

                            BalQty = ConvBalQty;
                            IssueQty = ConvIssueQty;

                            qDebug()<<" Bal Qty = "<<BalQty;
                            qDebug()<<" IssueQty Qty = "<<IssueQty;

                            if(IssueQty != BalQty){
                                int ret = MinQtyChecking(IssueQty,MinQty,weighAccuracyValueInGms.toFloat());
                                if(ret != 0){
                                    if(LangMode == 1){
                                        char dispbuff[256];
                                        memset(dispbuff, 0, sizeof(dispbuff));
                                        sprintf(dispbuff,"Issue Qty Should Be Multiple By Minimum Qty - %s",ui->tblCommIssue->item(row,9)->text().toAscii().data());
                                        miscObj.DisplayWideMsgBox((char *)dispbuff);
                                        qDebug()<<"**********"<<dispbuff;
                                    }else if(LangMode == 2){
                                        char dispbuff[256];
                                        memset(dispbuff, 0, sizeof(dispbuff));
                                        sprintf(dispbuff,"कृपया शून्य से अधिक मात्रा दर्ज करें - %s",ui->tblCommIssue->item(row,9)->text().toAscii().data());
                                        miscObj.tel_DisplayWideMsgBox(dispbuff);
                                    }

                                    ui->tblCommIssue->item(row, column)->setText("0");
                                    ui->tblCommIssue->item(row, column+1)->setText("0");
                                    return;
                                }
                            }
                        }
                        //////////////////////////
                        if(ui->tblCommIssue->item(row,8)->text() == "Y")
                        {
                            WeighRounding(minqty_flag,BalQty,MinQty,IssueQty,weighAccuracyValueInGms.toFloat(),IssueQtyOut);
                            qDebug() << "Roung Weight:" << IssueQtyOut;
                            ui->tblCommIssue->item(row, column)->setText(IssueQtyOut);
                        }
                        //////////////////////////
                    }
                }
            }

            if(DealerLoginBy == 2){
                float IssueQty = atof(ui->tblCommIssue->item(row, 3)->text().toAscii().data());
                if(IssueQty > 0.00)
                {
                    if(iModuleType == COMM_TXN)
                    {
                        float fdb_ClosingBal = 0.0;
                        QSqlQuery query;
                        if(PartialTxn!=1){
                            if(LangMode == 1)
                                query.prepare("SELECT closingBal FROM BenQuotaTable WHERE ItemType=?");
                            else if(LangMode == 2)
                                query.prepare("SELECT closingBal FROM BenQuotaTable WHERE commNamell=?");
                            query.addBindValue(ui->tblCommIssue->item(row, 0)->text().split("(").at(0));
                            if(!query.exec())
                                qDebug() << query.lastError();
                            else{
                                while(query.next()){
                                    fdb_ClosingBal = query.value(0).toString().toFloat(0);
                                }
                            }
                        }else
                            fdb_ClosingBal =  ui->tblCommIssue->item(row, 6)->text().toFloat();

                        qDebug()<<"ItemType"<<ui->tblCommIssue->item(row, 0)->text().split("(").at(0);
                        qDebug()<<"fdb_ClosingBal"<<fdb_ClosingBal;

                        if(atof(ui->tblCommIssue->item(row, column)->text().toAscii().data()) > fdb_ClosingBal){
                            qDebug() << "Exceeding Closing Balance";
                            if(LangMode ==1){
                                char dispbuff[256];
                                memset(dispbuff, 0, sizeof(dispbuff));
                                sprintf(dispbuff, "Please Issue Commodity upto Closing Balance only\nClosing Balance : %0.2f", fdb_ClosingBal);

                                miscObj.DisplayWideMsgBox((char *)dispbuff);
                                qDebug()<<"**********"<<dispbuff;
                            }
                            else if(LangMode==2){
                                char dispbuff[256];
                                memset(dispbuff, 0, sizeof(dispbuff));
                                sprintf(dispbuff, "कृपया केवल समापन शेष तक अंक कमोडिटी जारी करें: \n शेष राशि: %0.2f", fdb_ClosingBal);
                                miscObj.tel_DisplayWideMsgBox((char *)dispbuff);
                                qDebug()<<"**********"<<dispbuff;
                            }
                            ui->tblCommIssue->item(row, column)->setText("0");
                            ui->tblCommIssue->item(row, column+1)->setText("0");
                            return;
                        }
                    }

                    if(atof(ui->tblCommIssue->item(row, column)->text().toAscii().data()) > ui->tblCommIssue->item(row, 6)->text().toFloat())
                    {
                        qDebug() << "2.Exceeding Closing Balance";
                        if(LangMode ==1){
                            char dispbuff[256];
                            memset(dispbuff, 0, sizeof(dispbuff));
                            sprintf(dispbuff, "Please Issue Commodity upto Closing Balance only");

                            miscObj.DisplayWideMsgBox((char *)dispbuff);
                            qDebug()<<"**********"<<dispbuff;
                        }
                        else if(LangMode==2){
                            char dispbuff[256];
                            memset(dispbuff, 0, sizeof(dispbuff));
                            sprintf(dispbuff, "कृपया केवल समापन शेष तक अंक कमोडिटी जारी करें");
                            miscObj.tel_DisplayWideMsgBox((char *)dispbuff);
                            qDebug()<<"**********"<<dispbuff;
                        }
                        ui->tblCommIssue->item(row, column)->setText("0");
                        ui->tblCommIssue->item(row, column+1)->setText("0");
                        return;
                    }
                }
            }

            if((DealerLoginBy == 2) || (PartialTxn == 1))
            {
                int ret = 0;
                ret = ForwardDistrubutionValidation(row,column);
                qDebug()<<" ret value of 3 Months Validation ----- "<< ret;
                if(ret == -1){
                    return;
                }
            }
        }
        //////////////////////////////////////////////
    }
    if(column == 4){
        char buff[32];
        fTotalAmt = 0.00;
        fTotalQty = 0.00;
        int iNoOfRows = 0, i=0;
        iNoOfRows = ui->tblCommIssue->rowCount();
        for(i=0; i<iNoOfRows;i++){
            if(ui->tblCommIssue->item(i, column) != NULL){
                fTotalAmt += atof(ui->tblCommIssue->item(i, column)->text().toAscii().data());
                fTotalQty += atof(ui->tblCommIssue->item(i, column-1)->text().toAscii().data());
            }
        }
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "%0.2f", fTotalAmt);
        ui->lblTotalAmtVal->setText(buff);
    }
    return;
}

void Widget::on_pushButton_7_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Widget::on_btnQuitApp_clicked()
{
    exit(0);
}

void Widget::on_btnLogin_clicked()
{
    int ret = 0;

    /******** TIME LAP START*******/
    QTime tim;
    /******************************/

    iDealMemSelected = DEALER_SELECTED;

    int iRowCount = 0;
    int i = 0;
    int row = 0;

    iRowCount = ui->tableWidgetDealNomDetails->rowCount();
    for(i = 0; i < iRowCount; i++){
        qDebug()<< i;
        if(ui->tableWidgetDealNomDetails->item(i,0)->isSelected() == true)
            break;
    }
    if(i == iRowCount){
        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"Please Select Dealer/Nominee");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया डीलर/नॉमिनी  का चयन करें ");
        return;
    }
    row = ui->tableWidgetDealNomDetails->currentRow();
    qDebug()<<"***********Current Row No = "<<row;

    getDealerAuthTypeANDFusion(row,ui->tableWidgetDealNomDetails->item(row, 1)->text().toAscii().data());
    qDebug()<<"DelAuthMode"<<DelAuthMode;
    qDebug()<<"FusionStatus"<<FusionStatus;

    if(strcmp(DelAuthMode,"F") == 0){
        tim.restart();
        tim.start();

        Finger_Iris_Selection = 1;
        ret = AuthOwner();
        qDebug()<<"Ret Auth Owner : "<<ret;

        if(ret < 0){
            qDebug()<<"Dealer auth failed";
            return;
        }
    }
    else if(strcmp(DelAuthMode,"I") == 0){
        Finger_Iris_Selection = 0;

        int iRowCount = 0;
        int i = 0;
        iRowCount = ui->tableWidgetDealNomDetails->rowCount();
        for(int i = 0; i < iRowCount; i++){
            if(ui->tableWidgetDealNomDetails->item(i,0)->isSelected() == true)
                break;
        }
        if(i == iRowCount){
            miscObj.AudioTest((char *)"176");
            if(LangMode == 1)
                miscObj.DisplayMsgBox((char *)"Please Select Dealer/Nominee");
            else
                miscObj.tel_DisplayWideMsgBox((char *)"कृपया डीलर/नॉमिनी  का चयन करें ");
            return;
        }

        tim.restart();
        tim.start();

        int row = 0;
        row = ui->tableWidgetDealNomDetails->currentRow();

        char cMemUID[16];
        memset(cMemUID, 0, sizeof(cMemUID));
        strncpy(cMemUID, ui->tableWidgetDealNomDetails->item(row, 5)->text().toAscii().data(), 12);
        if(QString(cMemUID).isEmpty() || QString(cMemUID).isNull()){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"UID is Not Present...\nPlease Select Other Member");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"UID मौजूद नहीं है ... \nकृपया अन्य सदस्य का चयन करें");
            return;
        }

        i = 1;
        while(1){
            if((i == 1) || (i == 3) || (i == 5)){
                int Irisret = Capture_Iris(LEFT_EYE, cMemUID);
                if(Irisret == 0)
                    break;
                else if(Irisret == -2)
                    return;
            }else if((i == 2) || (i == 4) || (i == 6)){
                int Irisret = Capture_Iris(RIGHT_EYE, cMemUID);
                if(Irisret == 0)
                    break;
                else if(Irisret == -2)
                    return;
            }
            if(i == 2){
                miscObj.AudioTest((char *)"171");
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox((char *)"Exceeded Attempts for Iris Authentication...\nPlease Opt for Scan-FP");
                else
                    miscObj.tel_DisplayWideMsgBox((char *)"आईरिस प्रमाणीकरण के लिए अधिक प्रयास हुए\nकृपया स्कैन-एफपी चुनें");
                return;
            }
            i++;
        }
    }
    else if(strcmp(DelAuthMode,"P") == 0){
        qDebug()<<"We are going for Dealer password";

        if(otp_popup_verify(dealer_password)< 0)
            return;

        miscObj.AudioTest((char *)"178");
        popup_msg *dlgPopup3 = new popup_msg(this);

        if(LangMode == 1)
            dlgPopup3->ui->lblPopup->setText("Please Wait...\nAuthenticating Dealer...");
        else if(LangMode == 2)
            dlgPopup3->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..\nडीलर प्रमाणीकरण हो रहा है.."));

        dlgPopup3->show();
        QCoreApplication::processEvents();
        QCoreApplication::processEvents();

        sleep(2); //Kept For Audio

        system("rm -rf /mnt/jffs2/PWDACKrequest.xml");
        if(PwdackRequest_Response("/mnt/jffs2/PWDACKrequest.xml")< 0)
        {
            dlgPopup3->close();
            dlgPopup3->deleteLater();
            return;
        }
        dlgPopup3->close();
        dlgPopup3->deleteLater();
    }
    else
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Invalid Dealer type...");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"अमान्य डीलर");
        return;
    }

    nLogout=1;

    logout_timer = new QTimer(this);
    connect(logout_timer, SIGNAL(timeout()), this, SLOT(logout()));
    logout_timer->start(1000*60*30);

    if(DealerTxnFlag == 1)
    {
        if(Qtxnflow == "B")
            return;
        else
        {
            row = ui->tableWidgetDealNomDetails->currentRow();
            getcomTxn_type = ui->tableWidgetDealNomDetails->item(row, 6)->text().trimmed();
            DisplayCommDetails();
        }
    }
    else
    {

        if(CallingMenuDrivenService() < 0)
            return;

        EnableDisableMenus();

        row = ui->tableWidgetDealNomDetails->currentRow();
        qDebug()<<"***********Current Row No = "<<row;

        qDebug()<<"Member Name = " << ui->tableWidgetDealNomDetails->item(row, 0)->text().toAscii().data();

        QSqlQuery query;
        query.clear();
        query.prepare("UPDATE ShopOwnerDetails SET ShopID=?, Name=?, UID=?");
        query.addBindValue(ui->ldtShpId->text());
        query.addBindValue(ui->tableWidgetDealNomDetails->item(row, 0)->text()); //subbu-04/11/2017
        query.addBindValue(ui->tableWidgetDealNomDetails->item(row, 5)->text().toAscii().data());
        query.exec();

        if(CheckOfflineDealerAuth()<0)
            return;

        char lapbuff[32];
        memset(lapbuff, 0, sizeof(lapbuff));
        sprintf(lapbuff,"Lap Time : %d", tim.elapsed());
        tim.restart();

        miscObj.AudioTest((char *)"57");

        query.clear();
        query.prepare("SELECT type FROM CardTypes");
        if(!query.exec())
            qDebug() << query.lastError();
        else{
            QStringList listOfCards;
            ui->comboTypeOfCard->clear();
            listOfCards.clear();
            listOfCards.append("SELECT");
            while(query.next()){
                listOfCards.append(query.value(0).toString());
            }
            ui->comboTypeOfCard->addItems(listOfCards);
        }
        FILE *fp = fopen("/mnt/jffs2/Hybrid.txt","w");
        fprintf(fp,"Ofline File");
        fclose(fp);
        ui->stackedWidget->setCurrentIndex(1);
    }
    return;
}

int Widget::otp_popup_verify(QString OTP)
{
    bool ok;
    char buff[100]="";
    memset(buff,0,sizeof(buff));
    sprintf(buff,"%s","Please Enter password");

    QString text;
    if(LangMode == 1)
        text = QInputDialog::getText(this, tr("ONLINE Login"),tr(buff), QLineEdit::Password,"", &ok);
    else if(LangMode==2)
        text = QInputDialog::getText(this, tr("ONLINE  Login"),QString::fromUtf8("कृपया पासवर्ड दर्ज करें"), QLineEdit::Password,"", &ok);

    if(ok && !text.isEmpty())
    {
        qDebug()<<"OTP"<<OTP<<endl;
        qDebug()<<"text"<<text<<endl;

        if(strcmp(OTP.toAscii().data(),text.toAscii().data())== 0)
            return 0;
        else{
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Entered Wrong Password...");
            else if(LangMode == 2)
            {
                miscObj.tel_DisplayWideMsgBox((char *)"गलत पासवर्ड दर्ज किया गया");
                //                miscObj.DisplayWideMsgBox((char *)"Entered Wrong Password...");
                //                sleep(3 );
            }
            return -1;
        }

    }else if(ok && text.isEmpty()){
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Enter Correct Password ...");
        else if(LangMode == 2)
        {
            miscObj.tel_DisplayWideMsgBox((char *)"सही पासवर्ड दर्ज करें");
            //            miscObj.DisplayWideMsgBox((char *)"Enter Correct Password ...");
            //            sleep(3);
        }
        return -2;
    }else{
        return -1;
    }
    return 0;
}

int Widget::CallingMenuDrivenService()
{
    qDebug()<<"CallingMenuDrivenService"<<endl;

    popup_msg *dlgPopup = new popup_msg(this);

    database_queries dbqry;
    LangMode = dbqry.getLangMode();

    if(LangMode == 1)
        dlgPopup->ui->lblPopup->setText(trUtf8("Please wait...\nDownloading Menus"));
    else if(LangMode == 2)
        dlgPopup->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.\n मेनू डाउनलोड हो रहा है."));

    dlgPopup->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    system("rm /mnt/jffs2/MenuDisplay_Req.xml");
    system("rm /mnt/jffs2/MenuDisplay_Resp.xml");

    FILE *fp;
    fp = fopen("/mnt/jffs2/MenuDisplay_Req.xml","w");
    if(fp == NULL)
    {
        qDebug() << "/mnt/jffs2/MenuDisplay_Req.xml File Not Found" << endl;

        dlgPopup->close();
        dlgPopup->deleteLater();
        return -1;
    }
    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));

    miscObj.GetMachineId(cmachineID);

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0)
    {
        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"सेशन आईडी नहीं मिला \nकृपया पुन: प्रयास करें");

        return -1;
    }

    char shopId[32];
    memset(shopId, 0, sizeof(shopId));
    getShopId(shopId);

    fprintf(fp, "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:menuDisplayService>");
    fprintf(fp, "<shop_number>%s</shop_number>", shopId);
    fprintf(fp, "<fpsSessionId>%s</fpsSessionId>", SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp, "</ns1:menuDisplayService></SOAP-ENV:Body></SOAP-ENV:Envelope>");

    fclose(fp);

    qDebug() << "************/mnt/jffs2/MenuDisplay_Req.xml************";
    system("cat /mnt/jffs2/MenuDisplay_Req.xml");
    qDebug();
    qDebug() << "******************************************************";

    char cUrlName[256];

    QSqlQuery query;
    query.clear();
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

    //    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/MenuDisplay_Req.xml", cUrlName, "/mnt/jffs2/MenuDisplay_Resp.xml", SERVER_TIME_OUT);

    int ret_http_code = UrlPingCheck(cUrlName);

    if(ret_http_code == 1)
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/MenuDisplay_Req.xml", cUrlName, "/mnt/jffs2/MenuDisplay_Resp.xml", responseTimedOutTimeInSec.toInt());
    }
    else
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/MenuDisplay_Req.xml", PDS_URL2, "/mnt/jffs2/MenuDisplay_Resp.xml", responseTimedOutTimeInSec.toInt());
    }

    qDebug() << "Curl Cmd: " << cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0)
    {
        dlgPopup->close();
        dlgPopup->deleteLater();

        return -1;
    }

    qDebug() << "**********/mnt/jffs2/MenuDisplay_Resp.xml********";
    system("cat /mnt/jffs2/MenuDisplay_Resp.xml");
    qDebug();
    qDebug() << "************************************************";

    dlgPopup->close();
    dlgPopup->deleteLater();

    if(ParsingFirstCondition((char *)"/mnt/jffs2/MenuDisplay_Resp.xml") < 0)
        return -1;

    if(ParseMenuDisplay("/mnt/jffs2/MenuDisplay_Resp.xml") < 0)
        return -1;

    return 0;
}

int Widget::ParseMenuDisplay(char *filename)
{
    qDebug()<<"ParseMenuDisplay"<<endl;
    QString fpsPofflineToken,fpsTokenAllowdOrnotStatus,pOfflineDurationTimeInaDay,pOfflineTransactionTime,AllotYear,AllotMonth,pOfflineStoppedDate,receiveGoodsOfflineEndDate;//rsa_key

    QSqlQuery query;
    QString Qservice, Qslno, Qstatus;

    query.exec("DELETE FROM MenuTable");

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In ParseMenuDisplay, Error Reading File";
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
            if(xml.name() == "service")
            {
                xml.readNext();
                Qservice = xml.text().toString();
                qDebug() << "Qservice : " << Qservice;
            }

            if(xml.name() == "slno")
            {
                xml.readNext();
                Qslno = xml.text().toString();
                qDebug() << "Qslno : " << Qslno;
            }

            if(xml.name() == "status")
            {
                xml.readNext();
                Qstatus = xml.text().toString();
                qDebug() << "Qstatus : " << Qstatus;

                query.clear();
                query.prepare("INSERT INTO MenuTable(Service, Slno, Status)VALUES(:Service, :Slno, :Status)");
                query.bindValue(":Service", Qservice);
                query.bindValue(":Slno", Qslno);
                query.bindValue(":Status", Qstatus);
                if(!query.exec())
                    qDebug() << query.lastError();
            }
            if(xml.name() == "allocationMonth")
            {
                xml.readNext();
                AllotMonth = xml.text().toString();
                qDebug() << "AllotMonth:"<<AllotMonth;
            }
            if(xml.name() == "allocationYear")
            {
                xml.readNext();
                AllotYear = xml.text().toString();
                qDebug() << "AllotYear:"<<AllotYear;
            }
            if(xml.name() == "fpsToken")
            {
                xml.readNext();
                fpsPofflineToken = xml.text().toString();
                qDebug() << "fpsPofflineToken:"<<fpsPofflineToken;
            }
            if(xml.name() == "fpsTokenAllowdOrnotStatus")
            {
                xml.readNext();
                fpsTokenAllowdOrnotStatus = xml.text().toString();
                qDebug() << "fpsTokenAllowdOrnotStatus:" << fpsTokenAllowdOrnotStatus;
            }
            if(xml.name() == "pOfflineDurationTimeInaDay")
            {
                xml.readNext();
                pOfflineDurationTimeInaDay = xml.text().toString();
                qDebug() << "pOfflineDurationTimeInaDay:" << pOfflineDurationTimeInaDay;
            }
            if(xml.name() == "pOfflineStoppedDate")
            {
                xml.readNext();
                pOfflineStoppedDate = xml.text().toString();
                qDebug() << "pOfflineStoppedDate:" << pOfflineStoppedDate;
            }

            if(xml.name() == "pOfflineTransactionTime")
            {
                xml.readNext();
                pOfflineTransactionTime = xml.text().toString();
                qDebug() << "pOfflineTransactionTime:" << pOfflineTransactionTime;
                QSqlQuery OfflineQuery;

                OfflineQuery.clear();
                if(!OfflineQuery.exec("CREATE TABLE IF NOT EXISTS PartialOnlineData(OffPassword text,OfflineLogin text,OfflineTxnTime text,Duration text,leftOfflineTime text,lastlogindate text,lastlogintime text,lastlogoutdate text,lastlogouttime text,AllotMonth text,AllotYear text,pOfflineStoppedDate text)"))
                {
                    qDebug() << "22. " << OfflineQuery.lastError();
                    return -1;
                }
                OfflineQuery.clear();
                if(!OfflineQuery.exec("delete from PartialOnlineData"))
                {

                }
                char TmpStr[512];
                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"insert into PartialOnlineData values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",fpsPofflineToken.toAscii().data(),fpsTokenAllowdOrnotStatus.toAscii().data(),pOfflineDurationTimeInaDay.toAscii().data(),pOfflineTransactionTime.toAscii().data(),pOfflineTransactionTime.toAscii().data(),QDateTime::currentDateTime().toString("dd-MM-yyyy").toAscii().data(),QDateTime::currentDateTime().toString("hh:mm:ss").toAscii().data(),QDateTime::currentDateTime().toString("dd-MM-yyyy").toAscii().data(),QDateTime::currentDateTime().toString("hh:mm:ss").toAscii().data(),AllotMonth.toAscii().data(),AllotYear.toAscii().data(),pOfflineStoppedDate.toAscii().data());
                qDebug()<<"Offline :"<<TmpStr;
                OfflineQuery.clear();
                if(!OfflineQuery.exec(TmpStr))
                {

                }
            }
            if(xml.name() == "skey")
            {
                xml.readNext();
                skey = xml.text().toString();

                QFile file("/mnt/jffs2/impds.ini");
                if(file.open(QIODevice::Text | QIODevice::Append))
                {
                    QTextStream out(&file);
                    out << "SKey=" << skey;
                    file.close();
                }
                QFile file1("/mnt/jffs2/swavalamban.ini");
                if(file1.open(QIODevice::Text | QIODevice::Append))
                {
                    QTextStream out(&file1);
                    out << "SKey=" <<skey<<"\n";
                    out << "pmsWadh="<<g_pmsWadh<<"\n";
                    out << "consent_header="<<consentHeader;
                    file1.close();
                }
            }

            if(xml.name() == "receiveGoodsOfflineEndDate")
            {
                xml.readNext();
                receiveGoodsOfflineEndDate = xml.text().toString();
                qDebug() << "receiveGoodsOfflineEndDate:" << receiveGoodsOfflineEndDate;
                QSqlQuery OfflineQuery;

                OfflineQuery.clear();
                if(!OfflineQuery.exec("CREATE TABLE IF NOT EXISTS StoppedDate(Menu_Name text,Stop_Date text)"))
                {
                    qDebug() << "23. " << OfflineQuery.lastError();
                    return -1;
                }

                if(!OfflineQuery.exec("delete from StoppedDate"))
                {

                }
                char TmpStr[512];
                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"insert into StoppedDate values('RGO','%s')",receiveGoodsOfflineEndDate.toAscii().data());
                qDebug()<<"Offline :"<<TmpStr;
                OfflineQuery.clear();
                if(!OfflineQuery.exec(TmpStr))
                {

                }

            }
        }
    }
    file.close();
    //    qDebug() <<"rsafile------------>" <<rsa_key;
    //    QFile rsafile("/mnt/jffs2/rsa_key.txt");
    //    rsafile.open(QIODevice::WriteOnly);
    //    rsafile.write(rsa_key.toAscii().data());
    //    rsafile.close();
    return 0;
}

void Widget::EnableDisableMenus()
{
    qDebug()<<"EnableDisableMenus"<<endl;
    QSqlQuery query;

    QString QService, QStatus;

    query.prepare("SELECT Service, Status FROM MenuTable");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            QService = query.value(0).toString();
            QStatus = query.value(1).toString();

            if((QStatus == "Y") || (QStatus == "y"))
            {
                if(QService == "getPDSFpsNoDetails")
                {
                    ui->ldtGetOwnDetails->setEnabled(true);
                }else if(QService == "getePDSRationCardDetails")
                {
                    ui->BtnPDS->setEnabled(true);
                    //ui->BtnCashlessPDS->setEnabled(true);
                }else if(QService == "getStockDetails")
                {
                    ui->btnReceiveGoods->setEnabled(true);
                }else if(QService == "getKerosene")
                {
                    ui->btnKerosene->setEnabled(true);
                }else if(QService == "getDailyReport")
                {
                    ui->btnDailySales->setEnabled(true);
                }else if(QService == "getStockReportDetails")
                {
                    ui->btnStockRegister->setEnabled(true);
                }
                else if(QService ==  "getICDSDetails")
                {
                    //ui->BtnICDS->setEnabled(true);
                }else if(QService == "getFestCardDetails")
                {
                    //ui->btnFestivalRation->setEnabled(true);
                }else if(QService == "getSchoolsDetails")
                {
                    //ui->btnMDM->setEnabled(true);
                }else if(QService == "getBeneficiaryBankMasterDetails")
                {
                    //ui->BtnBenMaster->setEnabled(true);
                }else if(QService == "getBankMasterDetails")
                {
                    //ui->BtnBankMaster->setEnabled(true);
                }else if(QService == "getMobileOTP")
                {
                }
                else if(QService == "getMobileNumberUpdate")
                {
                    //ui->btnMobileUpdate->setEnabled(true);
                }
                else if(QService == "getDTRCommDetails")
                {
                    //ui->btnDrot->setEnabled(true);
                }
                else if(QService == "getEKYCAuthenticateRD")
                {
                    ui->btnEKYC_Update->setEnabled(true);
                }
                //                else if(QService =="receiveStockAtPOS")
                else if(QService == "getFpsStockDetails")
                {
                    ui->btnReceiveGoods->setEnabled(true);
                }
                else if(QService =="getFpsStockDetailsOffline")
                {
                    ui->OfflineReceiveGoodsButton->setEnabled(true);
                }
                else if(QService == "getBFDSecureAuthenticate")
                {
                    //                    ui->label_15->show();
                }else if(QService == "getInspectorDetails")
                {
                    ui->btn_inspector->setEnabled(true);
                }else if(QService == "getEKYCMemberDetailsRD")
                {
                    ui->btnEKYC_Update->setEnabled(true);
                }else if(QService == "beneficiaryVerificationDetails")
                {
                    ui->btn_BenVerify->setEnabled(true);
                }else if(QService == "beneficiaryOTPRequestMobileSeed")
                {
                    ui->btnAadhaarServices->setEnabled(true);
                    ui->btnMobile_Seeding->setEnabled(true);
                    //ui->btnMobile_Seeding->show();
                    //ui->label_74->show();
                }
                else if(QService == "impdsMEService"){
                    ui->btnImpds->setEnabled(true);
                }
                else if(QService == "getPMSBeneficiaryDetails"){
                    ui->BtnSwavalamban->setEnabled(true);
                }
                else if(QService == "getOfflineUploadTxns"){
                    ui->UploadDataButton->setEnabled(true);
                }
            }
            else if((QStatus == "N") || (QStatus == "n"))
            {
                if(QService == "getPDSFpsNoDetails")
                {
                    ui->ldtGetOwnDetails->setEnabled(true);
                }else if(QService == "getePDSRationCardDetails")
                {
                    ui->BtnPDS->setDisabled(true);
                    //ui->BtnCashlessPDS->setDisabled(true);
                }else if(QService == "getStockDetails")
                {
                    ui->btnReceiveGoods->setDisabled(true);
                }else if(QService == "getKerosene")
                {
                    ui->btnKerosene->setDisabled(true);
                }else if(QService == "getDailyReport")
                {
                    ui->btnDailySales->setDisabled(true);
                }else if(QService == "getStockReportDetails")
                {
                    ui->btnStockRegister->setDisabled(true);
                }else if(QService ==  "getICDSDetails")
                {
                    //ui->BtnICDS->setDisabled(true);
                }else if(QService == "getFestCardDetails")
                {
                    //ui->btnFestivalRation->setDisabled(true);
                }else if(QService == "getSchoolsDetails")
                {
                    //ui->btnMDM->setDisabled(true);
                }else if(QService == "getBeneficiaryBankMasterDetails")
                {
                    //ui->BtnBenMaster->setDisabled(true);
                }else if(QService == "getBankMasterDetails")
                {
                    //ui->BtnBankMaster->setDisabled(true);
                }else if(QService == "getMobileOTP")
                {

                }else if(QService == "getMobileNumberUpdate")
                {
                    //ui->btnMobileUpdate->setDisabled(true);
                }else if(QService == "getDTRCommDetails")
                {
                    //ui->btnDrot->setDisabled(true);
                }else if(QService == "getEKYCAuthenticateRD")
                {
                    ui->btnEKYC_Update->setDisabled(true);
                }
                //                else if(QService =="receiveStockAtPOS")
                else if(QService == "getFpsStockDetails")
                {
                    // ui->label_47->hide();
                    ui->btnReceiveGoods->setDisabled(true);
                }
                else if(QService =="getFpsStockDetailsOffline")
                {
                    ui->OfflineReceiveGoodsButton->setDisabled(true);
                }
                else if(QService == "getInspectorDetails")
                {
                    ui->btn_inspector->setDisabled(true);
                }else if(QService == "getEKYCMemberDetailsRD")
                {
                    ui->btnEKYC_Update->setDisabled(true);
                }else if(QService == "beneficiaryOTPRequestMobileSeed")
                {
                    ui->btnMobile_Seeding->setDisabled(true);
                    // ui->btnMobile_Seeding->hide();
                    //ui->label_74->hide();
                }
                else if(QService == "impdsMEService"){
                    ui->btnImpds->setDisabled(true);
                }
                else if(QService == "getPMSBeneficiaryDetails"){
                    ui->BtnSwavalamban->setDisabled(true);
                }
                else if(QService == "getOfflineUploadTxns"){
                    ui->UploadDataButton->setDisabled(true);
                }
            }
        }
    }
    return;
}

int Widget::AuthOwner()
{
    int ret = 0;
    popup_msg *dlgPopup2 = new popup_msg(this);
    GPRS objGprs;
    if(objGprs.Check_Network_Available() < 0){
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        return -1;
    }
    char FileName[128];
    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/DealerAuth.xml");

    system("rm /mnt/jffs2/DealerAuth.xml");
    system("rm /mnt/jffs2/AuthResponse.xml");

    int iRowCount = 0;
    int i = 0;

    iRowCount = ui->tableWidgetDealNomDetails->rowCount();

    qDebug()<<"No Of Row in tableWidgetDealNomDetails = "<<iRowCount;
    for(i = 0; i < iRowCount; i++){
        qDebug()<< i;
        if(ui->tableWidgetDealNomDetails->item(i,0)->isSelected() == true)
            break;
    }
    qDebug()<< i <<"<=====>"<<iRowCount;
    if(i == iRowCount){
        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"Please Select Dealer/Nominee");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया डीलर/नॉमिनी  का चयन करें.");
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        return -3;
    }

    int row = 0;
    row = ui->tableWidgetDealNomDetails->currentRow();
    qDebug()<<"***********Current Row No = "<<row;

    char AuthUid[16];
    memset(AuthUid, 0, sizeof(AuthUid));

    strcpy(AuthUid, ui->tableWidgetDealNomDetails->item(row, 5)->text().toAscii().data());

    if(QString(AuthUid).isNull() || QString(AuthUid).isEmpty()){
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"UID is Empty...\nPlease Select Other Member");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"UID रिक्त है ...\n कृपया अन्य सदस्य का चयन करें");
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        return -1;
    }

    dlgPopup2->close();
    dlgPopup2->deleteLater();

    char b64Templ1[2048];
    memset(b64Templ1, 0, sizeof(b64Templ1));

    char b64Templ2[2048];
    memset(b64Templ2, 0, sizeof(b64Templ2));

    int iFusionCount = 0;
    int iFusionLimit=0;
    int Delpwdflag=0;

    if(ui->tableWidgetDealNomDetails->item(row, 6)->text() == "D")
        QwadhStatus=QwadhStatus_D;
    else if(ui->tableWidgetDealNomDetails->item(row, 6)->text() == "N1")
        QwadhStatus=QwadhStatus_N1;
    else if(ui->tableWidgetDealNomDetails->item(row, 6)->text() == "N2")
        QwadhStatus=QwadhStatus_N2;

    qDebug()<<"FusionStatus"<<FusionStatus;

    if(strcmp(FusionStatus,"0") == 0){
        iFusionEnabledFlag=0;
        iFusionCount=1;
        Delpwdflag=0;
    }else{
        iFusionEnabledFlag=1;
        iFusionCount=0;
        Delpwdflag=0;
    }

FUSION_IMPL:
    qDebug()<<"DelAuthMode"<<DelAuthMode;
    qDebug()<<"iFusionEnabledFlag===="<<iFusionEnabledFlag;

    if(iFusionEnabledFlag == 0 && strcmp(DelAuthMode,"F") == 0){
        //        if(rdCallinit(1) < 0){
        //            qDebug()<<"rdCallinit Failed"<<endl;
        //            return -1;
        //        }

        qDebug()<<"************with out fusion";
        if(QwadhStatus=="N"||DealerTxnFlag == 1)
        {
            if(rdCallinit(1) < 0){
                qDebug()<<"rdCallinit Failed"<<endl;
                return -1;
            }
        }
        else if(QwadhStatus=="Y" && DealerTxnFlag == 1)
        {
            if(rdCallinit(1) < 0){
                qDebug()<<"rdCallinit Failed"<<endl;
                return -1;
            }
        }
        else if(QwadhStatus=="Y" && DealerTxnFlag == 0)
        {
            qDebug()<<"Qwadh_Dealer_global"<<Qwadh_Dealer_global;
            if(rdCallinit_ekyc(1,Qwadh_Dealer_global.toAscii().data())< 0)
            {
                qDebug()<<"rdCallinit_ekyc Failed"<<endl;
                return -1;
            }
        }

    }else if(iFusionEnabledFlag == 1 && strcmp(DelAuthMode,"F") == 0 && Delpwdflag == 0){
        //        iFusionLimit = miscObj.getFusionCount();
        //        qDebug()<<"Fusion Limit is  "<<iFusionLimit;
        //        qDebug()<<"iFusionCount"<<iFusionCount;
        //        if(iFusionCount > iFusionLimit){
        //            qDebug()<<"Fusion Count Exceeded";
        //            Delpwdflag=1;
        //            goto FUSION_IMPL;
        //        }
        //        if(rdCallinit(2)< 0){
        //            qDebug()<<"rdCallinit Failed"<<endl;
        //            return -1;
        //        }


        qDebug()<<"************with  fusion";


        iFusionLimit = miscObj.getFusionCount();
        qDebug()<<"Fusion Limit is  "<<iFusionLimit;
        qDebug()<<"iFusionCount"<<iFusionCount;
        if(iFusionCount > iFusionLimit){
            qDebug()<<"Fusion Count Exceeded Dealer";
            Delpwdflag=1;
            goto FUSION_IMPL;
        }
        if(QwadhStatus=="N" ||DealerTxnFlag == 1)
        {
            if(rdCallinit(2)< 0){
                qDebug()<<"rdCallinit Failed"<<endl;
                return -1;
            }
        }
        else if(QwadhStatus=="Y" && DealerTxnFlag == 1)
        {
            if(rdCallinit(2)< 0){
                qDebug()<<"rdCallinit Failed"<<endl;
                return -1;
            }
        }
        else if(QwadhStatus=="Y" && DealerTxnFlag == 0)
        {
            qDebug()<<"Qwadh_Dealer_global"<<Qwadh_Dealer_global;
            if(rdCallinit_ekyc(1,Qwadh_Dealer_global.toAscii().data())< 0)
            {
                qDebug()<<"rdCallinit_ekyc Failed"<<endl;
                return -1;
            }
        }

        iFusionCount++;
    }else if(Delpwdflag == 1)
        return -1;

    //////////////////////
    ret = prepareAuthReqFile(FileName, AuthUid, b64Templ1, b64Templ2);
    qDebug()<<"ret form Prepare Out File : "<<ret;

    if(ret < 0){
        miscObj.AudioTest((char *)"177");

        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Finger Not Placed On Scanner...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"फिंगर स्कैनर पर रखा नहीं");

        return -2;
    }

    miscObj.AudioTest((char *)"178");
    popup_msg *dlgPopup3 = new popup_msg(this);

    if(LangMode == 1)
        dlgPopup3->ui->lblPopup->setText("Please Wait...\nAuthenticating Dealer...");
    else if(LangMode == 2)
        dlgPopup3->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..\nडीलर प्रमाणीकरण हो रहा है.."));

    dlgPopup3->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    sleep(2); //Kept For Audio

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

    qDebug()<<"????????????? FINGER print cUrlName"<<cUrlName<<endl;
    system("cat /mnt/jffs2/DealerAuth.xml");

//    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/AuthResponse.xml", SERVER_TIME_OUT);
    int ret_http_code = UrlPingCheck(cUrlName);

    if(ret_http_code == 1)
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/AuthResponse.xml", responseTimedOutTimeInSec.toInt());
    }
    else
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, PDS_URL2, "/mnt/jffs2/AuthResponse.xml", responseTimedOutTimeInSec.toInt());
    }

    qDebug() << "\nAuthenticating Shop Owner - Curl Cmd: " << cmdCurl;
    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, AUTH_SERVER) < 0){
        dlgPopup3->close();
        dlgPopup3->deleteLater();
        return -3;
    }

    dlgPopup3->close();
    dlgPopup3->deleteLater();

    qDebug() << "*****AuthOwner AuthResponse.xml*****************";
    //    system("cp /mnt/jffs2/AuthResponse_Sucess.xml /mnt/jffs2/AuthResponse.xml");
    system("cat /mnt/jffs2/AuthResponse.xml");
    qDebug() << "************************************************";

    int res = ParsingFirstCondition((char *)"/mnt/jffs2/AuthResponse.xml");

    if(res == -2)
        return -1;

    if(res < 0){
        qDebug()<<"Finger Iris Selection Value = "<<Finger_Iris_Selection;
        if(Finger_Iris_Selection == 1){
            int res = ParsingFirstConditionFusion((char *)"/mnt/jffs2/AuthResponse.xml");
            if(res == -1){
                iFusionEnabledFlag = 1;
                memset(b64Templ2, 0, sizeof(b64Templ2));
                strcpy(b64Templ2, b64Templ1);
                goto FUSION_IMPL;
                return -1;
            }else{
                iFusionEnabledFlag = 0;
            }
        }

        return -1;
    }


    if(parseAuthResponse("/mnt/jffs2/AuthResponse.xml") < 0)
        return -1;

    if(iFusionEnabledFlag == 1 && strcmp(FusionStatus,"0") == 0)
    {
        char DealerType[8];
        memset(DealerType, 0, sizeof(DealerType));
        qDebug()<<"Fusion Update To server for Dealer &&&"<<endl;
        row = ui->tableWidgetDealNomDetails->currentRow();
        qDebug()<<"Current Row : "<<row;
        if(row == 0)
        {
            strcpy(DealerType, "DEL");
        }
        else if(row == 1)
        {
            strcpy(DealerType, "REP1");
        }
        else if(row == 2)
        {
            strcpy(DealerType, "REP2");
        }
        qDebug()<<"Dealer Type for Fusion updation :"<<DealerType;

        FusionUpdateToServer(DealerType, "1", AuthUid,DealerType);
    }

    return 0;
}

int Widget::ValidatingErrors(char *filename)
{
    QFile file(filename);

    if(!file.exists())
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"No Response From Server...\nPlease Try Again");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"सर्वर प्रतिसाद नहीं दे रहा है\nकृपया पुन: प्रयास करें");

        return -1;
    }
    if(file.size() == 0)
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Server is Busy...\nPlease Try Again..."); //Empty Response From Server
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"सर्वर व्यस्त है ...\nकृपया पुनः प्रयास करें ..");

        return -1;
    }

    file.open(QIODevice::ReadOnly);
    QString s;

    QTextStream s1(&file);
    s.append(s1.readAll());
    if(s.contains("String index out of range", Qt::CaseInsensitive) == true)
    {
        miscObj.DisplayWideMsgBox((char *)"String index out of range - Invalid Response");
        file.close();
        return -1;
    }
    else if(s.contains("404 error page", Qt::CaseInsensitive) == true)
    {
        miscObj.DisplayWideMsgBox((char *)"Server is Not Active");
        file.close();
        return -1;
    }
    else if(s.contains("404 Not Found", Qt::CaseInsensitive) == true)
    {
        miscObj.DisplayWideMsgBox((char *)"404 Not Found");
        file.close();
        return -1;
    }
    else if(s.contains("Server error", Qt::CaseInsensitive) == true)
    {
        miscObj.DisplayWideMsgBox((char *)"Server error - Invalid Response");
        file.close();
        return -1;
    }
    else if(s.contains("java.lang.NullPointerException", Qt::CaseInsensitive) == true)
    {
        miscObj.DisplayWideMsgBox((char *)"Please Call PMU");
        file.close();
        return -1;
    }
    else if(s.contains("HTTP Status 404", Qt::CaseInsensitive) == true)
    {
        miscObj.DisplayWideMsgBox((char *)"Server is Not Active");
        file.close();
        return -1;
    }
    else if(s.contains("does not match an operation", Qt::CaseInsensitive) == true)
    {
        miscObj.DisplayWideMsgBox((char *)"Please Call PMU");
        file.close();
        return -1;
    }
    else if(s.contains("faultstring", Qt::CaseInsensitive) == true)
    {
        miscObj.DisplayWideMsgBox((char *)"Fault string in Response");
        file.close();
        return -1;
    }//faultstring
    file.close();
    return 0;
}

int Widget::AuthBen()
{
    int ret = 0;
    qDebug()<<"We are at AuthBen() ...............";

    GPRS objGprs;
    if(objGprs.Check_Network_Available() < 0)
    {
        CheckOfflineTxnScanFp();
        return -1;
    }

    char FileName[64];
    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/DealerAuth.xml");

    system("rm /mnt/jffs2/DealerAuth.xml");
    system("rm /mnt/jffs2/AuthResponse.xml");

    char b64Templ1[2048];
    memset(b64Templ1, 0, sizeof(b64Templ1));

    char b64Templ2[2048];
    memset(b64Templ2, 0, sizeof(b64Templ2));

    int iFusionCount = 0;
    int iFusionLimit = 0;

    getFusionStatus();
    qDebug()<<"FusionStatus"<<FusionStatus<<endl;

    member_wadhstatus.clear();
    member_wadhstatus=ui->twdgtEntitlement->item(ui->twdgtEntitlement->currentRow(),8)->text();

    if(strcmp(FusionStatus,"0") == 0)
    {
        iFusionEnabledFlag=0;
        iFusionCount=1;
        memekycflag = 0;
    }
    else if(strcmp(FusionStatus ,"1") == 0)
    {
        iFusionEnabledFlag=1;
        iFusionCount=0;
        memekycflag=0;
    }

FUSION_IMPL:

    popup_msg *dlgPopupFusion = new popup_msg(this);

    qDebug()<<"BenAuthMode_Finger at AuthBen():"<<BenAuthMode_Finger<<endl;

    if(iFusionEnabledFlag == 0)
    {
        //        memekycflag=0;
        //        if(rdCallinit(1) < 0)
        //        {
        //            qDebug()<<"rdCallinit Failed"<<endl;
        //            return -1;
        //        }


        memekycflag=0;
        if(member_wadhstatus=="N")
        {
            if(rdCallinit(1) < 0)
            {
                qDebug()<<"rdCallinit Failed"<<endl;
                return -1;
            }
        }
        if(member_wadhstatus=="Y")
        {

            if(rdCallinit_ekyc(1,Qwadh_Member_global.toAscii().data())< 0)
            {
                qDebug()<<"rdCallinit_ekyc Failed"<<endl;

                return -1;
            }
        }


    }
    else if(iFusionEnabledFlag == 1 && strcmp(BenAuthMode_Finger,"Y") == 0 && memekycflag == 0)
    {
        //memmobotpflag=0;
        //        memekycflag=0;
        //        qDebug()<<"Fusion Count = "<<iFusionCount;
        //        iFusionLimit = miscObj.getFusionCount();
        //        qDebug()<<"Fusion Limit : "<<iFusionLimit;

        //        if(iFusionCount > iFusionLimit)
        //        {
        //            qDebug()<<"Fusion Count Exceeded";
        //            memekycflag=1;

        //            goto FUSION_IMPL;
        //        }
        //        memmobotpflag=0;
        //        if(rdCallinit(2)< 0)
        //        {
        //            qDebug()<<"rdCallinit Failed"<<endl;
        //            return -1;
        //        }

        memekycflag=0;
        qDebug()<<"Fusion Count = "<<iFusionCount;
        iFusionLimit = miscObj.getFusionCount();
        qDebug()<<"Fusion Limit : "<<iFusionLimit;

        if(iFusionCount > iFusionLimit)
        {
            qDebug()<<"Fusion Count Exceeded Benficiary";
            memekycflag=1;

            goto FUSION_IMPL;
        }
        memmobotpflag=0;

        if(member_wadhstatus=="N")
        {
            if(rdCallinit(2)< 0)
            {
                qDebug()<<"rdCallinit Failed"<<endl;
                return -1;
            }
        }
        if(member_wadhstatus=="Y")
        {
            if(rdCallinit_ekyc(1,Qwadh_Member_global.toAscii().data())< 0)
            {
                qDebug()<<"rdCallinit_ekyc Failed"<<endl;
                return -1;
            }
        }
        iFusionCount++;
    }
    else
    {
        qDebug()<<" &&&&&&&& fault String Else &&&&&&&&&&"<<endl;
        //        return -1;
    }

    if(memekycflag == 1 && Qtxnflow == "F")//
    {
        qDebug()<<" &&&&&&&& Going farward &&&&&&&&&&"<<Qtxnflow<<endl;
        qDebug()<<"Going for EKYC"<<endl;
        return 0;
    }
    else if(memekycflag == 1 && Qtxnflow == "B")
    {
        qDebug()<<" &&&&&&&& Going Back &&&&&&&&&&"<<Qtxnflow<<endl;
        return -2;
    }
    else if(memekycflag == 2)
    {
        qDebug()<<"Going for OTP Based AUTH"<<endl;
        return 0;
    }

    if(iMainTransaction == NCR_TRANSACTION)
        ret = prepareNCR_AuthReqFile(FileName, MemberUID, b64Templ1, b64Templ2);
    else
        ret = prepareAuthReqFile(FileName, MemberUID, b64Templ1, b64Templ2);

    if(ret < 0)
    {
        if(getFusionStatus() != 0)
        {
            qDebug()<<"dlgpopup2 closed.";
        }
        if(iFusionEnabledFlag == 1)
        {
            qDebug()<<"dlgPopupFusion closed";
            dlgPopupFusion->close();
            dlgPopupFusion->deleteLater();
        }
        else
        {
            qDebug()<<"dlgpopup2 closed..";
        }
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Finger Not Placed on Scanner");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"उंगली स्कैनर पर रखी नहीं");
        return -1;
    }
    qDebug()<<"ret form Prepare Out File : "<<ret;

    if(getFusionStatus() != 0)
    {
        qDebug()<<"dlgpopup2 closed...";
    }
    if(iFusionEnabledFlag == 1)
    {
        qDebug()<<"dlgPopupFusion closed.";
        dlgPopupFusion->close();
        dlgPopupFusion->deleteLater();
    }
    else
    {
        qDebug()<<"dlgpopup2 closed....";
    }
    popup_msg *dlgPopup1 = new popup_msg(this);
    if(iDealMemSelected == MEMBER_SELECTED||iDealMemSelected == MDM_TRANSACTION)
    {
        if((iMainTransaction == PDS_TRANSACTION)||((iMainTransaction == MDM_TRANSACTION) ))
        {
            miscObj.AudioTest((char *)"187");
            if(LangMode == 1)
                dlgPopup1->ui->lblPopup->setText("Please Wait...\nAuthenticating Finger Print...");
            else if(LangMode == 2)
                dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.\nउंगली की छाप प्रमाणित हो रहा है."));

        }
    }
    else if(iDealMemSelected == ROUTE_OFFICER_SELECTED)
    {
        if(LangMode == 1)
            dlgPopup1->ui->lblPopup->setText("Please Wait...\nAuthenticating Finger Print...");
        else if(LangMode == 2)
            dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.\nउंगली की छाप प्रमाणित हो रहा है."));

    }
    else if(iDealMemSelected == DEALER_SELECTED)
    {
        if(LangMode == 1)
            dlgPopup1->ui->lblPopup->setText("Please Wait...\nAuthenticating Dealer...");
        else if(LangMode == 2)
            dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..\n विक्रेता प्रमाणीकरण हो रहा है."));
    }

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

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

    if(iMainTransaction == NCR_TRANSACTION)
    {
//        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/AuthResponse.xml", SERVER_TIME_OUT);

        int ret_http_code = UrlPingCheck(cUrlName);
        if(ret_http_code == 1)
        {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/AuthResponse.xml", responseTimedOutTimeInSec.toInt());
        }
        else
        {
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName,PDS_URL2, "/mnt/jffs2/AuthResponse.xml", responseTimedOutTimeInSec.toInt());
        }
    }
    else
    {
//        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/AuthResponse.xml", SERVER_TIME_OUT);
        int ret_http_code = UrlPingCheck(cUrlName);
        if(ret_http_code == 1)
        {
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/AuthResponse.xml", responseTimedOutTimeInSec.toInt());
        }
        else {
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName,PDS_URL2, "/mnt/jffs2/AuthResponse.xml", responseTimedOutTimeInSec.toInt());
        }
    }

    qDebug()<<"\nCurl Cmd : "<<cmdCurl;
    int systemRet = system(cmdCurl);

    if(SystemCurlResponse(systemRet, AUTH_SERVER) < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        CheckOfflineTxnScanFp();
        return -1;
    }

    qDebug() << "*****************AuthResponse.xml****************";
    //    system("cp /mnt/jffs2/AuthResponse2.xml /mnt/jffs2/AuthResponse.xml");
    system("cat /mnt/jffs2/AuthResponse.xml");
    qDebug() << "*************************************************";

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    if(ParsingFirstCondition((char *)"/mnt/jffs2/AuthResponse.xml") < 0)
    {
        qDebug()<<"Finger Iris Selection Value = "<<Finger_Iris_Selection;

        if(Finger_Iris_Selection == 0)//Only for Finger Print
        {
            int ret = ParsingFirstConditionFusion("/mnt/jffs2/AuthResponse.xml");
            if(ret == -1)
            {
                iFusionEnabledFlag = 1;
                memset(b64Templ2, 0, sizeof(b64Templ2));
                strcpy(b64Templ2, b64Templ1);
                //PDS_TRANSACTION ICDS_TRANSACTION PDS_CASHLESS_TRANSACTION FEST_TRANSACTION MDM_TRANSACTION HAWKER_TRANSACTION HAWKER_DOWNLOADING RCV_GOODS_TRANSACTION DEALER_DOWNLOADING

                qDebug() << "After ParsingFirstConditionFusion - iMainTransaction ::" <<iMainTransaction;
                //if((iMainTransaction == PDS_TRANSACTION)||(iMainTransaction == MDM_TRANSACTION))
                //                if(iMainTransaction == PDS_TRANSACTION || iModuleType == STOCK_ENTRY)
                if(iMainTransaction == PDS_TRANSACTION)
                {
                    goto FUSION_IMPL;
                }
                if(iMainTransaction == MDM_TRANSACTION)
                {
                    return -1;
                }
            }
            else if(ret == -2)
            {
                iFusionEnabledFlag = 0;

                if(Qtxnflow == "F")
                {
                    if (eKYCPrompt != "Y")
                        return -1;
                    if(EYC_Manual_Calling()< 0)
                        return -1;
                }
                else if(Qtxnflow =="B")//added by subbu 16022019
                {
                    return -2;
                }
                return 0;
            }
            else
            {
                iFusionEnabledFlag = 0;
                iFusionEnabledFlag = 3;

                if(Qtxnflow == "B")
                {
                    qDebug()<<"Qtxnflow back"<<Qtxnflow;
                    return -2;
                }
            }
        }
        return -1;
    }

    if(parseAuthResponse("/mnt/jffs2/AuthResponse.xml") < 0)
        return -1;

    if(iFusionEnabledFlag == 1 && strcmp(FusionStatus,"0") == 0)
    {
        qDebug()<<"Fusion Update To server";
        FusionUpdateToServer("MEM", "1", MemberUID,ui->twdgtEntitlement->item(ui->twdgtEntitlement->currentRow(),6)->text().toAscii().data());
    }

    return 0;
}

int Widget::AuthBenRecvGoods()
{
    int ret = 0;
    qDebug()<<"We are at AuthBenRecvGoods() ...............";

    GPRS objGprs;
    if(objGprs.Check_Network_Available() < 0)
    {
        CheckOfflineTxnScanFp();
        return -1;
    }

    char FileName[64];
    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/DealerAuth.xml");

    system("rm /mnt/jffs2/DealerAuth.xml");
    system("rm /mnt/jffs2/AuthResponse.xml");

    char b64Templ1[2048];
    memset(b64Templ1, 0, sizeof(b64Templ1));

    char b64Templ2[2048];
    memset(b64Templ2, 0, sizeof(b64Templ2));

    int iFusionCount = 0;
    int iFusionLimit = 0;

    getFusionStatus();
    qDebug()<<"FusionStatus"<<FusionStatus<<endl;

    if(strcmp(FusionStatus,"0") == 0)
    {
        iFusionEnabledFlag=0;
        iFusionCount=1;
        memekycflag = 0;
    }
    else if(strcmp(FusionStatus ,"1") == 0)
    {
        iFusionEnabledFlag=1;
        iFusionCount=0;
        memekycflag=0;
    }

    QwadhStatus= "N";

FUSION_IMPL:

    popup_msg *dlgPopupFusion = new popup_msg(this);

    qDebug()<<"BenAuthMode_Finger at AuthBenRecvGoods():"<<BenAuthMode_Finger<<"---"<<iFusionEnabledFlag<<"---"<<memekycflag<<endl;

    if(iFusionEnabledFlag == 0)
    {
        memekycflag=0;
        if(rdCallinit(1) < 0)
        {
            qDebug()<<"rdCallinit Failed"<<endl;
            return -1;
        }
    }
    //    else if(iFusionEnabledFlag == 1 && strcmp(BenAuthMode_Finger,"Y") == 0 && memekycflag == 0)
    else if(iFusionEnabledFlag == 1 && memekycflag == 0)
    {
        //memmobotpflag=0;
        memekycflag=0;
        qDebug()<<"Fusion Count = "<<iFusionCount;
        iFusionLimit = miscObj.getFusionCount();
        qDebug()<<"Fusion Limit : "<<iFusionLimit;

        if(iFusionCount > iFusionLimit)
        {
            qDebug()<<"Fusion Count Exceeded";
            memekycflag=1;

            goto FUSION_IMPL;
        }
        memmobotpflag=0;
        if(rdCallinit(2)< 0)
        {
            qDebug()<<"rdCallinit Failed"<<endl;
            return -1;
        }
        iFusionCount++;
    }
    else
    {
        qDebug()<<" &&&&&&&& fault String Else &&&&&&&&&&"<<endl;
        //        return -1;
    }

    if(memekycflag == 1 && Qtxnflow == "F")//
    {
        qDebug()<<" &&&&&&&& Going farward &&&&&&&&&&"<<Qtxnflow<<endl;
        qDebug()<<"Going for EKYC"<<endl;
        return -2;
    }
    else if(memekycflag == 1 && Qtxnflow == "B")
    {
        qDebug()<<" &&&&&&&& Going Back &&&&&&&&&&"<<Qtxnflow<<endl;
        return -2;
    }
    else if(memekycflag == 2)
    {
        qDebug()<<"Going for OTP Based AUTH"<<endl;
        return 2;
    }

    if(iMainTransaction == NCR_TRANSACTION)
        ret = prepareNCR_AuthReqFile(FileName, MemberUID, b64Templ1, b64Templ2);
    else
        ret = prepareAuthReqFile(FileName, MemberUID, b64Templ1, b64Templ2);

    if(ret < 0)
    {
        if(getFusionStatus() != 0)
        {
            qDebug()<<"dlgpopup2 closed.";
        }
        if(iFusionEnabledFlag == 1)
        {
            qDebug()<<"dlgPopupFusion closed";
            dlgPopupFusion->close();
            dlgPopupFusion->deleteLater();
        }
        else
        {
            qDebug()<<"dlgpopup2 closed..";
        }
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Finger Not Placed on Scanner");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"उंगली स्कैनर पर रखी नहीं");
        else if(LangMode == 3)
            miscObj.tel_DisplayWideMsgBox((char *)"স্ক্যানারে আঙুল রাখেনি");
        return -1;
    }
    qDebug()<<"ret form Prepare Out File : "<<ret;

    if(getFusionStatus() != 0)
    {
        qDebug()<<"dlgpopup2 closed...";
    }
    if(iFusionEnabledFlag == 1)
    {
        qDebug()<<"dlgPopupFusion closed.";
        dlgPopupFusion->close();
        dlgPopupFusion->deleteLater();
    }
    else
    {
        qDebug()<<"dlgpopup2 closed....";
    }
    popup_msg *dlgPopup1 = new popup_msg(this);
    if(iDealMemSelected == MEMBER_SELECTED||iDealMemSelected == MDM_TRANSACTION)
    {
        if((iMainTransaction == PDS_TRANSACTION)||((iMainTransaction == MDM_TRANSACTION) ))
        {
            miscObj.AudioTest((char *)"187");
            if(LangMode == 1)
                dlgPopup1->ui->lblPopup->setText("Please Wait...\nAuthenticating Finger Print...");
            else if(LangMode == 2)
                dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.\nउंगली की छाप प्रमाणित हो रहा है."));
            else if(LangMode == 3)
                dlgPopup1->ui->lblPopup->setText(trUtf8("দয়া করে অপেক্ষা করুন ...\nআঙুলের মুদ্রণ প্রমাণীকরণ করা হচ্ছে ..."));

        }
    }
    else if(iDealMemSelected == ROUTE_OFFICER_SELECTED)
    {
        if(LangMode == 1)
            dlgPopup1->ui->lblPopup->setText("Please Wait...\nAuthenticating Finger Print...");
        else if(LangMode == 2)
            dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.\nउंगली की छाप प्रमाणित हो रहा है."));
        else if(LangMode == 3)
            dlgPopup1->ui->lblPopup->setText(trUtf8("দয়া করে অপেক্ষা করুন ..\nআঙুলের মুদ্রণ প্রমাণীকরণ করা হচ্ছে ..."));

    }
    else if(iDealMemSelected == DEALER_SELECTED)
    {
        if(LangMode == 1)
            dlgPopup1->ui->lblPopup->setText("Please Wait...\nAuthenticating Dealer...");
        else if(LangMode == 2)
            dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..\n विक्रेता प्रमाणीकरण हो रहा है."));
        else if(LangMode == 3)
            dlgPopup1->ui->lblPopup->setText(trUtf8("দয়া করে অপেক্ষা করুন ..\nআঙুলের মুদ্রণ প্রমাণীকরণ করা হচ্ছে ..."));
    }

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

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

    if(iMainTransaction == NCR_TRANSACTION)
    {
        int ret_http_code = UrlPingCheck(cUrlName);

        if(ret_http_code == 1)
        {
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/AuthResponse.xml", responseTimedOutTimeInSec.toInt());
        }
        else {
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, PDS_URL2, "/mnt/jffs2/AuthResponse.xml", responseTimedOutTimeInSec.toInt());
        }
    }
    else
    {
        int ret_http_code = UrlPingCheck(cUrlName);

        if(ret_http_code == 1)
        {
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/AuthResponse.xml", responseTimedOutTimeInSec.toInt());
        }
        else
        {
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, PDS_URL2, "/mnt/jffs2/AuthResponse.xml", responseTimedOutTimeInSec.toInt());
        }
    }

    qDebug()<<"\nCurl Cmd : "<<cmdCurl;
    int systemRet = system(cmdCurl);

    if(SystemCurlResponse(systemRet, AUTH_SERVER) < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        CheckOfflineTxnScanFp();
        return -1;
    }

    qDebug() << "*****************AuthResponse.xml****************";
    //    system("cp /mnt/jffs2/AuthResponse2.xml /mnt/jffs2/AuthResponse.xml");
    system("cat /mnt/jffs2/AuthResponse.xml");
    qDebug() << "*************************************************";

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    if(ParsingFirstCondition((char *)"/mnt/jffs2/AuthResponse.xml") < 0)
    {
        qDebug()<<"Finger Iris Selection Value = "<<Finger_Iris_Selection;

        if(Finger_Iris_Selection == 0)//Only for Finger Print
        {
            int ret = ParsingFirstConditionFusion("/mnt/jffs2/AuthResponse.xml");
            if(ret == -1)
            {
                iFusionEnabledFlag = 1;
                memset(b64Templ2, 0, sizeof(b64Templ2));
                strcpy(b64Templ2, b64Templ1);
                //PDS_TRANSACTION ICDS_TRANSACTION PDS_CASHLESS_TRANSACTION FEST_TRANSACTION MDM_TRANSACTION HAWKER_TRANSACTION HAWKER_DOWNLOADING RCV_GOODS_TRANSACTION DEALER_DOWNLOADING

                qDebug() << "After ParsingFirstConditionFusion - iMainTransaction ::" <<iMainTransaction;
                //if((iMainTransaction == PDS_TRANSACTION)||(iMainTransaction == MDM_TRANSACTION))
                //                if(iMainTransaction == PDS_TRANSACTION || iModuleType == STOCK_ENTRY)
                if(iMainTransaction == PDS_TRANSACTION)
                {
                    goto FUSION_IMPL;
                }
                if(iMainTransaction == MDM_TRANSACTION)
                {
                    return -1;
                }
            }
            else if(ret == -2)
            {
                iFusionEnabledFlag = 0;

                if(Qtxnflow == "F")
                {
                    //                    if (eKYCPrompt != "Y")
                    //                        return -1;
                    //                    if(EYC_Manual_Calling()< 0)
                    return -1;
                }
                else if(Qtxnflow =="B")//added by subbu 16022019
                {
                    return -2;
                }
                else
                    return -2;
            }
            else
            {
                iFusionEnabledFlag = 0;
                iFusionEnabledFlag = 3;

                if(Qtxnflow == "B")
                {
                    qDebug()<<"Qtxnflow back"<<Qtxnflow;
                    return -2;
                }
            }
        }
        return -1;
    }

    if(parseAuthResponse("/mnt/jffs2/AuthResponse.xml") < 0)
        return -1;

    //    if(iFusionEnabledFlag == 1 && strcmp(FusionStatus,"0") == 0)
    //    {
    //        qDebug()<<"Fusion Update To server";
    //        FusionUpdateToServer("MEM", "1", MemberUID,ui->twdgtEntitlement->item(ui->twdgtEntitlement->currentRow(),6)->text().toAscii().data());
    //    }

    return 0;
}


void Widget::on_btnLogout_clicked()
{
    /*
    ui->ldtShpId->setFocus();
    //PartialOnlineTimer->stop();
    if(DealerLoginBy == 2)
            PartialOnlineTimer->stop();
    ui->stackedWidget->setCurrentIndex(0);
    */

    if(DealerLoginBy ==2)
    {
        ui->ldtShpId->setFocus();
        PartialOnlineTimer->stop();
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }
    else
    {
        nLogout=1;
        logout();
        // PartialOnlineTimer->stop();
        if(partialOnlineOfflineStatus.toUpper() == "Y")
        {
            UploadData.terminate(); //thread stop
        }
        ui->ldtShpId->setFocus();
        ui->stackedWidget->setCurrentIndex(0);

        return;
    }
    return;
}

void Widget::on_btnUpdateSW_clicked()
{
    ui->stackedWidget->setCurrentIndex(17);
    return;
}

void Widget::prepareBmp(QString text, int mode, QString dummystr, int iFontSize, int iAlign)
{

    QBitmap bitmap(384,mode);
    QFont font("Verdana",iFontSize);
    font.setPointSize(iFontSize);//Tarak
    bitmap.fill(Qt::white);

    int multiplier =1;
    int len=text.length();
    int temp_len;
    temp_len=len;
    //21:normal, 22:Bold, 23:Double Width, 24:Double Height, 25:Condensed, 26:reverse video on, 27:reverse video off
    switch(mode)
    {
    case 20:while(1)
        {
            temp_len-=32;
            if(temp_len>0)
                multiplier+=1;
            else
            {
                break;
            }
        }
        bitmap.fill(Qt::black);
        break;
    case 21:while(1)
        {
            temp_len-=32;
            if(temp_len>0)
                multiplier+=1;
            else
            {
                break;
            }
        }
        break;

    case 22:
        while(1)
        {
            //temp_len-=32;
            temp_len-=62;
            if(temp_len>0)
                multiplier+=1;
            else
            {
                break;
            }
        }
        bitmap.fill(Qt::black);
        font.setStretch(QFont::ExtraCondensed);
        break;
    case 23:
        while(1)
        {
            temp_len-=20;
            if(temp_len>0)
                multiplier+=1;
            else
            {
                break;
            }
        }

        font.setStretch(QFont::ExtraExpanded);
        break;
    case 24:while(1)
        {
            temp_len-=24;
            if(temp_len>0)
                multiplier+=1;
            else
            {
                //multiplier=1;
                break;
            }
        }

        font.setPointSize(25);
        break;
    case 25:while(1)
        {
            temp_len-=42;
            if(temp_len>0)
                multiplier+=1;
            else
            {
                //multiplier=1;
                break;
            }
        }

        font.setStretch(QFont::Condensed);
        break;
    case 26:while(1)
        {
            temp_len-=32;
            if(temp_len>0)
                multiplier+=1;
            else
            {
                //multiplier=1;
                break;
            }
        }
        bitmap.fill(Qt::black);

        font.setStretch(QFont::Condensed);
        //        font.setStretch(QFont::StretchResolved);

        break;
    case 27:while(1)
        {
            temp_len-=22;
            if(temp_len>0)
                multiplier+=1;
            else
            {
                //multiplier=1;
                break;
            }
        }
        bitmap.fill(Qt::black);

        font.setStretch(QFont::Condensed);
        font.setStretch(QFont::StretchResolved);

        break;

    case 28:while(1)
        {
            temp_len-=42;
            if(temp_len>0)
                multiplier+=1;
            else
            {
                //multiplier=1;
                break;
            }
        }
        bitmap.fill(Qt::black);

        font.setStretch(QFont::Condensed);
        font.setStretch(QFont::StretchResolved);

        break;


    default:break;

    }
    QBitmap bit_scaled=bitmap.scaled(384,(mode*multiplier));
    QPainter painter(&bit_scaled);
    painter.setPen(Qt::white);
    painter.setFont(font);

    if(iAlign == ALIGN_CENTER)
    {
        painter.drawText(0,0,384,(25*multiplier),Qt::TextWrapAnywhere|Qt::AlignCenter|Qt::AlignTop, dummystr);
    }
    else if(iAlign == ALIGN_LEFT)
    {
        painter.drawText(0,0,384,(25*multiplier),Qt::TextWrapAnywhere|Qt::AlignLeft|Qt::AlignTop, dummystr);
    }
    else if(iAlign == ALIGN_RIGHT)
    {
        painter.drawText(0,0,384,(25*multiplier),Qt::TextWrapAnywhere|Qt::AlignRight|Qt::AlignTop, dummystr);
    }

    painter.save();
    bit_scaled.save("/mnt/jffs2/sample.bmp");
    painter.restore();

    LogoPrint objPrn;
    objPrn.PrintBmpText();

}

void Widget::on_comboTypeOfCard_activated(int index)
{
    qDebug() << "%%%%%%%%%%%index = " << index;
    if(index > 0)
        miscObj.AudioTest((char *)"43");

    char buff[64];
    memset(buff, 0, sizeof(buff));
    if(ui->comboTypeOfCard->currentIndex() != 0)
    {
        sprintf(buff, "%s", ui->comboTypeOfCard->currentText().toAscii().data());
    }
    else
        ui->ldtRationCard->clear();

    ui->ldtRationCard->setText(buff);
    ui->ldtRationCard->setFocus();
    return;
}


int Widget::getFusionStatus()
{
    QSqlQuery query;
    memset(FusionStatus,0,sizeof(FusionStatus));

    qDebug()<<"Memnber UID for Fusion : "<<MemberUID;
    query.prepare("SELECT MemberFusion FROM CustomerFamily WHERE MemberUID = ?");
    query.addBindValue(MemberUID);
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            memset(FusionStatus,0,sizeof(FusionStatus));
            strcpy(FusionStatus,query.value(0).toString().toAscii().constData());
        }
    }

    qDebug()<<"FusionStatus : "<<FusionStatus<<"####";

    if(strcmp(FusionStatus , "1") == 0)
    {
        qDebug()<<"Fusion is there";
        return 1;
    }
    else
    {
        strcpy(FusionStatus , "0");
        qDebug()<<"Fusion is not there";
    }
    return 0;
}

int Widget::getDealerFusionStatus(char *DealerUID)
{
    memset(FusionStatus,0,sizeof(FusionStatus));
    int iDealerCount;

    QSqlQuery query;
    query.prepare("SELECT COUNT(fpsuidno) FROM Dealer WHERE fpsuidno = ?");
    query.addBindValue(DealerUID);
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            iDealerCount = query.value(0).toInt(0);
        }
    }
    query.clear();

    memset(FusionStatus,0,sizeof(FusionStatus));
    if(iDealerCount == 1||iDealerCount == 2||iDealerCount == 3)
    {
        query.prepare("SELECT dealer_fusion FROM Dealer WHERE fpsuidno = ?");
        query.addBindValue(DealerUID);
        if(!query.exec())
            qDebug() << query.lastError();
        else
        {
            while(query.next())
            {
                memset(FusionStatus,0,sizeof(FusionStatus));
                strcpy(FusionStatus,query.value(0).toString().toAscii().constData());
            }
        }
        query.clear();
    }
    qDebug()<<"FusionStatus : "<<FusionStatus<<"####";

    if(strcmp(FusionStatus , "1") == 0)
    {
        qDebug()<<"Fusion is there";
        return 1;
    }
    else
    {
        strcpy(FusionStatus , "0");
        qDebug()<<"Fusion is not there";
    }
    qDebug()<<"Fusion is not there";
    return 0;
}


int Widget::getUIDStatus()
{
    QSqlQuery query;
    char UID_Status[256];
    char szMemberName[128];
    QString QMemberName;

    qDebug()<<"Memnber UID to Know its Status : "<<MemberUID;

    query.prepare("SELECT MemberNameEn, UID_Status FROM CustomerFamily WHERE MemberUID = ?");
    query.addBindValue(MemberUID);
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            memset(szMemberName,0,sizeof(szMemberName));
            strcpy(szMemberName,query.value(0).toString().toAscii().constData());

            QMemberName = query.value(0).toString();
            memset(UID_Status,0,sizeof(UID_Status));
            strcpy(UID_Status,query.value(1).toString().toAscii().constData());

            qDebug()<<"QMember Name : "<<QMemberName<<"####";
            qDebug()<<"Member Name  : "<<szMemberName<<"####";
            qDebug()<<"UID Status   : "<<UID_Status<<"####";
        }
    }
    if(QMemberName.contains("[SUS]"))
    {
        qDebug()<<"[SUS]";
        miscObj.DisplayWideMsgBox(UID_Status);
        return -1;
    }
    else if(QMemberName.contains("[DUP]"))
    {
        qDebug()<<"[DUP]";
        miscObj.DisplayWideMsgBox(UID_Status);
        return -2;
    }

    return 0;
}

int Widget::GetDealerAuthFileName(char *filename)
{
    char mid[16];
    char RTC[32], DATE[16], TIME[16], JULIAN_DAYS[8];

    memset(RTC, 0, sizeof(RTC));
    memset(DATE, 0, sizeof(DATE));
    memset(TIME, 0, sizeof(TIME));
    memset(JULIAN_DAYS, 0, sizeof(JULIAN_DAYS));

    GetRTC_Params(RTC, DATE, TIME, JULIAN_DAYS);

    memset(mid, 0, sizeof(mid));
    if(miscObj.GetMachineId(mid) < 0)
        return -1;
    else if(strlen(mid) <= 0)
        strcpy(mid, "1234");

    sprintf(filename, "/mnt/jffs2/DealerAuth.xml");

    return 0;
}

int Widget::GetCardHolderFileName(char *filename)
{
    char mid[16];
    char RTC[32], DATE[16], TIME[16], JULIAN_DAYS[8];


    memset(RTC, 0, sizeof(RTC));
    memset(DATE, 0, sizeof(DATE));
    memset(TIME, 0, sizeof(TIME));
    memset(JULIAN_DAYS, 0, sizeof(JULIAN_DAYS));

    GetRTC_Params(RTC, DATE, TIME, JULIAN_DAYS);

    memset(mid, 0, sizeof(mid));
    if(miscObj.GetMachineId(mid) < 0)
        return -1;
    else if(strlen(mid) <= 0)
        strcpy(mid, "1234");


    sprintf(filename, "/mnt/jffs2/RC_%s_%s_%s.xml", mid, DATE, TIME);
    qDebug()<<"File Name: "<<filename;


    return 0;
}

int Widget::prepareDealerReqFile(char *FileName)
{
    QFile file(FileName);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug()<< FileName << "File Open Failed...";
        return -1;
    }

    QTextStream out(&file);

    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));

    miscObj.GetMachineId(cmachineID);

    char szSimNo[64];
    memset(szSimNo, 0, sizeof(szSimNo));
    ReadSimSerialNo(szSimNo);
    qDebug()<<"@@@@"<<szSimNo;

    // Calculating checksum of present binary in device
    system("md5sum /usr/bin/commonpds > /mnt/jffs2/checksum.txt");

    QFile file1("/mnt/jffs2/checksum.txt");
    if(!file1.open(QIODevice::ReadOnly)){
        qDebug() << "error opening file: " << file1.error();
        return -1;
    }

    QTextStream instream(&file1);
    QString checksum = instream.readLine();
    file1.close();

    char buff[4096];
    memset(buff, 0, sizeof(buff));

    sprintf(buff, "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getPDSFpsNoDetails><VersionNo>%s</VersionNo><deviceID>%s</deviceID><token>%s</token><key>%s</key><simID>%s</simID><checkSum>%s</checkSum><longtude>%s</longtude><latitude>%s</latitude><vendorId>%s</vendorId><simStatus>%s</simStatus></ns1:getPDSFpsNoDetails></SOAP-ENV:Body></SOAP-ENV:Envelope>", NEW_VERSION,cmachineID, HD_PASSWORD_INIT, HD_HTS_INIT, szSimNo,checksum.remove("/usr/bin/commonpds").toAscii().data(),"","","","");
    out<<buff;
    file.close();

    return 0;
}


int Widget::prepareAuthReqFile(char *FileName, char *AuthUid, char *b64Templ1, char *b64Templ2)
{
    qDebug()<< "Inside prepareAuthReqFile"<<"Out File (" <<FileName << ") Preperation ....";

    char cmachineID[16],AuthType[8]="";
    memset(cmachineID, 0, sizeof(cmachineID));
    memset(AuthType,0,sizeof(AuthType));

    miscObj.GetMachineId(cmachineID);

    char buff[4096];
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "rm %s", FileName);
    system(buff);

    QFile file(FileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug()<< FileName << " file Open failed";
        return -1;
    }

    QTextStream out(&file);

    memset(buff, 0, sizeof(buff));

    qDebug()<<"#################1############";

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0){
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
        if(ui->stackedWidget->currentIndex() == 23) //receive goods Authentication page
        {
            strcpy(cDealMembuff, ui->ldtShpId->text().toAscii().data());
            strcpy(authMode,ui->tableWidgetRecieveGoodsAuth->item(ui->tableWidgetRecieveGoodsAuth->currentRow(), 4)->text().toAscii().data());
        }
        else
        {
            strcpy(cDealMembuff, ui->ldtShpId->text().toAscii().data());
            strcpy(authMode,ui->tableWidgetDealNomDetails->item(ui->tableWidgetDealNomDetails->currentRow(), 6)->text().toAscii().data());
        }

    }
    else if(iDealMemSelected == MEMBER_SELECTED) //subbu test 06/11/2017
    {
        strcpy(cDealMembuff,ui->lblRCNO_Val->text().toAscii().data());
        strcpy(authMode, "B");
    }
    else if(iDealMemSelected == INSPECTOR_SELECTED)
    {
        strcpy(cDealMembuff, ui->ldtShpId->text().toAscii().data());
        strcpy(authMode, "V");
    }
    else if(iDealMemSelected == MDM_TRANSACTION)
    {
        strcpy(cDealMembuff,ui->lblRCNO_Val->text().toAscii().data());
        strcpy(authMode, "S");
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
    fprintf(fp,"%s","<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    fprintf(fp,"%s","<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://www.uidai.gov.in/authentication/uid-auth-request/2.0\" xmlns:ns2=\"http://service.fetch.rationcard/\">");
    fprintf(fp,"%s","<SOAP-ENV:Body>");
    fprintf(fp,"%s","<ns2:getAuthenticateNICAuaAuthRD2>");
    fprintf(fp,"<fpsSessionId>%s</fpsSessionId>",SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp,"<Shop_no>%s</Shop_no>",szShopId);
    //    fprintf(fp, "<User_Id>%s</User_Id>", cDealMembuff);
    fprintf(fp, "<uidNumber>%s</uidNumber>", AuthUid);
    fprintf(fp, "<udc>%s</udc>", cmachineID);

    if(DealerTxnFlag == 1){
        strcpy(authMode,ui->tableWidgetDealNomDetails->item(ui->tableWidgetDealNomDetails->currentRow(), 6)->text().toAscii().data());

        qDebug() << "Authenticaiton Mode::" << authMode;

        if(strcmp(authMode,"D") == 0)
            fprintf(fp, "<authMode>%s</authMode>", "DEL");
        else if(strcmp(authMode,"N1") == 0)
            fprintf(fp, "<authMode>%s</authMode>", "NOM1");
        else
            fprintf(fp, "<authMode>%s</authMode>", "NOM2");

        fprintf(fp, "<User_Id>%s</User_Id>", ui->lblRCNO_Val->text().toAscii().data());
    }
    else
    {
        fprintf(fp, "<authMode>%s</authMode>", authMode);
        fprintf(fp, "<User_Id>%s</User_Id>", cDealMembuff);
    }

    fprintf(fp, "<%s>", "auth_packet");
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
    fprintf(fp,"<scannerId>%s</scannerId>",dev_details.scanner_id);

    if(iFusionEnabledFlag == 0)
        strcpy(AuthType,"Bio");
    else if(iFusionEnabledFlag == 1)
        strcpy(AuthType,"Fusion");

    fprintf(fp,"<authType>%s</authType>",AuthType); //no fusion=Bio fusion =Fusion

    if(iDealMemSelected == DEALER_SELECTED)
    {
        fprintf(fp,"<memberId>%s</memberId>",authMode);
        if(DealerTxnFlag == 1)
            fprintf(fp,"<wadhStatus>%s</wadhStatus>","N");
        else if(DealerTxnFlag == 0)
            fprintf(fp,"<wadhStatus>%s</wadhStatus>",QwadhStatus.toAscii().data());
    }
    else if(iDealMemSelected == MEMBER_SELECTED)
    {
        fprintf(fp,"<memberId>%s</memberId>",ui->twdgtEntitlement->item(ui->twdgtEntitlement->currentRow(), 6)->text().toAscii().data());
        fprintf(fp,"<wadhStatus>%s</wadhStatus>",member_wadhstatus.toAscii().data());
    }
    else if(iDealMemSelected == INSPECTOR_SELECTED)
    {
        fprintf(fp,"<memberId>%s</memberId>",authMode);
    }
    else
    {
        fprintf(fp,"<memberId>%s</memberId>",authMode);
    }
    fprintf(fp,"<Resp><errCode>%s</errCode><errInfo>%s</errInfo><nmPoints>%s</nmPoints><fCount>%s</fCount><fType>%s</fType><iCount>%s</iCount><iType>%s</iType><pCount>%s</pCount><pType>%s</pType><qScore>%s</qScore></Resp>",pid_info.err_code,pid_info.err_info,pid_info.nmPoints,pid_info.fCount,pid_info.fType,pid_info.iCount,pid_info.iType,pid_info.pCount,pid_info.pType,pid_info.qScore);;
    fprintf(fp,"%s","</ns2:getAuthenticateNICAuaAuthRD2>");
    fprintf(fp,"%s","</SOAP-ENV:Body>");
    fprintf(fp,"%s","</SOAP-ENV:Envelope>");

    fclose(fp);


    free(pid_info.pid);
    pid_info.pid = NULL;

    return 0;
}

int Widget::prepareNCR_AuthReqFile(char *FileName, char *AuthUid, char *b64Templ1, char *b64Templ2)
{
    qDebug()<< " INSIDE prepareNCR_AuthReqFile"<<"Out File (" <<FileName << ") Preperation ....";

    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));

    miscObj.GetMachineId(cmachineID);

    char buff[4096];
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "rm %s", FileName);
    system(buff);

    QFile file(FileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug()<< FileName << " file Open failed";
        return -1;
    }

    QTextStream out(&file);

    memset(buff, 0, sizeof(buff));
    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0){
        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }


    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL){
        qDebug() << "File Not Found" << endl;
        return -1;
    }

    fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ser=\"http://service.fetch.rationcard/\">");
    fprintf(fp, "<%s/>", "soapenv:Header");
    fprintf(fp, "<%s>", "soapenv:Body");
    fprintf(fp, "<%s>", "ser:getDTRSecureAuthenticate");
    fprintf(fp, "<fpsSessionId>%s</fpsSessionId>", SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp, "<Shop_no>%s</Shop_no>", ui->ldtShpId->text().toAscii().data());
    fprintf(fp, "<uidNumber>%s</uidNumber>", AuthUid);
    fprintf(fp, "<udc>%s</udc>", cmachineID);
    fprintf(fp, "<%s>", "auth_packet");
    fprintf(fp, "<certificateIdentifier>%s</certificateIdentifier>", bufExpiryStr);
    fprintf(fp, "<dataType>%s</dataType>", "X");
    fprintf(fp, "<encHmac>%s</encHmac>", hmac);
    fprintf(fp, "<secure_agency>%s</secure_agency>", "CivilSupplies");
    fprintf(fp, "<secure_agencyCode>%s</secure_agencyCode>", "1/100");
    fprintf(fp, "<secure_pid>%s</secure_pid>", pload);//secure_temp
    fprintf(fp, "<sessionKey>%s</sessionKey>", encryptedSessKey);
    fprintf(fp, "</%s>", "auth_packet");
    fprintf(fp, "</%s>", "ser:getDTRSecureAuthenticate");
    fprintf(fp, "</%s>", "soapenv:Body");
    fprintf(fp, "</%s>", "soapenv:Envelope");
    fclose(fp);

    return 0;
}

int Widget::prepareCardHolderReqFile(char *FileName)
{
    qDebug()<<"Out File (" <<FileName << ") Preparation ....";
    QFile file(FileName);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug()<< FileName << " file Open failed";
        return -1;
    }
    //if(ui->radioButton_UIDFLAG->isChecked()==true)
    //        rsaEncryption(ui->ldtRationCard->text(),"/mnt/jffs2/rsa_key.txt");
    QTextStream out(&file);

    char buff[2048];
    memset(buff, 0, sizeof(buff));

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0){
        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }
    char szShopId[16];
    memset(szShopId, 0, sizeof(szShopId));
    getShopId(szShopId);

    if(ui->radioButton_UIDFLAG->isChecked()==true)
        sprintf(buff, "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getePDSRationCardDetails><fpsSessionId>%s</fpsSessionId><stateCode>%s</stateCode><shop_no>%s</shop_no><password>%s</password><hts>%s</hts><id>%s</id><idType>%s</idType><mode>%s</mode></ns1:getePDSRationCardDetails></SOAP-ENV:Body></SOAP-ENV:Envelope>",SessionId,stateCode.toStdString().c_str(),szShopId, HD_PASSWORD, HD_HTS,asciiBase64Enc,cidType.toAscii().data(), cMode.toAscii().data());
    else
        sprintf(buff, "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getePDSRationCardDetails><fpsSessionId>%s</fpsSessionId><stateCode>%s</stateCode><shop_no>%s</shop_no><password>%s</password><hts>%s</hts><id>%s</id><idType>%s</idType><mode>%s</mode></ns1:getePDSRationCardDetails></SOAP-ENV:Body></SOAP-ENV:Envelope>",SessionId,stateCode.toStdString().c_str(),szShopId, HD_PASSWORD, HD_HTS, ui->ldtRationCard->text().toAscii().data(),cidType.toAscii().data(), cMode.toAscii().data());
    out<<buff;
    file.close();
    return 0;
}

void Widget::on_btnOK_Settings_clicked()
{
    char MacId[32];
    memset(MacId, 0, sizeof(MacId));
    strcpy(MacId, ui->ldtWM_MAC_ID->text().toAscii().data());
    qDebug()<<"MAC ID : "<< MacId;
    if(strlen(MacId) >= 17){
        setWM_MacId(MacId);
    }
    qDebug()<<"Lang Sel Current Index Val : "<<ui->comboLangSel->currentIndex();
    setLangMode(ui->comboLangSel->currentIndex());

    return;
}

int Widget::DealerAuth()
{
    miscObj.Check_mplayer_Exists();

    UidEncode_b64_flag = 1;
    Finger_Iris_Selection = 0;

    int ret = 0;
    miscObj.AudioTest((char *)"175");
    popup_msg *dlgPopup2 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup2->ui->lblPopup->setText("Please Wait...\nDownloading Dealer Details...");
    else if(LangMode == 2)
        dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.\nविक्रेता विवरण डाउनलोड हो रहा है."));

    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    /*char TmpStr[5];
    memset(TmpStr,0x00,sizeof(TmpStr));
    FILE *fp = popen("ping google.com -c 5|grep packets|awk {'print $7'}","r");
    fgets(TmpStr,sizeof(TmpStr),fp);
    pclose(fp);
    qDebug()<<"Ping TmpStr:"<<TmpStr;

    if(strncmp(TmpStr,"0%",2)==0)
    {
        qDebug()<<"Network available"<<endl;
    }
    else
    {
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        qDebug()<<"Network Not available"<<endl;

        nLogout=0;
        int RetVal = LoginByPassword();
        if(RetVal== -1){}
        else
            return -1;
        return -1;
    }*/
    GPRS objGprs;
    if(objGprs.Check_Network_Available() < 0){
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        CheckOfflineGetDealerDetails();
        return -1;
    }

    /****************Dealer Details Request,Response and Parsing************/

    char FileName[64];
    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/Dealer.xml");

    system("rm /mnt/jffs2/Dealer.xml");
    system("rm /mnt/jffs2/DealerDetails.xml");

    ret = prepareDealerReqFile(FileName);
    qDebug()<<"ret form Prepare Out File : "<<ret;

    char catCmd[128];
    memset(catCmd, 0, sizeof(catCmd));
    sprintf(catCmd, "cat %s", FileName);
    system(catCmd);

    char cUrlName[256];
    memset(cUrlName, 0, sizeof(cUrlName));

    QSqlQuery query;
    query.prepare("SELECT DealerURL FROM ShopOwnerDetails");
    if(!query.exec())
        qDebug() << "In SendTxnToServer" << query.lastError();
    else{
        while(query.next()){
            memset(cUrlName, 0, sizeof(cUrlName));
            strcpy(cUrlName, query.value(0).toString().toAscii().data());
        }
    }

    ///////////////
    sleep(2);
//    system("rm /mnt/jffs2/http_code.txt");
//    char check_url_online[1024]="";

//    memset(check_url_online, 0, sizeof(check_url_online));
//    sprintf(check_url_online, "curl -s -o /dev/null -w '%{http_code}' %s -m %d > /mnt/jffs2/http_code.txt",cUrlName,SERVER_TIME_OUT);
//    qDebug()<<"check_url_online"<<check_url_online;

//    int curl_sys = system(check_url_online);
//    qDebug() << "curl_sys = " << curl_sys;

//    int ret_http_code = 0;
    char cmdCurl[1024];
//    memset(cmdCurl, 0, sizeof(cmdCurl));

//    ret_http_code = check_http_error_code();

    int ret_http_code = UrlPingCheck(cUrlName);

    if(ret_http_code == 1)
    {
        qDebug()<<"URL IS LIVE WORKING";
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/DealerDetails.xml", SERVER_TIME_OUT);
    }
    else
    {
        qDebug()<<"URL IS NOT WORKING Switching to other url";
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, PDS_URL2, "/mnt/jffs2/DealerDetails.xml", SERVER_TIME_OUT);
    }
    ///////////////

    qDebug() << "\nCurl Cmd : " << cmdCurl;
    int sysret = system(cmdCurl);
    qDebug() << "sysret = " << sysret;

    if(SystemCurlResponse(sysret, EPDS_SERVER) < 0){
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        CheckOfflineGetDealerDetails();
        return -1;
    }

    dlgPopup2->close();
    dlgPopup2->deleteLater();

    qDebug() << "***********/mnt/jffs2/DealerDetails.xml************";
    system("cat /mnt/jffs2/DealerDetails.xml");
    qDebug() << "***************************************************";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/DealerDetails.xml") != 0){
        return -1;
    }

    /********Auto Update End*******************/
    if(parseDealerDetails("/mnt/jffs2/DealerDetails.xml") < 0){
        miscObj.DisplayWideMsgBox((char *)"Incorrect XML");
        return -1;
    }
    /*****Dealer Details Request,Response and Parsing end*****/
    return 0;
}


int Widget::check_http_error_code()
{
    QFile file("/mnt/jffs2/http_code.txt");
    if(!file.exists())
    {
        return -1;
    }
    if(file.size() == 0)
    {
        return -1;
    }

    file.open(QIODevice::ReadOnly);
    QString s;

    QTextStream s1(&file);
    s.append(s1.readAll());
    //if(s.contains("200", Qt::CaseSensitive) == true)
    if(!s.compare("200"))
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int Widget::ParsingFirstCondition(char *filename)
{
    Qtxnflow.clear();     //Added by santosh to avoid previous txn flow

    if(ValidatingErrors(filename) < 0)
        return -1;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In ParsingFirstCondition = " << filename;
        return -1;
    }
    QString TmpStr = QTextStream(&file).readAll();
    file.close();
    QDomDocument document;
    if(!document.setContent( TmpStr ) )
    {
        qDebug("Failed to parse the file into a DOM tree." );
        return -1;
    }
    QString QrespCode, QrespMessage;

    QrespCode.clear();
    QrespMessage.clear();
    //    Qtxnflow.clear();     //Added by santosh to avoid previous txn flow

    QXmlStreamReader xml(TmpStr);
    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "respCode")
            {
                xml.readNext();
                QrespCode = xml.text().toString();
                qDebug() << "respCode : " << QrespCode;
            }

            if(xml.name() == "respMessage")
            {
                xml.readNext();
                QrespMessage = xml.text().toString();
                qDebug() << "respMessage : " << QrespMessage;
            }

            if(xml.name() == "transaction_flow")
            {
                xml.readNext();
                Qtxnflow = xml.text().toString();
                qDebug() << "transaction_flow : " << Qtxnflow;
            }
        }
    }

    char dispbuff[256];
    memset(dispbuff, 0, sizeof(dispbuff));

    if(QrespCode.isEmpty() || QrespCode.isNull()){
        if(LangMode==1){
            sprintf(dispbuff, "%s", "Invalid Response...\nPlease Try Again...");
            miscObj.DisplayWideMsgBox((char *)dispbuff);
        }else if(LangMode==2){
            sprintf(dispbuff, "%s","अमान्य प्रतिक्रिया\n कृपया पुन: प्रयास करें.");
            miscObj.tel_DisplayWideMsgBox((char *)dispbuff);
        }
        return -1;
    }else if(QrespCode == "E00"){
        sprintf(dispbuff, "%s(%s)", QrespMessage.toAscii().data(), QrespCode.toAscii().data());
        miscObj.DisplayWideMsgBox((char *)dispbuff);
        return 0;
    }else if(QrespCode != "00"){
        sprintf(dispbuff, "%s(%s)", QrespMessage.toAscii().data(), QrespCode.toAscii().data());
        miscObj.DisplayWideMsgBox((char *)dispbuff);

        ui->ldtEkycCardNo->clear();
        return -1;
    }
    /*
    else if(QrespCode != "00" && Qtxnflow == "F")//added on 20042019
    {
        return 0;
    }
    */
    MobileStatusMessage.clear();
    MobileStatusMessage = QrespMessage;
    return 0;
}

int Widget::ParsingFirstConditionFusion(char *filename)
{
    QString transaction_flow;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "In ParsingFirstCondition for Fusion = " << filename;
        return -1;
    }

    if(file.size() == 0)
        return -1;

    QString QrespCode, QrespMessage;
    QXmlStreamReader xml(&file);
    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "respCode")
            {
                xml.readNext();
                QrespCode = xml.text().toString();
                qDebug() << "respCode : " << QrespCode;
            }

            if(xml.name() == "respMessage")
            {
                xml.readNext();
                QrespMessage = xml.text().toString();
                qDebug() << "respMessage : " << QrespMessage;
            }
            if(xml.name() == "transaction_flow")
            {
                xml.readNext();
                transaction_flow = xml.text().toString();
                qDebug() << "transaction_flow : " <<transaction_flow;
                Qtxnflow=transaction_flow;
            }

        }
    }

    file.close();

    // CMisc miscObj;
    char dispbuff[256];
    memset(dispbuff, 0, sizeof(dispbuff));

    if(QrespCode.isEmpty() || QrespCode.isNull()){
        sprintf(dispbuff, "%s", "Invalid Response...\nPlease Try Again...");
        miscObj.DisplayWideMsgBox((char *)dispbuff);
        //        return -1;
        return -3;
    }else if(QrespCode == "300" && transaction_flow=="F"){
        qDebug()<<"GO FOR FUSION....";
        return -1;
    }else if(QrespCode == "00"){
        qDebug()<<" AUTH SUCCESS...";
        return 0;
    }else{
        qDebug()<<"AUTH QrespCode === "<<QrespCode;
        return -2;
    }
    return 0;
}

int Widget::SystemCurlResponse(int sysret, int iserverflag)
{
    if(sysret == 1536){
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);

        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"Please Try Again");
        else if(LangMode == 7)
            miscObj.tel_DisplayMsgBox((char *)"कृपया पुनः प्रयास करें");
        return -1;
    }else if((sysret == 1792) || (sysret == 32512)){
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);

        if(iserverflag == AUTH_SERVER){
            if(LangMode == 1)
                miscObj.DisplayMsgBox((char *)"Network Error.Please Try Again.."); //SRDH
            else if(LangMode == 2)
                miscObj.tel_DisplayMsgBox((char *)"नेटवर्क त्रुटि। कृपया पुन: प्रयास करें"); //SRDH
        }else{
            if(LangMode == 1)
                miscObj.DisplayMsgBox((char *)"Could Not Connect to ePOS Server...");
            else if(LangMode == 2)
                miscObj.tel_DisplayMsgBox((char *)"ईपीओएस सर्वर से संपर्क नहीं हो सकता..."); //SRDH
        }
        return -1;
    }else if(sysret == 7168){ //Response Timeout
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);

        miscObj.AudioTest((char *)"92");
        if(iserverflag == AUTH_SERVER){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Request Timeout. Please Try again..");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"समय में अनुरोध का कोई प्रतिक्रिया नहीं मिला\nकृपया पुन: प्रयास करें");
        }else{
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Request Timeout. Please Try again..");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"समय में अनुरोध का कोई प्रतिक्रिया नहीं मिला\nकृपया पुन: प्रयास करें");
        }
        return -1;
    }else if(sysret == 512){
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);

        miscObj.AudioTest((char *)"181");
        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"Failed to initialize Request Service.");
        else if(LangMode == 2)
            miscObj.tel_DisplayMsgBox((char *)"अनुरोध को प्रारंभ करने में असफल...");
        return -1;
    }else if(sysret == 13312){
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);

        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"Network Error...\nPlease Try Again...");
        else if(LangMode == 2)
            miscObj.tel_DisplayMsgBox((char *)"नेटवर्क त्रुटि ...|कृपया पुनः प्रयास करें...");
        return -1;
    }else if(sysret != 0){
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);

        if(iserverflag == AUTH_SERVER){
            if(LangMode == 1)
                miscObj.DisplayMsgBox((char *)"Request Service Not Responding...");
            else if(LangMode == 2)
                miscObj.tel_DisplayMsgBox((char *)"किया गया अनुरोध के लिए |कोई प्रतिक्रिया नहीं मिल राहा है ...");
        }else{
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Server Connection Error...\nPlease Try Again After SomeTime...");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"नेटवर्क त्रुटि ... \n कृपया कुछ समय के बाद पुन: प्रयास करें ...");
        }
        return -1;
    }
    return 0;
}

void Widget::ShowDisplayMsg()
{
    QSqlQuery query;
    QString QStr;

    query.prepare("SELECT flag_msg FROM Settings");
    if(!query.exec())
        qDebug() << query.lastError();
    else{
        while(query.next()){
            QStr = query.value(0).toString().trimmed();
        }
    }
    qDebug( ) << "display msg +++++++++++++++++++++++++++++++++++++"<<QStr<<"++++++++++++++++++++++++++++++++++++++";
    if(QStr == "0"){

    }else{
        miscObj.DisplayWideMsgBox((char *)QStr.toAscii().data());
    }
    return;
}

void Widget::ShowAlertMsg()
{
    QSqlQuery query;
    QString QStr, QStrTel;

    query.prepare("SELECT message_eng, message_tel FROM Settings");
    if(!query.exec())
        qDebug() << query.lastError();
    else{
        while(query.next()){
            QStr = query.value(0).toString().trimmed();
            QStrTel = query.value(1).toString().trimmed();
        }
    }
    if(QStr == "0")
    {

    }else{
        database_queries dbqry;
        LangMode = dbqry.getLangMode();

        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)QStr.toAscii().data());
        else
            miscObj.tel_DisplayWideMsgBox(QStrTel.toAscii().data());
    }
    return;
}

void Widget::getHeadings()
{
    QSqlQuery query;

    query.prepare("SELECT pds_cl_tran_eng, pds_cl_tran_tel, pds_tran_eng, pds_tran_tel FROM Headers");
    if(!query.exec())
        qDebug() << query.lastError();
    else{
        while(query.next()){
            Qpds_cl_tran_eng = query.value(0).toString().trimmed();
            Qpds_cl_tran_tel = query.value(1).toString().trimmed();
            Qpds_tran_eng = query.value(2).toString().trimmed();
            Qpds_tran_tel = query.value(3).toString().trimmed();
        }
    }
    qDebug()<<Qpds_cl_tran_eng<<"####";
    qDebug()<<Qpds_cl_tran_tel<<"####";
    qDebug()<<Qpds_tran_eng<<"####";
    qDebug()<<Qpds_tran_tel<<"####";

    return;
}

int Widget::CheckBankMasterData()
{
    QSqlQuery query;

    int iCount = 0;

    query.prepare("SELECT COUNT(AgentID) FROM BankMasterTable");
    if(!query.exec())
        qDebug() << query.lastError();
    else{
        while(query.next()){
            iCount = query.value(0).toString().toInt(0);
        }
    }

    if(iCount == 0){
        miscObj.DisplayWideMsgBox((char *)"Please download Bank Master Data From Settings");
        return -1;
    }

    query.clear();
    query.prepare("SELECT COUNT(BankName) FROM BenBankTable");
    if(!query.exec())
        qDebug() <<  query.lastError();
    else{
        while(query.next()){
            iCount = query.value(0).toString().toInt(0);
        }
    }

    if(iCount == 0){
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please download Beneficiary Master Data From Settings");
        else if(LangMode ==2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया लाभार्थी मास्टर डेटा को सेटिंग्स से डाउनलोड करें");
        return -1;
    }


    QFile file;
    file.setFileName("/mnt/jffs2/HttpsCertificate.cer");
    if(!file.exists()){
        qDebug() << "Certificate File Not Found";
        //        if(IsHttps() > true)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Please Download HTTPS Certificate\nfrom Settings");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"कृपया सेटिंग्स से HTTPS सर्टिफिकेट डाउनलोड करें");
            return -1;
        }
    }
    return 0;
}

void Widget::on_ldtGetOwnDetails_clicked()
{
    if(LangMode == 1)
        pop_up("Please Wait....");
    else if(LangMode == 2)
        pop_up("कृपया प्रतीक्षा करें....");

    QTime tim;
    tim.restart();
    tim.start();

    miscObj.GetTransactionId(TxnId);

    if(UpdateDBTxnURL() < 0)
        return;

    if(DealerAuth() < 0)
        return;
    if(DealerLoginBy == 2)
        PartialOnlineTimer->stop();
    DealerLoginBy =0;
    ShowAlertMsg();

    memset(szHelpDeskNo, 0, sizeof(szHelpDeskNo));
    miscObj.getHelpDeskNo(szHelpDeskNo);
    if(strncmp(szHelpDeskNo,"NA",2)==0){
        ui->lblHelpDeskNo->hide();
        ui->lblHelpDesk->hide();
    }
    else
        ui->lblHelpDeskNo->setText(szHelpDeskNo);

    ////////////////////////////////////////////////////////////////
    DealerTxnFlag = 0; // Dealer Login
    iDealMemSelected = DEALER_SELECTED;
    DisplayDealerDetails();
    ////////////////////////////////////////////////////////////////
    SetAppType(stateCode.toInt());//changing application type according to state code

    ui->ConsentFormcheckBox_3->setChecked(true);
    ui->stackedWidget->setCurrentIndex(20);

    return;
}

void Widget::DisplayDealerDetails()
{
    QString DelName;
    QSqlQuery query;

    int tableCount = 0;
    query.clear();

    query.prepare("SELECT COUNT(ownername) FROM Dealer");
    if(!query.exec())
        qDebug() << query.lastError();
    else{
        while(query.next()){
            tableCount = query.value(0).toInt(0);
        }
    }
    if(tableCount == 0){
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Dealer Details is not registered at PDS server...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"विक्रेता विवरण पीडीएस सर्वर पर पंजीकृत नहीं है.");
        return;
    }

    ui->tableWidgetDealNomDetails->showMaximized();
    ui->tableWidgetDealNomDetails->clearContents();
    ui->tableWidgetDealNomDetails->setRowCount(tableCount);

    char buff[128];
    int row = 0;
    query.clear();

    query.prepare("SELECT ownername, fpsuidno, del_bfd_1, del_bfd_2, del_bfd_3,dealer_type FROM Dealer");
    if(!query.exec())
        qDebug() << query.lastError();
    else{
        while(query.next()){
            memset(buff,0,sizeof(buff));

            if(LangMode ==1){
                strcpy(buff, query.value(0).toString().toAscii().data());
                qDebug() << "0 DealerNameEn = " << buff;
                ui->tableWidgetDealNomDetails->setItem(row, 0, new QTableWidgetItem(buff));
            }else if(LangMode ==2){
                DelName.clear();
                DelName =query.value(0).toString();
                ui->tableWidgetDealNomDetails->setItem(row, 0, new QTableWidgetItem(DelName));
            }
            ui->tableWidgetDealNomDetails->item(row, 0)->setFlags( ui->tableWidgetDealNomDetails->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetDealNomDetails->item(row, 0)->setFlags( ui->tableWidgetDealNomDetails->item(row,0)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(1).toString().toAscii().data());

            ui->tableWidgetDealNomDetails->setItem(row, 1, new QTableWidgetItem(buff));
            ui->tableWidgetDealNomDetails->item(row, 1)->setFlags( ui->tableWidgetDealNomDetails->item(row,1)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetDealNomDetails->item(row, 1)->setFlags( ui->tableWidgetDealNomDetails->item(row,1)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(2).toString().toAscii().data());
            qDebug() << "0 del_bfd_1 = " << buff;

            ui->tableWidgetDealNomDetails->setItem(row, 2, new QTableWidgetItem(buff));
            ui->tableWidgetDealNomDetails->item(row, 2)->setFlags( ui->tableWidgetDealNomDetails->item(row,2)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetDealNomDetails->item(row, 2)->setFlags( ui->tableWidgetDealNomDetails->item(row,2)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(3).toString().toAscii().data());
            qDebug() << "0 del_bfd_2 = " << buff;

            ui->tableWidgetDealNomDetails->setItem(row, 3, new QTableWidgetItem(buff));
            ui->tableWidgetDealNomDetails->item(row, 3)->setFlags( ui->tableWidgetDealNomDetails->item(row,3)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetDealNomDetails->item(row, 3)->setFlags( ui->tableWidgetDealNomDetails->item(row,3)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(4).toString().toAscii().data());
            qDebug() << "0 del_bfd_3 = " << buff;

            ui->tableWidgetDealNomDetails->setItem(row, 4, new QTableWidgetItem(buff));
            ui->tableWidgetDealNomDetails->item(row, 4)->setFlags( ui->tableWidgetDealNomDetails->item(row,4)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetDealNomDetails->item(row, 4)->setFlags( ui->tableWidgetDealNomDetails->item(row,4)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(1).toString().toAscii().data());
            qDebug() << "5 Dealer Actual UID = " << buff;

            ui->tableWidgetDealNomDetails->setItem(row, 5, new QTableWidgetItem(buff));
            ui->tableWidgetDealNomDetails->item(row, 5)->setFlags( ui->tableWidgetDealNomDetails->item(row,5)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetDealNomDetails->item(row, 5)->setFlags( ui->tableWidgetDealNomDetails->item(row,5)->flags() | Qt::ItemIsSelectable);

            ui->tableWidgetDealNomDetails->setItem(row, 6, new QTableWidgetItem(query.value(5).toString().toAscii().data()));
            ui->tableWidgetDealNomDetails->item(row, 6)->setFlags( ui->tableWidgetDealNomDetails->item(row,6)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetDealNomDetails->item(row, 6)->setFlags( ui->tableWidgetDealNomDetails->item(row,6)->flags() | Qt::ItemIsSelectable);

            row++;
        }
    }

    ui->tableWidgetDealNomDetails->setColumnWidth(0,180);
    ui->tableWidgetDealNomDetails->setColumnWidth(1,140);
    ui->tableWidgetDealNomDetails->setColumnWidth(6,140);

    ui->tableWidgetDealNomDetails->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetDealNomDetails->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetDealNomDetails->verticalHeader()->hide();
    ui->tableWidgetDealNomDetails->selectRow(0);

    ui->tableWidgetDealNomDetails->setColumnHidden(2, true);
    ui->tableWidgetDealNomDetails->setColumnHidden(3, true);
    ui->tableWidgetDealNomDetails->setColumnHidden(4, true);
    ui->tableWidgetDealNomDetails->setColumnHidden(5, true);
    ui->tableWidgetDealNomDetails->setColumnHidden(1, true);

}

void Widget::DisplayCommDetails()
{
    char buff[32];
    int i = 0;
    QTime tim;

    tim.restart();
    tim.start();

    QSqlQuery query;
    query.prepare("SELECT BalQty FROM BenQuotaTable");
    if(!query.exec())
        qDebug() << "BenQuotaTable Bal Qty:"<<query.lastError();
    else
    {
        while(query.next())
        {
            if(query.value(0).toFloat(0) < 0)
            {
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox((char *)"Recieved Invalid Quantity From ePDS Server");
                else if(LangMode == 2)
                    miscObj.tel_DisplayWideMsgBox((char *)"ईपीडीएस सर्वर से अमान्य मात्रा प्राप्त हुई.");

                return;
            }
        }
    }
    int count = 0;
    QSqlQuery query1;
    query1.prepare("SELECT COUNT(ItemType) FROM BenQuotaTable where RationCardId=?");
    if(iMainTransaction == MDM_TRANSACTION)
        query1.addBindValue(ui->lineEditAnganwadiCardNo->text().toAscii().data());
    else
        query1.addBindValue(Rcno.toAscii().data());
    if(!query1.exec())
    {
        qDebug() << "BenQuotaTable COUNT:"<< query1.lastError();
        return;
    }
    else
    {
        while(query1.next())
            count = query1.value(0).toInt(0);
    }
    ui->tblCommIssue->setRowCount(count);

    int row2 = 0;
    qDebug()<<" ?????????????????????????? ++++++Rcno"<<Rcno<<endl;


    query.clear();
    query.prepare("select ItemType, MonthlyQuota, ItemPrice, ItemCode, BalQty, AvailedQty, MeasureUnit, RequiredQty, closingBal,commNamell,weighingFlag,minimumqty,allocationType,allotedMonth,allotedYear from BenQuotaTable where RationCardId=? ");
    if(iMainTransaction == PDS_TRANSACTION)
        query.addBindValue(Rcno.toAscii().data());
    else
        query.addBindValue(ui->lineEditAnganwadiCardNo->text().toAscii().data());
    query.exec();

    while(query.next())
    {
        if(LangMode == 1)
        {
            if(iMainTransaction == PDS_TRANSACTION)
            {
                memset(buff,0,sizeof(buff));
                sprintf(buff, "%s(%s)", query.value(0).toString().toAscii().data(), query.value(1).toString().toAscii().data());
                qDebug()<<"0 Item Type  - Total Qty (Monthly Quota) = "<<buff;

                ui->tblCommIssue->setItem(row2, 0, new QTableWidgetItem(buff));
            }
            else
            {
                QString commidityname;
                QString s1,s2;
                s1="(";
                s2=")";
                commidityname.clear();

                memset(buff,0,sizeof(buff));
                commidityname=query.value(9).toString()+s1+query.value(1).toString()+s2;

                qDebug()<<"commidityname"<<commidityname;
                ui->tblCommIssue->setItem(row2, 0, new QTableWidgetItem(commidityname));
            }
        }
        else if(LangMode == 2)
        {
            QString commidityname;
            QString s1,s2;
            s1="(";
            s2=")";
            commidityname.clear();

            memset(buff,0,sizeof(buff));
            commidityname=query.value(9).toString()+s1+query.value(1).toString()+s2;

            qDebug()<<"commidityname"<<commidityname;
            ui->tblCommIssue->setItem(row2, 0, new QTableWidgetItem(commidityname));

        }
        ui->tblCommIssue->item(row2, 0)->setFlags( ui->tblCommIssue->item(row2,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
        //        ui->tblCommIssue->item(row2, 0)->setFlags(ui->tblCommIssue->item(row2, 0)->flags() ^ Qt::ItemIsSelectable);

        char iqty[32],prate[32];
        char TmpString[300];

        if(iMainTransaction == PDS_TRANSACTION)
        {
            query1.clear();
            memset(TmpString,0x00,sizeof(TmpString));
            memset(iqty,0,sizeof(iqty));
            memset(prate,0,sizeof(prate));

            sprintf(TmpString,"select (select count() from sqlite_master where type='table' and name = 'KeyRegister'),(select count() from KeyRegister where RcId = '%s' and CommCode ='%s')",Rcno.toAscii().data(),query.value(3).toString().toAscii().data());
            qDebug()<<"TmpString:"<<TmpString;
            if(!query1.exec(TmpString))
            {
                qDebug()<<"No table found KeyRegister "<<endl;
            }
            query1.next();
            if((query1.value(0).toString().toInt()!=0)&&(query1.value(1).toString().toInt()!=0))
            {
                query1.clear();
                memset(TmpString,0x00,sizeof(TmpString));

                sprintf(TmpString,"select balanceEntitlement from KeyRegister where RcId = '%s' and CommCode ='%s' and allocationType='%s' and allotedMonth='%s' and allotedYear='%s'",Rcno.toAscii().data(),query.value(3).toString().toAscii().data(),query.value(12).toString().toAscii().data(),query.value(13).toString().toAscii().data(),query.value(14).toString().toAscii().data());
                qDebug()<<"TmpString:"<<TmpString;
                if(query1.exec(TmpString))
                {

                }
                query1.next();

                qDebug()<<"Local  Issue Qty offline:"<<atof(query1.value(0).toString().toAscii().data());
                qDebug()<<"Total Qty :"<<atof(query.value(1).toString().toAscii().data());
                qDebug()<<"Balance  Qty from Server:"<<atof(query.value(4).toString().toAscii().data());

                if(query1.value(0).toString().toFloat() < query.value(4).toString().toFloat())
                {
                    memset(buff,0,sizeof(buff));
                    sprintf(buff,"%s",query1.value(0).toString().toAscii().data());
                    qDebug()<<"1 Bal Qty = "<<buff;
                    strcpy(iqty,buff);
                }
                else
                {
                    //Bal Qty
                    memset(buff,0,sizeof(buff));
                    strcpy(buff, query.value(4).toString().toAscii().data());
                    qDebug()<<"In else Bal Qty = "<<buff;
                    strcpy(iqty,buff);
                }
            }
            else
            {
                //Key register tables not found
                //Bal Qty
                memset(buff,0,sizeof(buff));
                strcpy(buff, query.value(4).toString().toAscii().data());
                qDebug()<<"In else Bal Qty = "<<buff;
                strcpy(iqty,buff);
            }
        }
        else
        {
            memset(iqty,0,sizeof(iqty));
            memset(buff,0,sizeof(buff));

            strcpy(buff, query.value(4).toString().toAscii().data());
            qDebug()<<"In else Bal Qty = "<<buff;
            strcpy(iqty,buff);
        }
        ui->tblCommIssue->setItem(row2, 1, new QTableWidgetItem(buff));
        ui->tblCommIssue->item(row2, 1)->setFlags(ui->tblCommIssue->item(row2, 1)->flags() ^ Qt::ItemIsEditable);
        ui->tblCommIssue->item(row2, 1)->setFlags(ui->tblCommIssue->item(row2, 1)->flags() ^ Qt::ItemIsSelectable);

        memset(prate,0,sizeof(prate));
        //Item Price
        memset(buff,0,sizeof(buff));
        strcpy(buff, query.value(2).toString().toAscii().data());
        qDebug()<<"2 Item Price = "<<buff;
        strcpy(prate,buff);

        ui->tblCommIssue->setItem(row2, 2, new QTableWidgetItem(buff));
        ui->tblCommIssue->item(row2, 2)->setFlags(ui->tblCommIssue->item(row2, 2)->flags() ^ Qt::ItemIsEditable);
        ui->tblCommIssue->item(row2, 2)->setFlags(ui->tblCommIssue->item(row2, 2)->flags() ^ Qt::ItemIsSelectable);

        //        memset(buff,0,sizeof(buff));
        //        strcpy(buff, query.value(4).toString().toAscii().data());
        //        qDebug()<<"1 Bal Qty = "<<buff;
        //        strcpy(iqty,buff);


        //        ui->tblCommIssue->setItem(row2, 3, new QTableWidgetItem(iqty));

        //        //amt
        //        memset(buff,0,sizeof(buff));
        //        sprintf(buff,"%0.2f",atof(iqty)*atof(prate));
        //        qDebug()<<iqty<<"*"<<prate <<" = Amt Bufff::: "<<buff;

        //        ui->tblCommIssue->setItem(row2, 4, new QTableWidgetItem(buff));

        if(query.value(8).toString().toFloat() >= query.value(4).toString().toFloat())
        {
            ui->tblCommIssue->setItem(row2, 3, new QTableWidgetItem(iqty));

            memset(buff,0,sizeof(buff));
            sprintf(buff,"%0.2f",atof(iqty)*atof(prate));
            qDebug()<<iqty<<"*"<<prate <<" = Amt Bufff::: "<<buff;

            ui->tblCommIssue->setItem(row2, 4, new QTableWidgetItem(buff));
        }
        else
        {
            ui->tblCommIssue->setItem(row2, 3, new QTableWidgetItem("0"));
            ui->tblCommIssue->setItem(row2, 4, new QTableWidgetItem("0"));
        }

        ui->tblCommIssue->item(row2, 3)->setData(Qt::BackgroundColorRole, QVariant(QColor(Qt::green)));
        //        ui->tblCommIssue->item(0,3)

        if(strcmp(query.value(10).toString().toAscii().data(),"Y")==0)
        {
            qDebug()<<"&&&&&&&&&&&&&&&&& get weight from weighing scale";

            ui->tblCommIssue->item(row2, 3)->setFlags(ui->tblCommIssue->item(row2, 3)->flags() ^ Qt::ItemIsEditable);
            ui->tblCommIssue->item(row2, 3)->setFlags(ui->tblCommIssue->item(row2, 3)->flags() ^ Qt::ItemIsSelectable);
        }
        else
        {
            qDebug()<<"&&&&&&&&&&&&&&&&& Enter weight Manully";

            ui->tblCommIssue->item(row2, 3)->setFlags(ui->tblCommIssue->item(row2, 3)->flags() | Qt::ItemIsEditable);
            ui->tblCommIssue->item(row2, 3)->setFlags(ui->tblCommIssue->item(row2, 3)->flags() ^ Qt::ItemIsSelectable);
        }

        //        ui->tblCommIssue->setItem(row2, 4, new QTableWidgetItem("0.00"));

        //Item Code
        memset(buff,0,sizeof(buff));
        strcpy(buff, query.value(3).toString().toAscii().data());

        ui->tblCommIssue->item(row2, 4)->setFlags( ui->tblCommIssue->item(row2,4)->flags() ^ Qt::ItemIsEditable);
        ui->tblCommIssue->item(row2, 4)->setFlags(ui->tblCommIssue->item(row2, 4)->flags() ^ Qt::ItemIsSelectable);

        qDebug()<<"5 Item Code = "<<buff;

        ui->tblCommIssue->setItem(row2, 5, new QTableWidgetItem(buff));
        ui->tblCommIssue->item(row2, 5)->setFlags( ui->tblCommIssue->item(row2,5)->flags() ^ Qt::ItemIsEditable);
        ui->tblCommIssue->item(row2, 5)->setFlags(ui->tblCommIssue->item(row2, 5)->flags() ^ Qt::ItemIsSelectable);


        if(iMainTransaction == PDS_TRANSACTION)
        {
            query1.clear();
            memset(TmpString,0x00,sizeof(TmpString));
            sprintf(TmpString,"select (select count() from sqlite_master where type='table' and name = 'Pos_Ob'),(select count() from Pos_Ob where CommCode ='%s')",query.value(3).toString().toAscii().data());
            qDebug()<<"TmpString :"<<TmpString;
            if(!query1.exec(TmpString))
            {
                qDebug()<<"No table found Cb "<<endl;
            }
            query1.next();
            if((query1.value(0).toString().toInt()!=0)&&(query1.value(1).toString().toInt()!=0))
            {
                query1.clear();
                query1.prepare("select closingBalance from Pos_Ob where CommCode =?");
                query1.bindValue(":CommCode",query.value(3).toString().toAscii().data());
                if(query1.exec())
                {

                }
                query1.next();
                qDebug()<<"Local Db CB :"<<query1.value(0).toString().toFloat();
                qDebug()<<"Server Db CB :"<<query.value(8).toString().toFloat();
                if(query1.value(0).toString().toFloat()<query.value(8).toString().toFloat())
                {
                    ui->tblCommIssue->setItem(row2, 6, new QTableWidgetItem(query1.value(0).toString()));
                }
                else
                {
                    ui->tblCommIssue->setItem(row2, 6, new QTableWidgetItem(query.value(8).toString()));
                }
            }
            else
            {
                ui->tblCommIssue->setItem(row2, 6, new QTableWidgetItem(query.value(8).toString()));
            }
        }
        else
        {
            ui->tblCommIssue->setItem(row2, 6, new QTableWidgetItem(query.value(8).toString()));
        }
        //ui->tblCommIssue->setItem(row2, 6, new QTableWidgetItem(query.value(8).toString()));
        ui->tblCommIssue->item(row2, 6)->setFlags( ui->tblCommIssue->item(row2,6)->flags() ^ Qt::ItemIsEditable);
        ui->tblCommIssue->item(row2, 6)->setFlags(ui->tblCommIssue->item(row2, 6)->flags() ^ Qt::ItemIsSelectable);

        //by subbu-29122017
        memset(buff,0,sizeof(buff));
        sprintf(buff, "%s", query.value(3).toString().toAscii().data());
        ui->tblCommIssue->setItem(row2, 7, new QTableWidgetItem(buff));
        ui->tblCommIssue->item(row2, 7)->setFlags(ui->tblCommIssue->item(row2, 7)->flags() ^ Qt::ItemIsEditable);
        ui->tblCommIssue->item(row2, 7)->setFlags(ui->tblCommIssue->item(row2, 7)->flags() ^ Qt::ItemIsSelectable);

        ui->tblCommIssue->setItem(row2, 8, new QTableWidgetItem(query.value(10).toString().toAscii().data()));
        //for minimum qty
        ui->tblCommIssue->setItem(row2, 9, new QTableWidgetItem(query.value(11).toString().toAscii().data()));

        ////
        ui->tblCommIssue->setItem(row2, 11, new QTableWidgetItem(query.value(12).toString().toAscii().data()));
        ui->tblCommIssue->item(row2, 11)->setFlags(ui->tblCommIssue->item(row2, 11)->flags() ^ Qt::ItemIsEditable);
        ui->tblCommIssue->item(row2, 11)->setFlags(ui->tblCommIssue->item(row2, 11)->flags() ^ Qt::ItemIsSelectable);

        ui->tblCommIssue->setItem(row2, 12, new QTableWidgetItem(query.value(13).toString().toAscii().data()));
        ui->tblCommIssue->item(row2, 12)->setFlags(ui->tblCommIssue->item(row2, 12)->flags() ^ Qt::ItemIsEditable);
        ui->tblCommIssue->item(row2, 12)->setFlags(ui->tblCommIssue->item(row2, 12)->flags() ^ Qt::ItemIsSelectable);

        ui->tblCommIssue->setItem(row2,13, new QTableWidgetItem(query.value(14).toString().toAscii().data()));
        ui->tblCommIssue->item(row2, 13)->setFlags(ui->tblCommIssue->item(row2, 13)->flags() ^ Qt::ItemIsEditable);
        ui->tblCommIssue->item(row2, 13)->setFlags(ui->tblCommIssue->item(row2, 13)->flags() ^ Qt::ItemIsSelectable);

        ui->tblCommIssue->setRowHeight(row2,20);

        row2++;
    }

    if(row2 == 0)
    {
        qDebug()<<"No Data in BenQuota Table";
        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"No Commodities For This Card");
        if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"इस कार्ड के लिए कोई वस्तु नहीं है.");

        return;
    }
    qDebug()<<"Column(1) Width = "<<ui->tblCommIssue->columnWidth(1);

    ui->tblCommIssue->setColumnWidth(0,120);
    ui->tblCommIssue->setColumnWidth(1,60);
    ui->tblCommIssue->setColumnWidth(2,60);
    ui->tblCommIssue->setColumnWidth(3,70);
    ui->tblCommIssue->setColumnWidth(4,60);
    ui->tblCommIssue->setColumnWidth(6,60);

    ui->tblCommIssue->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblCommIssue->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblCommIssue->verticalHeader()->hide();
    ui->tblCommIssue->selectRow(0);

    if(ui->tblCommIssue->item(0,8)->text() == "Y")
    {
        ui->btnGetWt->show();
        ui->comboCommMode->show();
    }
    else
    {
        ui->btnGetWt->hide();
        ui->comboCommMode->hide();
    }

    ui->tblCommIssue->setColumnHidden(5, true);
    ui->tblCommIssue->setColumnHidden(7, true);//29122017 by subbu
    ui->tblCommIssue->setColumnHidden(8, true);
    ui->tblCommIssue->setColumnHidden(9, true);
    ui->tblCommIssue->setColumnHidden(10, true);
    ui->tblCommIssue->setColumnHidden(11, true);
    ui->tblCommIssue->setColumnHidden(12, true);
    ui->tblCommIssue->setColumnHidden(13, true);
    ui->tblCommIssue->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tblCommIssue->horizontalHeader()->setMinimumWidth(0);

    //Go to next page to display the items in table widget
    iCommMode = getCommMode();

    qDebug()<<"Communication Mode To Weing Machine = "<<iCommMode;

    fTotalAmt = 0.00;
    fTotalQty = 0.00;
    ui->lblTotalAmtVal->setText("0.00");

    /******* TIME LAP STOP **********/

    char lapbuff[32];
    memset(lapbuff, 0, sizeof(lapbuff));
    sprintf(lapbuff,"Lap Time : %d", tim.elapsed());
    tim.restart();

    /********************************/

    Finger_Iris_Selection = 0;

    if(iMainTransaction == MDM_TRANSACTION || iMainTransaction == ICDS_TRANSACTION)
    {
        qDebug()<<"DISABLING DYNAMIC";
        ui->btnGetWt->hide();
        ui->comboCommMode->hide();

    }
    else
    {
        qDebug()<<"ENABLING DYNAMIC";
    }
    iModuleType = COMM_TXN;
    fTotalAmt = 0.00;
    fTotalQty = 0.00;
    int iNoOfRows = 0;

    iNoOfRows = ui->tblCommIssue->rowCount();
    for(i=0; i<iNoOfRows;i++)
    {
        if(ui->tblCommIssue->item(i, 4) != NULL)
        {
            fTotalAmt += atof(ui->tblCommIssue->item(i, 4)->text().toAscii().data());
        }
    }
    memset(buff, 0, sizeof(buff));
    sprintf(buff,"%0.2f",fTotalAmt);
    ui->lblTotalAmtVal->setText(buff);
    ui->stackedWidget->setCurrentIndex(6);
}

int Widget::InsertCardTypesIntoDB(QString str)
{
    QSqlQuery query;

    if(str.length() == 0){
        miscObj.DisplayWideMsgBox((char *)"Card Type Not Found");
        return -1;
    }

    QStringList list = str.split(',', QString::SkipEmptyParts);
    qDebug() << "List Of Cards = " << list;

    int NoOfCards = list.count();

    query.clear();
    query.exec("DELETE FROM CardTypes");

    for(int i = 0; i < NoOfCards; i++){
        query.clear();
        query.prepare("INSERT INTO CardTypes(type) VALUES(:type)");
        query.bindValue(":type", list.at(i));
        if(!query.exec()){
            qDebug() << "INSERT INTO CardTypes = " << query.lastError();
            return -1;
        }
    }
    return 0;
}

int Widget::parseDealerDetails(char *filename)
{
    QString QflasMessage1,QflasMessage2, QdealerFusion,Qdealer_type,QdelBfd1,QdelBfd2,QdelBfd3,QdelName,QdelNamell,QdelUid,QfpsId,QfpsSessionId,Qlatitude,QloginRequestTime,Qlongtude,QrouteOffEnable,QweighingStatus,QeKYCPrompt;
    //    QString QlogoutTime,QminQty,QversionUpdateRequired; //commented becoz not using
    QString QceritificatePath,QfusionAttempts,QhelplineNumber,QmessageEng,QmessageLl,QpdsClTranEng,QpdsClTranLl,QpdsTranEng,QpdsTransLl,Qtoken,QwsdlURLAuth,QwsdlURLPDS,QwsdlURLReceive,impdsURL;
    QString QidType,QidValue,QopeValue,QoprMode;
    QString Qdeltype;
    QString Qdist_code;
    QString reasonId,reasonValue,QwadhStatus_local;

    QSqlQuery query;

    query.exec("DELETE FROM TransactionModes");
    query.clear();
    query.exec("DELETE FROM Dealer");
    query.clear();
    query.exec("DELETE FROM Nominee");
    query.clear();
    query.exec("DELETE FROM StateInfo");
    query.clear();

    QwadhStatus_local.clear();
    QwadhStatus_D.clear();
    QwadhStatus_N1.clear();
    QwadhStatus_N2.clear();

    QList <QString>list_tmp;
    Reasons_Vec.clear();

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "In parseDealerDetails, Error Reading File";
        return -1;
    }
    if(file.size() == 0)
        return -1;

    QXmlStreamReader xml(&file);

    while(!xml.atEnd() && !xml.hasError()){
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement){
            if(xml.name() == "dealer_password"){
                xml.readNext();
                dealer_password = xml.text().toString();
                qDebug() << "dealer_password : " << dealer_password;
            }
            if(xml.name() == "QflasMessage1"){
                xml.readNext();
                QflasMessage1 = xml.text().toString();
                qDebug() << "QflasMessage1 : " << QflasMessage1;

                QSqlQuery qry;
                qry.prepare("UPDATE Settings SET flag_msg=?");
                qry.addBindValue(QflasMessage1);
                if(!qry.exec())
                    qDebug() << "UPDATE Settings SET QflasMessage1 = " << qry.lastError();
            }
            if(xml.name() == "QflasMessage2"){
                xml.readNext();
                QflasMessage2 = xml.text().toString();
                qDebug() << "QflasMessage2 : " << QflasMessage2;
            }
            if(xml.name() == "dealerFusion"){
                xml.readNext();
                QdealerFusion = xml.text().toString();
                qDebug() << "dealerFusion : " <<QdealerFusion;

            }
            if(xml.name() == "authType"){
                xml.readNext();
                Qdealer_type = xml.text().toString();
                qDebug() << "Qdealer_type : " <<Qdealer_type;
            }
            if(xml.name() == "dealer_type"){
                xml.readNext();
                Qdeltype = xml.text().toString();
                qDebug() << "Qdeltype : " <<Qdeltype;
            }
            if(xml.name() == "delBfd1"){
                xml.readNext();
                QdelBfd1 = xml.text().toString();
                qDebug() << "QdelBfd1 : " << QdelBfd1;

            }
            if(xml.name() == "delBfd2"){
                xml.readNext();
                QdelBfd2 = xml.text().toString();
                qDebug() << "QdelBfd2 : " << QdelBfd2;

            }
            if(xml.name() == "delBfd3"){
                xml.readNext();
                QdelBfd3 = xml.text().toString();
                qDebug() << "QdelBfd3 : " << QdelBfd3;
            }
            if(xml.name() == "delName"){
                xml.readNext();
                QdelName = xml.text().toString();
                qDebug() << "QdelName : " << QdelName;

            }
            if(xml.name() == "delNamell"){
                xml.readNext();
                QdelNamell = xml.text().toString();
                qDebug() << "QdelNamell : " << QdelNamell;

            }
            if(xml.name() == "delUid"){
                xml.readNext();
                QdelUid = xml.text().toString();
                qDebug() << "QdelUid : " << QdelUid;

                query.clear();
                query.prepare("INSERT INTO Dealer (ownername, fpsuidno, del_bfd_1, del_bfd_2, del_bfd_3, fpsSessionId, vendorName, login_time, dist_code, dealer_fusion, auth_type_del,dealer_type) VALUES(:ownername, :fpsuidno, :del_bfd_1, :del_bfd_2, :del_bfd_3, :fpsSessionId, :vendorName, :login_time, :dist_code, :dealer_fusion,:auth_type_del,:dealer_type)");

                if(LangMode==1)
                    query.bindValue(":ownername",QdelName);
                else if(LangMode==2)
                    query.bindValue(":ownername",QdelNamell);

                query.bindValue(":fpsuidno",QdelUid);
                query.bindValue(":del_bfd_1",QdelBfd1);
                query.bindValue(":del_bfd_2",QdelBfd2);
                query.bindValue(":del_bfd_3",QdelBfd3);
                query.bindValue(":fpsSessionId","");
                query.bindValue(":vendorName","");
                query.bindValue(":login_time","");
                query.bindValue(":dist_code","");
                query.bindValue(":dealer_fusion",QdealerFusion);
                query.bindValue(":auth_type_del",Qdealer_type);
                query.bindValue(":dealer_type",Qdeltype);


                if(!query.exec())
                    qDebug() << query.lastError();
                else
                    qDebug()<<"Dealer Details inserted";
            }
            if(xml.name() == "wadhStatus"){
                xml.readNext();

                QwadhStatus_local = xml.text().toString();
                qDebug() << "QwadhStatus_local:" << QwadhStatus_local;

                if(Qdeltype == "D")
                {
                    QwadhStatus_D=QwadhStatus_local;
                    qDebug()<<"QwadhStatus_D:"<<QwadhStatus_D;
                }
                if(Qdeltype == "N1")
                {
                    QwadhStatus_N1=QwadhStatus_local;
                    qDebug()<<"QwadhStatus_N1:"<<QwadhStatus_N1;
                }
                if(Qdeltype == "N2")
                {
                    QwadhStatus_N2=QwadhStatus_local;
                    qDebug()<<"QwadhStatus_N2:"<<QwadhStatus_N2;
                }
            }

            if(xml.name() == "wadhValue"){
                xml.readNext();
                Qwadh_Dealer_global= xml.text().toString();
                qDebug() << "Qwadh_Dealer_global: " <<Qwadh_Dealer_global;
            }

            if(xml.name() == "fpsId"){
                xml.readNext();
                QfpsId= xml.text().toString();
                qDebug() << "QfpsId : " <<QfpsId;
                ui->label_shopid->setText(QfpsId);

                QSqlQuery query;
                query.prepare("UPDATE ShopOwnerDetails SET ShopID=?");
                query.addBindValue(QfpsId);
                if(!query.exec())
                    qDebug()<<"Shop No" << query.lastError();
            }
            if(xml.name() == "fpsSessionId"){
                xml.readNext();
                QfpsSessionId = xml.text().toString();
                qDebug() << "QfpsSessionId : " <<QfpsSessionId;

                QSqlQuery query;
                query.prepare("UPDATE Dealer SET fpsSessionId=?");
                query.addBindValue(QfpsSessionId);
                if(!query.exec())
                    qDebug()<< query.lastError();
            }
            if(xml.name() == "distCode"){
                xml.readNext();
                Qdist_code = xml.text().toString();
                qDebug() << "distCode: " << Qdist_code;
                Qdist_code_impds = Qdist_code;
            }
            if(xml.name() == "latitude"){
                xml.readNext();
                Qlatitude = xml.text().toString();
                qDebug() << "Qlatitude: " <<Qlatitude;
            }
            if(xml.name() == "loginRequestTime"){
                xml.readNext();
                QloginRequestTime = xml.text().toString();
                qDebug() << "QloginRequestTime: " <<QloginRequestTime;

                QSqlQuery query;
                query.prepare("UPDATE Dealer SET login_time=?");
                query.addBindValue(QloginRequestTime);
                if(!query.exec())
                    qDebug()<< query.lastError();
            }
            if(xml.name() == "logoutTime"){
                xml.readNext();
                Qlogout= xml.text().toString();
                qDebug() << "Qlogout: " <<Qlogout;

            }
            if(xml.name() == "longtude"){
                xml.readNext();
                Qlongtude = xml.text().toString();
                qDebug() << "Qlongtude: " <<Qlongtude;
            }
            if(xml.name() == "minQty"){
                xml.readNext();
                minqty_flag = xml.text().toString();
                qDebug() << "minqty_flag: " <<minqty_flag;
            }
            if(xml.name() == "weighAccuracyValueInGms"){
                xml.readNext();
                weighAccuracyValueInGms = xml.text().toString();
                qDebug() << "weighAccuracyValueInGms: " <<weighAccuracyValueInGms;
            }
            if(xml.name() == "routeOffEnable"){
                xml.readNext();
                QrouteOffEnable= xml.text().toString();
                qDebug() << "QrouteOffEnable: " <<QrouteOffEnable;

                QSqlQuery qry;
                qry.prepare("UPDATE Settings SET route_off_enable=?");
                qry.addBindValue(QrouteOffEnable);
                if(!qry.exec())
                    qDebug() << "route_off_enable = " << qry.lastError();

            }
            if(xml.name() == "versionUpdateRequired"){
                xml.readNext();
                QversionStatus= xml.text().toString();
                qDebug() << "QversionUpdateRequired: " <<QversionStatus;
            }
            if(xml.name() == "weighingStatus"){
                xml.readNext();
                QweighingStatus= xml.text().toString();
                qDebug() << "QweighingStatus: " <<QweighingStatus;

                QSqlQuery qry;
                qry.prepare("UPDATE Settings SET WeighStatus=?");
                qry.addBindValue(QweighingStatus);
                if(!qry.exec())
                    qDebug() << "QweighingStatus = " << qry.lastError();
            }
            if(xml.name() == "eKYCPrompt"){
                xml.readNext();
                QeKYCPrompt= xml.text().toString();
                eKYCPrompt = QeKYCPrompt;
                qDebug() << "QeKYCPrompt: " <<eKYCPrompt;
            }
            if(xml.name() == "ceritificatePath"){
                xml.readNext();
                QceritificatePath= xml.text().toString();
                qDebug() << "QceritificatePath: " <<QceritificatePath;
            }
            if(xml.name() == "ceritificatePath"){
                xml.readNext();
                QceritificatePath= xml.text().toString();
                qDebug() << "QceritificatePath: " <<QceritificatePath;
            }
            if(xml.name() == "fusionAttempts"){
                xml.readNext();
                QfusionAttempts= xml.text().toString();
                qDebug() << "QfusionAttempts: " <<QfusionAttempts;

                QSqlQuery qry;
                qry.prepare("UPDATE Settings SET fusion_attempts=?");
                qry.addBindValue(QfusionAttempts);
                if(!qry.exec())
                    qDebug() << "fusion attempts = " << qry.lastError();
            }
            if(xml.name() == "helplineNumber"){
                xml.readNext();
                QhelplineNumber= xml.text().toString();
                qDebug() << "QhelplineNumber: " <<QhelplineNumber;

                QSqlQuery qry;
                qry.prepare("UPDATE Settings SET helpline_number=?");
                qry.addBindValue(QhelplineNumber);
                if(!qry.exec())
                    qDebug() << "helpline number = " << qry.lastError();
            }
            if(xml.name() == "keyregisterDownloadStatus"){
                xml.readNext();
                PartiDataDownloadFlag = xml.text().toString();
                qDebug() << "PartiDataDownloadFlag : " << PartiDataDownloadFlag;
            }
            if(xml.name() == "partialOnlineOfflineStatus"){
                xml.readNext();
                partialOnlineOfflineStatus = xml.text().toString();
                qDebug() << "partialOnlineOfflineStatus:" << partialOnlineOfflineStatus;

            }
            if(xml.name() == "responseTimedOutTimeInSec"){
                xml.readNext();
                responseTimedOutTimeInSec = xml.text().toString();
                qDebug() << "responseTimedOutTimeInSec:" << responseTimedOutTimeInSec;
            }
            if(xml.name() == "keyregisterDataDeleteStatus"){
                xml.readNext();
                PartialTxnDeleteFlag = xml.text().toString();
                qDebug() << "PartialTxnDeleteFlag:" << PartialTxnDeleteFlag;
            }
            if(xml.name() == "fpsCbDownloadStatus"){
                xml.readNext();
                fpsCbDownloadStatus = xml.text().toString();
                qDebug() << "fpsCbDownloadStatus:" << fpsCbDownloadStatus;
            }
            if(xml.name() == "messageEng"){
                xml.readNext();
                QmessageEng= xml.text().toString();
                qDebug() << "QmessageEng: " <<QmessageEng;

                QSqlQuery qry;
                qry.prepare("UPDATE Settings SET message_eng=?");
                qry.addBindValue(QmessageEng);
                if(!qry.exec())
                    qDebug() << "QmessageEng = " << qry.lastError();
            }
            if(xml.name() == "messageLl"){
                xml.readNext();
                QmessageLl= xml.text().toString();
                qDebug() << "QmessageLl: " <<QmessageLl;

                QSqlQuery qry;
                qry.prepare("UPDATE Settings SET message_tel=?");
                qry.addBindValue(QmessageLl.toUtf8());
                if(!qry.exec())
                    qDebug() << "QmessageLl= " << qry.lastError();
            }
            if(xml.name() == "pdsClTranEng"){
                xml.readNext();
                QpdsClTranEng= xml.text().toString();
                qDebug() << "QpdsClTranEng: " <<QpdsClTranEng;

                QSqlQuery qry;
                qry.prepare("UPDATE Headers SET pds_cl_tran_eng=?");
                qry.addBindValue(QpdsClTranEng);
                if(!qry.exec())
                    qDebug() << "pds cl tran eng = " << qry.lastError();

            }
            if(xml.name() == "pdsClTranLl"){
                xml.readNext();
                QpdsClTranLl= xml.text().toString();
                qDebug() << "QpdsClTranLl: " <<QpdsClTranLl;

                QSqlQuery qry;
                qry.prepare("UPDATE Headers SET pds_cl_tran_tel=?");
                qry.addBindValue(QpdsClTranLl.toUtf8());
                if(!qry.exec())
                    qDebug() << "pds cl tran tel = " << qry.lastError();
            }
            if(xml.name() == "pdsTranEng"){
                xml.readNext();
                QpdsTranEng= xml.text().toString();
                qDebug() << "QpdsTranEng: " <<QpdsTranEng;

                QSqlQuery qry;
                qry.prepare("UPDATE Headers SET pds_tran_eng=?");
                qry.addBindValue(QpdsTranEng);
                if(!qry.exec())
                    qDebug() << "pds tran eng = " << qry.lastError();
            }
            if(xml.name() == "pdsTransLl"){
                xml.readNext();
                QpdsTransLl= xml.text().toString();
                qDebug() << "QpdsTransLl: " <<QpdsTransLl;

                QSqlQuery qry;
                qry.prepare("UPDATE Headers SET pds_tran_tel=?");
                qry.addBindValue(QpdsTransLl.toUtf8());
                if(!qry.exec())
                    qDebug() << "pds tran tel = " << qry.lastError();
            }
            if(xml.name() == "token"){
                xml.readNext();
                Qtoken= xml.text().toString();
                qDebug() << "Qtoken: " <<Qtoken;

                memset(HD_PASSWORD, 0, sizeof(HD_PASSWORD));
                strcpy(HD_PASSWORD, Qtoken.toAscii().data());
            }
            //wsdlURLAuth
            if(xml.name() == "wsdlOffline"){
                xml.readNext();
                Offline_and_ReceiveStock_URL = xml.text().toString().trimmed();
                qDebug() << "QwsdlOffline: " << Offline_and_ReceiveStock_URL;
            }
            if(xml.name() == "wsdlURLAuth"){
                xml.readNext();
                QwsdlURLAuth= xml.text().toString();
                qDebug() << "QwsdlURLAuth: " <<QwsdlURLAuth;
            }
            if(xml.name() == "wsdlURLPDS"){
                xml.readNext();
                QwsdlURLPDS= xml.text().toString();
                qDebug() << "QwsdlURLPDS: " <<QwsdlURLPDS;

                QSqlQuery query;
                query.prepare("UPDATE Settings SET URL=?");
                query.addBindValue(QwsdlURLPDS);

                if(!query.exec())
                    qDebug() << "1. UpdateURLs Dynamic = " << query.lastError();
            }
            if(xml.name() == "wsdlURLReceive"){
                xml.readNext();
                QwsdlURLReceive= xml.text().toString();
                qDebug() << "QwsdlURLReceive: " <<QwsdlURLReceive;
            }

            if(xml.name() == "impdsURL")
            {
                xml.readNext();
                impdsURL = xml.text().toString();
                qDebug() << "impdsURL : " << impdsURL;

                QFile file("/mnt/jffs2/impds.ini");
                file.remove();
                if(file.open(QIODevice::ReadWrite | QIODevice::Text))
                {
                    QTextStream out(&file);
                    out << "[General]\n";
                    out << "URL=" << impdsURL.trimmed() << "\n";
                    out << "DataBase=/mnt/jffs2/db_commonpds_2_3.db\n";
//                    out << "SKey=" << skey;
                    file.close();
                }
            }
            if(xml.name() =="pmsMenuNameEn")
            {
                xml.readNext();
                PmsMenuEn=xml.text().toString();
                ui->BtnSwavalamban->setText(PmsMenuEn);
            }
            if(xml.name() == "pmsURL")
            {
                xml.readNext();
                pmsURL = xml.text().toString();
                qDebug() << "pmsURL : " << pmsURL;

                QFile file1("/mnt/jffs2/swavalamban.ini");
                file1.remove();
                if(file1.open(QIODevice::ReadWrite | QIODevice::Text))
                {
                    QTextStream out(&file1);
                    out << "[General]\n";

                    out << "URL=" << pmsURL.trimmed() << "\n";
                    out << "DataBase=/mnt/jffs2/db_commonpds_2_3.db"<<"\n";
                    file1.close();
                }
            }
            if(xml.name() == "pmsWadh")
            {
                xml.readNext();
                g_pmsWadh = xml.text().toString();
                qDebug() << "g_pmsWadh : " << g_pmsWadh;
            }
            if(xml.name() == "irisStatus"){
                xml.readNext();
                irisStatus= xml.text().toString();
                qDebug() << "1.irisStatus: " <<irisStatus;
            }
            //Print Paper Flag//
            if(xml.name() == "paperRequiredFlag"){
                xml.readNext();
                paperRequiredFlag = xml.text().toString();
//                paperRequiredFlag = "N";
                qDebug() << "PaperRequiredFlag: " <<paperRequiredFlag;

                QFile file("/mnt/jffs2/Flags.txt");
                file.remove();
                if(file.open(QIODevice::ReadWrite | QIODevice::Text))
                {
                    QTextStream out(&file);
                    out<<paperRequiredFlag.trimmed();
                    file.close();
                }
            }
            //////////Reasons to back from Txn //////////////////////////
            if(xml.name() == "reasonId"){
                xml.readNext();
                reasonId = xml.text().toString();
                qDebug() << "reasonId : " << reasonId;
                list_tmp.push_back(reasonId);
            }
            if(xml.name() == "reasonValue"){
                xml.readNext();
                reasonValue = xml.text().toString();
                qDebug() << "reasonValue : " << reasonValue;
                list_tmp.push_back(reasonValue);
                Reasons_Vec.push_back(list_tmp);
                list_tmp.clear();
            }
            /////////////////////////////////////////////////////////////
            ///////////Common application state information//////////////

            if(xml.name() == "consentHeader"){
                xml.readNext();
                consentHeader= xml.text().toString();
                qDebug() << "consentHeader: " <<consentHeader;
            }
            if(xml.name() == "stateCode"){
                xml.readNext();
                stateCode= xml.text().toString();
                qDebug() << "stateCode: " <<stateCode;
            }
            if(xml.name() == "stateNameEn"){
                xml.readNext();
                stateNameEn= xml.text().toString();
                qDebug() << "stateNameEn: " <<stateNameEn;
            }
            if(xml.name() == "stateNameLl"){
                xml.readNext();
                stateNameLl= xml.text().toString();
                qDebug() << "stateNameLl: " <<stateNameLl;
            }
            if(xml.name() == "stateProfile"){
                xml.readNext();
                stateProfile= xml.text().toString();
                qDebug() << "stateProfile: " <<stateProfile;
            }
            if(xml.name() == "stateReceiptHeaderEn"){
                xml.readNext();
                stateReceiptHeaderEn= xml.text().toString();
                qDebug() << "stateReceiptHeaderEn: " <<stateReceiptHeaderEn;
            }
            if(xml.name() == "stateReceiptHeaderLl"){
                xml.readNext();
                stateReceiptHeaderLl= xml.text().toString();
                qDebug() << "stateReceiptHeaderLl: " <<stateReceiptHeaderLl;
            }
            if(xml.name() == "statefpsId"){
                xml.readNext();
                statefpsId= xml.text().toString();
                qDebug() << "statefpsId: " <<statefpsId;

                QSqlQuery query;
                query.clear();

                query.prepare("INSERT INTO StateInfo(stateCode,stateNameEn,stateNameLl,stateProfile,stateReceiptHeaderEn,stateReceiptHeaderLl,statefpsId,consentHeader) VALUES(:stateCode,:stateNameEn,:stateNameLl,:stateProfile,:stateReceiptHeaderEn,:stateReceiptHeaderLl,:statefpsId,:consentHeader)");
                query.bindValue(":stateCode", stateCode);
                query.bindValue(":stateNameEn", stateNameEn);
                query.bindValue(":stateNameLl", stateNameLl);
                query.bindValue(":stateProfile", stateProfile);
                query.bindValue(":stateReceiptHeaderEn", stateReceiptHeaderEn);
                query.bindValue(":stateReceiptHeaderLl", stateReceiptHeaderLl);
                query.bindValue(":statefpsId", statefpsId);
                query.bindValue(":consentHeader", consentHeader);
                if(!query.exec())
                    qDebug()<<"StateInfo" << query.lastError();
                else
                    qDebug()<<"StateInfo inserted";
            }
            ///////////Common application state information//////////////

            if(xml.name() == "idType"){
                xml.readNext();
                QidType= xml.text().toString();
                qDebug() << "QidType: " <<QidType;
            }
            if(xml.name() == "idValue"){
                xml.readNext();
                QidValue= xml.text().toString();
                qDebug() << "QidValue: " <<QidValue;
            }
            if(xml.name() == "opeValue"){
                xml.readNext();
                QopeValue= xml.text().toString();
                qDebug() << "QopeValue: " <<QopeValue;
            }
            if(xml.name() == "oprMode"){
                xml.readNext();
                QoprMode= xml.text().toString();
                qDebug() << "QoprMode: " <<QoprMode;

                QSqlQuery query;
                query.clear();
                query.prepare("INSERT INTO TransactionModes(idType, idValue, opeValue, oprMode) VALUES(:idType, :idValue, :opeValue, :oprMode)");
                query.bindValue(":idType", QidType);
                query.bindValue(":idValue", QidValue);
                query.bindValue(":opeValue", QopeValue);
                query.bindValue(":oprMode", QoprMode);
                if(!query.exec())
                    qDebug()<<"TransactionModes" << query.lastError();
                else
                    qDebug()<<"TransactionModes inserted";
            }
        }
    }
    file.close();

    //DEBUG//
    QList <QString> RecordList;
    for(int i =0 ; i < Reasons_Vec.size() ; i++)
    {
        RecordList.clear();
        RecordList =  Reasons_Vec[i];
        qDebug() <<"--------------------------------" << RecordList.size();
        for(int j =0 ; j < RecordList.size() ; j++)
            qDebug() << RecordList[j];
    }
    //DEBUG//

    qDebug()<<"Parting doing"<<endl;

    return 0;
}

int Widget::parseAuthResponse(char *filename)
{
    database_queries qry;
    QFile rd_file(filename);

    if(!rd_file.exists()){
        qDebug() << "Dealer Auth Response File Not Found...";
        miscObj.DisplayWideMsgBox((char *)"Dealer Auth Response File Not Found...");
        return -1;
    }
    QDomDocument document;
    if(!document.setContent(&rd_file)){
        qDebug( "Failed to parse the file into a DOM tree." );
        rd_file.close();

        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Invalid Response Data Recieved...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"अवैध प्रतिक्रिया डेटा प्राप्त.");
        return -1;
    }
    rd_file.close();

    QDomNodeList list;
    list = document.elementsByTagName("return");
    QString QResponse = list.at(0).childNodes().at(1).toElement().text();
    qDebug()<<"\nFinal Dealer Auth Response : " << QResponse;

    QFile wr_file("/mnt/jffs2/DealerWriteresponse.xml");
    wr_file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&wr_file);
    out << QResponse;
    wr_file.close();

    QString Qauth_date, Qauth_reasonVal, Qauth_statusVal;
    QString Qauth_transaction_codeVal, Qauth_err_code;

    QFile *file = new QFile("/mnt/jffs2/DealerWriteresponse.xml");
    if(!file->open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "ParseBFD_Response(): Error Reading File";
        return -1;
    }

    QXmlStreamReader xml(file);
    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartDocument)
            continue;
        if(token == QXmlStreamReader::StartElement){
            qDebug() << xml.name();
            if(xml.name() == "error"){
                qDebug() << "Error Not Found" << endl;
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox((char *)"Invalid Response Data Recieved...");
                else if(LangMode == 2)
                    miscObj.tel_DisplayWideMsgBox((char *)"अवैध प्रतिक्रिया डेटा प्राप्त.");
                return -1;
            }

            while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "void property"))
            {
                QString QStr = xml.attributes().value("property").toString();
                if(QStr == "auth_date"){
                    xml.readNextStartElement();
                    xml.readNext();
                    Qauth_date = xml.text().toString();
                    qDebug() << "Qauth_date = " << Qauth_date;
                }else if(QStr == "auth_err_code"){
                    xml.readNextStartElement();
                    xml.readNext();
                    Qauth_err_code = xml.text().toString();
                    qDebug() << "Qauth_err_code = " << Qauth_err_code;
                }else if(QStr == "auth_reason"){
                    xml.readNextStartElement();
                    xml.readNext();
                    Qauth_reasonVal = xml.text().toString();
                    qDebug() << "Qauth_reasonVal = " << Qauth_reasonVal;
                }else if(QStr == "auth_status"){
                    xml.readNextStartElement();
                    xml.readNext();
                    Qauth_statusVal = xml.text().toString();
                    qDebug() << "Qauth_statusVal = " << Qauth_statusVal;
                }else if(QStr == "auth_transaction_code"){
                    xml.readNextStartElement();
                    xml.readNext();
                    Qauth_transaction_codeVal = xml.text().toString();

                    if(iDealMemSelected == ROUTE_OFFICER_SELECTED){
                        memset(gRoute_Off_auth_code, 0, sizeof(gRoute_Off_auth_code));
                        strcpy(gRoute_Off_auth_code, Qauth_transaction_codeVal.toAscii().data());
                        qDebug()<<"gRoute_Off_auth_code :"<< gRoute_Off_auth_code;
                    }else{
                        memset(gauth_transaction_code, 0, sizeof(gauth_transaction_code));
                        strcpy(gauth_transaction_code, Qauth_transaction_codeVal.toAscii().data());
                    }
                    if(qry.getRecvGoodsRuteOfficerStatus() != true){
                        memset(gRoute_Off_auth_code, 0, sizeof(gRoute_Off_auth_code));
                        strcpy(gRoute_Off_auth_code,"0"); //added by vivek 05082017
                    }
                    qDebug() << "Qauth_transaction_codeVal = " << Qauth_transaction_codeVal;
                }
                break;
            }
        }
    }
    char cRespMsg[64];
    memset(cRespMsg, 0, sizeof(cRespMsg));

    char dispbuff[128];
    memset(dispbuff, 0, sizeof(dispbuff));

    DisplayBFDRespCode(Qauth_statusVal.toAscii().data(), cRespMsg);

    //    if((Qauth_statusVal == "100") && Qtxnflow == "F")
    //        return 0; // -BSK.
    qDebug() << "1.Qauth_statusVal ::" << Qauth_statusVal << "Qtxnflow ::" << Qtxnflow;
    if((Qauth_statusVal == "100") && Qtxnflow == "F")
    {
        if(iDealMemSelected == MEMBER_SELECTED)
            getcomTxn_type="F";

        BenAuthSUCCESS = 1;
        qDebug() << "2.Qauth_statusVal ::" << Qauth_statusVal;
        return 0;
    }

    sprintf(dispbuff, "Response Message: %s(%s)", Qauth_reasonVal.toAscii().data(), Qauth_err_code.toAscii().data());

    //    if(Qauth_statusVal.isEmpty() || Qauth_statusVal.isNull() || Qtxnflow == "B")
    if(Qauth_statusVal.isEmpty() || Qauth_statusVal.isNull())
    {
        miscObj.DisplayWideMsgBox(dispbuff);
        return -1;
    }

    if(iDealMemSelected == MEMBER_SELECTED){
        qDebug()<<"MEMBER_SELECTED <<endl";
        miscObj.DisplayWideMsgBox(dispbuff);
    }else
        qDebug()<<"iDealMemSelected==="<<iDealMemSelected;
    return 0;
}

int Widget::parseupdateAndInsertServiceResponse(char *filename)
{
    QFile file(filename);
    if(!file.exists()){
        qDebug() << "File Not Found";
        return -1;
    }

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error Reading File" << endl;
        return -1;
    }

    QDomDocument document;
    if(!document.setContent( &file ) ){
        qDebug( "Failed to parse the file into a DOM tree." );
        file.close();
        return -1;
    }
    file.close();

    QDomNodeList list;

    list = document.elementsByTagName("return"); //env:Body
    QString QResponse = list.at(0).childNodes().at(0).toElement().text();
    qDebug()<<"\nFinal Response : " << QResponse;

    if(QResponse.contains("SUCCESSFULL",Qt::CaseInsensitive) == true)
        return 0;
    else if(QResponse.length() == 0){
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Invalid Response Data Recieved...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"अमान्य डेटा प्राप्त हुआ.");
        return -1;
    }else{
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);
        miscObj.DisplayWideMsgBox(QResponse.toAscii().data());
        return -1;
    }
    return 0;
}

int Widget::GetCardHolderDetails(int iRationType)
{
    qDebug()<<"Inside GetCardHolderDetails";
    int ret = 0;

    GPRS objGprs;
    miscObj.AudioTest((char *)"183");

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == ENGLISH)
        dlgPopup1->ui->lblPopup->setText("Please Wait...\nDownloading Members Details...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.\nसदस्यों विवरण डाउनलोड हो रहा है. ."));
    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    /*if(partialOnlineOfflineStatus == "Y" || partialOnlineOfflineStatus == "y")
    {
          char TmpStr[5];
          memset(TmpStr,0x00,sizeof(TmpStr));
          FILE *fp = popen("ping google.com -c 5|grep packets|awk {'print $7'}","r");
          fgets(TmpStr,sizeof(TmpStr),fp);
          pclose(fp);
          qDebug()<<"Ping TmpStr:"<<TmpStr;
          if(strncmp(TmpStr,"0%",2)!=0)
          {
              dlgPopup1->close();
              dlgPopup1->deleteLater();

              if(ui->radioButton_UIDFLAG->isChecked()==true)
              {
                      miscObj.DisplayMsgBox((char *)"No Network\nPlease select \"RC NO\" and Enter \"RC NO\" for doing TXN in OFFLINE");
                      ui->ldtRationCard->setText("");
                      ui->radioButton_RCNOFLAG->setChecked(true);
                      on_radioButton_RCNOFLAG_clicked(true);
                      return -1;
              }
              GetOfflineCommDetails();
              return -1;
      }
    }*/

    if(objGprs.Check_Network_Available() < 0){
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        CheckOfflineTxnGetDetails();
        return -1;
    }
    char FileName[64];
    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/CardHolder.xml");

    system("rm /mnt/jffs2/CardHolder.xml");
    system("rm /mnt/jffs2/CardHolderDetails.xml");

    qDebug()<<"Ration Type : "<<iRationType;

    if(iRationType  == 1)
        ret = prepareCardHolderReqFile(FileName);

    system("cat /mnt/jffs2/CardHolder.xml");

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

    if(iRationType == 1)
    {
        //        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/CardHolderDetails.xml", SERVER_TIME_OUT);

        int ret_http_code = UrlPingCheck(cUrlName);

        if(ret_http_code == 1)
        {
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/CardHolderDetails.xml", responseTimedOutTimeInSec.toInt());
        }
        else
        {
            sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName,PDS_URL2, "/mnt/jffs2/CardHolderDetails.xml", responseTimedOutTimeInSec.toInt());
        }
    }

    qDebug()<<"\nCurl Cmd : "<<cmdCurl;
    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0){
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        CheckOfflineTxnGetDetails();
        return -1;
    }

    qDebug() << "************CardHolderDetails.xml**********";
    system("cat /mnt/jffs2/CardHolderDetails.xml");
    qDebug() << "*******************************************";

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    if(ParsingFirstCondition((char *)"/mnt/jffs2/CardHolderDetails.xml") < 0)
        return -1;

    query.clear();
    query.exec("DELETE FROM CustomerFamily");
    query.clear();
    query.exec("DELETE FROM CustomerMaster");
    query.clear();
    query.exec("DELETE FROM BenQuotaTable");
    query.clear();
    query.exec("DELETE FROM ItemMaster");
    query.clear();
    query.exec("DELETE FROM FestDetails");

    parseCardHolderDetails("/mnt/jffs2/CardHolderDetails.xml");
    return 0;
}

int Widget::parseCardHolderDetails(char *filename)
{
    QFile file(filename);
    if(!file.exists()){
        qDebug() << "File Not Found";
        return -1;
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error Reading File" << endl;
        return -2;
    }
    QString TmpStr = QTextStream(&file).readAll();
    file.close();
    QDomDocument document;
    if(!document.setContent(TmpStr) ){
        qDebug( "Failed to parse the file into a DOM tree." );
        return -3;
    }
    QString RationCardId, Name;
    int ChildCount=0,i = 0, j=0;
    QDomNodeList list;
    QDomNodeList RCDetails;

    RCDetails = document.elementsByTagName("rationCardDetails");
    MinQtyStr.clear();
    ChildCount = RCDetails.at(0).childNodes().count();
    qDebug() << "Child Count = " << ChildCount;
    int k = 0, l=0;
    for(i=0, j=0; i< ChildCount; i++){
        qDebug()<<RCDetails.at(0).childNodes().at(i).nodeName().toAscii().data();
        if(RCDetails.at(0).childNodes().at(i).nodeName() == "carddetails"){
            list = document.elementsByTagName("carddetails");
            QString address         = list.at(0).childNodes().at(0).toElement().text();
            QString cardHolderName  = list.at(0).childNodes().at(1).toElement().text();
            QString cardType        = list.at(0).childNodes().at(2).toElement().text();
            QString familyMemCount  = list.at(0).childNodes().at(3).toElement().text();
            QString houseHoldCardNo = list.at(0).childNodes().at(4).toElement().text();
            QString lpgStatus       = list.at(0).childNodes().at(5).toElement().text();
            QString lpgType         = list.at(0).childNodes().at(6).toElement().text();
            QString mobileNoUpdate  = list.at(0).childNodes().at(7).toElement().text(); //added by vivek 01082017
            QString officeName      = list.at(0).childNodes().at(8).toElement().text();
            QString rcId            = list.at(0).childNodes().at(9).toElement().text();
            QString schemeId        = list.at(0).childNodes().at(10).toElement().text();

            SurveyStsFlag.clear();
            SurveyMsgEn.clear();
            SurveyMsgLl.clear();
            SurveyMinQty.clear();

            SurveyMsgEn             = list.at(0).childNodes().at(11).toElement().text();
            SurveyMsgLl             = list.at(0).childNodes().at(12).toElement().text();
            SurveyMinQty            = list.at(0).childNodes().at(13).toElement().text();
            SurveyStsFlag           = list.at(0).childNodes().at(14).toElement().text();

            qDebug()<<"SurveyStsFlag:"<<SurveyStsFlag;
            qDebug()<<"SurveyMsgEn:"<<SurveyMsgEn.toUtf8();
            qDebug()<<"SurveyMsgLl:"<<SurveyMsgLl.toUtf8();

            QString type_card       = list.at(0).childNodes().at(15).toElement().text();

            zcommboCommCode_global.clear();
            zcommboCommCode_global= list.at(0).childNodes().at(16).toElement().text();

            zcommboStatus_global.clear();
            zcommboStatus_global = list.at(0).childNodes().at(17).toElement().text();

            //<zheadmobileno>9912043219</zheadmobileno>
            QString zheadmobileno= list.at(0).childNodes().at(18).toElement().text();
            QString znpnsBalance_global=list.at(0).childNodes().at(19).toElement().text();

            Qwadh_Member_global.clear();
            Qwadh_Member_global=list.at(0).childNodes().at(20).toElement().text();

            qDebug()<<"Qwadh_Member_global :"<<Qwadh_Member_global;

            RationCardId = houseHoldCardNo;
            Rcno = houseHoldCardNo;
            Name = cardHolderName;

            qDebug()<<"houseHoldCardNo  : "<<houseHoldCardNo;
            qDebug()<<"cardHolderName   : "<<cardHolderName;
            qDebug()<<"address          : "<<address;
            qDebug()<<"lpgStatus        : "<<lpgStatus;
            qDebug()<<"lpgType          : "<<lpgType;
            qDebug()<<"familyMemCount   : "<<familyMemCount;
            qDebug()<<"officeName       : "<<officeName;
            qDebug()<<"rcId             : "<<rcId;
            qDebug()<<"schemeId         : "<<schemeId;
            qDebug()<<"type_card        : "<<type_card;
            qDebug()<<"cardType         : "<<cardType;
            qDebug()<<"mobileNoUpdate   : "<<mobileNoUpdate;
            qDebug()<<"znpnsBalance:"<<znpnsBalance_global;

            QSqlQuery query;
            query.prepare("INSERT INTO CustomerMaster(RationCardId, CustomerName, address, LpgGasStatus, LpgType, CardType, MemberCount, OfficeName, RcId, schemeId, type_card,znpnsbalance,headmobileno) VALUES(:RationCardId, :CustomerName, :address, :LpgGasStatus, :LpgType, :CardType, :MemberCount, :OfficeName, :RcId, :schemeId, :type_card, :znpnsbalance, :headmobileno)");
            query.bindValue(":RationCardId",houseHoldCardNo);
            query.bindValue(":CustomerName",cardHolderName);
            query.bindValue(":Address",     address);
            query.bindValue(":LpgGasStatus",lpgStatus);
            query.bindValue(":LpgType",     lpgType);
            query.bindValue(":CardType",    cardType);
            query.bindValue(":MemberCount", familyMemCount);
            query.bindValue(":OfficeName",  officeName);
            query.bindValue(":RcId",        rcId);
            query.bindValue(":schemeId",    schemeId);
            query.bindValue(":type_card",   type_card);
            query.bindValue(":znpnsbalance",znpnsBalance_global);
            query.bindValue("headmobileno",zheadmobileno);
            int ret = query.exec();
            if(ret == false){
                qDebug()<<query.lastError();
                return -1;
            }
        }else if(RCDetails.at(0).childNodes().at(i).nodeName() == "commDetails"){
            list = document.elementsByTagName("commDetails");
            QString allocationType = list.at(j).childNodes().at(0).toElement().text();
            QString allotedMonth = list.at(j).childNodes().at(1).toElement().text();
            QString allotedYear  = list.at(j).childNodes().at(2).toElement().text();
            QString availedQty  = list.at(j).childNodes().at(3).toElement().text();
            QString balQty      = list.at(j).childNodes().at(4).toElement().text();
            QString closingBal  = list.at(j).childNodes().at(5).toElement().text();
            QString commName    = list.at(j).childNodes().at(6).toElement().text();
            QString commNamell  = list.at(j).childNodes().at(7).toElement().text(); //New Tag Added 15052017 vivek
            QString commcode    = list.at(j).childNodes().at(8).toElement().text();
            QString measureUnit = list.at(j).childNodes().at(9).toElement().text();
            QString minQty      = list.at(j).childNodes().at(10).toElement().text();
            MinQtyStr.push_back(list.at(j).childNodes().at(10).toElement().text());
            QString price       = list.at(j).childNodes().at(11).toElement().text();
            QString requiredQty = list.at(j).childNodes().at(12).toElement().text();
            QString totQty      = list.at(j).childNodes().at(13).toElement().text();
            QString weighingflag= list.at(j).childNodes().at(14).toElement().text();

            qDebug()<<"allocationType: "<<allocationType;
            qDebug()<<"allotedMonth  : "<<allotedMonth;
            qDebug()<<"allotedYear   : "<<allotedYear;
            qDebug()<<"availedQty   : "<<availedQty;
            qDebug()<<"balQty       : "<<balQty;
            qDebug()<<"closingBal   : "<<closingBal;
            qDebug()<<"commName     : "<<commName;
            qDebug()<<"commName_ll  : "<<commNamell.toUtf8(); // QcommNamell (Language Converstion from English to hind vivek 17052017)
            qDebug()<<"commcode     : "<<commcode;
            qDebug()<<"measureUnit  : "<<measureUnit;
            qDebug()<<"price        : "<<price;
            qDebug()<<"requiredQty  : "<<requiredQty;
            qDebug()<<"totQty       : "<<totQty<<"#########";

            QSqlQuery query;
            query.prepare("INSERT INTO BenQuotaTable (RationCardId, CardHolderName, BalQty, MonthlyQuota, ItemCode, ItemType, ItemPrice, AvailedQty, MeasureUnit, RequiredQty, closingBal, commNamell,weighingFlag,minimumqty,allocationType,allotedMonth,allotedYear) VALUES( :RationCardId, :CardHolderName, :BalQty, :MonthlyQuota, :ItemCode, :ItemType, :ItemPrice, :AvailedQty, :MeasureUnit, :RequiredQty, :closingBal, :commNamell,:weighingFlag,:minimumqty,:allocationType,:allotedMonth,:allotedYear)");
            query.bindValue(":RationCardId",Rcno.toAscii().data());
            query.bindValue(":CardHolderName",  Name);
            query.bindValue(":BalQty",          balQty);
            query.bindValue(":MonthlyQuota",    totQty);
            query.bindValue(":ItemCode",commcode);
            query.bindValue(":ItemType",commName.trimmed());
            query.bindValue(":ItemPrice",price);
            query.bindValue(":AvailedQty",availedQty);
            query.bindValue(":MeasureUnit", measureUnit);
            query.bindValue(":RequiredQty", requiredQty);
            query.bindValue(":closingBal", closingBal);
            query.bindValue(":commNamell",commNamell);
            query.bindValue(":weighingFlag",weighingflag);
            query.bindValue(":minimumqty",minQty);
            query.bindValue(":allocationType",allocationType);
            query.bindValue(":allotedMonth",allotedMonth);
            query.bindValue(":allotedYear",allotedYear);

            int ret = query.exec();
            if(ret == false){
                qDebug()<<query.lastError();
                return -1;
            }
            query.clear();
            query.prepare("INSERT INTO ItemMaster (ItemCode, ItemType, ItemNameEn, ItemNameTel, ItemPrice, Month, Year,MeasureUnit) VALUES( :ItemCode, :ItemType, :ItemNameEn, :ItemNameTel, :ItemPrice, :Month, :Year,:MeasureUnit)");
            query.bindValue(":ItemCode",    commcode);
            query.bindValue(":ItemType",    commName.trimmed());
            query.bindValue(":ItemNameEn",  commName.trimmed());
            query.bindValue(":ItemNameTel", commNamell);
            query.bindValue(":ItemPrice",   price);
            query.bindValue(":Month",       "");
            query.bindValue(":Year",        "");
            query.bindValue(":MeasureUnit", measureUnit);

            ret = query.exec();
            if(ret == false)
                qDebug()<<query.lastError();
            query.clear();
            if(LangMode == 1){
                query.prepare("UPDATE ItemMaster SET ItemPrice=? WHERE ItemNameEn = ?");
                query.addBindValue(price);
                query.addBindValue(commName);
            }
            if(LangMode == 2){
                query.prepare("UPDATE ItemMaster SET ItemPrice=? WHERE ItemNameTel= ?");
                query.addBindValue(price);
                query.addBindValue(commNamell);
            }
            if(!query.exec()){
                qDebug() <<"UPDATE ItemMaster" <<query.lastError();
                return -1;
            }
            j++;
        }else if(RCDetails.at(0).childNodes().at(i).nodeName() == "memberdetails"){
            list = document.elementsByTagName("memberdetails");
            QString bfd_1           = list.at(k).childNodes().at(0).toElement().text();
            QString bfd_2           = list.at(k).childNodes().at(1).toElement().text();
            QString bfd_3           = list.at(k).childNodes().at(2).toElement().text();
            QString memberName      = list.at(k).childNodes().at(3).toElement().text();
            QString memberNamell    = list.at(k).childNodes().at(4).toElement().text();
            QString memberFusion    = list.at(k).childNodes().at(5).toElement().text();
            QString uid             = list.at(k).childNodes().at(6).toElement().text();
            QString uidStatus       = list.at(k).childNodes().at(7).toElement().text();
            QString mem_auth_finger = list.at(k).childNodes().at(8).toElement().text();
            QString mem_auth_iris   = list.at(k).childNodes().at(9).toElement().text();
            //            QString dealerFlag      = list.at(k).childNodes().at(10).toElement().text(); //zdealerFlag
            QString mem_auth_manual = list.at(k).childNodes().at(10).toElement().text();
            QString memberId        = list.at(k).childNodes().at(11).toElement().text();
            QString mem_auth_otp    = list.at(k).childNodes().at(12).toElement().text();
            QString zwgenWadhAuth= list.at(k).childNodes().at(13).toElement().text();

            qDebug() << "CHECK MemberID IS PRESENCE IN RESPONSE FILE for CardHolderDetails";
            qDebug()<<"**************************************************";
            qDebug() << "bfd_1 : " << bfd_1;
            qDebug() << "bfd_2 : " << bfd_2;
            qDebug() << "bfd_3 : " << bfd_3;
            qDebug() <<"memberName   : "<<memberName;
            qDebug() <<"memberNamell   : "<<memberNamell.toUtf8(); // tesing 17052017
            qDebug() <<"memberFusion   : "<<memberFusion;
            qDebug() <<"UID          : "<<uid<<"#########";
            qDebug() <<"UID Status   : "<<uidStatus<<"#########";
            qDebug() <<"mem_auth_finger:"<<mem_auth_finger;
            qDebug() <<"mem_auth_iris:"<<mem_auth_iris;
            qDebug() <<"mem_auth_manual:"<<mem_auth_manual;
            //            qDebug() <<"dealerFlag:" <<dealerFlag;
            qDebug() <<"memberId:"<<memberId;
            qDebug() <<"mem_auth_otp&&&:"<<mem_auth_otp;

            QSqlQuery query;
            query.prepare("INSERT INTO CustomerFamily (RationCardId, MemberNameEn, MemberUID, UID, bfd_1, bfd_2, bfd_3, MemberFusion, UID_Status, MemberId, MemberNamell,Memberauth_finger,Memberauth_iris,Memberauth_manual,Memberauth_otp) values(:RationCardId, :MemberNameEn, :MemberUID, :UID, :bfd_1, :bfd_2, :bfd_3, :MemberFusion, :UID_Status, :MemberId, :MemberNamell,:Memberauth_finger,:Memberauth_iris,:Memberauth_manual,:Memberauth_otp)");
            query.bindValue(":RationCardId",RationCardId);
            query.bindValue(":MemberNameEn",memberName);
            query.bindValue(":MemberUID", uid);
            query.bindValue(":UID",zwgenWadhAuth);
            query.bindValue(":bfd_1", bfd_1);
            query.bindValue(":bfd_2", bfd_2);
            query.bindValue(":bfd_3", bfd_3);
            query.bindValue(":MemberFusion", memberFusion);
            query.bindValue(":UID_Status", uidStatus);
            query.bindValue(":MemberId",memberId);
            query.bindValue(":MemberNamell",memberNamell);
            query.bindValue(":Memberauth_finger",mem_auth_finger);
            query.bindValue(":Memberauth_iris",mem_auth_iris);
            query.bindValue(":Memberauth_manual",mem_auth_manual); //zdealerFlag
            query.bindValue(":Memberauth_otp",mem_auth_otp);
            int ret = query.exec();
            if(ret == false){
                qDebug()<<query.lastError();
                return -1;
            }
            k++;
        }else if(RCDetails.at(0).childNodes().at(i).nodeName() == "festDetails"){
            list = document.elementsByTagName("festDetails");
            QString commodity_code  = list.at(l).childNodes().at(0).toElement().text();
            QString commodity_name  = list.at(l).childNodes().at(1).toElement().text();
            QString price           = list.at(l).childNodes().at(2).toElement().text();
            QString quantity        = list.at(l).childNodes().at(3).toElement().text();
            QString unit            = list.at(l).childNodes().at(4).toElement().text();

            qDebug() << "commodity_code : " << commodity_code;
            qDebug() << "commodity_name : " << commodity_name;
            qDebug() << "price          : " << price;
            qDebug() << "quantity        : "<<quantity;
            qDebug() << "unit            : "<<unit<<"#########";

            QSqlQuery query;
            query.prepare("INSERT INTO FestDetails (CommodityCode, CommodityName, Price, Quantity, Unit) values(:CommodityCode, :CommodityName, :Price, :Quantity, :Unit)");
            query.bindValue(":CommodityCode",commodity_code);
            query.bindValue(":CommodityName",commodity_name.toUtf8());
            query.bindValue(":Price"        ,price);
            query.bindValue(":Quantity"     ,quantity);
            query.bindValue(":Unit"         ,unit);
            int ret = query.exec();
            if(ret == false){ qDebug()<<query.lastError();return -1;}
            l++;
        }else if(RCDetails.at(0).childNodes().at(i).nodeName() == "festMessage"){
            list = document.elementsByTagName("festMessage");
            FestMsgEn   = list.at(0).childNodes().at(0).toElement().text();
            FestMsgTel  = list.at(0).childNodes().at(1).toElement().text();
            qDebug() << "FestMsgEn : " << FestMsgEn;
            qDebug() << "FestMsgTel : " << FestMsgTel;
        }else if(RCDetails.at(0).childNodes().at(i).nodeName() == "previousBal"){
            list = document.elementsByTagName("previousBal");
            due_tag_display = list.at(0).childNodes().at(0).toElement().text();
            due_tag_display_tel = list.at(0).childNodes().at(1).toElement().text();
            previous_amount = list.at(0).childNodes().at(2).toElement().text();

            qDebug() << "due tag display eng : " << due_tag_display;
            qDebug() << "previous amount : " << previous_amount;
        }
    }
    return 0;
}

void Widget::DisplayBFDRespCode(char *pucRespCode, char *pucRespMsg)
{
    int ucLoop = 0;
    char pucRespMessage[64];

    for(ucLoop = 0; ucLoop < RESP_CODE_INDEX; ucLoop++){
        if(strcmp(pucRespCode, gstRespCodeAndMessage[ucLoop].aucRespCode) == 0){
            qDebug() << "*****------------";
            memset(pucRespMessage, 0, sizeof(pucRespMessage));
            memcpy(pucRespMessage, gstRespCodeAndMessage[ucLoop].aucRespMessage, 64);
            strcpy(pucRespMsg, pucRespMessage);
            break;
        }else
            strcpy(pucRespMsg, "Invalid Response Code Recieved");
    }
}

void Widget::on_BtnBack_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(18);
    return;
}

void Widget::on_BtnUSBUpdateApp_clicked()
{
    if(PatchUpdateOfflineCheck()<0)
        return;
    int ret = 0;
    USB usbObj;

    qDebug() << "************************************";
    qDebug() << "Updating New Application Through USB";
    qDebug() << "************************************";

    QMessageBox msg;
    msg.setStyleSheet("background-color: rgb(35, 173, 111)");
    msg.setWindowFlags(Qt::FramelessWindowHint);

    if(LangMode == 1){
        msg.setText("Do You Want to Update New Application?");
        QPushButton *yesBtn = msg.addButton("Yes",QMessageBox::ActionRole);
        QPushButton *noBtn = msg.addButton("No",QMessageBox::ActionRole);
        msg.exec();
        if(msg.clickedButton() == noBtn)
            return;
    }else if(LangMode == 2){
        msg.setText(trUtf8("क्या आपको नया ऐप अपडेट करना है?"));
        QPushButton *yesBtn = msg.addButton(trUtf8("हाँ!"),QMessageBox::ActionRole);
        QPushButton *noBtn = msg.addButton(trUtf8("नहीं"),QMessageBox::ActionRole);
        msg.exec();
        if(msg.clickedButton() == noBtn)
            return;
    }

    popup_msg *objPopup = new popup_msg(this);
    if(LangMode == 1)
        objPopup->ui->lblPopup->setText("Please Wait...\nUpdating New Application...");
    else if(LangMode == 2)//एप्लिकेशन
        objPopup->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..\nनया ऐप अपडेट किया जा रहा है"));
    objPopup->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    ret = gl11_host_pwron();
    sleep(3);

    if(usbObj.USBMount() == 0){
        QFile file("/mnt/usb/commonpds.zip");
        if(!file.exists()){
            usbObj.USBUnMount();

            gl11_host_pwroff();
            gl11_lcdbklt_on();
            gl11_lcdbklt_time_duration(90);

            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"File Not Present in USB...\nPress OK to Exit...");
            else  if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"फ़ाइल यूएसबी में मौजूद नहीं है\nबाहर निकलने के लिए ओके दबाएं.");
            objPopup->close();
            objPopup->deleteLater();
            return;
        }
        RemovingUpdateFiles();
        system("cp /mnt/usb/commonpds.zip /mnt/jffs2/");
        ret = Cal_md5Checksum_Install();
        if(ret != 0){
            objPopup->close();
            objPopup->deleteLater();

            usbObj.USBUnMount();
            gl11_host_pwroff();
            gl11_lcdbklt_on();
            gl11_lcdbklt_time_duration(90);

            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Updating New Application Failed...\nPress OK to Exit...");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"नया एप्लिकेशन अपडेट करना विफल हुआ.\nबाहर निकलने के लिए ओके दबाएं.");
            return;
        }
    }else{
        usbObj.USBUnMount();

        gl11_host_pwroff();
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"USB Not Mounted...\nPress OK to Exit...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"USB उपलब्ध नहीं है\n बाहर निकलने के लिए OK दबाएं.");
        objPopup->close();
        objPopup->deleteLater();
        return;
    }
    usbObj.USBUnMount();
    gl11_host_pwroff();
    objPopup->close();
    objPopup->deleteLater();
    gl11_lcdbklt_on();
    gl11_lcdbklt_time_duration(90);

    if(LangMode == 1)
        miscObj.DisplayWideMsgBox((char *)"New Application Copied...\nPress OK to Exit...");
    else if(LangMode == 2)
        miscObj.tel_DisplayWideMsgBox((char *)"नया ऐप कॉपी कर दिया गया है. \nबाहर निकलने के लिए ओके दबाएं.");

    sleep(2);
    system("reboot");
}

void Widget::RemovingUpdateFiles()
{
    system("rm /mnt/jffs2/commonpds.zip");
    system("rm /mnt/jffs2/md5checksum.txt");
    system("rm /mnt/jffs2/commonpds");
    system("rm /mnt/jffs2/script.sh");
    return;
}

void Widget::on_BtnGPRSUpdateApp_clicked()
{
    if(PatchUpdateOfflineCheck()<0)
        return;
    qDebug() << "************************************";
    qDebug() << "Updating New Application Through GPRS";
    qDebug() << "************************************";

    GPRS objGprs;
    if(objGprs.Check_Network_Available() < 0)
        return;

    QMessageBox msg;
    msg.setStyleSheet("background-color: rgb(35, 173, 111)");
    msg.setWindowFlags(Qt::FramelessWindowHint);
    if(LangMode == 1){
        msg.setText("Do You Want to Update New Application?");
        QPushButton *yesBtn = msg.addButton("Yes",QMessageBox::ActionRole);
        QPushButton *noBtn = msg.addButton("No",QMessageBox::ActionRole);
        msg.exec();
        if(msg.clickedButton() == noBtn)
            return;
    }else if(LangMode == 2){
        msg.setText(trUtf8("क्या आपको नया ऐप अपडेट करना है?"));
        QPushButton *yesBtn = msg.addButton(trUtf8("हाँ!"),QMessageBox::ActionRole);
        QPushButton *noBtn = msg.addButton(trUtf8("नहीं"),QMessageBox::ActionRole);
        msg.exec();
        if(msg.clickedButton() == noBtn)
            return;
    }

    popup_msg *objPopup = new popup_msg(this);
    if(LangMode == 1)
        objPopup->ui->lblPopup->setText("Please Wait...\nUpdating New Application...");
    else if(LangMode == 2)
        objPopup->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..\nनया ऐप अपडेट किया जा रहा है."));
    objPopup->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    RemovingUpdateFiles();

    int ret = 0;
    char curlbuff[512];
    memset(curlbuff, 0, sizeof(curlbuff));

    for(int i=0;i<12;i++){
        memset(curlbuff, 0, sizeof(curlbuff));

        qDebug() << "Statecode before update app::" << stateCode;

        if(stateCode.isEmpty())
            sprintf(curlbuff,"wget -c -T 24 ftp://mktg:mktg@115.111.229.10/NEPDS/HYBRID_JDL/commonpds.zip -O /mnt/jffs2/commonpds.zip");
        else
            sprintf(curlbuff,"wget -c -T 24 ftp://mktg:mktg@115.111.229.10/NEPDS/%s/commonpds.zip -O /mnt/jffs2/commonpds.zip",stateCode.trimmed().toAscii().data());

//        sprintf(curlbuff,"wget -c -T 24 ftp://mktg:mktg@115.111.229.10/NEPDS/HYBRID/commonpds.zip -O /mnt/jffs2/commonpds.zip");

        qDebug() << "FTP URL : " << curlbuff<<"retrying count"<<i;
        ret = system(curlbuff);
        qDebug() << "ret = " << ret;
        if(ret==0)
            break;
    }
    qDebug() << "ret = " << ret;
    if(ret == 4864){
        objPopup->close();
        objPopup->deleteLater();

        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);

        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"File Not Found On Server...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"फ़ाइल सर्वर पर उपलब्ध नहीं है");
        return;
    }
    if(ret != 0){
        objPopup->close();
        objPopup->deleteLater();

        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Download Unsuccessful...\nPlease Try Again...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"डाउनलोड असफल\n कृपया पुनः प्रयास करें.");
        return;
    }
    ret = Cal_md5Checksum_Install();
    if(ret == 0){
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);

        objPopup->close();
        objPopup->deleteLater();

        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Updated With New Application...\nPress OK to Exit...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"नया ऐप कॉपी कर दिया गया है \nबाहर निकलने के लिए ओके दबाएं.");
        sleep(2);
        system("reboot");
    }else{
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);

        objPopup->close();
        objPopup->deleteLater();
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Updating New Application Failed...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"नया ऐप अद्यतन विफल हुआ");
    }
    objPopup->close();
    objPopup->deleteLater();
    return;
}

int Widget::getEJ_Count()
{
    QSqlQuery query;
    int iCount = 0;

    query.prepare("SELECT COUNT(PriAccNum) FROM CustomerTxn WHERE EJFlag = ?");
    query.addBindValue("1");
    if(!query.exec())
        qDebug() << query.lastError();
    else{
        while(query.next()){
            iCount = query.value(0).toInt(0);
        }
    }
    query.clear();
    qDebug()<<"EJ Count = "<<iCount;
    return iCount;
}
int Widget::Cal_md5Checksum_Install()
{
    int ret = 0;
    system("chdir /mnt/jffs2/");
    system("unzip -o /mnt/jffs2/commonpds.zip -d /mnt/jffs2/");
    int md5flag = system("md5sum -c /mnt/jffs2/md5checksum.txt");
    qDebug() << "MD5 Flag = " << md5flag;
    if(md5flag != 0){
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);
        miscObj.DisplayWideMsgBox((char *)"File Not Downloaded Properly...\nPlease Try Again...");
        return -1;
    }
    system("dos2unix /mnt/jffs2/script.sh");
    system("chmod +x /mnt/jffs2/script.sh");
    ret = system("sh /mnt/jffs2/script.sh");
    qDebug() << "ret 2 = " << ret;
    if(ret != 0){
        qDebug() << "script.sh file Not Found";
        return -1;
    }
    return 0;
}


void Widget::on_BtnDeviceSettings_clicked()
{
    char cMachineID[16];

    memset(cMachineID, 0, sizeof(cMachineID));
    miscObj.GetMachineId(cMachineID);
    ui->lineEditMachineID->setText(cMachineID);

    QString str;
    QFile file("/etc/rd_info.txt");
    if(file.open(QIODevice::ReadOnly)){
        str = file.readAll();
        qDebug() << "string content:" << str;
        ui->lbl_appver->setText(str);
    }else
        ui->lbl_appver->setText("");
    file.close();
    ui->stackedWidget->setCurrentIndex(9);
    return;
}

void Widget::on_BtnBack_5_clicked()
{
    ui->ldtShpId->setFocus();
    ui->stackedWidget->setCurrentIndex(0);
    return;
}

void Widget::on_btnSettings_2_clicked()
{
    if(UpdateDBTxnURL() < 0)
        return;
    ui->stackedWidget->setCurrentIndex(18);
    return;
}

void Widget::on_btnBack_clicked()
{
    nLogout=0;
    if(DealerTxnFlag == 1)
    {
        ui->ConsentFormcheckBox->setChecked(true);
        ui->ldtRationCard->clear();
        ui->stackedWidget->setCurrentIndex(4);
    }
    else
        ui->stackedWidget->setCurrentIndex(0);
    return;
}

int Widget::app_ver()
{
    qDebug() << "Version No - " << NEW_VERSION;
    return 0;
}

void Widget::GetAppVersion(char *szVersion)
{
    QSqlQuery query;
    query.prepare("SELECT appver FROM VersionMaintain");
    if(!query.exec())
        qDebug() << query.lastError();
    else{
        while(query.next()){
            strcpy(szVersion, query.value(0).toString().toAscii().data());
        }
    }
    query.clear();
}

void Widget::PlayRupeesPaisa()
{
    QString str(ui->lblTotalAmtVal->text());
    qDebug() << "ui->lblTotalAmtVal_2->text() = " << str;

    QStringList list = str.split('.');

    int iRupees = list.at(0).toInt(0);
    int iPaise = list.at(1).toInt(0);

    qDebug() << "Rupees = " << iRupees;
    qDebug() << "Paise = " << iPaise;

    char buff[16];
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "%05d", iRupees);
    if(iRupees != 0)
        miscObj.play_amt(buff, 1);

    if(iPaise != 0 ){
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "%05d", iPaise);
        miscObj.play_amt(buff, 2);
    }
    int LangIdentifier = miscObj.GetAudioFlag();

    char framebuff[256];
    memset(framebuff, 0, sizeof(framebuff));
    sprintf(framebuff,"mplayer -ao alsa:device=hw=0.0 /mnt/jffs2/mp3/%d02000.mp3", LangIdentifier);
    system(framebuff);
    return;
}

void Widget::InitialiseBluetooth()
{
    char bt_names[40][256]={0};
    char *ptr;

    popup_msg *dlgPopup = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup->ui->lblPopup->setText("Please Wait...\nBluetooth Initialising...");
    else
        dlgPopup->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें ... \n ब्लूटूथ प्रारंभ कर रहा है ..."));
    dlgPopup->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    bt_pwroff();

    int ret = 0;
    ret = bt_pwr_on();
    if(ret < 0){
        qDebug() << "Bluetooth Power On Failed...";
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Bluetooth Initialising Failed...");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"ब्लूटूथ प्रारंभ करना विफल ...");
        dlgPopup->close();
        dlgPopup->deleteLater();
        return;
    }

    dlgPopup->close();
    dlgPopup->deleteLater();

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode ==1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...\nScanning For Devices...");
    else if (LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें\nडिवाइसेज की खोज चल रही है"));
    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    int FLAG = 0;
    ret = scanning(bt_names);
    if(ret == 0){
        qDebug() << "Bluetooth Scanning Failed";
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Bluetooth Scanning Failed...");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"ब्लूटूथ स्कैनिंग विफल ...");
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }
    int i;
    for(i=0;i < ret ;i++){
        ptr = strstr(bt_names[i],"APPDS_VNTK@2015");
        if(ptr != NULL){
            FLAG=1;
            break;
        }
    }
    if(FLAG == 1){
        printf("device == %s\n",bt_names[i]);
        memset(buff_btId, 0, sizeof(buff_btId));
        strncpy(buff_btId,bt_names[i]+16,17);
        printf("btaddr == ------------------%s----------------------\n",buff_btId);
    }else if(FLAG == 0){
        qDebug() << "Weighing Scale not found";
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Weighing Scale Not Found");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"वजन पैमाना नहीं मिला");
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    popup_msg *dlgPopup2 = new popup_msg(this);
    if(LangMode ==1)
        dlgPopup2->ui->lblPopup->setText("Please Wait...\nPairing with Weighing Machine...");
    else if(LangMode == 2)
        dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें ... वजन मशीन के साथ \n जोड़ा जा रहा है"));
    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    ret = bt_paring(buff_btId, "94321");
    if(ret < 0){
        qDebug() << "Bluetooth Pairing Failed";
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Pairing With Weighing Machine Failed...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"वजन मशीन के साथ जुड़ना विफल");
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        return;
    }

    ret = bt_connect(buff_btId);
    if(ret !=0){
        printf("node creation and connection failure \n");
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Weighing Machine Not Connected...");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"वजन मशीन कनेक्ट नहीं है ...");
        system("rfcomm release 0");
        sleep(1);
    }
    dlgPopup2->close();
    dlgPopup2->deleteLater();
    return;
}

void Widget::on_ldtRationCard_cursorPositionChanged(int arg1, int arg2)
{
    ui->ldtRationCard->setText(ui->ldtRationCard->text().toAscii().toUpper());
    return;
}

void Widget::popupNothing()
{
    popup_nothing *dlgPopup2 = new popup_nothing(this);
    dlgPopup2->ui2->lblDummy->setText("");
    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    dlgPopup2->close();
    dlgPopup2->deleteLater();
    sleep(1);
    return;
}

void Widget::ConsentForm()
{
    char szConsentForm[1024];
    memset(szConsentForm, 0, sizeof(szConsentForm));

    ui->plainTextEdit->clear();
    if(LangMode == 1){
        sprintf(szConsentForm, "I hereby state that I have no objection in authenticating myself with Aadhaar based authentication system and consent to providing my Aadhaar number, Biometrics and/or One Time Pin (OTP) data for Aadhaar based authentication for the purposes of availing of the \"%s\" from \"%s\".\nI understand that the Biometrics and/or OTP I provide for authentication shall be used only for authenticating my identity through the Aadhaar Authentication system for that specific transaction and for no other purposes.\nI understand that \"%s\" shall ensure security and confidentiality of my personal identity data provided for the purpose of Aadhaar based authentication.", "Ration/PDS Service", consentHeader.toStdString().c_str(),consentHeader.toStdString().c_str());
        ui->plainTextEdit->insertPlainText(szConsentForm);
    }else if(LangMode == 2){
        ui->plainTextEdit->insertPlainText(trUtf8("मैं इस बात की पुष्टि करता हूँ  कि , स्वयं को आधार से  आधारित प्रमाणीकरण प्रणाली  में, प्रमाणित करने में मुझे कोई आपत्ति नहीं है तथा खाद्य और नागरिक आपूर्ति विभाग, "));
        ui->plainTextEdit->insertPlainText(consentHeader);
        ui->plainTextEdit->insertPlainText(trUtf8(" से मिलने वाले लाभ उठाने हेतु, मैं आधार से आधारित प्रमाणीकरण के लिए  मेरे आधार नंबर , बायोमेट्रिक और/या  ओ.टी.पी. जानकारी देने के लिए सहमत हूँ । मैं समझता हूँ कि, मेरे द्वारा प्रमाणीकरण के लिए आधार आधारित प्रणाली को दी गयी मेरी बायोमेट्रिक और/या  ओ.टी.पी. जानकारी, केवल विशिष्ट लेनदेन में, मेरी पहचान को प्रमाणित करने के लिए इस्तेमाल की जाएगी एवं अन्य कोई प्रयोजन में  उपयोग नहीं  की जाएगी।  मैं समझता हूँ कि मेरे द्वारा आधार आधरित प्रमाणीकरण के उद्देश्य के लिए  दी गयी  व्यक्तिगत जानकारी गोपनीय एवं सुरक्षित रहेगी।"));

//        ui->plainTextEdit->insertPlainText(trUtf8("मैं इस बात की पुष्टि करता हूँ  कि , स्वयं को आधार से  आधारित प्रमाणीकरण प्रणाली  में, प्रमाणित करने में मुझे कोई आपत्ति नहीं है तथा खाद्य और नागरिक आपूर्ति विभाग,नागालैंड से मिलने वाले लाभ उठाने हेतु, मैं आधार से आधारित प्रमाणीकरण के लिए  मेरे आधार नंबर , बायोमेट्रिक और/या  ओ.टी.पी. जानकारी देने के लिए सहमत हूँ । मैं समझता हूँ कि, मेरे द्वारा प्रमाणीकरण के लिए आधार आधारित प्रणाली को दी गयी मेरी बायोमेट्रिक और/या  ओ.टी.पी. जानकारी, केवल विशिष्ट लेनदेन में, मेरी पहचान को प्रमाणित करने के लिए इस्तेमाल की जाएगी एवं अन्य कोई प्रयोजन में  उपयोग नहीं  की जाएगी।  मैं समझता हूँ कि मेरे द्वारा आधार आधरित प्रमाणीकरण के उद्देश्य के लिए  दी गयी  व्यक्तिगत जानकारी गोपनीय एवं सुरक्षित रहेगी।"));
    }

    QTextCursor textCursor=ui->plainTextEdit->textCursor();
    textCursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor,1);
    ui->plainTextEdit->setTextCursor(textCursor);

    return;
}

/*void Widget::on_BtnOK_2_clicked()
{
    if(iConsentFormBackFlag == 20)
    {
        ui->stackedWidget->setCurrentIndex(20);
    }
    else if(iConsentFormBackFlag == 5)
    {
        //        ui->ConsentFormcheckBox->setChecked(true);
        ui->stackedWidget->setCurrentIndex(5);
    }
    //    else if(iConsentFormBackFlag == 3)
    //        ui->stackedWidget->setCurrentIndex(3);
    //    else if(iConsentFormBackFlag == 10)
    //        ui->stackedWidget->setCurrentIndex(10);
    else if(iConsentFormBackFlag == 23)
        ui->stackedWidget->setCurrentIndex(23);
    else if(iConsentFormBackFlag == 42)
        ui->stackedWidget->setCurrentIndex(42);
    else if(iConsentFormBackFlag == 43)
        ui->stackedWidget->setCurrentIndex(43);
    else if(iConsentFormBackFlag == 45)
        ui->stackedWidget->setCurrentIndex(45);
    else if(iConsentFormBackFlag == 50)
        ui->stackedWidget->setCurrentIndex(50);
    return;
}*/

void Widget::on_BtnOK_2_clicked()
{
    if(!ui->ConsentCheckBox->isChecked())
    {
        if(LangMode==1)
            miscObj.DisplayWideMsgBox((char *)"Please Select a Consent Message!");
        else if(LangMode==2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया एक सहमति संदेश चुनें!");

        QString ConsentReasonStr = "0-N";
        if(SendTxnCancelReason(ConsentReasonStr,'C') < 0)
            return;

        return;
    }

    if(iConsentFormBackFlag == 1) // login
    {
        ui->stackedWidget->setCurrentIndex(20);
        on_btnLogin_clicked();
    }
    else if(iConsentFormBackFlag == 2) // issue
    {
        qDebug()<<"IN ISUUES MODULE";
        memset(asciiBase64Enc,0x00,sizeof(asciiBase64Enc));
        ui->stackedWidget->setCurrentIndex(5);

        qDebug()<<"After Refreshing the asciiBase64Enc";

        DealerTxnFlag = 0; //initialising dealer txn flag
        BenAuthSUCCESS = 0;

        ui->lblRC_NO_Summary->setText(ui->lblRCNO_Val->text().toAscii().data());
        if(TxnAllotedBetweenTime()<0)
            return;
        Qtxnflow.clear();

        if(LangMode == 1)
            pop_up("Please Wait....");
        else if(LangMode == 2)
            pop_up("कृपया प्रतीक्षा करें....");
        else if(LangMode == 3)
            pop_up("অনুগ্রহপূর্বক অপেক্ষা করুন..");

        ui->ldtEKYC_UID_Entered->clear();

        miscObj.Check_mplayer_Exists();
        int row = 0;
        row = ui->twdgtEntitlement->currentRow();

        if(iMainTransaction == EKYC_TRANSACTION){

            qDebug()<<"***********Current Row No = "<<row;
            qDebug()<<"Member Name = "<<ui->twdgtEntitlement->item(row, 0)->text().toAscii().data();
            qDebug()<<"Member UID =  "<<ui->twdgtEntitlement->item(row, 1)->text().toAscii().data();

            if(ui->twdgtEntitlement->item(row, 2)->text().toUpper()=="N"){
                if(LangMode==1)
                    miscObj.DisplayWideMsgBox((char *)"UID Already Seeded");
                else if(LangMode==2)
                    miscObj.tel_DisplayWideMsgBox((char *)"आधार कार्ड से पहले ही जोड़ा गया है.");//subrata
                else if(LangMode==3)
                    miscObj.tel_DisplayWideMsgBox((char *)"প্রথম থেকেই আদার কার্ড এ সংযুক্ত আছে।.");//subrata
                return;
            }
            else if(ui->twdgtEntitlement->item(row, 2)->text().toUpper()=="Y")
            {
                PerformEKYC();
                return;
            }
            else
            {
                miscObj.DisplayWideMsgBox((char *)"Invalid Seeded Type");
                return;
            }
        }

        ui->pushButton_2->setDisabled(1);

        qDebug() << "Dealer Transaction flag:" << ui->twdgtEntitlement->item(row, 7)->text();
        qDebug() << " UID at Dealer Transaction flag:" << ui->twdgtEntitlement->item(row, 1)->text();

        if((ui->twdgtEntitlement->item(row, 7)->text().toUpper()=="D") && (ui->twdgtEntitlement->item(row, 1)->text().toUpper()=="NA") )// Benificiary Dealer Txn
        {
            miscObj.DisplayWideMsgBox((char *)"Transaction linked to dealer");
            DealerTxnFlag = 1; // Benificiary Dealer Txn
            DisplayDealerDetails();
            //        ui->ConsentFormcheckBox_3->setChecked(true);
            ui->stackedWidget->setCurrentIndex(20);
        }else
            BenficiaryAuth();

        ui->pushButton_2->setEnabled(1);

        Finger_Iris_Selection=0;
        return;
    }
    else if(iConsentFormBackFlag == 3)
    {
        qDebug()<<"IN ISUUES SEEDING MODULE";
        ui->stackedWidget->setCurrentIndex(42);

        if(miscObj.validate_uid(ui->ldtEKYC_UID_Entered->text().toAscii().data()) < 0){
            qDebug()<<"Invalid UID for EKYC";
            if(LangMode==1)
                miscObj.DisplayMsgBox((char *)"Invalid UID");
            if(LangMode==2)
                miscObj.tel_DisplayMsgBox((char *)"अमान्य यूआईडी");
            if(LangMode==3)
                miscObj.tel_DisplayMsgBox((char *)"অমান্য UID");
            return;
        }

        ui->ChkBox_Y_POI_POA->setChecked(true);
        ui->ChkBox_N_POI_POA->setChecked(false);

        ui->ChkBox_Y_Mob_eMail->setChecked(true);
        ui->ChkBox_N_Mob_eMail->setChecked(false);

        rsaEncryption(ui->ldtEKYC_UID_Entered->text().toAscii().data(),skey);

        EKYC_TEST_FLAG = 1;

        EKYC_FP_Submit();

        EKYC_TEST_FLAG = 0;
        return;
    }
    else if(iConsentFormBackFlag == 4)
    {
        ui->stackedWidget->setCurrentIndex(45);

        iDealMemSelected = INSPECTOR_SELECTED; //Added by vivek 01072017

        if(ui->lEdit_InpsAuth->text().isEmpty())
        {
            if(LangMode==1)
                miscObj.DisplayMsgBox((char *) "Please Enter UID");
            else if(LangMode==2)
                miscObj.tel_DisplayMsgBox((char *)"कृपया 12 अंक यूआईडी नंबर दर्ज करें.");
            else if(LangMode==3)
                miscObj.tel_DisplayMsgBox((char *)"দয়া করে 12 অংকের UID নাম্বার দিন।");

            ui->lEdit_InpsAuth->setFocus();
            return;
        }

        if(miscObj.validate_uid(ui->lEdit_InpsAuth->text().toAscii().data()) < 0)
        {
            if(LangMode==1)
                miscObj.DisplayWideMsgBox((char *)"INVALID UID");
            else if(LangMode==2)
                miscObj.tel_DisplayMsgBox((char *)"अमान्य यूआईडी");
            else if(LangMode==3)
                miscObj.tel_DisplayMsgBox((char *)"অমান্য UID");


            ui->lEdit_InpsAuth->clear();
            ui->lEdit_InpsAuth->setFocus();
            return;
        }

        rsaEncryption(ui->lEdit_InpsAuth->text().toAscii().data(),skey);

        if(AuthInspector() < 0)
        {
            ui->lEdit_InpsAuth->clear();
            ui->lEdit_InpsAuth->setFocus();
            ui->stackedWidget->setCurrentIndex(45);
            return;
        }
        ///Send Observation Details to the Server
        if(Submit_Inspector_Verfication() < 0)
        {
            ui->lEdit_InpsAuth->clear();
            ui->lEdit_InpsAuth->setFocus();
            return ;
        }
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

                prn_close();

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

                prn_close();

                ui->lEdit_InpsAuth->clear();
                dlgPopup2->close();
                dlgPopup2->deleteLater();
                return;
            }

            getHeadings();

            if (LangMode == 1)
                PrintTxnReceiptInspection();
            else if(LangMode == 2)
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
        return;
    }
    else if(iConsentFormBackFlag == 5)
    {
        ui->stackedWidget->setCurrentIndex(23);
        Finger_Iris_Selection = 1;  //Finger Scanner

        int recicegoods_rows=ui->tableWidgetRecieveGoodsAuth->rowCount();
        qDebug()<<"recicegoods_rows"<<recicegoods_rows;

        if(recicegoods_rows==0)
            return;

        if(RecieveGoodsAuth() == 0)
        {
            /* Check Printing Condition*/

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
                return;
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
                return;
            }

            prn_close();

            if(iDealMemSelected == DEALER_SELECTED)
            {
                if(getRecvGoodsRuteOfficerStatus() != true)
                {
                    qDebug()<<"IF RO only true";
                    if(SendReceiveStockTXNToServer() == 0) //New Code Added by vivek
                    {
                        Print_ReceiveStock(1);
                    }
                    //Clear the Widgets after success txn
                    ui->comboTruckChitNo->clear();
                    ui->label_TruckNo->clear();
                    ui->labelRO->clear();
                    ui->tableWidgetStockTable_3->clearContents();

                    ui->stackedWidget->setCurrentIndex(1);
                    return;
                }
                else
                {
                    // IF Dealer and RO Both are True
                    ui->BtnDealerRecGoods->setDisabled(true);
                    ui->BtnDealerRecGoods->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(255, 255, 255);"); // white
                    ui->BtnRouteOfficer->setEnabled(true);
                    ui->BtnRouteOfficer->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(255, 85, 0);"); // orange
                    ui->tableWidgetRecieveGoodsAuth->clearContents();

                    for(int i = ui->tableWidgetRecieveGoodsAuth->rowCount(); i > 0; i--)
                    {
                        ui->tableWidgetRecieveGoodsAuth->removeRow(i-1);
                    }
                }
            }
            if(iDealMemSelected ==  ROUTE_OFFICER_SELECTED)
            {
                qDebug()<<"IF RO and Dealer or Dealer only True";
                if(SendReceiveStockTXNToServer() == 0) //New Code Added by vivek
                {
                    Print_ReceiveStock(2);
                }
                ui->comboTruckChitNo->clear();
                ui->label_TruckNo->clear();
                ui->labelRO->clear();
                ui->tableWidgetStockTable_3->clearContents();
                ui->stackedWidget->setCurrentIndex(1);
                return;
            }
        }

        return;
    }
    else if(iConsentFormBackFlag == 6)
    {
        iDealMemSelected = BEN_VERIFICATION;
        char UIDStatus[160]="";
        memset(UIDStatus,0,sizeof(UIDStatus));

        int currentrow = ui->tableWidgetBenVerify->currentRow();

        strcpy(UIDStatus,ui->tableWidgetBenVerify->item(currentrow,2)->text().toAscii().data());

        qDebug()<<"UIDStatus"<<UIDStatus<<endl;

        //if(strcmp(UIDStatus,"Verified")==0)
        if(ui->tableWidgetBenVerify->item(currentrow,4)->text().toUpper() == "Y")
        {
            ui->ldtBenVerifyCardNo->clear();

            if(LangMode == 1)
                miscObj.DisplayWideMsgBox("UID is Already Verified");
            else if(LangMode == 2)
                miscObj.tel_DisplayMsgBox("UID पहले से सत्यापित है");

            sleep(2);
            return;
        }

        system("rm -rf /mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml");

        if(EKYC_Calling(2) < 0)
        {
            //        ui->ldtBenVerifyCardNo->clear();
            //        ui->stackedWidget->setCurrentIndex(49);
            return; //doubt
        }
        else
        {
            Print_Beneficiarydetails("/mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml");
            ui->ldtBenVerifyCardNo->clear();
            ui->stackedWidget->setCurrentIndex(1);
        }
    }

//    else if(iConsentFormBackFlag == 6)
//    {
//        ui->stackedWidget->setCurrentIndex(50);

//        iDealMemSelected = BEN_VERIFICATION;
//        char UIDStatus[160]="";
//        memset(UIDStatus,0,sizeof(UIDStatus));

//        int currentrow = ui->tableWidgetBenVerify->currentRow();

//        strcpy(UIDStatus,ui->tableWidgetBenVerify->item(currentrow,2)->text().toAscii().data());

//        qDebug()<<"UIDStatus"<<UIDStatus<<endl;

//        if(ui->tableWidgetBenVerify->item(currentrow,2)->text().toUpper() == "N")
//        {
//            ui->ldtBenVerifyCardNo->clear();

//            if(LangMode == 1)
//                miscObj.DisplayWideMsgBox("UID is Already Verified");
//            else if(LangMode == 2)
//                miscObj.tel_DisplayMsgBox("UID पहले से सत्यापित है");
//            else
//                miscObj.tel_DisplayMsgBox("UID पहले से सत्यापित है");
//            sleep(2);
//            return;
//        }
//        else if(ui->tableWidgetBenVerify->item(currentrow,2)->text().toUpper() == "Y")
//        {

//            system("rm -rf /mnt/jffs2/MEMBER_EKYCAuthenticateResp.json");

//            if(EKYC_Calling(2) < 0)
//            {
//                //        ui->ldtBenVerifyCardNo->clear();
//                //        ui->stackedWidget->setCurrentIndex(49);

//                return; //doubt
//            }
//            else
//            {
//                dob_adds_comm_flag=0;
//                int ret_benf=0;
//                Print_Benefryflag=0;
//                ret_benf=Print_Beneficiarydetails("/mnt/jffs2/MEMBER_EKYCAuthenticateResp.json");
//                if(ret_benf==1)
//                {
//                    dob_adds_comm_flag=2;
//                    Print_Benefryflag=1;

//                    ui->stackedWidget->setCurrentIndex(57);
//                    ui->ldtBenVerifyCardNo->clear();
//                    return;
//                }
//                if(ret_benf==3)
//                {
//                    dob_adds_comm_flag=2;
//                    Print_Benefryflag=1;
//                    ui->stackedWidget->setCurrentIndex(58);
//                    ui->ldtBenVerifyCardNo->clear();
//                    return;

//                }
//                if(ret_benf==2)
//                {
//                    dob_adds_comm_flag=2;
//                    Print_Benefryflag=1;
//                    ui->stackedWidget->setCurrentIndex(59);
//                    ui->ldtBenVerifyCardNo->clear();
//                    return;

//                }

//                ui->ldtBenVerifyCardNo->clear();
//                ui->stackedWidget->setCurrentIndex(1);
//            }
//        }
//        else
//        {
//            miscObj.DisplayWideMsgBox("Invalid type from serer");
//            return;

//        }
//        return;


//    }
    else if(iConsentFormBackFlag == 7)
    {
        ui->stackedWidget->setCurrentIndex(5);


        qDebug()<<"Iris pressed"<<endl;
        BenAuthSUCCESS = 0;
        int iScannerStatus=device_search();
        qDebug()<<"iScannerStatus"<<iScannerStatus;

        if(iScannerStatus == -1)
        {
            if(LangMode==1)
                pop_up((char *)"IRIS Scanner Not connected");
            else if(LangMode==2)
                pop_up((char *)"आईरिस स्कैनर जुड़ा नहीं है");
            else if(LangMode==3)
                pop_up((char *)"आईरिस स्कैनर जुड़ा नहीं है");

            sleep(3);
            return;
        }

        int row = 0;
        char uid[36]="";
        memset(uid,0,sizeof(uid));

        row = ui->twdgtEntitlement->currentRow();
        strcpy(uid,ui->twdgtEntitlement->item(row, 1)->text().toAscii().data());

        ui->BtnBenIRIS->setDisabled(1);
        Finger_Iris_Selection = 1;

        on_pushButton_2_clicked();

        Finger_Iris_Selection = 0;
        ui->BtnBenIRIS->setEnabled(1);

        return;
    }

}


void Widget::on_BtnConsentForm_clicked()
{
    iConsentFormBackFlag = 20;
    ConsentForm();
    ui->stackedWidget->setCurrentIndex(21);
    return;
}

void Widget::on_BtnConsentForm_2_clicked()
{
    iConsentFormBackFlag = 5;
    ConsentForm();
    ui->stackedWidget->setCurrentIndex(21);
    return;
}

void Widget::on_BtnConsentForm_6_clicked()
{
    iConsentFormBackFlag = 43;
    ConsentForm();
    ui->stackedWidget->setCurrentIndex(21);
    return;
}

void Widget::on_BtnConsentForm_5_clicked()
{
    iConsentFormBackFlag = 42;
    ConsentForm();
    ui->stackedWidget->setCurrentIndex(21);
    return;
}

void Widget::on_BtnConsentForm_4_clicked()
{
    iConsentFormBackFlag = 23;
    ConsentForm();
    ui->stackedWidget->setCurrentIndex(21);
    return;
}

//void Widget::on_BtnConsentForm_3_clicked()
//{
//    iConsentFormBackFlag = 10;
//    ConsentForm();
//    ui->stackedWidget->setCurrentIndex(21);
//    return;
//}

void Widget::on_BtnConsentForm_7_clicked()
{
    iConsentFormBackFlag = 50;
    ConsentForm();
    ui->stackedWidget->setCurrentIndex(21);
    return;
}

void Widget::on_BtnConsentForm_8_clicked()
{
    iConsentFormBackFlag = 45;
    ConsentForm();
    ui->stackedWidget->setCurrentIndex(21);
    return;
}



void Widget::on_tblCommIssue_itemClicked(QTableWidgetItem *item)
{
    qDebug() << "item = " << item->text();
    return;
}

int Widget::Capture_Iris(int iEyeSelect, char *cUID)
{
    int iSelectIrisScanner=device_search();

    qDebug()<<"IRIS device"<<iSelectIrisScanner;
    popup_msg *dlgPopup1 = new popup_msg(this);

    if(iEyeSelect == LEFT_EYE){
        miscObj.AudioTest((char *)"172");
        if(LangMode ==1)
            dlgPopup1->ui->lblPopup->setText("Please Place IRIS Scanner\nnear \"Left Eye\"");
        else
            dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया आईरिस स्कैनर पर बाएं नेत्र रखें."));
    }else if(iEyeSelect == RIGHT_EYE){
        miscObj.AudioTest((char *)"173");
        if(LangMode ==1)
            dlgPopup1->ui->lblPopup->setText("Please Place IRIS Scanner\nnear \"Right Eye\"");
        else
            dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया आईरिस स्कैनर पर दाहिना नेत्र रखें"));
    }
    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    GPRS objGprs;
    if(objGprs.Check_Network_Available() < 0){
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return -1;
    }

    if(iSelectIrisScanner == BIOMATIQUES){
        int ret = Calling_Biomatiques_Iris(iEyeSelect);
        if(ret == -1){
            dlgPopup1->close();
            dlgPopup1->deleteLater();
            return -1;
        }
        if(ret == -2){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox("IRIS RD IS NOT READY");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"आईरिस आरडी तैयार नहीं है");
            dlgPopup1->close();
            dlgPopup1->deleteLater();
            return -2;
        }
        if(ret == -3){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox("Start the Biomatiques RD Service");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"बॉयोमीट्रिक आरडी सेवा शुरू करें.");
            dlgPopup1->close();
            dlgPopup1->deleteLater();
            return -2;
        }
        dlgPopup1->close();
        dlgPopup1->deleteLater();
    }else if(iSelectIrisScanner == IRITECH){
        int ret=Calling_Iritech_Iris(iEyeSelect);

        qDebug()<<"ret IRitech"<<ret<<endl;
        if(ret == -1){
            dlgPopup1->close();
            dlgPopup1->deleteLater();
            return -1;
        }
        if(ret == -2){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox("IRIS RD IS NOT READY");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"आईरिएस आरडी शुरू नहीं हुई ");
            dlgPopup1->close();
            dlgPopup1->deleteLater();
            return -2;
        }
        if(ret == -3){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox("Start the IRITECH RD Service");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"IRITECH आरडी सेवा शुरू करें");
            dlgPopup1->close();
            dlgPopup1->deleteLater();
            return -2;
        }
        dlgPopup1->close();
        dlgPopup1->deleteLater();
    }

    miscObj.AudioTest((char *)"182");
    popup_msg *dlgPopup2 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup2->ui->lblPopup->setText("Please Wait...\nAuthenticating Iris...");
    else if(LangMode == 2)
        dlgPopup2->ui->lblPopup->setText(trUtf8("एक आंख का प्रमाणित हो रहा है.\nकृपया प्रतीक्षा करें."));
    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    sleep(2); //Kept For Audio

    char buff[64];
    memset(buff, 0, sizeof(buff));
    //    char cSessionId[16];
    //    memset(cSessionId, 0, sizeof(cSessionId));
    database_queries qry;

    if(qry.getSessionId(SessionId) < 0){
        dlgPopup2->close();
        dlgPopup2->deleteLater();

        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }

    char cDealMembuff[32];
    memset(cDealMembuff, 0, sizeof(cDealMembuff));
    if(iDealMemSelected == DEALER_SELECTED)
        strcpy(cDealMembuff, ui->ldtShpId->text().toAscii().data());
    else if(iDealMemSelected == MEMBER_SELECTED)
        strcpy(cDealMembuff,ui->lblRCNO_Val->text().toAscii().data());
    else
        strcpy(cDealMembuff, ui->ldtShpId->text().toAscii().data());

    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));
    miscObj.GetMachineId(cmachineID);

    FILE *fp;
    fp = fopen("/mnt/jffs2/IrisReq.xml","w");
    if(fp == NULL){
        qDebug() << "File Not Found" << endl;
        return -1;
    }

    char ServiceIris[64];
    memset(ServiceIris, 0, sizeof(ServiceIris));

    if(iMainTransaction == NCR_TRANSACTION){
        strcpy(ServiceIris, "getDTRSecureIRISAuthenticate");
        fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ser=\"http://service.fetch.rationcard/\">");
        fprintf(fp, "<%s/>", "soapenv:Header");
        fprintf(fp, "<%s>", "soapenv:Body");
        fprintf(fp, "<%s>", "ser:getDTRSecureIRISAuthenticate");
        fprintf(fp, "<fpsSessionId>%s</fpsSessionId>",SessionId);
        fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
        fprintf(fp, "<Shop_no>%s</Shop_no>", ui->ldtShpId->text().toAscii().data());
        fprintf(fp, "<uidNumber>%s</uidNumber>", cUID);
        fprintf(fp, "<udc>%s</udc>", cmachineID);
        fprintf(fp, "<%s>", "auth_packet");
        fprintf(fp, "<certificateIdentifier>%s</certificateIdentifier>", bufExpiryStr);
        fprintf(fp, "<dataType>%s</dataType>", "X");
        fprintf(fp, "<encHmac>%s</encHmac>", hmac);
        fprintf(fp, "<secure_agency>%s</secure_agency>", "CivilSupplies");
        fprintf(fp, "<secure_agencyCode>%s</secure_agencyCode>", "1/100");
        fprintf(fp, "<secure_pid>%s</secure_pid>", pload);//secure_temp
        fprintf(fp, "<sessionKey>%s</sessionKey>", encryptedSessKey);
        fprintf(fp, "</%s>", "auth_packet");
        fprintf(fp, "</%s>", "ser:getDTRSecureIRISAuthenticate");
        fprintf(fp, "</%s>", "soapenv:Body");
        fprintf(fp, "</%s>", "soapenv:Envelope");
    }
    else{
        if(iSelectIrisScanner == IRITECH){
            fprintf(fp,"%s","<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
            fprintf(fp,"%s","<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://www.uidai.gov.in/authentication/uid-auth-request/2.0\" xmlns:ns2=\"http://service.fetch.rationcard/\">");
            fprintf(fp,"%s","<SOAP-ENV:Body>");
            fprintf(fp,"%s","<ns2:getAuthenticateNICAuaAuthRD2>");
            fprintf(fp,"<fpsSessionId>%s</fpsSessionId>",SessionId);
            fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
            fprintf(fp,"<Shop_no>%s</Shop_no>",ui->ldtShpId->text().toAscii().data());
            fprintf(fp, "<User_Id>%s</User_Id>", cDealMembuff);
            fprintf(fp, "<uidNumber>%s</uidNumber>", cUID);
            fprintf(fp, "<udc>%s</udc>", cmachineID);
            fprintf(fp, "<authMode>%s</authMode>","B");
            fprintf(fp, "<%s>", "auth_packet");
            fprintf(fp,"<ns1:certificateIdentifier>%s</ns1:certificateIdentifier>",iriTech_pid_Info.ci);
            fprintf(fp,"<ns1:dataType>%s</ns1:dataType>","X");
            fprintf(fp,"<ns1:dc>%s</ns1:dc>",iriTech_pid_Info.iriTech_dev_data.dc);
            fprintf(fp,"<ns1:dpId>%s</ns1:dpId>",iriTech_pid_Info.iriTech_dev_data.dpId);
            fprintf(fp,"<ns1:encHmac>%s</ns1:encHmac>",iriTech_pid_Info.Hmac);
            fprintf(fp,"<ns1:mc>%s</ns1:mc>",iriTech_pid_Info.iriTech_dev_data.mc);
            fprintf(fp,"<ns1:mid>%s</ns1:mid>",iriTech_pid_Info.iriTech_dev_data.mi);
            fprintf(fp,"<ns1:rdId>%s</ns1:rdId>",iriTech_pid_Info.iriTech_dev_data.rdsId);
            fprintf(fp,"<ns1:rdVer>%s</ns1:rdVer>",iriTech_pid_Info.iriTech_dev_data.rdsVer);
            fprintf(fp,"<ns1:secure_pid>%s</ns1:secure_pid>",iriTech_pid_Info.pid);
            fprintf(fp,"<ns1:sessionKey>%s</ns1:sessionKey>",iriTech_pid_Info.Skey);
            fprintf(fp,"%s","</auth_packet>");
            fprintf(fp,"<password>%s</password>",HD_PASSWORD);
            fprintf(fp,"<scannerId>%s</scannerId>",dev_details.scanner_id);
            fprintf(fp,"<authType>%s</authType>","IRIS");
            fprintf(fp,"<memberId>%s</memberId>",ui->twdgtEntitlement->item(ui->twdgtEntitlement->currentRow(), 6)->text().toAscii().data());
            fprintf(fp,"<Resp><errCode>%s</errCode><errInfo>%s</errInfo><nmPoints>%s</nmPoints><fCount>%s</fCount><fType>%s</fType><iCount>%s</iCount><iType>%s</iType><pCount>%s</pCount><pType>%s</pType><qScore>%s</qScore></Resp>",iriTech_pid_Info.err_code,iriTech_pid_Info.err_info,iriTech_pid_Info.nmPoints,iriTech_pid_Info.fCount,iriTech_pid_Info.fType,iriTech_pid_Info.iCount,iriTech_pid_Info.iType,iriTech_pid_Info.pCount,iriTech_pid_Info.pType,iriTech_pid_Info.qScore);;

            fprintf(fp,"<wadhStatus>%s</wadhStatus>",member_wadhstatus.toAscii().data()); ///added
            fprintf(fp,"%s","</ns2:getAuthenticateNICAuaAuthRD2>");
            fprintf(fp,"%s","</SOAP-ENV:Body>");
            fprintf(fp,"%s","</SOAP-ENV:Envelope>");
        }

        else if(iSelectIrisScanner == BIOMATIQUES){
            fprintf(fp,"%s","<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
            fprintf(fp,"%s","<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://www.uidai.gov.in/authentication/uid-auth-request/2.0\" xmlns:ns2=\"http://service.fetch.rationcard/\">");
            fprintf(fp,"%s","<SOAP-ENV:Body>");
            fprintf(fp,"%s","<ns2:getAuthenticateNICAuaAuthRD2>");
            fprintf(fp,"<fpsSessionId>%s</fpsSessionId>",SessionId);
            fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
            fprintf(fp,"<Shop_no>%s</Shop_no>",ui->ldtShpId->text().toAscii().data());
            fprintf(fp, "<User_Id>%s</User_Id>", cDealMembuff);
            fprintf(fp, "<uidNumber>%s</uidNumber>", cUID);
            fprintf(fp, "<udc>%s</udc>", cmachineID);
            fprintf(fp, "<authMode>%s</authMode>","B");
            fprintf(fp, "<%s>", "auth_packet");
            fprintf(fp,"<ns1:certificateIdentifier>%s</ns1:certificateIdentifier>",ttt->ci);
            fprintf(fp,"<ns1:dataType>%s</ns1:dataType>","X");
            fprintf(fp,"<ns1:dc>%s</ns1:dc>",ttt->dev_info.dc);
            fprintf(fp,"<ns1:dpId>%s</ns1:dpId>",ttt->dev_info.dpid);
            fprintf(fp,"<ns1:encHmac>%s</ns1:encHmac>",ttt->Hmac);
            fprintf(fp,"<ns1:mc>%s</ns1:mc>",ttt->dev_info.mc);
            fprintf(fp,"<ns1:mid>%s</ns1:mid>",ttt->dev_info.mi);
            fprintf(fp,"<ns1:rdId>%s</ns1:rdId>",ttt->dev_info.rdsid);
            fprintf(fp,"<ns1:rdVer>%s</ns1:rdVer>",ttt->dev_info.rdsver);
            fprintf(fp,"<ns1:secure_pid>%s</ns1:secure_pid>",ttt->pid);
            fprintf(fp,"<ns1:sessionKey>%s</ns1:sessionKey>",ttt->Skey);
            fprintf(fp,"%s","</auth_packet>");
            fprintf(fp,"<password>%s</password>",HD_PASSWORD);
            fprintf(fp,"<scannerId>%s</scannerId>",dev_details.scanner_id);
            fprintf(fp,"<authType>%s</authType>","IRIS");
            fprintf(fp,"<memberId>%s</memberId>",ui->twdgtEntitlement->item(ui->twdgtEntitlement->currentRow(), 6)->text().toAscii().data());
            fprintf(fp,"<Resp><errCode>%s</errCode><errInfo>%s</errInfo><nmPoints>%s</nmPoints><fCount>%s</fCount><fType>%s</fType><iCount>%s</iCount><iType>%s</iType><pCount>%s</pCount><pType>%s</pType><qScore>%s</qScore></Resp>",iriTech_pid_Info.err_code,iriTech_pid_Info.err_info,iriTech_pid_Info.nmPoints,iriTech_pid_Info.fCount,iriTech_pid_Info.fType,iriTech_pid_Info.iCount,iriTech_pid_Info.iType,iriTech_pid_Info.pCount,iriTech_pid_Info.pType,iriTech_pid_Info.qScore);;
            fprintf(fp,"<wadhStatus>%s</wadhStatus>",member_wadhstatus.toAscii().data()); ///added naresh
            fprintf(fp,"%s","</ns2:getAuthenticateNICAuaAuthRD2>");
            fprintf(fp,"%s","</SOAP-ENV:Body>");
            fprintf(fp,"%s","</SOAP-ENV:Envelope>");
        }
        else
        {
            dlgPopup2->close();
            dlgPopup2->deleteLater();
            return -1;
        }
    }

    fclose(fp);

    char cUrlName[256];
    memset(cUrlName, 0, sizeof(cUrlName));
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

    int ret_http_code = UrlPingCheck(cUrlName);

    if(ret_http_code == 1)
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/IrisReq.xml", cUrlName, "/mnt/jffs2/IrisResponse.xml", responseTimedOutTimeInSec.toInt());
        qDebug()<<"\nCurl Cmd : "<<cmdCurl;
    }
    else
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/IrisReq.xml",PDS_URL2, "/mnt/jffs2/IrisResponse.xml", responseTimedOutTimeInSec.toInt());
        qDebug()<<"\nCurl Cmd : "<<cmdCurl;
    }

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, AUTH_SERVER) < 0){
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        return -1;
    }
    dlgPopup2->close();
    dlgPopup2->deleteLater();

    qDebug() << "**********IrisResponse.xml*********";
    system("cat /mnt/jffs2/IrisResponse.xml");
    qDebug() << "***********************************************";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/IrisResponse.xml") < 0)
        return -1;

    if(Parse_Iris_Response("/mnt/jffs2/IrisResponse.xml") < 0)
        return -1;
    return 0;
}


char * Widget::ReadIrisFile(char *filename, int IrisLen)
{
    QString s;
    QFile file(filename);
    if(!file.exists()){
        qDebug() << "File Not Found";
        miscObj.DisplayWideMsgBox((char *)"Iris File Not Found...\nPlease Try Again...");
        return "-1";
    }
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "Reading Iris File Failed";
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Reading Iris Failed...\nPlease Try Again...");
        else if(LangMode==2)
            miscObj.DisplayWideMsgBox((char *)" आईरिस स्कैनिंग विफल .\nकृपया फिर से प्रयास करें ...");
        return "-1";
    }

    QTextStream s1(&file);
    s.append(s1.readAll());

    qDebug() << "Iris Base64 Data = " << s;
    qDebug() << "Iris Len = " << s.length();

    IrisLen = s.length();

    buffIris = (char *)malloc(IrisLen+10);
    if(buffIris == 0){
        printf("ERROR: Out of memory\n");
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Scanned Iris Quality is Poor...\nPlease Try Again...");
        else if(LangMode == 2)
            miscObj.DisplayWideMsgBox((char *)"स्कैन की गई आईरिस गुणवत्ता खराब है\nPlease Try Again...");
        return "-1";
    }

    qDebug() << "copied";
    memset(buffIris, 0, IrisLen+10);
    memcpy(buffIris, s.toAscii().data(), IrisLen);
    qDebug() << "after copied";

    return buffIris;
}

int Widget::Parse_Iris_Response(char *filename)
{
    database_queries qry;
    QFile rd_file(filename);
    if(!rd_file.exists()){
        qDebug() << "Dealer Auth Response File Not Found...";
        miscObj.DisplayWideMsgBox((char *)"Dealer Auth Response File Not Found...");
        return -1;
    }

    QDomDocument document;
    if(!document.setContent(&rd_file)){
        qDebug( "Failed to parse the file into a DOM tree." );
        rd_file.close();

        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Invalid Response Data Recieved...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"अवैध प्रतिक्रिया डेटा प्राप्त.");
        return -1;
    }
    rd_file.close();

    QDomNodeList list;
    list = document.elementsByTagName("return"); //env:Body
    QString QResponse = list.at(0).childNodes().at(1).toElement().text();   //added for testing by vivek
    qDebug()<<"\nFinal Dealer Auth Response : " << QResponse;

    QFile wr_file("/mnt/jffs2/IrisWriteresponse.xml");
    wr_file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&wr_file);
    out << QResponse;
    wr_file.close();

    QString Qauth_date, Qauth_reasonVal, Qauth_statusVal;
    QString Qauth_transaction_codeVal, Qauth_err_code;

    QFile *file = new QFile("/mnt/jffs2/IrisWriteresponse.xml");
    if(!file->open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "ParseBFD_Response(): Error Reading File";
        return -1;
    }

    QXmlStreamReader xml(file);

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;
        if(token == QXmlStreamReader::StartElement){
            qDebug() << xml.name();
            if(xml.name() == "error"){
                qDebug() << "Error Not Found" << endl;
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox((char *)"Invalid Response Data Recieved...");
                else if(LangMode == 2)
                    miscObj.tel_DisplayWideMsgBox((char *)"अवैध प्रतिक्रिया डेटा प्राप्त.");
                return -1;
            }
            while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "void property")){
                QString QStr = xml.attributes().value("property").toString();
                if(QStr == "auth_date"){
                    xml.readNextStartElement();
                    xml.readNext();
                    Qauth_date = xml.text().toString();
                    qDebug() << "Qauth_date = " << Qauth_date;
                }else if(QStr == "auth_err_code"){
                    xml.readNextStartElement();
                    xml.readNext();
                    Qauth_err_code = xml.text().toString();
                    qDebug() << "Qauth_err_code = " << Qauth_err_code;
                }else if(QStr == "auth_reason"){
                    xml.readNextStartElement();
                    xml.readNext();
                    Qauth_reasonVal = xml.text().toString();
                    qDebug() << "Qauth_reasonVal = " << Qauth_reasonVal;
                }else if(QStr == "auth_status"){
                    xml.readNextStartElement();
                    xml.readNext();
                    Qauth_statusVal = xml.text().toString();
                    qDebug() << "Qauth_statusVal = " << Qauth_statusVal;
                }else if(QStr == "auth_transaction_code"){
                    xml.readNextStartElement();
                    xml.readNext();
                    Qauth_transaction_codeVal = xml.text().toString();
                    if(iDealMemSelected == ROUTE_OFFICER_SELECTED){
                        memset(gRoute_Off_auth_code, 0, sizeof(gRoute_Off_auth_code));
                        strcpy(gRoute_Off_auth_code, Qauth_transaction_codeVal.toAscii().data());
                        qDebug()<<"gRoute_Off_auth_code :"<< gRoute_Off_auth_code;
                    }else{
                        memset(gauth_transaction_code, 0, sizeof(gauth_transaction_code));
                        strcpy(gauth_transaction_code, Qauth_transaction_codeVal.toAscii().data());
                    }
                    if(qry.getRecvGoodsRuteOfficerStatus() != true){
                        memset(gRoute_Off_auth_code, 0, sizeof(gRoute_Off_auth_code));
                        strcpy(gRoute_Off_auth_code,"0"); //added by vivek 05082017
                    }
                    qDebug() << "Qauth_transaction_codeVal = " << Qauth_transaction_codeVal;
                }
                break;
            }
        }
    }
    char cRespMsg[64];
    memset(cRespMsg, 0, sizeof(cRespMsg));

    char dispbuff[128];
    memset(dispbuff, 0, sizeof(dispbuff));

    DisplayBFDRespCode(Qauth_statusVal.toAscii().data(), cRespMsg);
    if((Qauth_statusVal != "100") || Qauth_statusVal.isEmpty() || Qauth_statusVal.isNull()){
        sprintf(dispbuff, "Response Message: %s(%s)", Qauth_reasonVal.toAscii().data(), Qauth_err_code.toAscii().data());

        miscObj.DisplayWideMsgBox(dispbuff);
        return -1;
    }

    if((Qauth_statusVal == "100") && Qtxnflow == "F")
    {
        if(iDealMemSelected == MEMBER_SELECTED)
            getcomTxn_type="I";

        BenAuthSUCCESS = 1;
        return 0;
    }
    return 0;
}

bool Widget::GotoConsentForm()
{
    ui->stackedWidget->setCurrentIndex(21);
}

//void Widget::on_BtnDealerScanFP_clicked()
//{
//    if(!ui->ConsentFormcheckBox_3->isChecked())
//    {
//        if(LangMode == 1)
//            miscObj.DisplayWideMsgBox((char *)"Please accept consent form");
//        else if(LangMode == 2)
//            miscObj.tel_DisplayWideMsgBox("कृपया सहमति प्रपत्र स्वीकार करें");

//        QString ConsentReasonStr = "0-N";
//        if(SendTxnCancelReason(ConsentReasonStr,'C') < 0)
//            return;
//        return;
//    }
//    ui->BtnDealerScanFP->setDisabled(1);
//    on_btnLogin_clicked();
//    ui->BtnDealerScanFP->setEnabled(1);
//    return;
//}

void Widget::on_BtnDealerScanFP_clicked()
{
    iConsentFormBackFlag = 1;
    ui->ConsentCheckBox->setChecked(true);
    ConsentForm();
    ui->stackedWidget->setCurrentIndex(21);
    return;
}

int Widget::parsegetViewStockRecievedResponse()
{
    QFile file("/mnt/jffs2/getViewStockRecievedResp.xml");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error Reading Param File";
        return -1;
    }

    QSqlQuery query;
    query.exec("DELETE FROM getViewStockRecieved");

    QXmlStreamReader xml(&file);

    QString Qcomm, Qcommoditycode;
    QString Qdispatch_id, Qdo_ro_no;
    QString Qentry_date, Qtruck_chit_no, Qtruck_no;
    QString Qalloted_year, Qalloted_month;
    QString QKRA, QOB, QQI, QqaD, QqnD, QqtD, Qreceived_quantity, Qreleased_quantity;
    QString Qstock_received;

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;
        if(token == QXmlStreamReader::StartElement){
            if(xml.name() == "alloted_month"){
                xml.readNext();
                Qalloted_month = xml.text().toString();
                qDebug() << "Qalloted_month : " << Qalloted_month;
            }else if(xml.name() == "alloted_year"){
                xml.readNext();
                Qalloted_year = xml.text().toString();
                qDebug() << "Qalloted_year : " << Qalloted_year;
            }else if(xml.name() == "comm"){
                xml.readNext();
                Qcomm = xml.text().toString();
                qDebug() << "Qcomm : " << Qcomm;
            }else if(xml.name() == "commoditycode"){
                xml.readNext();
                Qcommoditycode = xml.text().toString();
                qDebug() << "Qcommoditycode : " << Qcommoditycode;
            }else if(xml.name() == "dispatch_id"){
                xml.readNext();
                Qdispatch_id = xml.text().toString();
                qDebug() << "Qdispatch_id : " << Qdispatch_id;
            }else if(xml.name() == "do_ro_no"){
                xml.readNext();
                Qdo_ro_no = xml.text().toString();
                qDebug() << "Qdo_ro_no : " << Qdo_ro_no;
            }else if(xml.name() == "entry_date"){
                xml.readNext();
                Qentry_date = xml.text().toString();
                qDebug() << "Qentry_date : " << Qentry_date;
            }else if(xml.name() == "KRA"){
                xml.readNext();
                QKRA = xml.text().toString();
                qDebug() << "QKRA : " << QKRA;
            }else if(xml.name() == "OB"){
                xml.readNext();
                QOB = xml.text().toString();
                qDebug() << "QOB : " << QOB;
            }else if(xml.name() == "QI"){
                xml.readNext();
                QQI = xml.text().toString();
                qDebug() << "QQI : " << QQI;
            }else if(xml.name() == "qaD"){
                xml.readNext();
                QqaD = xml.text().toString();
                qDebug() << "QqaD : " << QqaD;
            }else if(xml.name() == "qnD"){
                xml.readNext();
                QqnD = xml.text().toString();
                qDebug() << "QqnD : " << QqnD;
            }else if(xml.name() == "qtD"){
                xml.readNext();
                QqtD = xml.text().toString();
                qDebug() << "QqtD : " << QqtD;
            }else if(xml.name() == "received_quantity"){
                xml.readNext();
                Qreceived_quantity = xml.text().toString();
                qDebug() << "Qreceived_quantity : " << Qreceived_quantity;
            }else if(xml.name() == "released_quantity"){
                xml.readNext();
                Qreleased_quantity = xml.text().toString();
                qDebug() << "Qreleased_quantity : " << Qreleased_quantity;
            }else if(xml.name() == "stock_received"){
                xml.readNext();
                Qstock_received = xml.text().toString();
                qDebug() << "stock_received : " << Qstock_received;
            }else if(xml.name() == "truck_chit_no"){
                xml.readNext();
                Qtruck_chit_no = xml.text().toString();
                qDebug() << "Qtruck_chit_no : " << Qtruck_chit_no;
            }else if(xml.name() == "truck_no"){
                xml.readNext();
                Qtruck_no = xml.text().toString();
                qDebug() << "Qtruck_no : " << Qtruck_no;

                query.clear();
                query.prepare("INSERT INTO getViewStockRecieved(alloted_month, alloted_year, comm, commoditycode, dispatch_id, do_ro_no, entry_date, KRA, OB, QI, qaD, qnD, qtD, received_quantity, released_quantity, stock_received, truck_chit_no, truck_no) VALUES(:alloted_month, :alloted_year, :comm, :commoditycode, :dispatch_id, :do_ro_no, :entry_date, :KRA, :OB, :QI, :qaD, :qnD, :qtD, :received_quantity, :released_quantity, :stock_received, :truck_chit_no, :truck_no)");
                query.bindValue("alloted_month", Qalloted_month);
                query.bindValue(":alloted_year", Qalloted_year);
                query.bindValue(":comm", Qcomm);
                query.bindValue(":commoditycode", Qcommoditycode);
                query.bindValue(":dispatch_id", Qdispatch_id);
                query.bindValue(":do_ro_no", Qdo_ro_no);
                query.bindValue(":entry_date", Qentry_date);
                query.bindValue(":KRA", QKRA);
                query.bindValue(":OB", QOB);
                query.bindValue(":QI", QQI);
                query.bindValue(":qaD", QqaD);
                query.bindValue(":qnD", QqnD);
                query.bindValue(":qtD", QqtD);
                query.bindValue(":received_quantity", Qreceived_quantity);
                query.bindValue(":released_quantity", Qreleased_quantity);
                query.bindValue(":stock_received", Qstock_received);
                query.bindValue(":truck_chit_no", Qtruck_chit_no);
                query.bindValue(":truck_no", Qtruck_no);
                if(!query.exec()){
                    qDebug() << query.lastError();
                    qDebug() << "Stock Details Insertion Failed";

                    if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Stock Details Insertion Failed");
                    else if(LangMode == 2)
                        miscObj.DisplayWideMsgBox((char *)"स्टॉक विवरण प्रविष्टि विफल");
                    return -1;
                }
                else
                    qDebug() << "Stock Details Insertion Successfully";
            }
        }
    }
    return 0;
}

void Widget::on_BtnScanIris_clicked()
{
    Finger_Iris_Selection = 0;  //Iris Scanner
    if(RecieveGoodsAuth() == 0){
        ui->BtnDealerRecGoods->setEnabled(true);
        ui->BtnDealerRecGoods->setStyleSheet("color: rgb(0, 25, 255);background-color: rgb(255, 0, 255);");
        ui->BtnRouteOfficer->setStyleSheet("color: rgb(0, 25, 255);background-color: rgb(255, 255, 255);");

        if(iDealMemSelected ==  DEALER_SELECTED/*ROUTE_OFFICER_SELECTED*/){
            if(iModuleType == KEROSENE_TXN){
                ui->stackedWidget->setCurrentIndex(25);
                return;
            }else{
                if(SendReceiveStockToServer() == 0)
                    Print_ReceiveStock(2);
                ui->stackedWidget->setCurrentIndex(22);
                return;
            }
        }else if((iDealMemSelected ==  HAWKER_SELECTED)){
            if(iModuleType == KEROSENE_HWKR_TXN){
                qDebug()<<"Hawker Transaction.....";
                ui->stackedWidget->setCurrentIndex(25);
                return;
            }
        }
    }
    return;
}

int Widget::SendReceiveStockToServer()
{
    QSqlQuery query;
    systemret = system("rm /mnt/jffs2/StockUpdateReq.xml");
    systemret = system("rm /mnt/jffs2/StockUpdateResp.xml");

    FILE *fp;
    fp = fopen("/mnt/jffs2/StockUpdateReq.xml","w");
    if(fp == NULL){
        qDebug() << "/mnt/jffs2/StockUpdateReq.xml File Not Found" << endl;
        return -1;
    }
    int iRecvGoodsCount = 0;
    iRecvGoodsCount = getCommLength(ui->comboTruckChitNo->currentText().toAscii().data()); // added in 4.2 version as per NIC requirements.
    qDebug()<<"iRecvGoodsCount = "<<iRecvGoodsCount;

    char dispachId[64];
    memset(dispachId, 0, sizeof(dispachId));
    query.clear();
    query.prepare("SELECT dispatch_id FROM StockDetails WHERE truck_chit_no=? AND do_ro_no=?");
    query.addBindValue(ui->comboTruckChitNo->currentText().toAscii().data());
    query.addBindValue(ui->labelRO->text().toAscii().data());
    if(!query.exec())
        qDebug() << query.lastError();
    else{
        while(query.next()){
            strcpy(dispachId, query.value(0).toString().toAscii().data());
        }
    }

    fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ser=\"http://service.fetch.rationcard/\">");
    fprintf(fp, "<%s/>", "soapenv:Header");
    fprintf(fp, "<%s>", "soapenv:Body");
    fprintf(fp, "<%s>", "ser:getStockInfo");
    fprintf(fp, "<%s>", "Stock_Entry");
    fprintf(fp, "<comm_length>%d</comm_length>", iRecvGoodsCount);
    fprintf(fp, "<dispatch_id>%s</dispatch_id>", dispachId);
    fprintf(fp, "<do_ro_no>%s</do_ro_no>", ui->labelRO->text().toAscii().data());
    fprintf(fp, "<receipt_id>%s</receipt_id>", TxnId);
    fprintf(fp, "<route_off_auth>%s</route_off_auth>", gRoute_Off_auth_code);
    fprintf(fp, "<route_uid>%s</route_uid>", gRoute_Off_UID/*ui->tableWidgetRecieveGoodsAuth->item(0,1)->text().toAscii().data()*/);
    fprintf(fp, "<shop_no>%s</shop_no>", ui->ldtShpId->text().toAscii().data());

    int rows = ui->tableWidgetStockTable->rowCount();

    for(int i = 0; i<rows; i++){
        fprintf(fp, "<%s>", "stockNewBean");
        char buf1[32];
        memset(buf1, 0, sizeof(buf1));
        QStringList list;
        list = ui->tableWidgetStockTable->item(i,0)->text().split('(', QString::SkipEmptyParts);
        qDebug() << "LIST ====" << list;
        strcpy(buf1, list.at(0).trimmed().toAscii().data());

        fprintf(fp, "<comm>%s</comm>", buf1);

        query.clear();
        query.prepare("SELECT commoditycode, released_quantity FROM StockDetails WHERE comm=? AND truck_chit_no=? AND do_ro_no=?");
        query.addBindValue(buf1);
        query.addBindValue(ui->comboTruckChitNo->currentText().toAscii().data());
        query.addBindValue(ui->labelRO->text().toAscii().data());
        if(!query.exec())
            qDebug() << query.lastError();
        else{
            while(query.next()){
                fprintf(fp, "<comm_code>%s</comm_code>", query.value(0).toString().toAscii().data());
                fprintf(fp, "<KRA>%s</KRA>", ui->tableWidgetStockTable->item(i,2)->text().toAscii().data());
                fprintf(fp, "<qaD>%s</qaD>", ui->tableWidgetStockTable->item(i,5)->text().toAscii().data());
                fprintf(fp, "<qnD>%s</qnD>", ui->tableWidgetStockTable->item(i,4)->text().toAscii().data());
                fprintf(fp, "<recieve_qty>%s</recieve_qty>", ui->tableWidgetStockTable->item(i,11)->text().toAscii().data());

                fprintf(fp,"<released_quantity>%s</released_quantity>", query.value(0).toString().toAscii().data());
            }
        }
        fprintf(fp, "</%s>", "stockNewBean");
    }
    fprintf(fp, "<truck_chit_no>%s</truck_chit_no>", ui->comboTruckChitNo->currentText().toAscii().data());
    fprintf(fp, "<truck_no>%s</truck_no>", ui->label_TruckNo->text().toAscii().data());
    fprintf(fp, "</%s>", "Stock_Entry");
    fprintf(fp, "</%s>", "ser:getStockInfo");
    fprintf(fp, "</%s>", "soapenv:Body");
    fprintf(fp, "</%s>", "soapenv:Envelope");

    fclose(fp);
    qDebug() << "******** /mnt/jffs2/StockUpdateReq.xml *******";
    systemret = system("cat /mnt/jffs2/StockUpdateReq.xml");
    qDebug() << "***************************************************";

    GPRS objGprs;

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...\nUploading Recieved Stock Data to Server...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..\n प्राप्त स्टॉक सर्वर पर अपलोड हो रहा है."));
    //        miscObj.tel_DisplayWideMsgBox((char *)"कृपया प्रतीक्षा करें..\n प्राप्त स्टॉक सर्वर पर अपलोड हो रहा है.");

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    if(objGprs.Check_Network_Available() < 0){
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return -1;
    }
    char cUrlName[256];
    memset(cUrlName, 0, sizeof(cUrlName));

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

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/StockUpdateReq.xml", cUrlName,  "/mnt/jffs2/StockUpdateResp.xml", SERVER_TIME_OUT);
    qDebug()<<"\nCurl Cmd : "<<cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0){
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return -1;
    }
    qDebug() << "********/mnt/jffs2/StockUpdateResp.xml*********";
    systemret = system("cat /mnt/jffs2/StockUpdateResp.xml");
    qDebug() << "***********************************************";

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    if(ParsingFirstCondition((char *)"/mnt/jffs2/StockUpdateResp.xml") < 0)
        return -1;
    return 0;
}

int Widget::getCommLength(char *TruckChitNo)
{
    int iCommLength = 0;

    QSqlQuery query;
    query.prepare("SELECT comm_length FROM TruckChitTable WHERE truck_chit_no=?");
    query.addBindValue(TruckChitNo);
    if(!query.exec())
        qDebug() << "while getCommLength = " << query.lastError();
    else{
        while(query.next()){
            iCommLength = query.value(0).toInt(0);
        }
    }
    qDebug()<<"Comm Length : "<<iCommLength;
    return iCommLength;
}

int Widget::SendReceiveStockToServer_4_1()
{
    QSqlQuery query;
    systemret = system("rm /mnt/jffs2/StockUpdateReq.xml");
    systemret = system("rm /mnt/jffs2/StockUpdateResp.xml");

    FILE *fp;
    fp = fopen("/mnt/jffs2/StockUpdateReq.xml","w");
    if(fp == NULL){
        qDebug() << "/mnt/jffs2/StockUpdateReq.xml File Not Found" << endl;
        return -1;
    }
    int iRecvGoodsCount = 0;
    iRecvGoodsCount = ui->tableWidgetStockTable->rowCount();

    qDebug()<<"iRecvGoodsCount = "<<iRecvGoodsCount;

    fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ser=\"http://service.fetch.rationcard/\">");
    fprintf(fp, "<%s/>", "soapenv:Header");
    fprintf(fp, "<%s>", "soapenv:Body");
    fprintf(fp, "<%s>", "ser:getStockEntry");
    fprintf(fp, "<%s>", "Stock_Entry");
    fprintf(fp, "<comm_length>%d</comm_length>", iRecvGoodsCount);//Added field on 27/11/16 based on NIC requirement in 3.2 release
    fprintf(fp, "<receipt_id>%s</receipt_id>", TxnId);
    fprintf(fp, "<route_uid>%s</route_uid>", gRoute_Off_UID/*ui->tableWidgetRecieveGoodsAuth->item(0,1)->text().toAscii().data()*/);
    fprintf(fp, "<route_off_auth>%s</route_off_auth>", gRoute_Off_auth_code);
    fprintf(fp, "<shop_no>%s</shop_no>", ui->ldtShpId->text().toAscii().data());
    fprintf(fp, "<%s>", "stockBean");

    int rows = ui->tableWidgetStockTable->rowCount();
    for(int i = 0; i<rows; i++){
        char buf1[32];
        memset(buf1, 0, sizeof(buf1));
        QStringList list;
        list = ui->tableWidgetStockTable->item(i,0)->text().split('(', QString::SkipEmptyParts);
        qDebug() << "LIST ====" << list;
        strcpy(buf1, list.at(0).trimmed().toAscii().data());

        fprintf(fp, "<comm>%s</comm>", buf1);
    }
    for(int i = 0; i<rows; i++){
        char buf1[32];
        memset(buf1, 0, sizeof(buf1));
        QStringList list;
        QString buf;

        list = ui->tableWidgetStockTable->item(i,0)->text().split('(', QString::SkipEmptyParts);
        qDebug() << "LIST ====" << list;
        if(LangMode==1)
            strcpy(buf1, list.at(0).trimmed().toAscii().data());
        if(LangMode==2)
            buf=list.at(0).trimmed();

        query.clear();
        query.prepare("SELECT commoditycode FROM StockDetails WHERE comm=? AND truck_chit_no=? AND do_ro_no=?");
        if(LangMode==1)
            query.addBindValue(buf1);
        if(LangMode==2)
            query.addBindValue(buf);

        query.addBindValue(ui->comboTruckChitNo->currentText().toAscii().data());
        query.addBindValue(ui->labelRO->text().toAscii().data());
        if(!query.exec())
            qDebug() << query.lastError();
        else{
            while(query.next()){
                fprintf(fp, "<comm_code>%s</comm_code>", query.value(0).toString().toAscii().data());
            }
        }
    }

    for(int i = 0; i<rows; i++){
        char buf1[32];
        memset(buf1, 0, sizeof(buf1));
        QStringList list;
        list = ui->tableWidgetStockTable->item(i,0)->text().split('(', QString::SkipEmptyParts);
        qDebug() << "LIST ====" << list;
        strcpy(buf1, list.at(0).trimmed().toAscii().data());

        query.clear();
        query.prepare("SELECT dispatch_id FROM StockDetails WHERE comm=? AND truck_chit_no=? AND do_ro_no=?");
        query.addBindValue(buf1);
        query.addBindValue(ui->comboTruckChitNo->currentText().toAscii().data());
        query.addBindValue(ui->labelRO->text().toAscii().data());
        if(!query.exec())
            qDebug() << query.lastError();
        else{
            while(query.next()){
                fprintf(fp, "<dispatch_id>%s</dispatch_id>", query.value(0).toString().toAscii().data());
            }
        }
    }

    for(int i = 0; i<rows; i++){
        char buf1[32];
        memset(buf1, 0, sizeof(buf1));
        QStringList list;
        list = ui->tableWidgetStockTable->item(i,0)->text().split('(', QString::SkipEmptyParts);
        qDebug() << "LIST ====" << list;
        strcpy(buf1, list.at(0).trimmed().toAscii().data());

        query.clear();
        query.prepare("SELECT released_quantity FROM StockDetails WHERE comm=? AND truck_chit_no=? AND do_ro_no=?");
        query.addBindValue(buf1);
        query.addBindValue(ui->comboTruckChitNo->currentText().toAscii().data());
        query.addBindValue(ui->labelRO->text().toAscii().data());
        if(!query.exec())
            qDebug() << query.lastError();
        else{
            while(query.next()){
                fprintf(fp,"<released_quantity>%s</released_quantity>", query.value(0).toString().toAscii().data());
            }
        }
    }

    for(int i=0; i<rows; i++)
        fprintf(fp, "<do_ro_no>%s</do_ro_no>", ui->labelRO->text().toAscii().data());
    for(int i=0; i<rows; i++)
        fprintf(fp, "<KRA>%s</KRA>", ui->tableWidgetStockTable->item(i,2)->text().toAscii().data());
    for(int i=0; i<rows; i++)
        fprintf(fp, "<qnD>%s</qnD>", ui->tableWidgetStockTable->item(i,4)->text().toAscii().data());
    for(int i=0; i<rows; i++)
        fprintf(fp, "<qaD>%s</qaD>", ui->tableWidgetStockTable->item(i,5)->text().toAscii().data());
    for(int i=0; i<rows; i++)
        fprintf(fp, "<recieve_qty>%s</recieve_qty>", ui->tableWidgetStockTable->item(i,11)->text().toAscii().data());
    for(int i=0; i<rows; i++)
        fprintf(fp, "<truck_chit_no>%s</truck_chit_no>", ui->comboTruckChitNo->currentText().toAscii().data());
    for(int i=0; i<rows; i++)
        fprintf(fp, "<truck_no>%s</truck_no>", ui->label_TruckNo->text().toAscii().data());

    fprintf(fp, "</%s>", "stockBean");
    fprintf(fp, "</%s>", "Stock_Entry");
    fprintf(fp, "</%s>", "ser:getStockEntry");
    fprintf(fp, "</%s>", "soapenv:Body");
    fprintf(fp, "</%s>", "soapenv:Envelope");
    fclose(fp);

    qDebug() << "******** /mnt/jffs2/StockUpdateReq.xml *******";
    systemret = system("cat /mnt/jffs2/StockUpdateReq.xml");
    qDebug() << "***************************************************";

    GPRS objGprs;
    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...\nUploading Recieved Stock Data to Server...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..\n प्राप्त स्टॉक सर्वर पर अपलोड हो रहा है."));
    //        miscObj.tel_DisplayWideMsgBox((char *)"कृपया प्रतीक्षा करें..\n प्राप्त स्टॉक सर्वर पर अपलोड हो रहा है.");

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    if(objGprs.Check_Network_Available() < 0){
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return -1;
    }

    char cUrlName[256];
    memset(cUrlName, 0, sizeof(cUrlName));

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

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/StockUpdateReq.xml", cUrlName,  "/mnt/jffs2/StockUpdateResp.xml", SERVER_TIME_OUT);
    qDebug()<<"\nCurl Cmd : "<<cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0){
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return -1;
    }

    qDebug() << "********/mnt/jffs2/StockUpdateResp.xml*********";
    systemret = system("cat /mnt/jffs2/StockUpdateResp.xml");
    qDebug() << "***********************************************";

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    if(ParsingFirstCondition((char *)"/mnt/jffs2/StockUpdateResp.xml") < 0)
        return -1;

    return 0;
}

int Widget::parseStockUpdateResp(char *filename)
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
        return -1;
    }

    QDomDocument document;
    if(!document.setContent( &file ) )
    {
        qDebug( "Failed to parse the file into a DOM tree." );
        file.close();
        return -1;
    }
    file.close();

    QDomNodeList list;

    list = document.elementsByTagName("return"); //env:Body
    QString QResponse = list.at(0).childNodes().at(0).toElement().text();
    qDebug()<<"\nFinal Response : " << QResponse;

    gl11_lcdbklt_time_duration(90);

    if(QResponse.contains("Stock Entry  Done SuccessFully",Qt::CaseInsensitive) == true)
    {
        return 0;
    }
    else if(QResponse.contains("SuccessFully",Qt::CaseInsensitive) == true)
    {
        return 0;
    }
    else if(QResponse.length() == 0)
    {
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Invalid Response Data Recieved...");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"अवैध प्रतिक्रिया डेटा प्राप्त.");

        return -1;
    }
    else
    {
        gl11_lcdbklt_on();
        gl11_lcdbklt_time_duration(90);
        miscObj.DisplayWideMsgBox(QResponse.toAscii().data());
        return -1;
    }

    return 0;
}


void Widget::on_tableWidgetStockTable_cellChanged(int row, int column)
{
    if((column == 11) && (ui->tableWidgetStockTable->item(row, 11)->text().toFloat(0) != 0))
    {
        qDebug() << "Row = " << row;
        qDebug() << "Column = " << column;

        qDebug() << "dispatched Qty = " << ui->tableWidgetStockTable->item(row, column - 7)->text().toFloat(0);

        float fdisppercent = 0.00;
        fdisppercent = (ui->tableWidgetStockTable->item(row, column - 7)->text().toFloat(0) * 1.5)/100;
        qDebug() << "fdisppercent = " << fdisppercent;

        if(ui->tableWidgetStockTable->item(row, column)->text().toFloat(0) > ui->tableWidgetStockTable->item(row, column - 7)->text().toFloat(0))
        {
            if(LangMode ==1)
                miscObj.DisplayWideMsgBox((char *)"Please Receive the Qty upto Dispatched Qty only");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"Please Receive the Qty upto Dispatched Qty only");

            ui->tableWidgetStockTable->item(row, column)->setText("0");

            return;
        }

        qDebug() << "(ui->tableWidgetStockTable->item(row, column - 7)->text().toFloat(0) - ui->tableWidgetStockTable->item(row, column)->text().toFloat(0)) = " << (ui->tableWidgetStockTable->item(row, column - 7)->text().toFloat(0) - ui->tableWidgetStockTable->item(row, column)->text().toFloat(0));

        //Commodity Loss Calculation
        if((ui->tableWidgetStockTable->item(row, column - 7)->text().toFloat(0) - ui->tableWidgetStockTable->item(row, column)->text().toFloat(0)) > fdisppercent)
        {
            if(LangMode ==1)
            {
                miscObj.DisplayWideMsgBox((char *)"Entered Qty is Very Less...\nKindly Check...");
            }
            else if(LangMode ==2)
            {
                miscObj.tel_DisplayWideMsgBox((char *)"दर्ज की गई मात्रा बहुत कम है ... \n कोई जाँच करें।");
            }

            return;
        }
    }

    return;
}

void Widget::parseCurrentStockReportRespDetailsToDisplay()
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

    //CMisc miscObj;
    miscObj.DDMMYYYY2DD_MM_YYYY(DATE, szDashedDt);
    miscObj.HHMMSS2HH_MM_SS(TIME, szDashedTm);

    qDebug()<< "###Availed Cards : "<< AvailedCards <<" Total Cards : " << TotalCards;

    QStockReportType = "PDS";
    memset(prn, 0, sizeof(prn));
    sprintf(prn, " Shop ID: %s", ui->ldtShpId->text().toAscii().data());

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

    systemret = system("rm /mnt/jffs2/my_xml.xml");
    FILE *fp;
    fp = fopen("/mnt/jffs2/my_xml.xml","w");
    if(fp == NULL)
    {
        qDebug() << "File Not Found" << endl;
        return;
    }
    int slno = 0;
    fprintf(fp, "%s|%s|%s\n", ui->ldtShpId->text().toAscii().data(), TotalCards.toAscii().data(), AvailedCards.toAscii().data());

    int ChildCount=0,i = 0;
    QDomNodeList list ;

    list = document.elementsByTagName("return");
    ChildCount = list.count();
    qDebug() << "parseCurrentStockReportRespDetails: Main List Child Count = " << ChildCount;

    for(i = 0; i < ChildCount; i++)
    {
        QString Qallot_qty = list.at(i).childNodes().at(0).toElement().text();
        QString Qclosing_balance = list.at(i).childNodes().at(1).toElement().text();
        QString Qcomm_name = list.at(i).childNodes().at(2).toElement().text();
        QString Qcommoditycode = list.at(i).childNodes().at(3).toElement().text();
        QString Qissued_qty = list.at(i).childNodes().at(4).toElement().text();
        QString Qopening_balance = list.at(i).childNodes().at(5).toElement().text();
        QString Qreceived_qty = list.at(i).childNodes().at(6).toElement().text();
        QString Qtotal_quantity = list.at(i).childNodes().at(7).toElement().text();

        qDebug() << Qallot_qty << " " << Qclosing_balance << " " << Qcomm_name << " " << Qcommoditycode << " " << Qissued_qty << " " << Qopening_balance << " " << Qreceived_qty << " " <<  Qtotal_quantity;

        if(Qallot_qty == "Server Error")
        {
            if(LangMode==1)
                miscObj.DisplayWideMsgBox((char *)"Server Error");
            else if(LangMode==2)
                miscObj.tel_DisplayWideMsgBox((char *)"सर्वर एरर ");

            return;
        }

        QString measureUnit;

        if(Qclosing_balance.contains("Success"))
            continue;
        else if(!Qallot_qty.isNull())
        {

            fprintf(stdout, "%d|%s|%s|%s\n", slno, Qcomm_name.toAscii().data(), Qclosing_balance.toAscii().data(), measureUnit.toAscii().data());

            if(strcasecmp(Qcomm_name.toAscii().data(), "Rice") == 0 || strcasecmp(Qcomm_name.toAscii().data(), "SUGAR") == 0 || strcasecmp(Qcomm_name.toAscii().data(), "Kerosene") == 0 || strcasecmp(Qcomm_name.toAscii().data(), "CSK") == 0)
            {
                if(strcasecmp(Qcomm_name.toAscii().data(), "Rice") == 0 || strcasecmp(Qcomm_name.toAscii().data(), "SUGAR") == 0 )
                    measureUnit = "Kg";
                if(strcasecmp(Qcomm_name.toAscii().data(), "Kerosene") == 0)
                    measureUnit = "Ltr";
                if(strcasecmp(Qcomm_name.toAscii().data(), "CSK") == 0)
                    measureUnit = "Pac";


                slno++;
                fprintf(fp, "%d|%s|%s|%s\n", slno,  Qcomm_name.toAscii().data(), Qclosing_balance.toAscii().data(), measureUnit.toAscii().data());
            }
        }
    }

    fclose(fp);

    systemret = system("chmod +x /root/test_cb");
    systemret = system("/root/test_cb /dev/ttyGS0 9600 /mnt/jffs2/my_xml.xml");

    return;
}

void Widget::on_BtnOK_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(22);
    return;
}

void Widget::on_BtnPreview_clicked()
{
    if(ui->comboTruckChitNo->currentIndex() == 0)
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please Select Truck Chit No");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया ट्रक चित संख्या का चयन करें");

        return;
    }

    return;
}

void Widget::on_btnKerosene_clicked()
{
    ui->stackedWidget->setCurrentIndex(37);
}


void Widget::on_BtnHawker_clicked()
{
    if(iMainTransaction == HAWKER_TRANSACTION)
    {
        qDebug()<<"HAWKER_TRANSACTION and HAWKER_SELECTED and KEROSENE_HWKR_DIS";
        iModuleType = KEROSENE_HWKR_DIS;
    }
    else
    {
        iDealMemSelected = HAWKER_SELECTED;
        iModuleType = KEROSENE_HWKR_TXN;
    }
    miscObj.Check_mplayer_Exists();
    GPRS objGprs;

    popup_msg *dlgPopup2 = new popup_msg(this);

    if(LangMode == 1)
        dlgPopup2->ui->lblPopup->setText("Downloading Hawker Details...");
    else
        dlgPopup2->ui->lblPopup->setText(trUtf8("हॉकर विवरण डाउनलोड कर रहा है ..."));
    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    if(objGprs.Check_Network_Available() < 0)
    {
        return;
    }
    systemret = system("rm /mnt/jffs2/RouteOfficerReq.xml");
    systemret = system("rm /mnt/jffs2/RouteOfficerResp.xml");

    char cTxnType[4];
    memset(cTxnType, 0, sizeof(cTxnType));
    strcpy(cTxnType, "h");

    char prepReq[1024];
    memset(prepReq, 0, sizeof(prepReq));

    sprintf(prepReq, "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getRoutOffDetail><shop_number>%s</shop_number><comm_type>%s</comm_type></ns1:getRoutOffDetail></SOAP-ENV:Body></SOAP-ENV:Envelope>", ui->ldtShpId->text().toAscii().data(), cTxnType);

    QFile file("/mnt/jffs2/RouteOfficerReq.xml");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug() << "/mnt/jffs2/RouteOfficerReq.xml File Open Failed...";
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        return;
    }

    QTextStream out(&file);

    out << prepReq;
    file.close();

    qDebug() << "********** /mnt/jffs2/RouteOfficerReq.xml *************";
    systemret = system("cat /mnt/jffs2/RouteOfficerReq.xml");
    qDebug() << "*******************************************************";

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

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/RouteOfficerReq.xml", cUrlName, "/mnt/jffs2/RouteOfficerResp.xml", responseTimedOutTimeInSec.toInt());

    qDebug() << "\nCurl Cmd : " << cmdCurl;
    int sysret = system(cmdCurl);
    qDebug() << "sysret = " << sysret;

    if(SystemCurlResponse(sysret, EPDS_SERVER) < 0)
    {
        dlgPopup2->close();
        dlgPopup2->deleteLater();

        return;
    }

    qDebug() << "********** /mnt/jffs2/RouteOfficerResp.xml *************";
    systemret = system("cat /mnt/jffs2/RouteOfficerResp.xml");
    qDebug() << "*******************************************************";

    dlgPopup2->close();
    dlgPopup2->deleteLater();

    if(ParsingFirstCondition((char *)"/mnt/jffs2/RouteOfficerResp.xml") < 0)
        return;

    if(parseRouteOfficerResponse() < 0)
        return;

    int row = 0;
    query.clear();
    query.prepare("SELECT COUNT(name) FROM RouteOfficerDetails");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            row = query.value(0).toInt(0);
        }
    }

    ui->tableWidgetRecieveGoodsAuth->setRowCount(row);
    ui->tableWidgetRecieveGoodsAuth->clearContents();

    query.clear();
    query.prepare("SELECT name, uid, mobile FROM RouteOfficerDetails");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        int row = 0;
        while(query.next())
        {
            if((query.value(0).toString() != "NA") && (query.value(1).toString() != "NA"))
            {
                ui->tableWidgetRecieveGoodsAuth->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
                ui->tableWidgetRecieveGoodsAuth->item(row, 0)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row, 0)->flags() ^ Qt::ItemIsEditable);
                ui->tableWidgetRecieveGoodsAuth->item(row,0)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row,0)->flags() | Qt::ItemIsSelectable);

                ui->tableWidgetRecieveGoodsAuth->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
                ui->tableWidgetRecieveGoodsAuth->item(row, 1)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row, 1)->flags() ^ Qt::ItemIsEditable);
                ui->tableWidgetRecieveGoodsAuth->item(row,1)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row,1)->flags() | Qt::ItemIsSelectable);

                ui->tableWidgetRecieveGoodsAuth->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
                ui->tableWidgetRecieveGoodsAuth->item(row, 2)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row, 2)->flags() ^ Qt::ItemIsEditable);
                ui->tableWidgetRecieveGoodsAuth->item(row,2)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row,2)->flags() | Qt::ItemIsSelectable);

                row++;
            }
        }
    }

    ui->tableWidgetRecieveGoodsAuth->setColumnWidth(0,120);
    ui->tableWidgetRecieveGoodsAuth->setColumnWidth(1,100);
    ui->tableWidgetRecieveGoodsAuth->setColumnWidth(2,100);

    ui->tableWidgetRecieveGoodsAuth->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetRecieveGoodsAuth->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetRecieveGoodsAuth->verticalHeader()->hide();
    ui->tableWidgetRecieveGoodsAuth->showMaximized();
    ui->tableWidgetRecieveGoodsAuth->selectRow(0);

    return;
}

int Widget::Calling_Biomatiques_Iris(int iEyeSelect)
{
    qDebug() << "In Calling_Biomatiques_Iris()";

    static int i;

    t = BiomatiquesRDS_Discovery();

    if(t == NULL)
    {
        return -3;
    }
    if(strcmp(t->status,"NOTREADY") == 0)
    {

        return -2;
    }
    if(t!=NULL)
    {
        printf("\n\n\n**************************************************************");
        printf("\n Port          = %d",t->port);
        printf("\n Status        = %s",t->status);
        printf("\n info          = %s",t->info);
        printf("\n capture_path  = %s",t->capture_Path);
        printf("\n Info_path     = %s",t->info_path);
        printf("\n**************************************************************\n");

        if(i == 0)
        {

            tt=BiomatiquesRDS_DeviceInfo(t);
            if(tt = NULL)
            {
                return -3;

            }
            if(tt!=NULL)
            {
                printf("\n\n*************************************************************\n");
                printf("\n dpId   = %s \n",tt->dpid);
                printf("\n rdsid  = %s \n",tt->rdsid);
                printf("\n rdsver = %s \n",tt->rdsver);
                printf("\n dc     = %s \n",tt->dc);
                printf("\n mi     = %s \n",tt->mi);
                printf("\n mc     = %s \n",tt->mc);
                printf("\n\n*************************************************************\n");

            }
        }

        i++;

        pidopts p;
        sprintf(p.env,"%s","P");  // S - staging P - production PP-preproduction
        sprintf(p.wadh,"%s","");

        ttt=BiomatiquesRDS_Capture(t,p);

        if(ttt == NULL)
        {
            return -3;
        }
        if(ttt!=NULL)
        {
            printf("\n\n*************************************************************\n");
            printf("\n *******************Response************************************ ");
            printf("\n\n err_code = %s",ttt->err_code);
            printf("\n\n err_info = %s",ttt->err_info);

            if(strcmp(ttt->err_code,"0") != 0)
            {
                CMisc misc;
                misc.DisplayWideMsgBox(ttt->err_info);
                return -1;
            }
            printf("\n\n fCount   = %s",ttt->fCount);
            printf("\n\n fType    = %s",ttt->fType);
            printf("\n\n iType    = %s",ttt->iType);
            printf("\n\n iCount   = %s",ttt->iCount);
            printf("\n\n pType    = %s",ttt->pType);
            printf("\n\n pCount   = %s",ttt->pCount);
            printf("\n\n dpId     =%s",ttt->dev_info.dpid);
            printf("\n\n rdsid    =%s",ttt->dev_info.rdsid);
            printf("\n\n rdsVer   =%s",ttt->dev_info.rdsver);
            printf("\n\n dc       =%s",ttt->dev_info.dc);
            printf("\n\n mi       =%s",ttt->dev_info.mi);
            printf("\n\n mc       =%s",ttt->dev_info.mc);
            printf("\n\n CI       = %s",ttt->ci);
            printf("\n\n Skey     = %s",ttt->Skey);
            printf("\n\n Hmac     = %s",ttt->Hmac);
            printf("\n\n type     = %s",ttt->type);
            printf("\n\n PID     = %s",ttt->pid);
            printf("\n\n*************************************************************\n");
        }
    }
    return 0;
}

void Widget::OnFrameRecieve(RGBImage * frame)
{
    qDebug()<<frame;
    return;
}

void Widget:: OnCaptureCompleted(BIOMTQS_ISO * ii)
{
    qDebug() << "Raw Len = " << ii->length;
    qDebug() << "Raw Data = " << ii->data;

    for(int i = 0; i < ii->length; i++)
        fprintf(stdout, "%02X ", ii->data[i]);
    fprintf(stdout, "\n");

    FILE *fp;
    fp = fopen("/mnt/jffs2/Eye_iso.jp2", "wb");
    int len = fwrite(ii->data, sizeof(unsigned char), ii->length, fp);
    qDebug() << "File LEn = " << len;
    fclose(fp);

    gl11_buzz_on();
    sleep(1);
    gl11_buzz_off();

    g_Capture_Var = 1;
    qDebug() << "g_Capture_Var = " << g_Capture_Var;

    return;
}

void Widget::OnCaptureCompletedImage(RGBImage * frame,int kind,int * retval)
{
    qDebug()<<frame<<kind<<retval;
}

void Widget::on_BtnPDS_clicked()
{
    if(CheckTxnDate()<0)
        return;

    ui->ldtRationCard->setReadOnly(0);
    ui->radioButton_RCNOFLAG->setChecked(true);

    char Distofficecode[16]="";
    memset(Distofficecode,0,sizeof(Distofficecode));
    database_queries qry;
    qry.getdistofficecode(Distofficecode);
    ui->ldtRationCard->setText(Distofficecode);
    if(LangMode == 1)
        ui->lblRationCard->setText("Card No.");
    else
        ui->lblRationCard->setText(QString::fromUtf8("कार्ड नंबर"));

    ui->ldtRationCard->clear();

    cidType = "R";

    iMainTransaction = PDS_TRANSACTION;
    /*Set Transaction Mode First*/    //Added By Vivek 06062017
    cMode = "CA";
    qDebug()<<"Current Transaction Mode ="<<cMode;
    /*****************************/
    Finger_Iris_Selection = 0;
    memset(gauth_transaction_code, 0, sizeof(gauth_transaction_code));

    ui->comboTypeOfCard->setCurrentIndex(0);

    if(LangMode == 1)
        ui->BtnLastReciept->setText("Last Reciept");
    UidEncode_b64_flag = 1;

    if(DealerLoginBy == 2)
        ui->GetCardStsBtn->show();
    else
        ui->GetCardStsBtn->hide();

    ui->radioButton_RCNOFLAG->setChecked(true);
    on_radioButton_RCNOFLAG_clicked(true);
    ui->stackedWidget->setCurrentIndex(4);
    return;
}

int Widget::get_btnOK_EntitleStatus()
{
    QSqlQuery query;
    QString QStatus;
    query.prepare("SELECT Status FROM MenuTable WHERE Service = ?");
    query.addBindValue("getePDSRationCardDetails");
    if(!query.exec())
        qDebug() << query.lastError();
    else{
        while(query.next()){
            QStatus = query.value(0).toString();
            if((QStatus == "N") || (QStatus == "n")){
                return -1;
            }
        }
    }
    return 0;
}

void Widget::on_BtnBack_12_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    return;
}


void Widget::on_ldtShpId_cursorPositionChanged(int arg1, int arg2)
{
    ui->ldtShpId->setText(ui->ldtShpId->text().toAscii().toUpper());
    qDebug() <<arg1 <<arg2;
    return;
}

int Widget::UpdateDBTxnURL()
{
    char cFirstTwoNo[4];
    char cShopID[32];

    memset(cShopID, 0, sizeof(cShopID));
    strcpy(cShopID, ui->ldtShpId->text().toAscii().data());

    memset(cFirstTwoNo, 0, sizeof(cFirstTwoNo));
    strncpy(cFirstTwoNo, cShopID, 2);

    QString QShopID(cFirstTwoNo);

    UpdateURLs(PDS_URL);

    return 0;
}

void Widget::UpdateURLs(QString QUrlBuff)
{
    qDebug() << "QUrlBuff = " << QUrlBuff;

    if(QUrlBuff.contains("epos95.ap.gov.in"))
        QDomainName = "epos95";
    else if(QUrlBuff.contains("epostest.ap.gov.in"))
        QDomainName = "epostest";
    else if(QUrlBuff.contains("epos216.ap.gov.in"))
        QDomainName = "epos216";
    else if(QUrlBuff.contains("epos215.ap.gov.in"))
        QDomainName = "epos215";
    else
        QDomainName = "";

    QSqlQuery query;

    query.prepare("UPDATE Settings SET URL=?");
    query.addBindValue(QUrlBuff);
    if(!query.exec())
        qDebug() << "1. UpdateURLs = " << query.lastError();

    query.clear();
    query.prepare("UPDATE ShopOwnerDetails SET DealerURL=?");
    query.addBindValue(QUrlBuff);
    if(!query.exec())
        qDebug() << "2. UpdateURLs = " << query.lastError();

    return;
}


void Widget::on_BtnLastReciept_clicked()
{
    receipt_type=1;
    if(ui->radioButton_RCNOFLAG->isChecked()){
        if(ui->ldtRationCard->text().length() == 0){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Please Enter RC Number");
            else
                miscObj.tel_DisplayMsgBox((char *)"कृपया कार्ड नंबर दर्ज करें.");
            ui->ldtRationCard->clear();
            return;
        }
    }
    if(ui->radioButton_UIDFLAG->isChecked()){
        if(ui->ldtRationCard->text().length() < 12){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Please enter 12 digit uid number ");
            else
                miscObj.tel_DisplayMsgBox((char *)"कृपया 12 अंक यूआईडी नंबर दर्ज करें.");
            ui->ldtRationCard->clear();
            return;
        }
        if(miscObj.validate_uid(ui->ldtRationCard->text().toAscii().data()) < 0){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Invalid UID");
            else
                miscObj.tel_DisplayMsgBox((char *)"अमान्य यूआईडी");
            ui->ldtRationCard->clear();
            return;
        }
        rsaEncryption(ui->ldtRationCard->text().toAscii().data(),skey);
    }
    if(iMainTransaction == PDS_TRANSACTION){
        if(ui->radioButton_RCNOFLAG->isChecked()){
            qDebug()<<"Ration last receipt";
            PrintLastRecipt("R");
        }else if(ui->radioButton_UIDFLAG->isChecked()){
            qDebug()<<"AAdhaar last receipt";
            PrintLastRecipt("U");
        }
    }
    ui->ldtRationCard->clear();
    return;
}

void Widget::PrintLastRecipt(char *radioflag)
{
    int ret = 0;
    char RCID[36]="";
    memset(RCID,0,sizeof(RCID));

    popup_msg *dlgPopup1 = new popup_msg(this);

    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...\nDownloading Details From Server...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..\n सर्वर से विवरण डाउनलोड हो रहा है"));

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    GPRS objGprs;
    if(objGprs.Check_Network_Available() < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }
    systemret = system("rm /mnt/jffs2/PrintLastRecptReq.xml");
    systemret = system("rm /mnt/jffs2/PrintLastRecptResp.xml");

    char buff[2048];
    memset(buff, 0, sizeof(buff));
    if(ui->radioButton_UIDFLAG->isChecked())
        sprintf(buff, "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getReprintDetails><exiting_ration_card>%s</exiting_ration_card><Shop_no>%s</Shop_no><idType>%s</idType><token>%s</token><fpsSessionId>%s</fpsSessionId><stateCode>%s</stateCode></ns1:getReprintDetails></SOAP-ENV:Body></SOAP-ENV:Envelope>",asciiBase64Enc, ui->ldtShpId->text().toAscii().data(),radioflag,HD_PASSWORD,SessionId,stateCode.toStdString().c_str());
    else
        sprintf(buff, "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getReprintDetails><exiting_ration_card>%s</exiting_ration_card><Shop_no>%s</Shop_no><idType>%s</idType><token>%s</token><fpsSessionId>%s</fpsSessionId><stateCode>%s</stateCode></ns1:getReprintDetails></SOAP-ENV:Body></SOAP-ENV:Envelope>", ui->ldtRationCard->text().toAscii().data(), ui->ldtShpId->text().toAscii().data(),radioflag,HD_PASSWORD,SessionId,stateCode.toStdString().c_str());

    QFile file("/mnt/jffs2/PrintLastRecptReq.xml");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug() << "/mnt/jffs2/PrintLastRecptReq.xml File Open Failed...";
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }

    QTextStream out(&file);
    out << buff;
    file.close();

    qDebug() << "********** /mnt/jffs2/PrintLastRecptReq.xml ***********";
    systemret = system("cat /mnt/jffs2/PrintLastRecptReq.xml");
    qDebug() << "*******************************************************";

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

    int ret_http_code = UrlPingCheck(cUrlName);

    if(ret_http_code == 1)
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/PrintLastRecptReq.xml", cUrlName,  "/mnt/jffs2/PrintLastRecptResp.xml", responseTimedOutTimeInSec.toInt());
    }
    else
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/PrintLastRecptReq.xml", PDS_URL2,  "/mnt/jffs2/PrintLastRecptResp.xml", responseTimedOutTimeInSec.toInt());
    }

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0){
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }

    qDebug() << "**********/mnt/jffs2/PrintLastRecptResp.xml*********";
    ret  = system("cat /mnt/jffs2/PrintLastRecptResp.xml");
    qDebug();
    qDebug() << "***********************************************";

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    if(ParsingFirstCondition((char *)"/mnt/jffs2/PrintLastRecptResp.xml") < 0)
        return;

    if(ParsePrintLastRecptResponse((char *)"/mnt/jffs2/PrintLastRecptResp.xml",RCID) < 0){
        miscObj.DisplayWideMsgBox((char *)"InvalID Response Parsing");
        return;
    }

    if(LangMode == 2)
        miscObj.tel_DisplayWideMsgBox((char *)"लेनदेन सफल था ...");
    else
        miscObj.DisplayWideMsgBox((char *)"Transaction Successfull");

    popup_msg *dlgPopup2 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup2->ui->lblPopup->setText("Please Wait...Printing...");
    else
        dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें ... मुद्रण..."));
    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    PrintLastReciptResponse(RCID);

    dlgPopup2->close();
    dlgPopup2->deleteLater();

    return;
}

int Widget::ParsePrintLastRecptResponse(char *filename,char *RCID)
{
    QSqlQuery query;
    query.exec("DELETE FROM PrintLastResp");

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error Reading PrintLastResp File";
        return -1;
    }

    QXmlStreamReader xml(&file);

    QString Qbal_qty, Qcarry_over, QComm_name, QMember_name, QRecipt_id,QComm_name_ll,QMember_name_ll;
    QString QRetail_price, Qtot_Qty, QTxn_time, Quid_refer_no, Qtotal_amt,Rc_id,QavailedFps,QcommAmount;

    QString scheme_desc_en;

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "availedFps")
            {
                xml.readNext();
                QavailedFps = xml.text().toString();
                qDebug() << "availedFps : " << QavailedFps;
            }
            if(xml.name() == "bal_qty")
            {
                xml.readNext();
                Qbal_qty = xml.text().toString();
                qDebug() << "Qbal_qty : " << Qbal_qty;
            }

            if(xml.name() == "carry_over")
            {
                xml.readNext();
                Qcarry_over = xml.text().toString();
                qDebug() << "Qcarry_over : " << Qcarry_over;
            }
            if(xml.name() == "commIndividualAmount")
            {
                xml.readNext();
                QcommAmount = xml.text().toString();
                qDebug() << "QcommAmount : " << QcommAmount;
            }
            if(xml.name() == "comm_name")
            {
                xml.readNext();
                QComm_name = xml.text().toString();
                qDebug() << "QComm_name : " << QComm_name;
            }
            if(xml.name() == "comm_name_ll")
            {
                xml.readNext();
                QComm_name_ll = xml.text().toString();
                qDebug() << "QComm_name_ll : " << QComm_name_ll;
            }

            if(xml.name() == "member_name")
            {
                xml.readNext();
                QMember_name = xml.text().toString();
                qDebug() << "QMember_name : " << QMember_name;
            }
            if(xml.name() == "member_name_ll")
            {
                xml.readNext();
                QMember_name_ll = xml.text().toString();
                qDebug() << "member_name_ll : " <<QMember_name_ll;
            }
            if(xml.name() == "reciept_id")
            {
                xml.readNext();
                QRecipt_id = xml.text().toString();
                qDebug() << "QRecipt_id : " << QRecipt_id;
            }

            if(xml.name() == "retail_price")
            {
                xml.readNext();
                QRetail_price = xml.text().toString();
                qDebug() << "QRetail_price : " << QRetail_price;
            }
            if(xml.name() == "scheme_desc_en")
            {
                xml.readNext();
                scheme_desc_en = xml.text().toString();
                qDebug() << "scheme_desc_en : " << scheme_desc_en;
            }
            if(xml.name() == "tot_amount")
            {
                xml.readNext();
                Qtotal_amt = xml.text().toString();
                qDebug() << "Qtotal_amt : " << Qtotal_amt;
            }

            if(xml.name() == "total_quantity")
            {
                xml.readNext();
                Qtot_Qty = xml.text().toString();
                qDebug() << "Qtot_Qty : " << Qtot_Qty;
            }

            if(xml.name() == "transaction_time")
            {
                xml.readNext();
                QTxn_time = xml.text().toString();
                qDebug() << "QTxn_time : " << QTxn_time;
            }
            if(xml.name() == "rcId")//ration id
            {
                xml.readNext();
                Rc_id = xml.text().toString();
                qDebug() << "Rc_id : " <<Rc_id;
                strcpy(RCID,Rc_id.toAscii().data());
            }

            if(xml.name() == "uid_refer_no")
            {
                xml.readNext();
                Quid_refer_no = xml.text().toString();
                qDebug() << "Quid_refer_no : " << Quid_refer_no;

                query.clear();
                query.prepare("INSERT INTO PrintLastResp(bal_qty, carry_over, comm_name, member_name, reciept_id, retail_price, tot_amount, total_qty, txn_time, uid_refer_no,rcId,comm_name_ll,scheme,member_name_ll,availedFps,commAmount) VALUES(:bal_qty, :carry_over, :comm_name, :member_name, :reciept_id, :retail_price, :tot_amount, :total_qty, :txn_time, :uid_refer_no,:rcId,:comm_name_ll,:scheme,:member_name_ll,:availedFps,:commAmount)");
                query.bindValue(":bal_qty", Qbal_qty);
                query.bindValue(":carry_over", Qcarry_over);
                query.bindValue(":comm_name", QComm_name);
                query.bindValue(":member_name", QMember_name);
                query.bindValue(":reciept_id", QRecipt_id);//receipt id
                query.bindValue(":retail_price", QRetail_price);
                query.bindValue(":tot_amount", Qtotal_amt);
                query.bindValue(":total_qty", Qtot_Qty);
                query.bindValue(":txn_time", QTxn_time);
                query.bindValue(":uid_refer_no", Quid_refer_no);
                query.bindValue(":rcId",RCID);
                query.bindValue(":comm_name_ll",QComm_name_ll);
                query.bindValue(":scheme",scheme_desc_en);
                query.bindValue(":member_name_ll",QMember_name_ll);
                query.bindValue(":availedFps",QavailedFps);

                query.bindValue(":commAmount",QcommAmount);

                if(!query.exec())
                {
                    qDebug() << query.lastError();
                    return -1;
                }
                else
                    qDebug() << "Last Print REciept Response Inserted Successfully";
            }
        }
    }
    return 0;
}

void Widget::PrintLastReciptResponse(char *RCID)
{
    QSqlQuery query;
    char prn[256];

    prn_open();
//    if(prn_paperstatus() < 0)
    if((prn_paperstatus() < 0) && (paperRequiredFlag.toUpper() != "N"))
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया कागज डालें");
        prn_close();
        return;
    }

    QString QReciept_id, QMember_name, Quid_refer_no, Qtxn_time,RC_id,Scheme,QMember_name_ll,QavailedFps;
    query.clear();
    QMember_name.clear();

    query.prepare("SELECT reciept_id, member_name, uid_refer_no, txn_time,rcId,scheme,member_name_ll,availedFps FROM PrintLastResp");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            QReciept_id = query.value(0).toString();
            QMember_name = query.value(1).toString();

            Quid_refer_no = query.value(2).toString();
            Qtxn_time = query.value(3).toString();
            RC_id = query.value(4).toString();
            Scheme = query.value(5).toString();
            QMember_name_ll = query.value(6).toString();
            QavailedFps = query.value(7).toString();
        }
    }

    QString AllotmentMonth,AllotmentYear;
    query.prepare("SELECT AllotMonth, AllotYear FROM PartialOnlineData");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            AllotmentMonth = query.value(0).toString();
            AllotmentYear  = query.value(1).toString();
        }
    }

    char cCardType[8];
    char cCardNo[32];

    memset(cCardNo, 0, sizeof(cCardNo));
    strcpy(cCardNo, ui->ldtRationCard->text().toAscii().data());

    memset(cCardType, 0, sizeof(cCardType));
    strncpy(cCardType, cCardNo, 3);

    QSqlQuery fps_name;
    QString  Fps_owner_name;
    fps_name.prepare("SELECT Name FROM ShopOwnerDetails");
    if(!fps_name.exec())
    {
        qDebug() << "at fps name print"<<fps_name.lastError();
    }
    else
    {
        while(fps_name.next())
        {
            Fps_owner_name = fps_name.value(0).toString();
        }

    }

    LogoPrint objLogo;
    objLogo.PrintLogo(stateCode.toInt());

    database_queries dbqry;
    LangMode = dbqry.getLangMode();

    if(LangMode == 1)
    {
        QString str;
        str.clear();
        str= stateReceiptHeaderEn;
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );

        memset(prn, 0, sizeof(prn));

        if(receipt_type == 2)
        {
            str.clear();
            str= "RECEIPT";
            prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
            str.clear();
        }
        else
        {
            str.clear();
            str= "LAST RECEIPT";
            prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
            str.clear();
        }

        prn_write_text((unsigned char *)prn, strlen(prn), 0);


        memset(prn, 0, sizeof(prn));
        strcpy(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "FPS Owner Name   :%s" ,Fps_owner_name.toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 1);

        char tshopNo[25];
        memset(tshopNo,0,sizeof(tshopNo));
        getShopId(tshopNo);
        sprintf(prn, "FPS No           :%s", tshopNo);
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        if(receipt_type == 1)
        {
            memset(prn, 0, sizeof(prn));
            sprintf(prn, "Availed FPS No   :%s", QavailedFps.toAscii().data());
            prn_write_text((unsigned char *)prn, strlen(prn), 0);
        }

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "Name of Consumer :%s", QMember_name.toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "Card No. : %s/%s",RCID,Scheme.toAscii().data());//ui->label_146->text().toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "Transaction ID   :%s", QReciept_id.toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "Date: %s", Qtxn_time.toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "AllotmentMonth   :%s", AllotmentMonth.toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "AllotmentYear    :%s", AllotmentYear.toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        strcpy(prn, "\n");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        //sprintf(prn,"Commodity    Lifted  B.Qty  Rate  Price");
        sprintf(prn,"Commodity    Lifted  Rate  Price");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        strcpy(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }
    else
    {
        QString str= "";
        str.clear();

        //        str= QString::fromUtf8(" नगालैंड सरकार ");
        //        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER);

        str= stateReceiptHeaderLl;
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
        str.clear();

        //        str.clear();
        //        str= QString::fromUtf8("खाद्य और सार्वजनिक वितरण केंद्र");
        //        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );

        if(receipt_type == 2)
        {
            str.clear();
            str= QString::fromUtf8(Qpds_tran_tel.toAscii().data());
            prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
            str.clear();
        }
        else if(receipt_type == 1)
        {
            str.clear();
            str= QString::fromUtf8("अंतिम रसीद");
            prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
            str.clear();
        }

        str.clear();
        memset(prn, 0, sizeof(prn));
        strcpy(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        QSqlQuery fps_name;
        QString  Fps_owner_name;

        fps_name.prepare("SELECT Name FROM ShopOwnerDetails");
        if(!fps_name.exec())
        {
            qDebug() << "at fps name print"<<fps_name.lastError();
        }
        else
        {
            while(fps_name.next())
            {
                Fps_owner_name = fps_name.value(0).toString();
            }

        }
        str= QString::fromUtf8("मालिक का नाम :") +Fps_owner_name;

        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);
        str.clear();

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
        str= QString::fromUtf8("एफपीएस संख्या:") + fps_id;
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);
        str.clear();

        if(receipt_type == 1)
        {
            str= QString::fromUtf8("लाभार्थी एफपीएस आईडी:") + QavailedFps;
            prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);
            str.clear();
        }

        str.clear();
        str= QString::fromUtf8("ग्राहक :") + QMember_name_ll;
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

        str.clear();
        str= QString::fromUtf8("कार्ड नंबर:") + RCID+"/"+Scheme;//ui->label_146->text();
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

        str.clear();
        str= QString::fromUtf8("लेनदेन आईडी:") + QReciept_id;
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

        str.clear();
        str= QString::fromUtf8("तारीख: ") + Qtxn_time;
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

        str.clear();
        str= QString::fromUtf8("आवंटन माह :") + AllotmentMonth;
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

        str.clear();
        str= QString::fromUtf8("आबंटन वर्ष :") + AllotmentYear;
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

        str.clear();
        //TmpString= QString::fromUtf8(" वस्तु   ")+ QString::fromUtf8("मात्रा(Kg)")+QString::fromUtf8(" शेष राशि ") + QString::fromUtf8("की दर ")+QString::fromUtf8("कीमत");
        str= QString::fromUtf8(" वस्तु   ")+ QString::fromUtf8("मात्रा(Kg)")+ QString::fromUtf8("की दर ")+QString::fromUtf8("कीमत");
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

        str.clear();
        memset(prn, 0, sizeof(prn));
        strcpy(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }
    query.clear();
    query.prepare("SELECT bal_qty, carry_over, comm_name, retail_price, total_qty,comm_name_ll,commAmount FROM PrintLastResp");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            float fcalc_amt = 0.00;
            fcalc_amt = query.value(3).toString().toFloat(0) * query.value(1).toString().toFloat(0);

            if(LangMode == 1)
            {
                memset(prn, 0, sizeof(prn));

                //sprintf(prn, "%s(%.02f)  %.02f  %.02f  %0.2f %0.2f", query.value(2).toString().toAscii().data(), query.value(4).toString().toFloat(0), query.value(1).toString().toFloat(0), query.value(0).toString().toFloat(0), query.value(3).toString().toFloat(0),  fcalc_amt);
                //sprintf(prn, "%s(%.02f)  %.02f  %0.2f %0.2f", query.value(2).toString().toAscii().data(), query.value(4).toString().toFloat(0), query.value(1).toString().toFloat(0),  query.value(3).toString().toFloat(0),  fcalc_amt);

                sprintf(prn, "%s(%.02f)  %s   %0.2f   %s", query.value(2).toString().toAscii().data(), query.value(4).toString().toFloat(0), query.value(1).toString().toAscii().data(),query.value(3).toString().toFloat(0), query.value(6).toString().toAscii().data() );

                prn_write_text((unsigned char *)prn, strlen(prn), 0);
            }
            else if(LangMode == 2)
            {
                memset(prn, 0, sizeof(prn));

                //sprintf(prn, "(%.02f)  %.02f  %.02f  %0.2f  %0.2f",  query.value(4).toString().toFloat(0), query.value(1).toString().toFloat(0), query.value(0).toString().toFloat(0), query.value(3).toString().toFloat(0),  fcalc_amt);
                //sprintf(prn, "(%.02f)  %.02f  %0.2f  %0.2f",  query.value(4).toString().toFloat(0), query.value(1).toString().toFloat(0),  query.value(3).toString().toFloat(0),  fcalc_amt);


                sprintf(prn, "(%.02f)     %s   %0.2f   %s",  query.value(4).toString().toFloat(0), query.value(1).toString().toAscii().data(),query.value(3).toString().toFloat(0),query.value(6).toString().toAscii().data());

                QString printlastrecp;
                printlastrecp.clear();
                QString itemname_ll=query.value(5).toString();
                printlastrecp = itemname_ll + QString(prn);

                prepareBmp(printlastrecp, 22, printlastrecp, 18, ALIGN_LEFT);

            }

        }
    }

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    float db_tot_amt = 0.00;

    query.clear();
    query.prepare("SELECT tot_amount FROM PrintLastResp");
    if(!query.exec())
        qDebug() <<  query.lastError();
    else
    {
        while(query.next())
        {
            db_tot_amt = query.value(0).toString().toFloat(0);
        }
    }
    if(LangMode == 1)
    {
        memset(prn, 0, sizeof(prn));
        sprintf(prn, "TOTAL Amount: %.02f", db_tot_amt);
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        strcpy(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        strcpy(prn, "    Public Distribution Dept ");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn,0x00,sizeof(prn));
        sprintf(prn," (Note : Quantity's in Kgs/Ltrs)");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }
    else if(LangMode == 2)
    {
        char szTotalAmt[32];
        memset(szTotalAmt, 0, sizeof(szTotalAmt));
        sprintf(szTotalAmt, "%.02f", db_tot_amt);

        QString tMottam = QString::fromUtf8("कुल राशि: ")+ ": " + szTotalAmt + "     ";
        prepareBmp(tMottam, 22, tMottam, 18, ALIGN_LEFT);

        memset(prn, 0, sizeof(prn));
        strcpy(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        QString str;
        str.clear();
        str= QString::fromUtf8("    सार्वजनिक वितरण विभाग    ");
        prepareBmp(QString::fromUtf8(str.toUtf8()),22,QString::fromUtf8(str.toUtf8()),18,ALIGN_CENTER);

        str.clear();
        str= QString::fromUtf8("    (नोट: किग्रा/लीटर में मात्रा)    ");
        prepareBmp(QString::fromUtf8(str.toUtf8()),22,QString::fromUtf8(str.toUtf8()),18,ALIGN_CENTER);

    }
    prn_paper_feed(5);

    prn_close();
    return;
}


int Widget::getAvailableBalance(char *szAvailBal, char *szAccBal)
{
    strcpy(szAvailBal, &szAccBal[9]);
    szAvailBal[12]='\0';
    return 0;
}

int Widget::Cashless_FP_Enroll()
{
    miscObj.AudioTest((char *)"32");
    popup_msg *dlgPopup = new popup_msg(this);

    if(LangMode == 2)
        dlgPopup->ui->lblPopup->setText("अपनी अंगुली को स्कैनर पर रखें");
    dlgPopup->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    char b64Templ[2048];
    char isoTemplate[1024];

    memset(isoTemplate, 0, sizeof(isoTemplate));
    memset(b64Templ, 0, sizeof(b64Templ));

    FILE *fp;
    char buf[2048];
    unsigned char fp_buf[2048];
    int fp_size=0;
    fp = fopen("/root/samplehpds","r");

    fseek(fp,0, SEEK_END);
    fp_size=ftell(fp);
    rewind(fp);

    fprintf(stdout, "fp_size = %d\n", fp_size);
    memset(buf,0, sizeof(buf));
    memset(fp_buf,0, sizeof(fp_buf));
    fread(fp_buf,fp_size,1,fp);

    fclose(fp);

    dlgPopup->close();
    dlgPopup->deleteLater();

    return 0;
}

void Widget::on_btnMobileUpdate_clicked()
{
    UidEncode_b64_flag = 1;

    ui->lineEditAadharNo->clear();
    ui->lineEditCust_Mobile->clear();

    ui->lineEditAadharNo->setFocus();
    ui->stackedWidget->setCurrentIndex(35);
    return;
}

void Widget::on_BtnBack_18_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    return;
}

void Widget::on_BtnOTP_clicked()
{
    if(ui->lineEditAadharNo->text().isEmpty() || ui->lineEditAadharNo->text().isNull())
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please Enter Customer Aadhaar No");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया ग्राहक आधार संख्या दर्ज करें");

        ui->lineEditAadharNo->setFocus();

        return;
    }
    if(miscObj.validate_uid(ui->lineEditAadharNo->text().toAscii().data()) < 0)
    {
        miscObj.AudioTest((char *)"47");
        if(LangMode == 1)
            miscObj.DisplayMsgBox((char *)"Invalid UID.");
        else if(LangMode == 2)
            miscObj.tel_DisplayMsgBox((char *)"अमान्य यूआईडी");

        miscObj.AudioTest((char *)"46");
        return;
    }
    if(ui->lineEditCust_Mobile->text().isEmpty() || ui->lineEditCust_Mobile->text().isNull())
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please Enter Customer Mobile No");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया ग्राहक मोबाइल नंबर दर्ज करें");

        ui->lineEditCust_Mobile->setFocus();

        return;
    }

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText("कृपया प्रतीक्षा करें...");

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    systemret = system("rm /mnt/jffs2/mobileotp_req.xml");
    systemret = system("rm /mnt/jffs2/mobileotp_resp.xml");

    FILE *fp;
    fp = fopen("/mnt/jffs2/mobileotp_req.xml","w");
    if(fp == NULL)
    {
        qDebug() << "File Not Found" << endl;
        return;
    }

    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));

    miscObj.GetMachineId(cmachineID);

    fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getMobileOTP>");
    fprintf(fp, "<fpsSessionId>%s</fpsSessionId>", SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp, "<Shop_no>%s</Shop_no>", ui->ldtShpId->text().toAscii().data());
    fprintf(fp, "<terminal_id>%s</terminal_id>", cmachineID);
    fprintf(fp, "<uidNumber>%s</uidNumber>", ui->lineEditAadharNo->text().toAscii().data());
    fprintf(fp, "<mobile_no>%s</mobile_no>", ui->lineEditCust_Mobile->text().toAscii().data());
    fprintf(fp, "%s", "</ns1:getMobileOTP></SOAP-ENV:Body></SOAP-ENV:Envelope>");

    fclose(fp);

    qDebug() << "**********/mnt/jffs2/mobileotp_req.xml*********";
    systemret = system("cat /mnt/jffs2/mobileotp_req.xml");
    qDebug() << "***********************************************";

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

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/mobileotp_req.xml", cUrlName, "/mnt/jffs2/mobileotp_resp.xml", responseTimedOutTimeInSec.toInt());

    qDebug()<<"\nCurl Cmd : "<<cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();

        return;
    }
    dlgPopup1->close();
    dlgPopup1->deleteLater();

    qDebug() << "**********/mnt/jffs2/mobileotp_resp.xml*********";
    systemret = system("cat /mnt/jffs2/mobileotp_resp.xml");
    qDebug();
    qDebug() << "***********************************************";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/mobileotp_resp.xml") < 0)
        return;

    ui->lineEditOTP->clear();

    ui->stackedWidget->setCurrentIndex(36);
    return;
}

void Widget::on_BtnBack_19_clicked()
{
    ui->stackedWidget->setCurrentIndex(35);
    return;
}

void Widget::on_BtnSubmit_3_clicked()
{
    if(ui->lineEditOTP->text().isEmpty() || ui->lineEditOTP->text().isNull())
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please Enter OTP");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"ओटीपी दर्ज करें.");

        return;
    }
    QString QDealerUID;
    QSqlQuery query;

    query.prepare("SELECT UID FROM ShopOwnerDetails");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            QDealerUID = query.value(0).toString();
        }
    }
    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया ग्राहक अपनी उंगली स्कैनर पर लगाए"));//
    else if(LangMode== 1)
        dlgPopup1->ui->lblPopup->setText("Please Place Customer Finger on Scanner");

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    char cUID_Dummy[16];
    char b64Templ[1024];

    memset(cUID_Dummy, 0, sizeof(cUID_Dummy));
    memset(b64Templ, 0, sizeof(b64Templ));

    strcpy(cUID_Dummy, ui->lineEditAadharNo->text().toAscii().data());

    char cCust_Mob_Hmac[512];
    char cCust_Mob_Skey[512];
    char cCust_Mob_Pload[1024];

    memset(cCust_Mob_Hmac, 0, sizeof(cCust_Mob_Hmac));
    memset(cCust_Mob_Skey, 0, sizeof(cCust_Mob_Skey));
    memset(cCust_Mob_Pload, 0, sizeof(cCust_Mob_Pload));

    for(int i = 0; i < hmac_len; i++)
    {
        cCust_Mob_Hmac[i] = hmac[i];
    }

    for(int i = 0; i < enc_sessn_key_len; i++)
    {
        cCust_Mob_Skey[i] = encryptedSessKey[i];
    }

    for(int i = 0; i < aes_enc_data_len; i++)
    {
        cCust_Mob_Pload[i] = pload[i];
    }


    dlgPopup1->close();
    dlgPopup1->deleteLater();

    popup_msg *dlgPopup2 = new popup_msg(this);
    if(LangMode == 2)
        dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया विक्रेता अपनी उंगली स्कैनर पर लगाए"));
    else
        dlgPopup2->ui->lblPopup->setText("Please Place Dealer Finger on Scanner");

    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    memset(cUID_Dummy, 0, sizeof(cUID_Dummy));
    memset(b64Templ, 0, sizeof(b64Templ));

    strcpy(cUID_Dummy, QDealerUID.toAscii().data());

    char cDeal_Mob_Hmac[512];
    char cDeal_Mob_Skey[512];
    char cDeal_Mob_Pload[1024];

    memset(cDeal_Mob_Hmac, 0, sizeof(cDeal_Mob_Hmac));
    memset(cDeal_Mob_Skey, 0, sizeof(cDeal_Mob_Skey));
    memset(cDeal_Mob_Pload, 0, sizeof(cDeal_Mob_Pload));

    for(int i = 0; i < hmac_len; i++)
    {
        cDeal_Mob_Hmac[i] = hmac[i];
    }

    for(int i = 0; i < enc_sessn_key_len; i++)
    {
        cDeal_Mob_Skey[i] = encryptedSessKey[i];
    }

    for(int i = 0; i < aes_enc_data_len; i++)
    {
        cDeal_Mob_Pload[i] = pload[i];
    }

    dlgPopup2->close();
    dlgPopup2->deleteLater();

    popup_msg *dlgPopup3 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup3->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup3->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.."));

    dlgPopup3->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    systemret = system("rm /mnt/jffs2/mobileupdate_req.xml");
    systemret = system("rm /mnt/jffs2/mobileupdate_resp.xml");

    FILE *fp;
    fp = fopen("/mnt/jffs2/mobileupdate_req.xml","w");
    if(fp == NULL)
    {
        qDebug() << "File Not Found" << endl;

        dlgPopup3->close();
        dlgPopup3->deleteLater();

        return;
    }
    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));

    miscObj.GetMachineId(cmachineID);


    fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getMobileNumberUpdate>");
    fprintf(fp, "<fpsSessionId>%s</fpsSessionId>", SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp, "<Shop_no>%s</Shop_no>", ui->ldtShpId->text().toAscii().data());
    fprintf(fp, "<terminal_id>%s</terminal_id>", cmachineID);
    fprintf(fp, "<resAuth>");
    fprintf(fp, "<res_Consent>%s</res_Consent>", "Y");
    fprintf(fp, "<res_certificateIdentifier>%s</res_certificateIdentifier>", bufExpiryStr);
    fprintf(fp, "<res_encHmac>%s</res_encHmac>", cCust_Mob_Hmac);
    fprintf(fp, "<res_mobile_no>%s</res_mobile_no>", ui->lineEditCust_Mobile->text().toAscii().data());
    fprintf(fp, "<res_otp>%s</res_otp>", ui->lineEditOTP->text().toAscii().data());
    fprintf(fp, "<res_secure_pid>%s</res_secure_pid>", cCust_Mob_Pload);
    fprintf(fp, "<res_sessionKey>%s</res_sessionKey>", cCust_Mob_Skey);
    fprintf(fp, "<res_uid>%s</res_uid>", ui->lineEditAadharNo->text().toAscii().data());
    //  fprintf(fp, "<timeStamp>%s</timeStamp>", QRes_Pid_ts.toAscii().data());
    fprintf(fp, "</resAuth>");
    fprintf(fp, "<oprAuth>");
    fprintf(fp, "<opr_certificateIdentifier>%s</opr_certificateIdentifier>", bufExpiryStr);
    fprintf(fp, "<opr_encHmac>%s</opr_encHmac>", cDeal_Mob_Hmac);
    fprintf(fp, "<opr_secure_pid>%s</opr_secure_pid>", cDeal_Mob_Pload);
    fprintf(fp, "<opr_sessionKey>%s</opr_sessionKey>", cDeal_Mob_Skey);
    fprintf(fp, "<opr_uid>%s</opr_uid>", QDealerUID.toAscii().data());
    fprintf(fp, "</oprAuth>");
    fprintf(fp, "</ns1:getMobileNumberUpdate></SOAP-ENV:Body></SOAP-ENV:Envelope>");
    fclose(fp);


    qDebug() << "*********/mnt/jffs2/mobileupdate_req.xml *********";
    systemret = system("cat /mnt/jffs2/mobileupdate_req.xml");
    qDebug();
    qDebug() << "***************************************************";

    char cUrlName[256];
    query.clear();
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

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/mobileupdate_req.xml", cUrlName,  "/mnt/jffs2/mobileupdate_resp.xml", responseTimedOutTimeInSec.toInt());

    qDebug()<<"\nCurl Cmd : "<<cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0)
    {
        dlgPopup3->close();
        dlgPopup3->deleteLater();

        return;
    }

    qDebug() << "***********/mnt/jffs2/mobileupdate_resp.xml************";
    systemret = system("cat /mnt/jffs2/mobileupdate_resp.xml");
    qDebug();
    qDebug() << "***************************************************";

    dlgPopup3->close();
    dlgPopup3->deleteLater();


    if(ParsingFirstCondition((char *)"/mnt/jffs2/mobileupdate_resp.xml") != 0)
    {
        miscObj.DisplayWideMsgBox((char *)"Mobile Number Updation Request Failed");
    }
    else
    {
        miscObj.DisplayWideMsgBox((char *)"Mobile Number Updation Request Successfully Posted");
    }

    ui->lineEditAadharNo->clear();
    ui->lineEditCust_Mobile->clear();

    ui->stackedWidget->setCurrentIndex(35);
    return;
}

void Widget::ReadSimSerialNo(char *buff)
{
    QFile file("/tmp/sim_details");
    if(!file.exists())
    {
        qDebug() << "/tmp/sim_details File Not Found";
        return;
    }

    file.open(QIODevice::ReadOnly);
    QString s;
    QTextStream s1(&file);
    s.append(s1.readAll());
    file.close();

    QStringList list = s.split(',');

    memset(buff, 0, sizeof(buff));
    strcpy(buff, list.at(0).trimmed().toAscii().data());

    return;
}


int Widget::FusionUpdateToServer(char *UserType, char *FusionIndication, char *FusionUID,char *MemberId)
{
    if(strcmp(FusionStatus, "1") == 0 )
    {
        qDebug()<<"No Need to update fusion to server";
        return -1;
    }

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.."));
    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    UidEncode_b64_flag = 1;//Kerosene entry, route officer

    QString QShop_no;
    QSqlQuery query;
    query.clear();
    query.prepare("SELECT ShopID FROM Shopownerdetails");
    if(!query.exec())
        qDebug() << "Unable to get shopid = " << query.lastError();
    else{
        while(query.next()){
            QShop_no = query.value(0).toString();
        }
    }
    systemret = system("rm /mnt/jffs2/UpdateFusionReq.xml");
    systemret = system("rm /mnt/jffs2/UpdateFusionResp.xml");

    QFile file("/mnt/jffs2/UpdateFusionReq.xml");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug() << "/mnt/jffs2/UpdateFusionReq.xml File Open Failed";
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return -1;
    }

    QTextStream out(&file);

    char buff[2048];
    memset(buff, 0, sizeof(buff));

    sprintf(buff, "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getFusionRecord><fpsSessionId>%s</fpsSessionId><stateCode>%s</stateCode><user_type>%s</user_type><shop_no>%s</shop_no><uidNumber>%s</uidNumber><member_fusion>%s</member_fusion><member_id>%s</member_id></ns1:getFusionRecord></SOAP-ENV:Body></SOAP-ENV:Envelope>", SessionId, stateCode.toStdString().c_str(), UserType, QShop_no.toAscii().data(), FusionUID, FusionIndication,MemberId );
    out << buff;
    file.close();

    qDebug() << "***************/mnt/jffs2/UpdateFusionReq.xml*****************";
    systemret = system("cat /mnt/jffs2/UpdateFusionReq.xml");
    qDebug() << "************************************************";

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

    int ret_http_code = UrlPingCheck(cUrlName);

    if(ret_http_code == 1)
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/UpdateFusionReq.xml", cUrlName, "/mnt/jffs2/UpdateFusionResp.xml", responseTimedOutTimeInSec.toInt());
    }
    else
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/UpdateFusionReq.xml",PDS_URL2, "/mnt/jffs2/UpdateFusionResp.xml", responseTimedOutTimeInSec.toInt());
    }

    qDebug() << "Curl Cmd: " << cmdCurl;
    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0){
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return -1;
    }
    dlgPopup1->close();
    dlgPopup1->deleteLater();

    qDebug() << "***************/mnt/jffs2/UpdateFusionResp.xml*****************";
    system((char *)"cat /mnt/jffs2/UpdateFusionResp.xml");
    qDebug() << "************************************************";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/UpdateFusionResp.xml") < 0)
        return -1;

    return 0;
}


/***************************************************************************************************************************/
/************                   CASHLESS NEW IMPLEMENTATIONS - SEP16                       ************/
/***************************************************************************************************************************/

void Widget::on_btnBackEKYC_clicked()
{
    ui->stackedWidget->setCurrentIndex(40);
}

void Widget::on_btnSubmitEKYC_clicked()
{
    if(miscObj.validate_uid(ui->ldtUID_EKYC->text().toAscii().data()) < 0)
    {
        qDebug()<<"Invalid UID for EKYC";
        miscObj.DisplayMsgBox((char *)"Invalid UID");
        return;
    }
    ui->ChkBox_Y_POI_POA->setChecked(true);
    ui->ChkBox_N_POI_POA->setChecked(false);

    ui->ChkBox_Y_Mob_eMail->setChecked(true);
    ui->ChkBox_N_Mob_eMail->setChecked(false);
    EKYC_TEST_FLAG = 1;

    EKYC_Submit();

    EKYC_TEST_FLAG = 0;
}

void Widget::on_ChkBox_Y_POI_POA_clicked(bool checked)
{
    ui->ChkBox_Y_POI_POA->setChecked(true);
    ui->ChkBox_N_POI_POA->setChecked(false);
}

void Widget::on_ChkBox_N_POI_POA_clicked(bool checked)
{
    ui->ChkBox_Y_POI_POA->setChecked(false);
    ui->ChkBox_N_POI_POA->setChecked(true);
}

void Widget::on_ChkBox_Y_Mob_eMail_clicked(bool checked)
{
    ui->ChkBox_Y_Mob_eMail->setChecked(true);
    ui->ChkBox_N_Mob_eMail->setChecked(false);
}

void Widget::on_ChkBox_N_Mob_eMail_clicked(bool checked)
{
    ui->ChkBox_Y_Mob_eMail->setChecked(false);
    ui->ChkBox_N_Mob_eMail->setChecked(true);
}

void Widget::EKYC_Submit()
{

    QString QDealerUID;
    QSqlQuery query;

    query.prepare("SELECT UID FROM ShopOwnerDetails");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            QDealerUID = query.value(0).toString();
        }
    }
    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया ग्राहक अपनी उंगली स्कैनर पर लागए"));
    else
        dlgPopup1->ui->lblPopup->setText("Please Place Customer Finger on Scanner");

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    char cUID_Dummy[16];
    char b64Templ[1024];

    memset(cUID_Dummy, 0, sizeof(cUID_Dummy));
    memset(b64Templ, 0, sizeof(b64Templ));

    strcpy(cUID_Dummy, ui->ldtUID_EKYC->text().toAscii().data());

    char cCust_Mob_Hmac[512];
    char cCust_Mob_Skey[512];
    char cCust_Mob_Pload[1024];

    memset(cCust_Mob_Hmac, 0, sizeof(cCust_Mob_Hmac));
    memset(cCust_Mob_Skey, 0, sizeof(cCust_Mob_Skey));
    memset(cCust_Mob_Pload, 0, sizeof(cCust_Mob_Pload));

    for(int i = 0; i < hmac_len; i++)
    {
        cCust_Mob_Hmac[i] = hmac[i];
    }

    for(int i = 0; i < enc_sessn_key_len; i++)
    {
        cCust_Mob_Skey[i] = encryptedSessKey[i];
    }

    for(int i = 0; i < aes_enc_data_len; i++)
    {
        cCust_Mob_Pload[i] = pload[i];
    }
    dlgPopup1->close();
    dlgPopup1->deleteLater();


    popup_msg *dlgPopup3 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup3->ui->lblPopup->setText("Please Wait...");
    else
        dlgPopup3->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.."));
    dlgPopup3->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    system("rm /mnt/jffs2/EKYCAuthenticateReq.xml");
    system("rm /mnt/jffs2/EKYCAuthenticateResp.xml");

    FILE *fp;
    fp = fopen("/mnt/jffs2/EKYCAuthenticateReq.xml","w");
    if(fp == NULL)
    {
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
    {
        strcpy(POI_POA, "Y");
    }
    else
    {
        strcpy(POI_POA, "N");
    }

    qDebug()<<"POI_ POA = "<<POI_POA;


    if(ui->ChkBox_Y_Mob_eMail->isChecked() == true)
    {
        strcpy(Mob, "Y");
    }
    else
    {
        strcpy(Mob, "N");
    }

    fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\"><SOAP-ENV:Body><ns1:getEKYCAuthenticate>");
    fprintf(fp, "<fpsSessionId>%s</fpsSessionId>", SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp, "<Shop_no>%s</Shop_no>", ui->ldtShpId->text().toAscii().data());
    fprintf(fp, "<terminal_id>%s</terminal_id>", cmachineID);
    fprintf(fp, "<ekycresAuth>");
    fprintf(fp, "<res_Consent_POIandPOA>%s</res_Consent_POIandPOA>", POI_POA);// "Y");
    fprintf(fp, "<res_Consent_mobileOREmail>%s</res_Consent_mobileOREmail>", Mob);// "Y");
    fprintf(fp, "<res_certificateIdentifier>%s</res_certificateIdentifier>", bufExpiryStr);
    fprintf(fp, "<res_encHmac>%s</res_encHmac>", cCust_Mob_Hmac);
    fprintf(fp, "<res_secure_pid>%s</res_secure_pid>", cCust_Mob_Pload);
    fprintf(fp, "<res_sessionKey>%s</res_sessionKey>", cCust_Mob_Skey);
    fprintf(fp, "<res_uid>%s</res_uid>", ui->ldtUID_EKYC->text().toAscii().data());
    fprintf(fp, "</ekycresAuth>");
    fprintf(fp, "</ns1:getEKYCAuthenticate></SOAP-ENV:Body></SOAP-ENV:Envelope>");
    fclose(fp);

    qDebug() << "*********/mnt/jffs2/EKYCAuthenticateReq.xml *********";
    system("cat /mnt/jffs2/EKYCAuthenticateReq.xml");
    qDebug();
    qDebug() << "***************************************************";

    char cUrlName[256];
    query.clear();
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

    int ret_http_code = UrlPingCheck(cUrlName);

    if(ret_http_code == 1)
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/EKYCAuthenticateReq.xml", cUrlName,  "/mnt/jffs2/EKYCAuthenticateResp.xml", responseTimedOutTimeInSec.toInt());
    }
    else
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/EKYCAuthenticateReq.xml", PDS_URL2,  "/mnt/jffs2/EKYCAuthenticateResp.xml", responseTimedOutTimeInSec.toInt());
    }

    qDebug()<<"\nCurl Cmd : "<<cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0)
    {
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


    if(ParsingFirstCondition((char *)"/mnt/jffs2/EKYCAuthenticateResp.xml") != 0)
    {
        //        miscObj.DisplayWideMsgBox((char *)"EKYC Request Failed");
    }

    ui->ldtUID_EKYC->clear();

    ui->stackedWidget->setCurrentIndex(41);
    return;
}

/********************** adding printing ********************************************/
void Widget :: PrintTxnReceipt()
{
    char prn[256];
    LogoPrint logo;
    logo.PrintLogo(stateCode.toInt());

    int currentRow = ui->twdgtEntitlement->currentRow();
    int currentRow_tbl = ui->tblCommIssue->currentRow();

    QString str= "";
    str.clear();

    //    str= QString::fromUtf8(" नगालैंड सरकार  ");
    //    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER);

    str= stateReceiptHeaderLl;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
    str.clear();

    //    str.clear();
    //    str= QString::fromUtf8("खाद्य और सार्वजनिक वितरण केंद्र");
    //    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );

    str.clear();
    str= QString::fromUtf8(Qpds_tran_tel.toAscii().data());
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
    str.clear();
    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    QSqlQuery fps_name;
    QString  Fps_owner_name;

    fps_name.prepare("SELECT Name FROM ShopOwnerDetails");
    if(!fps_name.exec())
    {
        qDebug() << "at fps name print"<<fps_name.lastError();
    }
    else
    {
        while(fps_name.next())
        {
            Fps_owner_name = fps_name.value(0).toString();
        }

    }

    str= QString::fromUtf8("मालिक का नाम :") +Fps_owner_name;

    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);
    str.clear();

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

    str.clear();
    str= QString::fromUtf8("ग्राहक :") + ui->twdgtEntitlement->item(currentRow,0)->text();
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

    str.clear();
    str= QString::fromUtf8("कार्ड नंबर:") + Rcno.toAscii().data()+"/"+ui->label_146->text();
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);


    str.clear();
    str= QString::fromUtf8("लेनदेन आईडी:") + receiptid;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

    str.clear();
    str= QString::fromUtf8("तारीख: ") + ui->labelDate->text() +"  "+ ui->labelTime->text();
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

    str.clear();
    str= QString::fromUtf8(" वस्तु   ")+ QString::fromUtf8("       मात्रा(Kg)")+QString::fromUtf8("  शेष राशि  ") + QString::fromUtf8("   की दर  ")+QString::fromUtf8("कीमत");

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    prepareBmp(QString::fromUtf8(str.toUtf8()),22,QString::fromUtf8(str.toUtf8()),18,ALIGN_LEFT);

    str.clear();
    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);
    int iRow = ui->tblCommIssue->rowCount();
    int i=0, SlNo=0;

    while(i<iRow)
    {
        if(ui->tblCommIssue->item(i, 3)->text().isEmpty() || (atof(ui->tblCommIssue->item(i, 3)->text().toAscii().data()) == 0.00))
        {
            qDebug()<<ui->tblCommIssue->item(i,0)->text()<<"is Empty";
            i++;
            continue;
        }
        ++SlNo;

        HindiNameComm=ui->tblCommIssue->item(i,0)->text();

        QString QcommNamellalign;
        QcommNamellalign.clear();
        QcommNamellalign = QString(HindiNameComm).leftJustified(15, ' ');
        char entitlement[180]="";
        memset(entitlement,0,sizeof(entitlement));

        float balqty=0;
        balqty=atof(ui->tblCommIssue->item(i,1)->text().toAscii().data())-atof(ui->tblCommIssue->item(i,3)->text().toAscii().data());

        sprintf(entitlement, "%-0.2f     %-0.2f       %-0.2f    %-0.2f",atof(ui->tblCommIssue->item(i,3)->text().toAscii().data()),balqty, atof(ui->tblCommIssue->item(i,2)->text().toAscii().data()),atof(ui->tblCommIssue->item(i,4)->text().toAscii().data()));

        str= QcommNamellalign+" "+entitlement;

        prepareBmp(QString::fromUtf8(str.toUtf8()),22,QString::fromUtf8(str.toUtf8()),18,ALIGN_LEFT);
        i++;
    }

    i=0;


    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    str.clear();
    str= QString::fromUtf8("कुल राशि :   ") + ui->lblTotalAmtVal_2->text();
    prepareBmp(QString::fromUtf8(str.toUtf8()),22,QString::fromUtf8(str.toUtf8()),19,ALIGN_CENTER);

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    str.clear();
    str= QString::fromUtf8("    सार्वजनिक वितरण विभाग:    ");
    prepareBmp(QString::fromUtf8(str.toUtf8()),22,QString::fromUtf8(str.toUtf8()),18,ALIGN_CENTER);

    prn_paper_feed(4);

}
/********************************** ADDING ENGLISH PRINT  ************************************/
void Widget :: PrintTxnReceiptEng()
{
    char prn[256];
    LogoPrint logo;
    logo.PrintLogo(stateCode.toInt());

    int currentRow = ui->twdgtEntitlement->currentRow();
    QString str;
    str.clear();

    str= stateReceiptHeaderEn;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
    str.clear();

    str= "RECEIPT";
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
    str.clear();

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    QSqlQuery fps_name;
    QString  Fps_owner_name;
    fps_name.prepare("SELECT Name FROM ShopOwnerDetails");
    if(!fps_name.exec())
    {
        qDebug() << "at fps name print"<<fps_name.lastError();
    }
    else
    {
        while(fps_name.next())
        {
            Fps_owner_name = fps_name.value(0).toString();
        }
    }
    memset(prn, 0, sizeof(prn));
    sprintf(prn,"FPS Owner Name :%s",Fps_owner_name.toAscii().data());
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

    memset(prn, 0, sizeof(prn));
    sprintf(prn,"FPS No :%s",fps_id.toAscii().data());
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn,"Name Of Consumer :%s", ui->twdgtEntitlement->item(currentRow,0)->text().toAscii().data());
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn,"Card No.  :%s/%s",Rcno.toAscii().data(),ui->label_146->text().toAscii().data());
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn,"Transaction ID :%s",receiptid.toAscii().data());
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    str= QString::fromUtf8("Date: ") + ui->labelDate->text() +"  "+ ui->labelTime->text();
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn,"%-12s %-7s %-6s %-7s %-7s","Commodity","Lifted","Bal.Qty","Rate","Price");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    int iRow = ui->tblCommIssue->rowCount();
    int i=0, SlNo=0;

    while(i<iRow)
    {
        if(ui->tblCommIssue->item(i, 3)->text().isEmpty() || (atof(ui->tblCommIssue->item(i, 3)->text().toAscii().data()) == 0.00))
        {
            qDebug()<<ui->tblCommIssue->item(i,0)->text()<<"is Empty";
            i++;
            continue;
        }
        ++SlNo;
        memset(prn, 0, sizeof(prn));

        float balqty=0;
        balqty=atof(ui->tblCommIssue->item(i,1)->text().toAscii().data())-atof(ui->tblCommIssue->item(i,3)->text().toAscii().data());

        sprintf(prn,"%-12s  %-6s %-0.2f %-7s %-6s",ui->tblCommIssue->item(i,0)->text().toAscii().data(),ui->tblCommIssue->item(i,3)->text().toAscii().data(),balqty,ui->tblCommIssue->item(i,2)->text().toAscii().data(),ui->tblCommIssue->item(i,4)->text().toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
        i++;
    }

    i=0;

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    str.clear();
    str= QString::fromUtf8("Total Amount:   ") + ui->lblTotalAmtVal_2->text();
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);


    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    str.clear();
    str= QString::fromUtf8("     Public Distribution Dept  ");
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

    str.clear();

    memset(prn, 0, sizeof(prn));
    strcpy(prn, "\n\n");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    prn_paper_feed(4);
}

int Widget::CL_Conclusion(char *BankRespStatus)
{
    char FileName[64];
    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/CL_ConclusionReq.xml");
    getCL_ConclusionReq(FileName, BankRespStatus);//, BankRespStatus);

    CL_ConclusionToServer(FileName);

    return 0;
}

int Widget::getCL_ConclusionReq(char *FileName, char *BankRespStatus)
{
    parse parobj;
    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));
    miscObj.GetMachineId(cmachineID);
    QSqlQuery query;

    QString QShop_no;
    query.clear();
    query.prepare("SELECT ShopID FROM Shopownerdetails");
    if(!query.exec())
        qDebug() << "Unable to get shopid = " << query.lastError();
    else
    {
        while(query.next())
        {
            QShop_no = query.value(0).toString();
        }
    }
    char TempRRN[32];
    memset(TempRRN, 0, sizeof(TempRRN));
    parobj.getTempRRN(TempRRN);

    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL)
    {
        qDebug() << "File Not Found" << endl;
        return -1;
    }

    fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns1=\"http://service.fetch.rationcard/\">");
    fprintf(fp, "<%s>", "SOAP-ENV:Body");
    fprintf(fp, "<%s>", "ns1:getCLConclusion");
    fprintf(fp, "<fpsSessionID>%s</fpsSessionID>", SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp, "<shopNo>%s</shopNo>", QShop_no.toAscii().data());
    fprintf(fp, "<terminalID>%s</terminalID>", cmachineID);
    fprintf(fp, "<existingRCNumber>%s</existingRCNumber>",Rcno.toAscii().data());
    fprintf(fp, "<totAmount>%s</totAmount>", ui->lblTotalAmtVal_2->text().toAscii().data());
    fprintf(fp, "<uidNo>%s</uidNo>", MemberUID);
    fprintf(fp, "<tempRRN>%s</tempRRN>", TempRRN);
    //fprintf(fp, "<bankRespCode>%s</bankRespCode>", dumpresp);
    fprintf(fp, "<bankRespStatus>%s</bankRespStatus>", BankRespStatus);
    fprintf(fp, "<password>%s</password>", HD_PASSWORD);//"11169d3abc20724bb857cc5ea59e3c42" );
    fprintf(fp, "<hts>%s</hts>", HD_HTS);
    fprintf(fp, "</%s>", "ns1:getCLConclusion");
    fprintf(fp, "</%s>", "SOAP-ENV:Body");
    fprintf(fp, "</%s>", "SOAP-ENV:Envelope");

    fclose(fp);
}


int Widget::CL_ConclusionToServer(char *ReqFile)
{
    GPRS objGprs;

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें."));

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    if(objGprs.Check_Network_Available() < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();

        return -1;
    }
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

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", ReqFile, cUrlName,  "/mnt/jffs2/CL_ConcluseResp.xml", SERVER_TIME_OUT);

    qDebug()<<"\nCurl Cmd : "<<cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();

        return -1;
    }

    qDebug() << "**********CL_ConcluseResp.xml*********";
    system("cat /mnt/jffs2/CL_ConcluseResp.xml");
    qDebug() << "***********************************************";

    dlgPopup1->close();
    dlgPopup1->deleteLater();

    if(ParsingFirstCondition("/mnt/jffs2/CL_ConcluseResp.xml") < 0)
        return -1;

    return 0;
}

void Widget::on_btnEKYC_Back_clicked()
{
    ui->ConsentFormcheckBox->setChecked(true);
    ui->stackedWidget->setCurrentIndex(5);
}

void Widget::on_btnEKYC_FP_clicked()
{
    iConsentFormBackFlag=3;
    ui->ConsentCheckBox->setChecked(true);
    ConsentForm();
    ui->stackedWidget->setCurrentIndex(21);
}

//void Widget::on_btnEKYC_FP_clicked()
//{
//    if(!ui->ConsentFormcheckBox_5->isChecked())
//    {
//        //        miscObj.DisplayWideMsgBox((char *)"Please accept consent form");
//        if(LangMode == 1)
//            miscObj.DisplayWideMsgBox((char *)"Please accept consent form");
//        else if(LangMode == 2)
//            miscObj.tel_DisplayWideMsgBox("कृपया सहमति प्रपत्र स्वीकार करें");

//        QString ConsentReasonStr = "0-N";
//        if(SendTxnCancelReason(ConsentReasonStr,'C') < 0)
//            return;
//        return;
//    }

//    if(miscObj.validate_uid(ui->ldtEKYC_UID_Entered->text().toAscii().data()) < 0){
//        qDebug()<<"Invalid UID for EKYC";
//        if(LangMode==1)
//            miscObj.DisplayMsgBox((char *)"Invalid UID");
//        if(LangMode==2)
//            miscObj.tel_DisplayMsgBox((char *)"अमान्य यूआईडी");
//        return;
//    }

//    ui->ChkBox_Y_POI_POA->setChecked(true);
//    ui->ChkBox_N_POI_POA->setChecked(false);

//    ui->ChkBox_Y_Mob_eMail->setChecked(true);
//    ui->ChkBox_N_Mob_eMail->setChecked(false);

//    rsaEncryption(ui->ldtEKYC_UID_Entered->text().toAscii().data(),skey);

//    EKYC_TEST_FLAG = 1;

//    EKYC_FP_Submit();

//    EKYC_TEST_FLAG = 0;
//}

/************************************************************************
              ADDED BY VIVEK
*************************************************************************/

/*@ Description: It will update app automatically using webservices*/

QString Widget::checkAppUpdate(char *filename)
{
    QString QversionStatus;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In checkAppUpdate, Error Reading File";
        return "-1";
    }

    if(file.size() == 0)
        return "-1";

    QXmlStreamReader xml(&file);

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "versionStatus")
            {
                xml.readNext();
                QversionStatus = xml.text().toString();
                qDebug() << "QversionStatus : " << QversionStatus;
            }
        }
    }

    return QversionStatus;

}

int Widget::getEkyc_MemberID(char *memberID)
{
    qDebug() << "getEkyc_MemberID MemberID : " << MemberUID;
    QSqlQuery query;
    query.prepare("SELECT MemberId FROM CustomerFamily WHERE MemberUID = ?");
    query.addBindValue(MemberUID);
    if(!query.exec())
        qDebug() << "getEkyc_MemberID"<<query.lastError();
    else{
        while(query.next())
        {
            memberID = query.value(0).toString().toAscii().data();
        }
    }
    return 0;
}


void Widget::on_btn_Back_inspectorDtls_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);

}

void Widget::on_btn_FPAuth_inspectorDtls_clicked()
{
    if(!ui->ConsentFormcheckBox_6->isChecked())
    {
        //        miscObj.DisplayWideMsgBox((char *)"Please accept consent form");
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please accept consent form");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox("कृपया सहमति प्रपत्र स्वीकार करें");

        QString ConsentReasonStr = "0-N";
        if(SendTxnCancelReason(ConsentReasonStr,'C') < 0)
            return;
        return;
    }

    qDebug()<<"Button Next Clicked in Inspector Details";
    CMisc objMisc;
    int iRowCount = 0;
    int i = 0;

    iRowCount = ui->tableWidet_InspectorDtls->rowCount();

    qDebug()<<"No Of Row in tableWidet_InspectorDtls = "<<iRowCount;
    for(i = 0; i < iRowCount; i++)
    {
        qDebug()<< "inside for >>>> i ===>" <<i;
        if(ui->tableWidet_InspectorDtls->item(i,0)->isSelected() == true)
        {
            break;
        }
    }
    qDebug()<< "i ===> "<< i << "iRowcount ===>" <<iRowCount;
    if(i == iRowCount)
    {
        objMisc.DisplayMsgBox((char *)"Please Select Incharge/Nominee");

        return ;
    }

    int row = 0;
    row = ui->tableWidet_InspectorDtls->currentRow();
    qDebug()<<"***********Current Row No = "<<row;

    //Clear Buffer
    memset(cInspector_UID,0, sizeof(cInspector_UID));
    strcpy(cInspector_UID , ui->tableWidet_InspectorDtls->item(row, 5)->text().toAscii().data());
    qDebug()<<"Incharge UID =  "<< cInspector_UID;

    return;
}

int Widget::showInspInchargeDetails()
{
    QSqlQuery query;
    int tableCount = 0;
    query.clear();
    query.prepare("SELECT COUNT(incharge_name) FROM InspList");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            tableCount = query.value(0).toInt(0);
        }
    }
    if(tableCount == 0)
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Inspector Details is not registered at AePDS server...");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"इंस्पेक्टर विवरण सर्वर पर पंजीकृत नहीं हैं");
        return -1;
    }
    query.clear();
    query.prepare("SELECT COUNT(insp_nom_name) FROM InspNominee");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            tableCount = query.value(0).toInt(0);
        }
    }
    \
    ui->tableWidet_InspectorDtls->showMaximized();
    ui->tableWidet_InspectorDtls->clearContents();
    ui->tableWidet_InspectorDtls->setRowCount(tableCount+1); //Dealer + Nominee
    char buff[128];
    int row = 0;
    query.clear();
    query.prepare("SELECT incharge_name, incharge_uid, insp_incharge_bfd1, insp_incharge_bfd2, insp_incharge_bfd3 FROM InspList");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(0).toString().toAscii().data());
            qDebug() << "0 incharge_name = " << buff;
            ui->tableWidet_InspectorDtls->setItem(row, 0, new QTableWidgetItem(buff));
            ui->tableWidet_InspectorDtls->item(row, 0)->setFlags( ui->tableWidet_InspectorDtls->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidet_InspectorDtls->item(row, 0)->setFlags( ui->tableWidet_InspectorDtls->item(row,0)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(1).toString().toAscii().data());

            qDebug() << "1 Masked incharge_uid = " << buff;
            ui->tableWidet_InspectorDtls->setItem(row, 1, new QTableWidgetItem(buff));
            ui->tableWidet_InspectorDtls->item(row, 1)->setFlags( ui->tableWidet_InspectorDtls->item(row,1)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidet_InspectorDtls->item(row, 1)->setFlags( ui->tableWidet_InspectorDtls->item(row,1)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(2).toString().toAscii().data());
            qDebug() << "2 insp_incharge_bfd1  = " << buff;
            ui->tableWidet_InspectorDtls->setItem(row, 2, new QTableWidgetItem(buff));
            ui->tableWidet_InspectorDtls->item(row, 2)->setFlags( ui->tableWidet_InspectorDtls->item(row,2)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidet_InspectorDtls->item(row, 2)->setFlags( ui->tableWidet_InspectorDtls->item(row,2)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(3).toString().toAscii().data());
            qDebug() << "3 insp_incharge_bfd2  = " << buff;
            ui->tableWidet_InspectorDtls->setItem(row, 3, new QTableWidgetItem(buff));
            ui->tableWidet_InspectorDtls->item(row, 3)->setFlags( ui->tableWidet_InspectorDtls->item(row,3)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidet_InspectorDtls->item(row, 3)->setFlags( ui->tableWidet_InspectorDtls->item(row,3)->flags() | Qt::ItemIsSelectable);

            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(4).toString().toAscii().data());
            qDebug() << "4 insp_incharge_bfd3  = " << buff;
            ui->tableWidet_InspectorDtls->setItem(row, 4, new QTableWidgetItem(buff));
            ui->tableWidet_InspectorDtls->item(row, 4)->setFlags( ui->tableWidet_InspectorDtls->item(row,4)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidet_InspectorDtls->item(row, 4)->setFlags( ui->tableWidet_InspectorDtls->item(row,4)->flags() | Qt::ItemIsSelectable);


            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(1).toString().toAscii().data());
            qDebug() << "5 incharge_uid = " << buff;
            ui->tableWidet_InspectorDtls->setItem(row, 5, new QTableWidgetItem(buff));
            ui->tableWidet_InspectorDtls->item(row, 5)->setFlags( ui->tableWidet_InspectorDtls->item(row,5)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidet_InspectorDtls->item(row, 5)->setFlags( ui->tableWidet_InspectorDtls->item(row,5)->flags() | Qt::ItemIsSelectable);

            row++;
        }
    }
    if(tableCount != 0)
    {
        query.clear();
        query.prepare("SELECT insp_nom_name, insp_nom_uid, insp_nom_bfd1, insp_nom_bfd2, insp_nom_bfd3 FROM InspNominee");
        if(!query.exec())
            qDebug() << query.lastError();
        else
        {
            while(query.next())
            {
                memset(buff,0,sizeof(buff));
                strcpy(buff, query.value(0).toString().toAscii().data());
                qDebug() << "0 insp_nom_name = " << buff;
                ui->tableWidet_InspectorDtls->setItem(row, 0, new QTableWidgetItem(buff));
                ui->tableWidet_InspectorDtls->item(row, 0)->setFlags( ui->tableWidet_InspectorDtls->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
                ui->tableWidet_InspectorDtls->item(row, 0)->setFlags( ui->tableWidet_InspectorDtls->item(row,0)->flags() | Qt::ItemIsSelectable);

                memset(buff,0,sizeof(buff));
                strcpy(buff, query.value(1).toString().toAscii().data());

                //memset(buff, 'x', 8); //cmt on 04/12//2017

                qDebug() << "1 insp_nom_uid (MASKED) = " << buff;
                ui->tableWidet_InspectorDtls->setItem(row, 1, new QTableWidgetItem(buff));
                ui->tableWidet_InspectorDtls->item(row, 1)->setFlags( ui->tableWidet_InspectorDtls->item(row,1)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
                ui->tableWidet_InspectorDtls->item(row, 1)->setFlags( ui->tableWidet_InspectorDtls->item(row,1)->flags() | Qt::ItemIsSelectable);

                memset(buff,0,sizeof(buff));
                strcpy(buff, query.value(2).toString().toAscii().data());
                qDebug() << "2 insp_nom_bfd1 = " << buff;
                ui->tableWidet_InspectorDtls->setItem(row, 2, new QTableWidgetItem(buff));
                ui->tableWidet_InspectorDtls->item(row, 2)->setFlags( ui->tableWidet_InspectorDtls->item(row,2)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
                ui->tableWidet_InspectorDtls->item(row, 2)->setFlags( ui->tableWidet_InspectorDtls->item(row,2)->flags() | Qt::ItemIsSelectable);

                memset(buff,0,sizeof(buff));
                strcpy(buff, query.value(3).toString().toAscii().data());
                qDebug() << "3 insp_nom_bfd2 = " << buff;
                ui->tableWidet_InspectorDtls->setItem(row, 3, new QTableWidgetItem(buff));
                ui->tableWidet_InspectorDtls->item(row, 3)->setFlags( ui->tableWidet_InspectorDtls->item(row,3)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
                ui->tableWidet_InspectorDtls->item(row, 3)->setFlags( ui->tableWidet_InspectorDtls->item(row,3)->flags() | Qt::ItemIsSelectable);

                memset(buff,0,sizeof(buff));
                strcpy(buff, query.value(4).toString().toAscii().data());
                qDebug() << "4 insp_nom_bfd3 = " << buff;
                ui->tableWidet_InspectorDtls->setItem(row, 4, new QTableWidgetItem(buff));
                ui->tableWidet_InspectorDtls->item(row, 4)->setFlags( ui->tableWidet_InspectorDtls->item(row,4)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
                ui->tableWidet_InspectorDtls->item(row, 4)->setFlags( ui->tableWidet_InspectorDtls->item(row,4)->flags() | Qt::ItemIsSelectable);

                memset(buff,0,sizeof(buff));
                strcpy(buff, query.value(1).toString().toAscii().data());
                qDebug() << "1 insp_nom_uid = " << buff;
                ui->tableWidet_InspectorDtls->setItem(row, 5, new QTableWidgetItem(buff));
                ui->tableWidet_InspectorDtls->item(row, 5)->setFlags( ui->tableWidet_InspectorDtls->item(row,5)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
                ui->tableWidet_InspectorDtls->item(row, 5)->setFlags( ui->tableWidet_InspectorDtls->item(row,5)->flags() | Qt::ItemIsSelectable);
                row++;

            }
        }
    }
    ui->tableWidet_InspectorDtls->setColumnWidth(0,180);
    ui->tableWidet_InspectorDtls->setColumnWidth(1,160);

    ui->tableWidet_InspectorDtls->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidet_InspectorDtls->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidet_InspectorDtls->verticalHeader()->hide();
    ui->tableWidet_InspectorDtls->selectRow(0);

    ui->tableWidet_InspectorDtls->setColumnHidden(2, true);
    ui->tableWidet_InspectorDtls->setColumnHidden(3, true);
    ui->tableWidet_InspectorDtls->setColumnHidden(4, true);
    ui->tableWidet_InspectorDtls->setColumnHidden(5, true);

    return 0;
}

void Widget::on_radioButton_RCNOFLAG_clicked(bool checked)
{
    char Distofficecode[32]="";
    memset(Distofficecode,0,sizeof(Distofficecode));

    QRegExp exp("([0-9]\\d{0,25})");
    QValidator *validator = new QRegExpValidator(exp,this);

    //Set Validator
    ui->ldtRationCard->setValidator(validator);
    ui->ldtRationCard->setEchoMode(QLineEdit::Normal);

    database_queries qry;
    qry.getdistofficecode(Distofficecode);
    ui->ldtRationCard->setText(Distofficecode);

    ui->ldtRationCard->clear();
    ui->ldtRationCard->setReadOnly(0);
    if(checked == true)
    {
        if(LangMode == 1)
            ui->lblRationCard->setText("Card No.");
        else if(LangMode == 2)
            ui->lblRationCard->setText(QString::fromUtf8("कार्ड नंबर"));

        /*cidType Initialization*/
        cidType = "R";
        ui->radioButton_UIDFLAG->setChecked(false);
        ui->ldtRationCard->setFocus();
    }
}

void Widget::on_radioButton_UIDFLAG_clicked(bool checked)
{
    QRegExp exp("([0-9]\\d{0,11})");
    QValidator *validator = new QRegExpValidator(exp,this);

    //Set Validator
    ui->ldtRationCard->setValidator(validator);
    ui->ldtRationCard->setEchoMode(QLineEdit::Password);

    ui->ldtRationCard->setReadOnly(0);
    ui->ldtRationCard->clear();

    if(checked == true)
    {
        if(LangMode==1)
            ui->lblRationCard->setText("Aadhaar No.");

        /*cidType Initialization*/
        cidType = "U";
        //ui->radioButton_RCNOFLAG->setChecked(false);
        ui->ldtRationCard->setFocus();
    }
}

int Widget::CreateMd5checksumFile(char *checksumNo)
{
    system("rm /mnt/jffs2/md5checksum.txt");
    QFile file("/mnt/jffs2/md5checksum.txt");
    QTextStream stream(&file);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        stream << checksumNo << " " << "/root/app.tar.bz2" << endl;
    }

    system("cat /mnt/jffs2/md5checksum.txt");
    return 0;
}

/* Recieve Goods Module Added by vivek 27062017 */

int Widget::prepareStockDetailsReq(char *FileName, char *cfpscode, char *cpassword)
{
    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));
    miscObj.GetMachineId(cmachineID);

    QFile file(FileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug()<< FileName << " file Open failed";
        return -1;
    }

    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL)
    {
        qDebug() << "File Not Found" << endl;
        return -1;
    }

    fprintf(fp, "%s", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    fprintf(fp, "%s","<soapenv:Envelope xmlns:soapenv=\" http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ser=\" http://services.nic.com/\">");
    fprintf(fp, "<%s>","<soapenv:Header/>");
    fprintf(fp, "<%s>","<soapenv:Body>");
    fprintf(fp, "<%s>","<ser:receiveStockAtPOS>");
    fprintf(fp,"<fps_Id>%s</fps_Id>",cfpscode);
    fprintf(fp,"<terminal_Id>%s</terminal_Id>",cmachineID);
    fprintf(fp,"<fpsSessionId>%s</fpsSessionId>",SessionId);
    fprintf(fp,"<stateCode>%s</stateCode>", stateCode.toAscii().data());
    fprintf(fp,"<password>%s</password>",cpassword);
    fprintf(fp,"%s","</ser:receiveStockAtPOS>");
    fprintf(fp,"%s","</soapenv:Body>");
    fprintf(fp,"%s","</soapenv:Envelope>");
    fclose(fp);

    return 0;
}

int Widget::ConvertStockDetailsRespToJson(char *filename)
{
    QString QStockJsonresponse;
    QString StockRcvJsonFileName = "/mnt/jffs2/StockDetailsRespJson.txt";

    QFile file(filename);
    QFile StockRcvJsonFile(StockRcvJsonFileName);

    QTextStream outstream(&StockRcvJsonFile);

    system("rm StockRecieve_JsonResp.txt");

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
            if(xml.name() == "ApiTofetchFpsTruckChallandatafromResult")
            {
                xml.readNext();
                QStockJsonresponse = xml.text().toString();
                qDebug() << "approveKey : " << QStockJsonresponse;
            }
        }
    }
    if(!StockRcvJsonFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "File not open";
    }
    outstream << QStockJsonresponse ; //Wite data to the file
    StockRcvJsonFile.close();
}

int Widget::SetLineEditValidation()  //added by vivek 01072017
{
    /*Set Format for Validation. Below expression "exp" set Valiidator to Accept only number and range is 0-11 numbers*/

    QRegExp exp("([0-9]\\d{0,11})");
    QValidator *validator = new QRegExpValidator(exp,this);

    //Set Validator
    ui->lEdit_InpsAuth->setValidator(validator);
}

/*Mobile update Added by vivek 01082017 */
QString Widget::checkMobileUpdate(char *filename)
{
    QString QMobileUpdateStatus;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In checkAppUpdate, Error Reading File";
        return "-1";
    }

    if(file.size() == 0)
        return "-1";

    QXmlStreamReader xml(&file);

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "mobileNoUpdate")
            {
                xml.readNext();
                QMobileUpdateStatus = xml.text().toString();
                qDebug() << "QMobileUpdateStatus : " << QMobileUpdateStatus;
            }
        }
    }

    return QMobileUpdateStatus;

}

int Widget::UpdateMobileNumber( char *MobileNo)
{
    GPRS objGprs;
    if(objGprs.Check_Network_Available() < 0)
        return -1;
    int ret = 0;
    popup_msg *dlgPopup2 = new popup_msg(this);

    if(LangMode == 1)
        dlgPopup2->ui->lblPopup->setText("Please Wait... \nMobile Number is Updating");
    else if(LangMode == 2)
        dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें ... \nमोबाइल नंबर अपडेट हो रहा है"));

    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();


    char FileName[64];
    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/MobileUpdate_Req.xml");

    system("rm /mnt/jffs2/MobileUpdate_Req.xml");
    system("rm /mnt/jffs2/MobileUpdate_Res.xml");

    ret = prepareUpdateMobileNoReq(FileName, MobileNo);
    qDebug()<<"ret form Prepare Out File : "<<ret;

    char catCmd[128];
    memset(catCmd, 0, sizeof(catCmd));
    sprintf(catCmd, "cat %s", FileName);
    system(catCmd);

    char cUrlName[256];
    memset(cUrlName, 0, sizeof(cUrlName));
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

    sleep(2); //Kept For Audio
    char cmdCurl[1024];
    memset(cmdCurl, 0, sizeof(cmdCurl));

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/MobileUpdate_Res.xml", responseTimedOutTimeInSec.toInt());

    qDebug() << "\nCurl Cmd : " << cmdCurl;

    int sysret = system(cmdCurl);
    qDebug() << "sysret = " << sysret;

    if(SystemCurlResponse(sysret, EPDS_SERVER) < 0)
    {
        dlgPopup2->close();
        dlgPopup2->deleteLater();

        return -1;
    }

    dlgPopup2->close();
    dlgPopup2->deleteLater();

    qDebug() << "***********/mnt/jffs2/MobileUpdate_Res.xml************";
    system("cat /mnt/jffs2/MobileUpdate_Res.xml");
    qDebug() << "***************************************************";


    if(ParsingFirstCondition((char *)"/mnt/jffs2/MobileUpdate_Res.xml") != 0)
    {
        return -1;
    }else
    {
        miscObj.DisplayWideMsgBox((char *)MobileStatusMessage.toAscii().data());
    }

    return 0;
}

int Widget::prepareUpdateMobileNoReq(char *FileName, char *cMobileNo)
{
    qDebug()<<"Out File (" <<FileName << ") Preperation ....";

    QSqlQuery query;
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

    char RcId[36];
    query.prepare("select RcId from CustomerMaster where RationCardId=?");
    query.addBindValue(Rcno.toAscii().data());
    query.exec();

    while(query.next())
    {
        memset(RcId,0,sizeof(RcId));
        strcpy(RcId, query.value(0).toString().toAscii().data());
        qDebug()<<"RcId = "<<RcId;
    }
    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL)
    {
        qDebug() << "File Not Found" << endl;
        return -1;
    }

    fprintf(fp, "%s","<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    fprintf(fp, "%s", "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://service.fetch.rationcard/\">");
    fprintf(fp, "<%s>", "SOAP-ENV:Body");
    fprintf(fp, "<%s>", "ns1:mobileNOBeneficiaryUpdate");
    fprintf(fp, "<fpsId>%s</fpsId>", szShopId);
    fprintf(fp, "<fpsSessionId>%s</fpsSessionId>", SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp, "<password>%s</password>", HD_PASSWORD);
    fprintf(fp, "<existingRCNumber>%s</existingRCNumber>",Rcno.toAscii().data());
    fprintf(fp, "<rcId>%s</rcId>", RcId);
    fprintf(fp, "<mobileNo>%s</mobileNo>", cMobileNo);
    fprintf(fp, "</%s>", "ns1:mobileNOBeneficiaryUpdate");
    fprintf(fp, "</%s>", "SOAP-ENV:Body");
    fprintf(fp, "</%s>", "SOAP-ENV:Envelope");
    fclose(fp);

    return 0;
}

void Widget::on_tableWidgetStockTable_3_cellChanged(int row, int column)
{
    char buf1[625]="";

    if(column == 5)
    {
        qDebug() << "Row = " << row;
        qDebug() << "Column = " << column;

        qDebug() << "Dispatched Qty = " << ui->tableWidgetStockTable_3->item(row, 4)->text().toFloat(0);

        float ReleasedQty = 0.00;

        ReleasedQty = ui->tableWidgetStockTable_3->item(row,4)->text().toFloat(0);
        qDebug() << "ReleasedQty = " << ReleasedQty;

        memset(buf1,0,sizeof(buf1));
        strcpy(buf1,ui->tableWidgetStockTable_3->item(row,column)->text().toAscii().data());
        qDebug()<<" ENTERD qty"<<buf1;

        for(int i=0;i<strlen(buf1);i++)
        {
            if(buf1[i] != '.' && buf1[i] != '0' && buf1[i] != '1' && buf1[i] != '2' && buf1[i] != '3' && buf1[i] != '4' && buf1[i] != '5' && buf1[i] != '6' && buf1[i] != '7' && buf1[i] != '8' && buf1[i] != '9')
            {
                ui->tableWidgetStockTable_3->item(row,column)->setText("0.00");
            }
        }
        if(ui->tableWidgetStockTable_3->item(row, column)->text().toFloat(0) > ui->tableWidgetStockTable_3->item(row, 4)->text().toFloat(0))
        {
            if(LangMode ==1)
                miscObj.DisplayWideMsgBox((char *)"Please Receive the Qty upto ReleasedQty only");
            else if(LangMode ==2)
                miscObj.tel_DisplayWideMsgBox((char *)"कृपया रिलीज़ क्विटी तक मात्रा प्राप्त करें");

            ui->tableWidgetStockTable_3->item(row, column)->setText("0");

            return;
        }
        qDebug() << "RO - Deliverd" << (ui->tableWidgetStockTable_3->item(row, 4)->text().toFloat(0) - ui->tableWidgetStockTable_3->item(row, column)->text().toFloat(0));
        //Negative Value Check
        if(ui->tableWidgetStockTable_3->item(row, column)->text().toFloat(0) < 0)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Please Enter Quantity greater than zero");
            else if(LangMode==2)
                miscObj.tel_DisplayWideMsgBox((char *)"कृपया शून्य से अधिक मात्रा दर्ज करें");

            ui->tableWidgetStockTable_3->item(row, column)->setText("0");
            return;
        }

    }
    return;
}

int Widget::pop_up(char *str)
{
    popup_msg *objPopup = new popup_msg(this);

    objPopup->ui->lblPopup->setText(QString::fromUtf8(str));

    objPopup->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    objPopup->close();
    objPopup->deleteLater();
    return 0;
}

void Widget::on_btnpoweroff_clicked()
{
    popup_msg *dlgPopup2 = new popup_msg(this);
    database_queries dbqry;
    int LangMode = dbqry.getLangMode();

    if(LangMode == 1)
        dlgPopup2->ui->lblPopup->setText("Logout ... \n Please wait ...");

    dlgPopup2->show();

    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    gl11_audio_power_off();

    gl11_iodev_close();

    dlgPopup2->close();
    dlgPopup2->deleteLater();

    kill_app();
    gl11_host_pwroff();
    system("poweroff");
    sleep(3);
    qApp->quit();
    exit(0);

}

void Widget::kill_app()
{
    char p_id[16]="";

    memset(p_id,0,sizeof(p_id));

    system("pidof mhpds > /mnt/jffs2/pidofmhpds");
    int fd9 = ::open("/mnt/jffs2/pidofmhpds",O_RDONLY);
    {
        int kee = read(fd9,p_id,6);
        qDebug() << "kee = " << kee<<p_id;
        qDebug() << "pidofmhpds = " <<p_id;
        memset(p_id,0,strlen(p_id));
        p_id[kee] = '\0';
        ::close(fd9);
    }
}

//RD-2.0 for finger print

int Widget::rdCallinit(int fpCount )
{
    qDebug()<<"Clicked FOR RD 2.0";
    int   ret=0,count=0;
    int sockfd=0,res_sys=0;
    fp_capture capture;

    memset(&rdStatus,0,sizeof(struct dev_discovery));
    memset(&dev_info,0,sizeof(struct device_info));
    memset(&pid_info,0,sizeof(struct pid_data));
    memset(&capture,0,sizeof(struct rd_capture));

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.."));

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    qDebug()<<"Connecting to RD Service....Please Wait";

    while(1)
    {

        res_sys = system("ps | grep /home/rdservice/rds > /mnt/jffs2/pid.txt");

        QFile file("/mnt/jffs2/pid.txt");
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "pid.txt File Not Found";
            count++;
            if(count == 5)
            {
                dlgPopup1->close();
                dlgPopup1->deleteLater();
                return -1;
            }
            continue;
        }

        if(file.size() == 0)
        {
            qDebug()<<"RD Service Not Started..........";
            sleep(3);
            count++;
            if(count == 5)
            {
                dlgPopup1->close();
                dlgPopup1->deleteLater();
                CMisc misc;
                if(LangMode==1)
                    misc.DisplayWideMsgBox((char *)"RD Service Not Started ...");
                else if(LangMode==2)
                    misc.tel_DisplayWideMsgBox((char *)"RD सेवा शुरू नहीं हुई");

                return -1;
            }
            continue;
        }
        else
        {
            qDebug()<<"---------------------------RD Service Connection success----------------------";
            break;
        }
    }
    count=0;

    while(1)
    {
        res_sys = rd_service_discovery(sockfd,&rdStatus);
        qDebug()<<"RES1 :"<<res_sys;

        if(res_sys < 0)
        {
            qDebug()<<"Trying to connect rd_service_discovery"<<endl;
            sleep(2);
            count++;
            if(count == 5)
            {

                dlgPopup1->close();
                dlgPopup1->deleteLater();

                CMisc misc;
                if(LangMode==1)
                    misc.DisplayWideMsgBox((char *)"RD service discovery Failed");
                else if(LangMode==2)
                    misc.tel_DisplayWideMsgBox((char *)"RD सर्विस की जाँच असफल हुआ");
                return -1;
            }
            continue;
        }
        else if(res_sys==0)
        {
            //rdStatus.info
            //

            qDebug()<<"1. rdStatus.status = "<<rdStatus.status<<endl;
            qDebug()<<"2 .rdStatus.info ="<<rdStatus.info<<endl;
            qDebug()<<"3 .rdStatus.cPath="<<rdStatus.cPath<<endl;
            qDebug()<<"4 .rdStatus.dPath ="<<rdStatus.dPath<<endl;
            break;
        }
    }
    dlgPopup1->close();
    dlgPopup1->deleteLater();

    CMisc miscObj;
    miscObj.AudioTest((char *)"32");

    popup_msg *dlgPopup2 = new popup_msg(this);

    if(fpCount==1)
    {
        if(LangMode == 1)
            dlgPopup2->ui->lblPopup->setText("Please Place Finger On Scanner");
        else if(LangMode == 2)
            dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया उंगली स्कैनर पर रखें"));
    }
    else
    {
        if(LangMode == 1)
            dlgPopup2->ui->lblPopup->setText("Please place two fingers on the \n scanner one after the other");
        else if(LangMode == 2)
            dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया एक के बाद एक दो उंगलियों को \n स्कैनर पर रखें"));
    }

    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    capture.Fp_Count = fpCount;
    sprintf(capture.FP_Type,"%s","0");		//	0 - FMR
    sprintf(capture.PID_Type,"%s","0");		//	0 - XML, 1 - ProtoBuf
    sprintf(capture.pidVer,"%s","2.0");
    sprintf(capture.env,"%s","P");                 //      PP - Preprod(20171105), P - Prod(20191231)
    capture.Timeout = 30000;//30 sec

    memset(&pid_info,0,sizeof(struct pid_data));
    pid_info.pid = NULL;
    res_sys = rd_fp_capture(sockfd,&capture,&pid_info);

    /*char  *Ret = "";
    rd_fp_capture_buffer(sockfd,&capture,&Ret);
    qDebug()<<"Ret:"<<Ret;*/

    qDebug()<<"RES3 :"<<res_sys;
    if(res_sys < 0)
    {
        qDebug()<<"rd_fp_capture -API FAILED";
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        return -1;
    }
    if(res_sys == 0)
    {
        qDebug()<<"err_code="<<pid_info.err_code<<endl;
        qDebug()<<"err_info ="<<pid_info.err_info<<endl;

        /*qDebug()<<"fCount ="<<pid_info.fCount<<endl;
        qDebug()<<"fType ="<<pid_info.fType<<endl;
        qDebug()<<"iCount ="<<pid_info.iCount<<endl;
        qDebug()<<"pCount ="<<pid_info.pCount<<endl;
        qDebug()<<"pType ="<<pid_info.pType<<endl;*/

        if(strcmp(pid_info.err_code,"0") != 0)
        {
            CMisc misc;
            char errmessag[120]="";
            memset(errmessag,0,sizeof(errmessag));
            sprintf(errmessag,"%s-ERROR-%s",pid_info.err_info,pid_info.err_code);

            misc.DisplayWideMsgBox(errmessag);

            dlgPopup2->close();
            dlgPopup2->deleteLater();
            return -1;
        }
    }
    if(pid_info.pid != NULL)
    {
        /*qDebug()<<"nmPoints="<<pid_info.nmPoints<<endl;
        qDebug()<<"qScore="<<pid_info.qScore<<endl;
        qDebug()<<"ci="<<pid_info.ci<<endl;
        qDebug()<<"Skey="<<pid_info.Skey<<endl;
        qDebug()<<"Hmac="<<pid_info.Hmac<<endl;
        qDebug()<<"pid type="<<pid_info.type<<endl;
        qDebug()<<"pid ="<<pid_info.pid<<endl;
        qDebug()<<"dc ="<<pid_info.dev_data.dc<<endl;
        qDebug()<<"mi ="<<pid_info.dev_data.mi<<endl;
        qDebug()<<"mc ="<<pid_info.dev_data.mc<<endl;
        qDebug()<<"dpId ="<<pid_info.dev_data.dpId<<endl;
        qDebug()<<"rdsId ="<<pid_info.dev_data.rdsId<<endl;
        qDebug()<<"rdsVer ="<<pid_info.dev_data.rdsVer<<endl;*/
    }

    memset(&dev_details,0,sizeof(struct device_data));
    ret = get_device_details(&dev_details);
    if(ret == 0)
    {
        qDebug()<<"Scanner ID:"<<dev_details.scanner_id<<endl;
        qDebug()<<"Device ID:"<<dev_details.pinpad_id<<endl;
        qDebug()<<"Device MAC ID:"<<dev_details.mac_id<<endl;

    }

    dlgPopup2->close();
    dlgPopup2->deleteLater();

    return 0;
}


/********************** get EYC RD-2.0 ***************************************/

int Widget::rdCallinit_ekyc(int fpCount,char *wadh )
{
    int ret=0;
    int sockfd=0;

    memset(&dev_info,0,sizeof(struct device_info));
    memset(&pid_info,0,sizeof(struct pid_data));

    unsigned char xmlData[500]="", dataBuffer[500]="";
    char *resp_buffer = NULL;

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.."));

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    qDebug()<<"Connecting to RD Service....Please Wait";


    dlgPopup1->close();
    dlgPopup1->deleteLater();


    CMisc miscObj;
    miscObj.AudioTest((char *)"32");

    popup_msg *dlgPopup2 = new popup_msg(this);

    if(fpCount==1)
    {
        if(LangMode == 1)
            dlgPopup2->ui->lblPopup->setText("Please Place Finger On Scanner");
        else if(LangMode == 2)
            dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया उंगली स्कैनर पर रखें"));
    }
    else
    {
        if(LangMode == 1)
            dlgPopup2->ui->lblPopup->setText("Please Place 2 Finger On Scanner");
        else
            dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया दूसरा उंगली स्कैनर पर रखें.."));
    }

    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();


    memset(xmlData,0,strlen((const char *)xmlData));
    memset(dataBuffer,0,strlen((const char *)dataBuffer));

    sprintf((char *)dataBuffer,"<PidOptions ver=\"1.0\"><Opts fCount=\"1\" fType=\"0\" env=\"P\" format=\"0\" pidVer=\"2.0\" timeout=\"10000\" wadh=\"%s\"/><Demo></Demo><CustOpts></CustOpts><Bios></Bios></PidOptions>\n",wadh);

    sprintf((char *)xmlData,"CAPTURE /rd/capture HTTP/1.1\nAccept: text/xml\nContent-Type: text/xml\nUser-Agent: axios/0.15.3\nHOST: 127.0.0.1:11100\nConnection: close\nContent-Length:%d\n%s",strlen((const char *)dataBuffer),dataBuffer);


    ret = rd_fp_capture_xml(sockfd,xmlData,&resp_buffer);

    if(ret == -1)
    {
        qDebug() << "rd_fp_capture failed.... \n";
        return -1;
    }
    if(ret == 0)
    {
        ret = Parse_RDService_Pidblock(&pid_info,(unsigned char*)resp_buffer);
        if(ret != 0)
        {
            miscObj.DisplayWideMsgBox((char *)"Invalid XML");
            return -1;
        }
        qDebug()<<"err_code ="<<pid_info.err_code<<endl;
        qDebug()<<"err_info ="<<pid_info.err_info<<endl;
        qDebug()<<"fCount ="<<pid_info.fCount<<endl;
        qDebug()<<"fType ="<<pid_info.fType<<endl;
        qDebug()<<"iCount ="<<pid_info.iCount<<endl;
        qDebug()<<"iType ="<<pid_info.iType<<endl;
        qDebug()<<"pCount ="<<pid_info.pCount<<endl;
        qDebug()<<"pType ="<<pid_info.pType<<endl;

        if(strcmp(pid_info.err_code,"0") != 0)
        {
            CMisc misc;
            char errmessag[120]="";
            memset(errmessag,0,sizeof(errmessag));
            sprintf(errmessag,"%s-ERROR-%s",pid_info.err_info,pid_info.err_code);

            misc.DisplayWideMsgBox(errmessag);

            dlgPopup2->close();
            dlgPopup2->deleteLater();
            return -1;

        }
        if(pid_info.pid != NULL)
        {
            qDebug()<<"nmPoints ="<<pid_info.nmPoints<<endl;
            qDebug()<<"qScore ="<<pid_info.qScore<<endl;
            qDebug()<<"ci ="<<pid_info.ci<<endl;
            qDebug()<<"Skey ="<<pid_info.Skey<<endl;
            qDebug()<<"Hmac ="<<pid_info.Hmac<<endl;
            qDebug()<<"pid type ="<<pid_info.type<<endl;
            qDebug()<<"pid ="<<pid_info.pid<<endl;
            qDebug()<<"dc ="<<pid_info.dev_data.dc<<endl;
            qDebug()<<"mi ="<<pid_info.dev_data.mi<<endl;
            qDebug()<<"mc ="<<pid_info.dev_data.mc<<endl;
            qDebug()<<"dpId ="<<pid_info.dev_data.dpId<<endl;
            qDebug()<<"rdsId ="<<pid_info.dev_data.rdsId<<endl;
            qDebug()<<"rdsVer = "<<pid_info.dev_data.rdsVer<<endl;

        }

    }

    dlgPopup2->close();
    dlgPopup2->deleteLater();
    return 0;
}

void Widget::set_RDIcon()
{
    QFile file("/home/rdservice");

    if(file.exists("/home/rdservice"))
    {
        QPixmap pixmap(":/rsc/images/rd_installed.png");
        ui->rd_lbl->setPixmap(pixmap);
        ui->rd_lbl_I->setPixmap(pixmap);
        ui->rd_lbl_N->setPixmap(pixmap);
        ui->rd_lbl_R->setPixmap(pixmap);
        ui->rd->setPixmap(pixmap);

        if(system("pidof rds &> /dev/null")==0)
        {
            QPixmap pixmap(":/rsc/images/rd_running.png");
            ui->rd_lbl->setPixmap(pixmap);
            ui->rd_lbl_I->setPixmap(pixmap);
            ui->rd_lbl_N->setPixmap(pixmap);
            ui->rd_lbl_R->setPixmap(pixmap);
            ui->rd->setPixmap(pixmap);
        }

        return;
    }
    else
    {
        QPixmap pixmap(":/rsc/images/rd_not_installed.png");
        ui->rd_lbl->setPixmap(pixmap);
        ui->rd_lbl_I->setPixmap(pixmap);
        ui->rd_lbl_N->setPixmap(pixmap);
        ui->rd_lbl_R->setPixmap(pixmap);
        ui->rd->setPixmap(pixmap);
        return;
    }
    file.close();
}

int Widget::getDealerAuthTypeANDFusion(int currrow,char *uid)
{
    char AuthMode[16]="",Fusion[16]="";
    memset(AuthMode,0,sizeof(AuthMode));
    memset(Fusion,0,sizeof(Fusion));
    memset(FusionStatus,0,sizeof(FusionStatus));
    memset(DelAuthMode,0,sizeof(DelAuthMode));

    QSqlQuery query;

    if(currrow == 0||currrow == 1||currrow == 2)
    {

        qDebug()<<"Dealer "<<endl;
        query.prepare("SELECT auth_type_del,dealer_fusion FROM Dealer WHERE fpsuidno=?");
        query.addBindValue(uid);

        if(!query.exec())
            qDebug() << query.lastError();
        else
        {
            while(query.next())
            {
                strcpy(AuthMode,query.value(0).toString().toAscii().data());
                strcpy(Fusion,query.value(1).toString().toAscii().data());
            }
        }
        query.clear();
        if(strcmp(Fusion,"1") == 0)
        {
            strcpy(FusionStatus , "1");
            strcpy(DelAuthMode,AuthMode);
        }
        else
        {
            strcpy(FusionStatus , "0");
            strcpy(DelAuthMode,AuthMode);
        }

    }
    return 0;
}

int Widget::getBeneficiary_AuthTypeANDFusion(int currrow,char *uid)
{
    char AuthMode_Finger[8],AuthMode_Iris[8]="",AuthMode_Manual[8]="",AuthMode_Otp[8]="",Fusion[16]="";

    memset(AuthMode_Finger,0,sizeof(AuthMode_Finger));
    memset(AuthMode_Iris,0,sizeof(AuthMode_Iris));
    memset(AuthMode_Manual,0,sizeof(AuthMode_Manual));
    memset(AuthMode_Otp,0,sizeof(AuthMode_Otp));
    memset(Fusion,0,sizeof(Fusion));

    memset(FusionStatus,0,sizeof(FusionStatus));

    memset(BenAuthMode_Finger,0,sizeof(BenAuthMode_Finger));
    memset(BenAuthMode_Iris,0,sizeof(BenAuthMode_Iris));
    memset(BenAuthMode_Manual,0,sizeof(BenAuthMode_Manual));
    memset(BenAuthMode_Otp,0,sizeof(BenAuthMode_Otp));

    QSqlQuery query;

    qDebug()<<"Beneficiary "<<endl;
    query.prepare("SELECT Memberauth_finger,Memberauth_iris,Memberauth_manual,Memberauth_otp,MemberFusion from CustomerFamily WHERE MemberUID=?");
    query.addBindValue(uid);

    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            strcpy(AuthMode_Finger,query.value(0).toString().toAscii().data());
            strcpy(AuthMode_Iris,query.value(1).toString().toAscii().data());
            strcpy(AuthMode_Manual,query.value(2).toString().toAscii().data());
            strcpy(AuthMode_Otp,query.value(3).toString().toAscii().data());
            strcpy(Fusion,query.value(4).toString().toAscii().data());
        }
    }
    query.clear();
    if(strcmp(Fusion,"1") == 0)
    {
        strcpy(FusionStatus , "1");

        strcpy(BenAuthMode_Finger,AuthMode_Finger);
        strcpy(BenAuthMode_Iris,AuthMode_Iris);
        strcpy(BenAuthMode_Manual,AuthMode_Manual);
        strcpy(BenAuthMode_Otp,AuthMode_Otp);
    }
    else
    {
        strcpy(FusionStatus , "0");
        strcpy(BenAuthMode_Finger,AuthMode_Finger);
        strcpy(BenAuthMode_Iris,AuthMode_Iris);
        strcpy(BenAuthMode_Manual,AuthMode_Manual);
        strcpy(BenAuthMode_Otp,AuthMode_Otp);

    }

    return 0;
}

int Widget::PwdackRequest_Response(char *FileName)
{
    char cmachineID[16]="",FpsId[16]="";
    char cUrlName[256];
    char cmdCurl[1024];

    memset(cmachineID,0,sizeof(cmachineID));
    memset(FpsId,0,sizeof(FpsId));

    miscObj.GetMachineId(cmachineID);

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0){
        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }

    QSqlQuery query;
    query.prepare("SELECT ShopID FROM  ShopOwnerDetails");
    if(!query.exec())
        qDebug() << "SELECT ShopID FROM  ShopOwnerDetails = " << query.lastError();
    else{
        while(query.next()){
            strcpy(FpsId,query.value(0).toString().toAscii().data());
        }
    }

    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL){
        qDebug() << "File Not Found" << endl;
        return -1;
    }
    fprintf(fp,"%s","<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ser=\"http://service.fetch.rationcard/\"><soapenv:Header/>");
    fprintf(fp,"%s","<soapenv:Body>");
    fprintf(fp,"%s","<ser:ackRequest>");
    //    fprintf(fp,"<fpsCard>%s</fpsCard >",FpsId);
    if(iDealMemSelected == DEALER_SELECTED)
    {
        fprintf(fp,"<fpsCard>%s</fpsCard >",FpsId);
    }
    if(iDealMemSelected == MEMBER_SELECTED)
    {
        fprintf(fp,"<fpsCard>%s</fpsCard >",ui->lblRCNO_Val->text().toAscii().data());
    }
    fprintf(fp,"<terminalId>%s</terminalId>",cmachineID);
    fprintf(fp,"<user_password>%s</user_password>",dealer_password.toAscii().data());
    fprintf(fp,"<fpsSessionId>%s</fpsSessionId>",SessionId);

    //    int row= ui->tableWidgetDealNomDetails->currentRow();
    //    if(strlen(ui->tableWidgetDealNomDetails->item(row, 1)->text().toAscii().data())==12)
    //        fprintf(fp,"<uidNumber>%s</uidNumber>",ui->tableWidgetDealNomDetails->item(row, 1)->text().toAscii().data());
    //    else
    //        fprintf(fp,"<uidNumber>%s</uidNumber>","0");

    if(iDealMemSelected ==DEALER_SELECTED)
    {
        int row= ui->tableWidgetDealNomDetails->currentRow();

        qDebug()<<"del uid len"<<strlen(ui->tableWidgetDealNomDetails->item(row, 1)->text().toAscii().data());
        if(strlen(ui->tableWidgetDealNomDetails->item(row, 1)->text().toAscii().data())==12)
            fprintf(fp,"<uidNumber>%s</uidNumber>",ui->tableWidgetDealNomDetails->item(row, 1)->text().toAscii().data());
        else
            fprintf(fp,"<uidNumber>%s</uidNumber>","0");
    }
    if(iDealMemSelected ==MEMBER_SELECTED)
    {
        if(strlen(MemberUID)==12)
            fprintf(fp,"<uidNumber>%s</uidNumber>",MemberUID);
        else
            fprintf(fp,"<uidNumber>%s</uidNumber>","0");
    }

    fprintf(fp,"<token>%s</token>",HD_PASSWORD);

    //    fprintf(fp,"<auth_type>%s</auth_type>","P");
    //    fprintf(fp,"<user_type>%s</user_type>",ui->tableWidgetDealNomDetails->item(row,6)->text().toAscii().data());
    //    fprintf(fp,"<memberId>%s</memberId>","0");
    //    fprintf(fp,"<fpsId>%s</fpsId>",ui->label_shopid->text().toAscii().data());


    if(iDealMemSelected == DEALER_SELECTED)
    {
        //if dealer using password authentication send P else if iris I else if Biometric F
        fprintf(fp,"<auth_type>%s</auth_type>","P");
        //Del user_type-D, Ben user_type-B
        fprintf(fp,"<user_type>%s</user_type>","D");
        fprintf(fp,"<memberId>%s</memberId>","0");

        fprintf(fp,"<fpsId>%s</fpsId>",ui->label_shopid->text().toAscii().data());

    }
    if(iDealMemSelected == MEMBER_SELECTED)
    {
        fprintf(fp,"<auth_type>%s</auth_type>","M");
        fprintf(fp,"<user_type>%s</user_type>","B");

        int row = ui->twdgtEntitlement->currentRow();
        fprintf(fp,"<memberId>%s</memberId>",ui->twdgtEntitlement->item(row,6)->text().toAscii().data());
        fprintf(fp,"<fpsId>%s</fpsId>",ui->label_shopid->text().toAscii().data());
    }

    fprintf(fp,"<stateCode>%s</stateCode>",stateCode.toAscii().data());
    fprintf(fp,"%s","</ser:ackRequest>");
    fprintf(fp,"%s","</soapenv:Body>");
    fprintf(fp,"%s","</soapenv:Envelope>");

    fclose(fp);

    qDebug() << "************/mnt/jffs2/PWDACKrequest.xml************";
    system("cat /mnt/jffs2/PWDACKrequest.xml");
    qDebug();
    qDebug() << "******************************************************";


    /*****/
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

    memset(cmdCurl, 0, sizeof(cmdCurl));

    int ret_http_code = UrlPingCheck(cUrlName);

    if(ret_http_code == 1)
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/PWDACKrequest.xml", cUrlName, "/mnt/jffs2/PWDACK_response.xml", responseTimedOutTimeInSec.toInt());
    }
    else
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/PWDACKrequest.xml", PDS_URL2, "/mnt/jffs2/PWDACK_response.xml", responseTimedOutTimeInSec.toInt());
    }

    qDebug() << "Curl Cmd: " << cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0){
        return -1;
    }

    qDebug() << "**********/mnt/jffs2/PWDACK_response.xml********";
    system("cat /mnt/jffs2/PWDACK_response.xml");
    qDebug();
    qDebug() << "************************************************";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/PWDACK_response.xml") < 0)
        return -1;

    if(ParsePWDACK("/mnt/jffs2/PWDACK_response.xml") < 0)
        return -1;

    return 0;
}

int Widget::ParsePWDACK(char *PWDACK_response)
{
    QString respCode,respMessage,respuidref;
    char dispbuff[512]="";
    memset(dispbuff,0,sizeof(dispbuff));

    QFile file(PWDACK_response);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In parsePWDACK_response, Error Reading File";
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
            if(xml.name() == "respCode")
            {
                xml.readNext();
                respCode= xml.text().toString();
                qDebug() << "respCode : " << respCode;
            }
            if(xml.name() == "respMessage")
            {
                xml.readNext();
                respMessage= xml.text().toString();
                qDebug() << "respMessage : " << respMessage;
            }
            if(xml.name() == "refNo")
            {
                xml.readNext();
                respuidref= xml.text().toString();
                qDebug() << "respuidref : " << respuidref;
            }
        }
    }
    file.close();

    if(respCode.isEmpty() || respCode.isNull())
    {
        sprintf(dispbuff, "%s", "Invalid Response...\nPlease Try Again...");
        miscObj.DisplayWideMsgBox((char *)dispbuff);

        return -1;
    }
    else if(respCode == "00")
    {
        memset(gauth_transaction_code,0,sizeof(gauth_transaction_code));
        strcpy(gauth_transaction_code,respuidref.toAscii().data());
        sprintf(dispbuff, "%s(%s)", respMessage.toAscii().data(), respCode.toAscii().data());
        return 0;
    }
    else if(respCode != "00")
    {
        sprintf(dispbuff, "%s(%s)", respMessage.toAscii().data(), respCode.toAscii().data());
        miscObj.DisplayWideMsgBox((char *)dispbuff);

        return -1;
    }
    return 0;
}

//void Widget::Member_EKYC_Status(int ekycflag)
int Widget::Member_EKYC_Status(int ekycflag)
{
    QSqlQuery query;

    char cmemberid[64]="";
    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));
    memset(cmemberid,0,sizeof(cmemberid));
    int currentrow=0;

    if(ekycflag == 1)
    {
        currentrow = ui->twdgtEntitlement->currentRow();
        strcpy(cmemberid,ui->twdgtEntitlement->item(currentrow,6)->text().toAscii().data());
    }
    else if(ekycflag == 2)
    {
        currentrow = ui->tableWidgetBenVerify->currentRow();
        strcpy(MemberName,ui->tableWidgetBenVerify->item(currentrow,0)->text().toAscii().data());
        strcpy(MemberUID,ui->tableWidgetBenVerify->item(currentrow,1)->text().toAscii().data());
        strcpy(cmemberid,ui->tableWidgetBenVerify->item(currentrow,3)->text().toAscii().data());

    }
    Qwadh_auth = Qwadh_Member_global;//
    if(ekycflag == 2)
        Qwadh_auth=Qwadh_global;

    qDebug()<<"Qwadh_auth==="<<Qwadh_auth;

    if(rdCallinit_ekyc(1,Qwadh_auth.toAscii().data())< 0)
    {
        qDebug()<<"rdCallinit_ekyc Failed"<<endl;

        return -1;
    }

    popup_msg *dlgPopup3 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup3->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup3->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें."));

    dlgPopup3->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    system("rm /mnt/jffs2/MEMBER_EKYCAuthenticateReq.xml");
    system("rm /mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml");

    FILE *fp;
    fp = fopen("/mnt/jffs2/MEMBER_EKYCAuthenticateReq.xml","w");
    if(fp == NULL)
    {
        qDebug() << "File Not Found" << endl;

        dlgPopup3->close();
        dlgPopup3->deleteLater();

        return -1;
    }

    miscObj.GetMachineId(cmachineID);

    //RD 2.0 subbu added 07/11/2017
    //MEMBER_EKYCAuth

    fprintf(fp,"%s","<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    fprintf(fp,"%s","<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://service.fetch.rationcard/\">");
    fprintf(fp,"%s","<SOAP-ENV:Body>");
    fprintf(fp,"%s","<ns1:getEKYCAuthenticateRD>");
    fprintf(fp,"<fpsSessionId>%s</fpsSessionId>",SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp,"<Shop_no>%s</Shop_no>",ui->ldtShpId->text().toAscii().data());
    fprintf(fp,"<terminal_id>%s</terminal_id>",cmachineID);

    if(ekycflag == 1)
        fprintf(fp,"<existingRCNumber>%s</existingRCNumber>",Rcno.toAscii().data());
    if(ekycflag == 2)
        fprintf(fp,"<existingRCNumber>%s</existingRCNumber>",RationCardId);

    fprintf(fp,"<rcMemberName>%s</rcMemberName>",MemberName);

    //auth fail uid==uid
    //uid -NA null---enter value at ekyc

    if(strlen(MemberUID)==12)
        fprintf(fp,"<rcUid>%s</rcUid>",MemberUID);
    else
        fprintf(fp,"<rcUid>%s</rcUid>","0");

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

    rsaEncryption(memekycuid,skey);
    fprintf(fp,"<res_uid>%s</res_uid>",asciiBase64Enc);

    fprintf(fp,"%s","</ekycresAuth>");
    fprintf(fp,"<password>%s</password>",HD_PASSWORD);


    if(ekycflag == 1)
        fprintf(fp,"<eKYCType>%s</eKYCType>","eKYCN");
    if(ekycflag == 2)
        fprintf(fp,"<eKYCType>%s</eKYCType>","eKYCV");

    fprintf(fp,"<Resp><errCode>%s</errCode><errInfo>%s</errInfo><nmPoints>%s</nmPoints><fCount>%s</fCount><fType>%s</fType><iCount>%s</iCount><iType>%s</iType><pCount>%s</pCount><pType>%s</pType><qScore>%s</qScore></Resp>",pid_info.err_code,pid_info.err_info,pid_info.nmPoints,pid_info.fCount,pid_info.fType,pid_info.iCount,pid_info.iType,pid_info.pCount,pid_info.pType,pid_info.qScore);;
    fprintf(fp,"%s","</ns1:getEKYCAuthenticateRD>");
    fprintf(fp,"%s","</SOAP-ENV:Body>");
    fprintf(fp,"%s","</SOAP-ENV:Envelope>");
    fclose(fp);

    free(pid_info.pid);
    pid_info.pid = NULL;

    system("cat /mnt/jffs2/MEMBER_EKYCAuthenticateReq.xml");
    qDebug();
    qDebug() << "***************************************************";

    char cUrlName[256];
    query.clear();
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

    int ret_http_code = UrlPingCheck(cUrlName);

    if(ret_http_code == 1)
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/MEMBER_EKYCAuthenticateReq.xml", cUrlName,  "/mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml", responseTimedOutTimeInSec.toInt());
    }
    else
    {
        sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/MEMBER_EKYCAuthenticateReq.xml", cUrlName,  "/mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml", responseTimedOutTimeInSec.toInt());
    }

    qDebug()<<"\nCurl Cmd : "<<cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0)
    {
        dlgPopup3->close();
        dlgPopup3->deleteLater();

        return -1;
    }

    qDebug() << "/mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml************";
    //    system("cp /mnt/jffs2/AuthResponse1.xml /mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml");
    system("cat /mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml");
    qDebug();
    qDebug() << "***************************************************";

    dlgPopup3->close();
    dlgPopup3->deleteLater();

    if(ParsingFirstCondition("/mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml") != 0)
    {
        qDebug()<<"ParsingFirstCondition!=0";
        parseSeedingcCardHolderDetails("/mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml"); //to get zdistrTxnId to send uid ref number
        return -2;
    }
    qDebug()<<"parseSeedingcCardHolderDetails"<<endl;

    if(parseSeedingcCardHolderDetails("/mnt/jffs2/MEMBER_EKYCAuthenticateResp.xml") < 0)
        return -1;

    return 0;
}

int Widget::Ekyc_uid_popup(char *uid)
{
    bool ok;
    char buff[100]="";
    memset(buff,0,sizeof(buff));

    sprintf(buff,"%s","Enter 12 digit Aadhaar Number");

    QString text;


    if(LangMode==1)
        text = QInputDialog::getText(this, tr("             eKYC"),tr(buff), QLineEdit::Password,"", &ok);
    else if(LangMode==2)
        text = QInputDialog::getText(this, tr("             eKYC"),QString::fromUtf8("12 अंक आधार संख्या दर्ज करें"), QLineEdit::Password,"", &ok);

    if(ok && !text.isEmpty())
    {
        if(text.length() == 12)
        {
            if(miscObj.validate_uid(text.toAscii().data()) == 0)
            {
                qDebug()<<"valid aadhaar";
                strcpy(uid,text.toAscii().data());

                return 0;
            }
            else
            {
                if(LangMode==1)
                    pop_up((char *)"Enter Correct Aadhaar Number ...");
                else if(LangMode==2)
                    pop_up((char *)"सही आधार संख्या दर्ज करें...");

                sleep(3);
                return -2;
            }

        }
        else
        {
            if(LangMode==1)
                pop_up((char *)"Enter 12 digit Aadhaar Number ...");
            else if(LangMode==2)
                pop_up((char *)"कृपया 12 अंक यूआईडी नंबर दर्ज करें..");

            return -2;
        }

    }
    else if(ok && text.isEmpty())
    {
        qDebug()<<"Nothing Entered";
        if(LangMode==1)
            pop_up((char *)"Enter 12 digit Aadhaar Number ...");
        else if(LangMode==2)
            pop_up((char *)"कृपया 12 अंक यूआईडी नंबर दर्ज करें..");

        sleep(3);
        return -2;
    }
    else
    {
        qDebug()<<"We enter cancel";
        return -1;
    }

    return 0;
}

int Widget::mobileno_otp_popup(char *OTP_MOBILE)
{
    bool ok;
    char buff[100]="",HeadMobNo[16]="";
    memset(buff,0,sizeof(buff));
    memset(HeadMobNo,0,sizeof(HeadMobNo));

    getHeadMobileno(HeadMobNo);

    sprintf(buff,"%s","Enter 10 digit Mobile Number");

    QString text;
    if(LangMode==1)
        text = QInputDialog::getText(this, tr("MOBILE NUMBER ENTRY"),tr(buff), QLineEdit::Normal,HeadMobNo, &ok);
    else if(LangMode==2)
    {
        QString strbuff;
        strbuff.clear();
        strbuff +=QString::fromUtf8("10 अंकों का मोबाइल नंबर दर्ज करें");
        text = QInputDialog::getText(this, tr("MOBILE NUMBER ENTRY"),strbuff, QLineEdit::Normal,HeadMobNo, &ok);
    }
    if(ok && !text.isEmpty())
    {
        if(text.length() == 10)
        {
            strcpy(OTP_MOBILE,text.toAscii().data());

        }
        else
        {
            if(LangMode==1)
                pop_up((char *)"Enter 10 digit Mobile Number ...");
            else if(LangMode==2)
                pop_up((char *)"कृपया केवल 10 अंकों का\n मोबाइल नंबर दर्ज करें.");

            return -2;
        }

    }
    else if(ok && text.isEmpty())
    {
        qDebug()<<"Nothing Entered";
        if(LangMode==1)
            pop_up((char *)"Enter 10 digit Mobile Number ...");
        else if(LangMode==2)
            pop_up((char *)"कृपया केवल 10 अंकों का\n मोबाइल नंबर दर्ज करें..");

        sleep(3);
        return -2;
    }
    else
    {
        qDebug()<<"We enter cancel for manual txn";
        if(LangMode==1)
            pop_up((char *)"Manual Transaction");
        else
            pop_up((char *)"अप्रमाणित लेनदेन");

        sleep(1);
        return -1;
    }
    return 0;
}


int Widget::OTP_Mobile_popup(char *OTP) //OTP Entry
{
    bool ok;
    char buff[100]="";
    memset(buff,0,sizeof(buff));

    sprintf(buff,"%s %s\n %s","Mobile OTP Sent to",otp_mobile,"Enter OTP");

    QString text;
    if(LangMode==1)
        text = QInputDialog::getText(this, tr("OTP Verify"),tr(buff), QLineEdit::Normal,"", &ok);
    else if(LangMode==2)
    {
        QString strbuff;
        strbuff.clear();
        strbuff +=QString::fromUtf8("ओटीपी को मोबाइल पर भेजा जा रहा है.");
        strbuff +='\n';
        strbuff += otp_mobile;
        strbuff += QString::fromUtf8("ओटीपी दर्ज करें");
        text = QInputDialog::getText(this, tr("OTP Verify"),strbuff, QLineEdit::Normal,"", &ok);
    }
    if(ok && !text.isEmpty())
    {
        if(text.length() == 6)
        {
            strcpy(OTP,text.toAscii().data());
        }
        else
        {
            if(LangMode==1)
                pop_up((char *)"Enter 6 digit Mobile OTP ...");
            else if(LangMode==2)
                pop_up((char *)"छठे ओटीपी नं दर्ज करें...");

            return -2;
        }

    }
    else if(ok && text.isEmpty())
    {
        qDebug()<<"Nothing Entered";
        if(LangMode==1)
            pop_up((char *)"Enter 6 digit Mobile OTP...");
        else if(LangMode==2)
            pop_up((char *)"छठे ओटीपी नं दर्ज करें...");

        sleep(3);
        return -2;
    }
    else
    {
        qDebug()<<"We enter cancel";
        return -1;
    }
    return 0;
}

int Widget::OTP_Mobile_popup_New(char *OTP) //OTP New
{
    char buff[100]="";
    memset(buff,0,sizeof(buff));
    QString strbuff;

    if(LangMode==1)
    {
        sprintf(buff,"%s %s","Mobile OTP Sent to",otp_mobile);
        strbuff.clear();
        strbuff.append(buff);
    }
    else if(LangMode==2)
    {
        qDebug("In Language1");
        strbuff.clear();
        strbuff +=QString::fromUtf8("ओटीपी को मोबाइल पर भेजा जा रहा है:\n");
        strbuff += otp_mobile;
    }
    QLineEdit *edit;
    QPushButton *Okb;
    QLabel *Nlabel;
    QDialog *Dlg =new QDialog;
    Dlg->setWindowFlags(Qt::FramelessWindowHint);
    Dlg->setGeometry(10,35,300,150);

    Nlabel = new QLabel(strbuff,Dlg);

    Nlabel->setGeometry(10,5,280,50);
    QLabel *OtpLabel;
    if(LangMode==1)
        OtpLabel = new QLabel("Enter OTP",Dlg);
    else if(LangMode==2)
        OtpLabel = new QLabel(QString::fromUtf8("ओटीपी दर्ज करें"),Dlg);
    OtpLabel->setGeometry(10,55,150,30);
    edit = new QLineEdit;
    edit->setValidator(new QRegExpValidator(QRegExp("[0-9]{0,6}"),this));
    edit->setParent(Dlg);
    edit->setMaxLength(6);
    edit->setGeometry(160,55,120,30);
    if(LangMode==1)
        Okb = new QPushButton("OK",Dlg);
    else if(LangMode==2)
        Okb = new QPushButton(QString::fromUtf8("ठीक"),Dlg);

    Okb->setGeometry(130,90,100,25);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), Dlg, SLOT(close()));
    connect(Okb, SIGNAL(clicked()), Dlg, SLOT(close()));
    qDebug()<<"OtppopTimeOut"<<OtppopTimeOut;

    timer->start(OtppopTimeOut); //45 sec
    Dlg->exec();
    QString Str = edit->text();
    qDebug()<<"OTP Value :::"<<Str;

    if(Str.length()==6)
        strcpy(OTP,Str.toAscii().data());
    else
        return -1;

    return 0;
}


void Widget::setsignalStrength()
{
    char CurrBuff[32]="",PrevBuff[32]="";
    int count = 0;

    QFile file("/var/gprs");
    if(!file.exists())
    {
        qDebug()<<"/var/gprs file not exit";
        QPixmap pixmap(":/rsc/images/gsm-not-initialised.png");
        ui->lbl_sig1->setPixmap(pixmap);
        ui->lbl_sig2->setPixmap(pixmap);
        ui->lbl_sig3->setPixmap(pixmap);
        ui->lbl_sig4->setPixmap(pixmap);
        ui->lbl_sig5->setPixmap(pixmap);
        ui->lbl_sig6->setPixmap(pixmap);
        ui->lbl_sig7->setPixmap(pixmap);
        ui->lbl_sig8->setPixmap(pixmap);

        return;
    }

    file.open(QIODevice::ReadOnly);

    QTextStream in(&file);

    QString str=in.readAll();

    str.chop(1);
    strncpy(CurrBuff,str.toAscii().data(),str.size());

    if((strcmp(PrevBuff,CurrBuff))==0)
    {
        count++;
        if(count==10)
        {
            count=0;
            goto label;
        }
        return;
    }
    else
    {
        count==0;
    }

label:
    strcpy(PrevBuff,CurrBuff);

    QStringList list = str.split(",");

    file.close();

    if(list.at(0)=="3")
    {
        QPixmap pixmap(":/rsc/images/gsm-not-initialised.png");
        ui->lbl_sig1->setPixmap(pixmap);
        ui->lbl_sig2->setPixmap(pixmap);
        ui->lbl_sig3->setPixmap(pixmap);
        ui->lbl_sig4->setPixmap(pixmap);
        ui->lbl_sig5->setPixmap(pixmap);

        ui->lbl_sig6->setPixmap(pixmap);
        ui->lbl_sig7->setPixmap(pixmap);
        ui->lbl_sig8->setPixmap(pixmap);

        return;
    }
    else if(list.at(0)=="4")
    {
        QPixmap pixmap(":/rsc/images/no-network.png");
        ui->lbl_sig1->setPixmap(pixmap);
        ui->lbl_sig2->setPixmap(pixmap);
        ui->lbl_sig3->setPixmap(pixmap);
        ui->lbl_sig4->setPixmap(pixmap);
        ui->lbl_sig5->setPixmap(pixmap);
        ui->lbl_sig6->setPixmap(pixmap);
        ui->lbl_sig7->setPixmap(pixmap);
        ui->lbl_sig8->setPixmap(pixmap);

        return;
    }
    else if(list.at(0) == "0")
    {
        if(list.at(1) == "X")
        {
            QPixmap pixmap(":/rsc/images/Ethernet-connected.png");
            ui->lbl_sig1->setPixmap(pixmap);
            ui->lbl_sig2->setPixmap(pixmap);
            ui->lbl_sig3->setPixmap(pixmap);
            ui->lbl_sig4->setPixmap(pixmap);
            ui->lbl_sig5->setPixmap(pixmap);
            ui->lbl_sig6->setPixmap(pixmap);
            ui->lbl_sig7->setPixmap(pixmap);
            ui->lbl_sig8->setPixmap(pixmap);
        }
        else if(list.at(1) == "W")
        {
            if(list.at(2)=="0")
            {
                QPixmap pixmap(":/rsc/images/wifi_0.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else if(list.at(2) == "1")
            {
                QPixmap pixmap(":/rsc/images/wifi_1.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else if(list.at(2) == "2")
            {
                QPixmap pixmap(":/rsc/images/wifi_2.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else if(list.at(2) == "3")
            {
                QPixmap pixmap(":/rsc/images/wifi_3.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else if(list.at(2) == "4")
            {
                QPixmap pixmap(":/rsc/images/wifi_4.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
        }
        return;
    }
    else if(list.at(0) == "1")
    {
        if(list.at(1)=="H")
        {
            if(list.at(2)=="0")
            {
                QPixmap pixmap(":/rsc/images/sim1_3g_0signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else if(list.at(2) == "1")
            {
                QPixmap pixmap(":/rsc/images/sim1_3g_1signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else if(list.at(2) == "2")
            {
                QPixmap pixmap(":/rsc/images/sim1_3g_2signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "3")
            {
                QPixmap pixmap(":/rsc/images/sim1_3g_3signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "4")
            {
                QPixmap pixmap(":/rsc/images/sim1_3g_4signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else
                qDebug() << "Invalid Option..." ;
        }
        if(list.at(1) == "E")
        {
            if(list.at(2)=="0")
            {
                QPixmap pixmap(":/rsc/images/sim1_2g_0signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else if(list.at(2)=="1")
            {
                QPixmap pixmap(":/rsc/images/sim1_2g_1signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else if(list.at(2)=="2")
            {
                QPixmap pixmap(":/rsc/images/sim1_2g_2signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2)=="3")
            {
                QPixmap pixmap(":/rsc/images/sim1_2g_3signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2)=="4")
            {
                QPixmap pixmap(":/rsc/images/sim1_2g_4signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else
                qDebug() << "Invalid Option..." ;
        }
        else if(list.at(1) == "N")
        {
            if(list.at(2)=="0")
            {
                QPixmap pixmap(":/rsc/images/sim1_0_signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "1")
            {
                QPixmap pixmap(":/rsc/images/sim1_1_signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "2")
            {
                QPixmap pixmap(":/rsc/images/sim1_2_signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);


            }
            else if(list.at(2) == "3")
            {
                QPixmap pixmap(":/rsc/images/sim1_3_signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "4")
            {
                QPixmap pixmap(":/images/sim1_4_signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else
                qDebug() << "Invalid Option..." ;
        }
        //added
        if(list.at(1) == "L")
        {
            if(list.at(2)=="0")
            {
                QPixmap pixmap(":/rsc/images/sim1_4g_0signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else if(list.at(2)=="1")
            {
                QPixmap pixmap(":/rsc/images/sim1_4g_1signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else if(list.at(2)=="2")
            {
                QPixmap pixmap(":/rsc/images/sim1_4g_2signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2)=="3")
            {
                QPixmap pixmap(":/rsc/images/sim1_4g_3signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2)=="4")
            {
                QPixmap pixmap(":/rsc/images/sim1_4g_4signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);
            }
            else
                qDebug() << "Invalid Option..." ;
        }

    }
    else if(list.at(0)=="2")
    {
        if(list.at(1)=="H")
        {
            if(list.at(2)=="0")
            {
                QPixmap pixmap(":/rsc/images/sim2_3g_0signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "1")
            {
                QPixmap pixmap(":/rsc/images/sim2_3g_1signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "2")
            {
                QPixmap pixmap(":/rsc/images/sim2_3g_2signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "3")
            {
                QPixmap pixmap(":/rsc/images/sim2_3g_3signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "4")
            {
                QPixmap pixmap(":/rsc/images/sim2_3g_4signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else
                qDebug() << "Invalid Option..." ;
        }
        else if(list.at(1) == "E")
        {
            if(list.at(2)=="0")
            {
                QPixmap pixmap(":/rsc/images/sim2_2g_0signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2)=="1")
            {
                QPixmap pixmap(":/rsc/images/sim2_2g_1signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2)=="2")
            {
                QPixmap pixmap(":/rsc/images/sim2_2g_2signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2)=="3")
            {
                QPixmap pixmap(":/rsc/images/sim2_2g_3signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2)=="4")
            {
                QPixmap pixmap(":/rsc/images/sim2_2g_4signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else
                qDebug() << "Invalid Option..." ;
        }
        //added
        else if(list.at(1) == "L")
        {
            if(list.at(2)=="0")
            {
                QPixmap pixmap(":/rsc/images/sim2_4g_0signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2)=="1")
            {
                QPixmap pixmap(":/rsc/images/sim2_4g_1signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2)=="2")
            {
                QPixmap pixmap(":/rsc/images/sim2_4g_2signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);

            }
            else if(list.at(2)=="3")
            {
                QPixmap pixmap(":/rsc/images/sim2_4g_3signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2)=="4")
            {
                QPixmap pixmap(":/rsc/images/sim2_4g_4signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else
                qDebug() << "Invalid Option..." ;
        }//ended
        else if(list.at(1) == "N")
        {
            if(list.at(2)=="0")
            {
                QPixmap pixmap(":/rsc/images/sim2_0_signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "1")
            {
                QPixmap pixmap(":/rsc/images/sim2_1_signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "2")
            {
                QPixmap pixmap(":/rsc/images/sim2_2_signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "3")
            {
                QPixmap pixmap(":/rsc/images/sim2_3_signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else if(list.at(2) == "4")
            {
                QPixmap pixmap(":/rsc/images/sim2_4_signal.png");
                ui->lbl_sig1->setPixmap(pixmap);
                ui->lbl_sig2->setPixmap(pixmap);
                ui->lbl_sig3->setPixmap(pixmap);
                ui->lbl_sig4->setPixmap(pixmap);
                ui->lbl_sig5->setPixmap(pixmap);
                ui->lbl_sig6->setPixmap(pixmap);
                ui->lbl_sig7->setPixmap(pixmap);
                ui->lbl_sig8->setPixmap(pixmap);

            }
            else
                qDebug() << "Invalid Option..." ;
        }
        else
            qDebug() << "Invalid Option..." ;
    }
    else if(list.at(0) == "W")
    {
        if(list.at(1)=="I")
        {
            QPixmap pixmap(":/rsc/images/wifi_no.png");
            ui->lbl_sig1->setPixmap(pixmap);
            ui->lbl_sig2->setPixmap(pixmap);
            ui->lbl_sig3->setPixmap(pixmap);
            ui->lbl_sig4->setPixmap(pixmap);
            ui->lbl_sig5->setPixmap(pixmap);
            ui->lbl_sig6->setPixmap(pixmap);
            ui->lbl_sig7->setPixmap(pixmap);
            ui->lbl_sig8->setPixmap(pixmap);
        }
    }
    else
    {
        qDebug()<<"No Case Executed....";
    }
}

void Widget::updateBatStatus()
{
    int AdapPresent=0;
    int BattSt=0;
    int BattVt=0,batVolt=0;
    int ret = 0;

    ret = gl11_GetBatteryInfo(&AdapPresent, &BattSt, &BattVt);
    if(ret == 0)
    {
        batVolt = BattVt;
    }
    else
        qDebug("function FAILED to get battery info");

    if(AdapPresent == 1)
    {
        if(batBuzzStatus)
        {
            gl11_buzz_off();
            batBuzzStatus = false;
        }
        if(BattVt == 0)
        {
            QPixmap batPix(":/rsc/images/no-battery.png");//No Battery Symbol
            ui->label_bat1->setPixmap(batPix);
            ui->label_bat2->setPixmap(batPix);
        }
        else
        {
            if(BattSt == 1)
            {
                if(movie == NULL)
                {
                    movie = new QMovie(":/rsc/images/Battery-charging-animation.gif");
                    ui->label_bat1->setMovie(movie);
                    ui->label_bat1->show();

                    ui->label_bat2->setMovie(movie);
                    ui->label_bat2->show();
                    movie->start();

                }
                else
                {
                    ui->label_bat1->setMovie(movie);
                    ui->label_bat1->show();
                    ui->label_bat2->setMovie(movie);
                    ui->label_bat2->show();
                    movie->start();
                }

            }
            if(BattSt == 0)
            {
                QPixmap batPix(":/rsc/images/Battery-100.png");//Battery Fully Charged Symbol
                ui->label_bat1->setPixmap(batPix);
                ui->label_bat2->setPixmap(batPix);
            }

        }
    }
    else
    {
        if(BattVt<=756)
        {
            QPixmap batPix(":/rsc/images/Battery-10.png");
            ui->label_bat1->setPixmap(batPix);
            ui->label_bat2->setPixmap(batPix);
            gl11_buzz_on();
            batBuzzStatus = true;
        }
        else
        {
            if(batBuzzStatus)
            {
                gl11_buzz_off();
                batBuzzStatus = false;
            }
            if(BattVt>756 && BattVt<=878)
            {
                QPixmap batPix(":/rsc/images/Battery-30.png");
                ui->label_bat1->setPixmap(batPix);
                ui->label_bat2->setPixmap(batPix);
            }
            if(BattVt>878 && BattVt<=902)
            {
                QPixmap batPix(":/rsc/images/Battery-50.png");
                ui->label_bat1->setPixmap(batPix);
                ui->label_bat2->setPixmap(batPix);
            }
            if(BattVt>902 && BattVt<=939)
            {
                QPixmap batPix(":/rsc/images/Battery-70.png");
                ui->label_bat1->setPixmap(batPix);
                ui->label_bat2->setPixmap(batPix);
            }
            if(BattVt>939)
            {
                QPixmap batPix(":/rsc/images/Battery-100.png");
                ui->label_bat1->setPixmap(batPix);
                ui->label_bat2->setPixmap(batPix);
            }
        }
    }
}


void Widget::on_BtnDealerIris_clicked()
{
    qDebug()<<"IRIS TEST ";

    on_btnLogin_clicked();

}


discoveryiris * BiomatiquesRDS_Discovery()
{
    discoveryiris * ret=NULL;
    int port=11100;
    for(port=11100;port<=11120;port++)
    {
        int res=chekPort(port);

        if(res==0 )
        {

            temp.port=port;

            ret=&temp;
            break;
        }
    }
    return ret;
}

dev_infoiris * BiomatiquesRDS_DeviceInfo(discoveryiris * rd)
{
    dev_infoiris *t=NULL;
    char cmd[200];
    sprintf(cmd,"curl -X DEVICEINFO http://127.0.0.1:%d%s",rd->port,rd->info_path);
    printf("\n DeviceInfo command %s \n",cmd);
    FILE *FP=popen(cmd,"r");
    if(FP!=NULL)
    {

        char path[100];
        char CurlResponse[2000];
        memset(CurlResponse,'\0',2000);
        while (fgets(path, sizeof(path)-1, FP) != NULL)
        {
            strcat(CurlResponse,path);
        }

        xmlDoc *doc = NULL;
        xmlNode *root_element = NULL;
        doc= xmlParseDoc((xmlChar *)CurlResponse);
        if(doc!=NULL)
        {


            root_element = xmlDocGetRootElement(doc);
            if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"DeviceInfo")==0)
            {
                xmlChar *uri;
                uri = xmlGetProp(root_element,(xmlChar *) "dpId");
                if(uri!=NULL)
                {
                    sprintf(temp_dev_info.dpid,"%s",uri);
                }
                uri = xmlGetProp(root_element, (xmlChar *)"rdsId");
                if(uri!=NULL)
                {
                    sprintf(temp_dev_info.rdsid,"%s",uri);
                }
                uri = xmlGetProp(root_element,(xmlChar *) "rdsVer");
                if(uri!=NULL)
                {
                    sprintf(temp_dev_info.rdsver,"%s",uri);
                }
                uri = xmlGetProp(root_element,(xmlChar *) "dc");
                if(uri!=NULL)
                {
                    sprintf(temp_dev_info.dc,"%s",uri);
                }
                uri = xmlGetProp(root_element, (xmlChar *)"mi");
                if(uri!=NULL)
                {
                    sprintf(temp_dev_info.mi,"%s",uri);
                }
                uri = xmlGetProp(root_element,(xmlChar *) "mc");
                if(uri!=NULL)
                {
                    sprintf(temp_dev_info.mc,"%s",uri);
                }
                t=0;
                xmlFree(uri);
                sprintf(temp_dev_info.raw_deviceInfo,"%s",CurlResponse);
                t=&temp_dev_info;
            }
            xmlFreeDoc(doc);


        }

    }
    return t;

}

captureiris * BiomatiquesRDS_Capture(discoveryiris * rd,pidopts opts)
{
    captureiris *cap=NULL;

    char PidOptions[500];
    sprintf(PidOptions,"<PidOptions ver=\"1.0\"> <Opts env=\"%s\" fCount=\"0\" fType=\"0\" iCount=\"1\" iType=\"0\" pCount=\"0\" pType=\"0\" format=\"0\"   pidVer=\"2.0\" timeout=\"10000\" otp=\"\" wadh=\"%s\" posh=\"\"/> <Demo></Demo><CustOpts></CustOpts></PidOptions>",opts.env,opts.wadh);
    char cmd[1000];
    sprintf(cmd,"curl -X CAPTURE -d '%s' http://127.0.0.1:%d%s",PidOptions,rd->port,rd->capture_Path);
    printf("\n Capture command %s \n",cmd);
    FILE *FP=popen(cmd,"r");
    if(FP!=NULL)
    {

        char path[100];
        char CurlResponse[10000];
        memset(CurlResponse,'\0',10000);
        while (fgets(path, sizeof(path)-1, FP) != NULL)
        {
            strcat(CurlResponse,path);
        }

        xmlDoc *doc = NULL;
        xmlNode *root_element = NULL;
        doc= xmlParseDoc((xmlChar *)CurlResponse);
        if(doc!=NULL){
            root_element = xmlDocGetRootElement(doc);
            if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"PidData")==0){
                root_element = root_element->xmlChildrenNode;
                while (root_element != NULL) {
                    xmlChar *uri;
                    if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"Resp")==0)
                    {
                        uri = xmlGetProp(root_element,(xmlChar *) "errCode");
                        if(uri!=NULL){
                            sprintf(temp_capture.err_code,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "errInfo");
                        if(uri!=NULL){
                            sprintf(temp_capture.err_info,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "pType");
                        if(uri!=NULL){
                            sprintf(temp_capture.pType,"%s",uri);
                        }
                        uri = xmlGetProp(root_element, (xmlChar *)"qScore");
                        if(uri!=NULL){
                            sprintf(temp_capture.qScore,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "fCount");
                        if(uri!=NULL){
                            sprintf(temp_capture.fCount,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "fType");
                        if(uri!=NULL){
                            sprintf(temp_capture.fType,"%s",uri);
                        }
                        uri = xmlGetProp(root_element, (xmlChar *)"iCount");
                        if(uri!=NULL){
                            sprintf(temp_capture.iCount,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "iType");
                        if(uri!=NULL){
                            sprintf(temp_capture.iType,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "pCount");
                        if(uri!=NULL){
                            sprintf(temp_capture.pCount,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "pType");
                        if(uri!=NULL){
                            sprintf(temp_capture.pType,"%s",uri);
                        }
                    }else if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"DeviceInfo")==0){
                        uri = xmlGetProp(root_element, (xmlChar *)"dpId");
                        if(uri!=NULL){
                            sprintf(temp_capture.dev_info.dpid,"%s",uri);
                        }
                        uri = xmlGetProp(root_element, (xmlChar *)"rdsId");
                        if(uri!=NULL){
                            sprintf(temp_capture.dev_info.rdsid,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "rdsVer");
                        if(uri!=NULL){
                            sprintf(temp_capture.dev_info.rdsver,"%s",uri);
                        }
                        uri = xmlGetProp(root_element, (xmlChar *)"dc");
                        if(uri!=NULL){
                            sprintf(temp_capture.dev_info.dc,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "mi");
                        if(uri!=NULL){
                            sprintf(temp_capture.dev_info.mi,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "mc");
                        if(uri!=NULL){
                            sprintf(temp_capture.dev_info.mc,"%s",uri);
                        }
                    }else if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"Skey")==0){
                        uri = xmlGetProp(root_element, (xmlChar *)"ci");
                        if(uri!=NULL){
                            sprintf(temp_capture.ci,"%s",uri);
                        }
                        sprintf(temp_capture.Skey,"%s",xmlNodeGetContent(root_element));
                    }else if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"Hmac")==0){
                        sprintf(temp_capture.Hmac,"%s",xmlNodeGetContent(root_element));
                    }else if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"Data")==0){
                        uri = xmlGetProp(root_element,(xmlChar *) "type");
                        if(uri!=NULL){
                            sprintf(temp_capture.type,"%s",uri);
                        }
                        sprintf(temp_capture.pid,"%s",xmlNodeGetContent(root_element));
                    }
                    root_element = root_element->next;
                }
                sprintf(temp_capture.raw_capture,"%s",CurlResponse);
                cap=&temp_capture;
            }
        }
    }
    return cap;
}

int chekPort(int port)
{
    int retval=-1;
    char cmd[50];
    sprintf(cmd,"curl -X RDSERVICE http://127.0.0.1:%d",port);
    FILE * FP=popen(cmd,"r");
    if(FP!=NULL){
        char path[100];
        char CurlResponse[1000];
        memset(CurlResponse,'\0',1000);
        while (fgets(path, sizeof(path)-1, FP) != NULL){
            strcat(CurlResponse,path);
        }
        printf("\n%s",CurlResponse);

        QString str(CurlResponse);
        if(str.contains("Biomatiques Iris (Model: EPI-1000)")){
            xmlDoc *doc = NULL;
            xmlNode *root_element = NULL;
            doc= xmlParseDoc((xmlChar *)CurlResponse);
            if(doc!=NULL){
                root_element = xmlDocGetRootElement(doc);
                if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"RDService")==0){
                    xmlChar *uri;
                    uri = xmlGetProp(root_element,(xmlChar *) "status");
                    if(uri!=NULL){
                        sprintf(temp.status,"%s",uri);
                    }
                    uri = xmlGetProp(root_element, (xmlChar *)"info");
                    if(uri!=NULL){
                        sprintf(temp.info,"%s",uri);
                    }
                    root_element = root_element->xmlChildrenNode;
                    while (root_element != NULL){
                        if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"Interface")==0){
                            uri = xmlGetProp(root_element, (xmlChar *)"id");
                            if(uri!=NULL && xmlStrcasecmp(uri,(xmlChar *)"CAPTURE")==0){
                                uri = xmlGetProp(root_element,(xmlChar *) "path");
                                sprintf(temp.capture_Path,"%s",uri);
                            }else if(uri!=NULL && xmlStrcasecmp(uri,(xmlChar *)"DEVICEINFO")==0){
                                uri = xmlGetProp(root_element, (xmlChar *)"path");
                                sprintf(temp.info_path,"%s",uri);
                            }
                        }
                        root_element = root_element->next;
                    }
                    sprintf(temp.raw_discovery,"%s",CurlResponse);
                    xmlFree(uri);
                    retval=0;
                }
            }
        }
    }
    return retval;
}

void Widget::on_btnmobupdateback_clicked()
{
    ui->stackedWidget->setCurrentIndex(40);
}

void Widget::on_btnmobupdatenext_clicked()
{
    GPRS objGprs;
    if(objGprs.Check_Network_Available() < 0)
        return;

    if(ui->leditcustrcno->text().length()==0){
        if(LangMode == ENGLISH)
            pop_up("Please Enter Ration Card Number");
        else if(LangMode == 2)
            pop_up("कृपया कार्ड नंबर दर्ज करें.");
        sleep(3);
        return;
    }
    if(ui->leditcustmobno->text().length()!=10){
        if(LangMode == ENGLISH)
            pop_up("Please Enter 10 digit Mobile Number");
        else if(LangMode == 2)
            pop_up("कृपया 12 अंक यूआईडी नंबर दर्ज करें.");

        sleep(3);
        return;
    }
    int ret = 0;
    ui->leditmobupdateotp->clear();

    popup_msg *dlgPopup2 = new popup_msg(this);

    if(LangMode == ENGLISH)
        dlgPopup2->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.."));

    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();


    char FileName[64];
    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/beneficiaryOTPRequest.xml");

    system("rm /mnt/jffs2/beneficiaryOTPRequest.xml");
    system("rm /mnt/jffs2/beneficiaryOTP_Response.xml");

    ret = prepare_beneficiaryOTPReq(FileName);
    qDebug()<<"ret form Prepare Out File : "<<ret;

    char catCmd[128];
    memset(catCmd, 0, sizeof(catCmd));
    sprintf(catCmd, "cat %s", FileName);
    system(catCmd);

    char cUrlName[256];
    memset(cUrlName, 0, sizeof(cUrlName));
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
    sleep(2); //Kept For Audio
    char cmdCurl[1024];
    memset(cmdCurl, 0, sizeof(cmdCurl));
    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/beneficiaryOTP_Response.xml", responseTimedOutTimeInSec.toInt());
    qDebug() << "\nCurl Cmd : " << cmdCurl;
    int sysret = system(cmdCurl);
    qDebug() << "sysret = " << sysret;

    if(SystemCurlResponse(sysret, EPDS_SERVER) < 0){
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        return;
    }
    dlgPopup2->close();
    dlgPopup2->deleteLater();

    qDebug() << "/mnt/jffs2/beneficiaryOTP_Response.xml************";
    system("cat /mnt/jffs2/beneficiaryOTP_Response.xml");
    qDebug() << "***************************************************";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/beneficiaryOTP_Response.xml") != 0){
        ui->leditcustrcno->clear();
        ui->leditcustmobno->clear();
        return;
    }
    if(ParsebeneficiaryOTP((char *)"/mnt/jffs2/beneficiaryOTP_Response.xml") < 0 ){
        ui->leditcustrcno->clear();
        ui->leditcustmobno->clear();
        return;
    }
    ui->stackedWidget->setCurrentIndex(48);
    return;
}

int Widget::prepare_beneficiaryOTPReq(char *FileName)
{
    qDebug()<<"Out File (" <<FileName << ") Preperation ....";

    QSqlQuery query;
    QFile file(FileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug()<< FileName << " file Open failed";
        return -1;
    }

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0){
        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }
    char szShopId[16];
    memset(szShopId, 0, 16);
    getShopId(szShopId);

    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL){
        qDebug() << "File Not Found" << endl;
        return -1;
    }
    fprintf(fp,"%s","<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ser=\"http://service.fetch.rationcard/\">");
    fprintf(fp,"%s","<soapenv:Header/>");
    fprintf(fp,"%s","<soapenv:Body>");
    fprintf(fp,"%s","<ser:beneficiaryOTPRequestMobileSeed>");
    fprintf(fp,"<fpsId>%s</fpsId>",szShopId);
    fprintf(fp,"<fpsSessionId>%s</fpsSessionId>",SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp,"<password>%s</password>",HD_PASSWORD);
    fprintf(fp,"<existingRCNumber>%s</existingRCNumber>",ui->leditcustrcno->text().toAscii().data());
    fprintf(fp,"<mobile_no>%s</mobile_no>",ui->leditcustmobno->text().toAscii().data());
    fprintf(fp,"%s","</ser:beneficiaryOTPRequestMobileSeed>");
    fprintf(fp,"%s","</soapenv:Body>");
    fprintf(fp,"%s","</soapenv:Envelope>");
    fclose(fp);


    return 0;
}

void Widget::on_leditmobupdatebk_clicked()
{
    ui->leditcustrcno->clear();
    ui->leditcustmobno->clear();
    ui->stackedWidget->setCurrentIndex(47);
}

void Widget::on_leditmobupdatesub_clicked()
{
    char member_id[24],serotp[10],refNo[64],entered_otp[10];
    char FileName[64];
    int ret=0;

    if(ui->leditmobupdateotp->text().length()!= 6){
        if(LangMode==1)
            pop_up("Please Enter 6 digit OTP");
        else if(LangMode==2)
            pop_up("कृपया 6 अंक ओटीपी दर्ज करें");
        sleep(2);
        return;
    }
    QSqlQuery query;
    memset(entered_otp,0,sizeof(entered_otp));
    strcpy(entered_otp,ui->leditmobupdateotp->text().toAscii().data());

    qDebug()<<"leditmobupdateotp"<<ui->leditmobupdateotp->text().toAscii().data();

    memset(member_id,0,sizeof(member_id));
    memset(serotp,0,sizeof(serotp));
    memset(refNo,0,sizeof(refNo));

    query.prepare("SELECT member_id, otp, refNo FROM OTPRESPONSE");
    if(!query.exec()){
        qDebug() << query.lastError();
        return;
    }else{
        while(query.next()){
            strcpy(member_id, query.value(0).toString().toAscii().data());
            strcpy(serotp, query.value(1).toString().toAscii().data());
            strcpy(refNo, query.value(2).toString().toAscii().data());
        }
    }
    qDebug()<<"serotp"<<serotp<<endl;
    qDebug()<<"entered_otp"<<entered_otp<<endl;

    if(strcmp(serotp,entered_otp)!=0){
        qDebug()<<"Enter Correct OTP Number";
        pop_up((char *)"Enter Correct OTP Number");
        sleep(2);
        return;
    }
    qDebug()<<"Entered OTP Correct"<<endl;

    popup_msg *dlgPopup2 = new popup_msg(this);

    if(LangMode == ENGLISH)
        dlgPopup2->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें.."));

    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();


    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/beneficiaryOTPVerifyRequest.xml");

    system("rm /mnt/jffs2/beneficiaryOTPVerifyRequest.xml");
    system("rm /mnt/jffs2/beneficiaryOTPVerify_Response.xml");

    ret=beneficiaryOTPVerify(FileName,member_id,refNo);

    qDebug()<<"ret form Prepare Out File : "<<ret;

    char catCmd[128];
    memset(catCmd, 0, sizeof(catCmd));
    sprintf(catCmd, "cat %s", FileName);
    system(catCmd);

    char cUrlName[256];
    memset(cUrlName, 0, sizeof(cUrlName));

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

    sleep(2);
    char cmdCurl[1024];
    memset(cmdCurl, 0, sizeof(cmdCurl));

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/beneficiaryOTPVerify_Response.xml", responseTimedOutTimeInSec.toInt());

    qDebug() << "\nCurl Cmd : " << cmdCurl;

    int sysret = system(cmdCurl);
    qDebug() << "sysret = " << sysret;

    if(SystemCurlResponse(sysret, EPDS_SERVER) < 0){
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        return;
    }
    dlgPopup2->close();
    dlgPopup2->deleteLater();

    qDebug() << "/mnt/jffs2/beneficiaryOTPVerify_Response.xml************";
    system("cat /mnt/jffs2/beneficiaryOTPVerify_Response.xml");
    qDebug() << "***************************************************";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/beneficiaryOTPVerify_Response.xml") != 0){
        ui->leditcustrcno->clear();
        ui->leditcustmobno->clear();
        return;
    }
    if(ParsebeneficiaryOTPVerify((char *)"/mnt/jffs2/beneficiaryOTPVerify_Response.xml") < 0){
        ui->leditcustrcno->clear();
        ui->leditcustmobno->clear();
    }

    ui->stackedWidget->setCurrentIndex(40);

    return;
}

int Widget::beneficiaryOTPVerify(char *FileName,char *MemerId,char *RefId)
{
    qDebug()<<"Out File (" <<FileName << ") Preperation ....";

    QFile file(FileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug()<< FileName << " file Open failed";
        return -1;
    }

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0){
        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }
    char szShopId[16];
    memset(szShopId, 0, 16);
    getShopId(szShopId);

    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL){
        qDebug() << "File Not Found" << endl;
        return -1;
    }
    fprintf(fp,"%s","<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ser=\"http://service.fetch.rationcard/\">");
    fprintf(fp,"%s","<soapenv:Header/>");
    fprintf(fp,"%s","<soapenv:Body>");
    fprintf(fp,"%s","<ser:beneficiaryOTPVerifyMobileSeed>");
    fprintf(fp,"<fpsId>%s</fpsId>",szShopId);
    fprintf(fp,"<fpsSessionId>%s</fpsSessionId>",SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp,"<password>%s</password>",HD_PASSWORD);
    fprintf(fp,"<existingRCNumber>%s</existingRCNumber>",ui->leditcustrcno->text().toAscii().data());
    fprintf(fp,"<member_id>%s</member_id>",MemerId);
    fprintf(fp,"<mobile_no>%s</mobile_no>",ui->leditcustmobno->text().toAscii().data());
    fprintf(fp,"<otp>%s</otp>",ui->leditmobupdateotp->text().toAscii().data());
    fprintf(fp,"<otpRef>%s</otpRef>",RefId);
    fprintf(fp,"%s","</ser:beneficiaryOTPVerifyMobileSeed>");
    fprintf(fp,"%s","</soapenv:Body>");
    fprintf(fp,"%s","</soapenv:Envelope>");
    fclose(fp);

    return 0;
}


int Widget::ParsebeneficiaryOTP(char *filename)
{
    QSqlQuery query;

    QString Qmember_id,Qotp,QrefNo,QrespCode;

    query.exec("DELETE FROM OTPRESPONSE");

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In ParsebeneficiaryOTP Response, Error Reading File";
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
            if(xml.name() == "member_id")
            {
                xml.readNext();
                Qmember_id = xml.text().toString();
                qDebug() << "Qmember_id  : " << Qmember_id ;
            }
            if(xml.name() == "otp")
            {
                xml.readNext();
                Qotp= xml.text().toString();
                //qDebug() << "Qotp : " << Qotp;
            }
            if(xml.name() == "refNo")
            {
                xml.readNext();
                QrefNo= xml.text().toString();
                qDebug() << "QrefNo : " << QrefNo;
            }
            if(xml.name() == "respCode")
            {
                xml.readNext();
                QrespCode = xml.text().toString();
                qDebug() << "QrespCode : " << QrespCode;

                query.clear();

                query.prepare("INSERT INTO OTPRESPONSE(member_id, otp, refNo)VALUES(:member_id, :otp, :refNo)");
                query.bindValue(":member_id", Qmember_id);
                query.bindValue(":otp", Qotp);
                query.bindValue(":refNo", QrefNo);
                if(!query.exec())
                    qDebug() << query.lastError();
                else
                    qDebug()<<"OTPRESPONSE Table Inserted"<<endl;
            }
        }
    }

    file.close();
    return 0;
}


int Widget::ParsebeneficiaryOTPVerify(char *filename)
{
    QString QrespCode, QrespMessage;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In ParsebeneficiaryOTP Response, Error Reading File";
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
            if(xml.name() == "respCode")
            {
                xml.readNext();
                QrespCode= xml.text().toString();
            }
            if(xml.name() == "respMessage")
            {
                xml.readNext();
                QrespMessage= xml.text().toString();
            }

        }
    }
    file.close();
    char dispbuff[625]="";
    memset(dispbuff,0,sizeof(dispbuff));

    if(QrespCode =="00")
    {
        sprintf(dispbuff, "%s(%s)", QrespMessage.toAscii().data(), QrespCode.toAscii().data());
        miscObj.DisplayWideMsgBox((char *)dispbuff);

        ui->leditmobupdateotp->clear();
        return 0;
    }
    else if(QrespCode != "00")
    {
        sprintf(dispbuff, "%s(%s)", QrespMessage.toAscii().data(), QrespCode.toAscii().data());
        miscObj.DisplayWideMsgBox((char *)dispbuff);

        ui->leditmobupdateotp->clear();

        return -1;
    }

    return 0;
}

int Widget::prepare_beneficiaryOTPAUTHReq(char *FileName)
{
    char MembID[16]="";
    memset(MembID,0,sizeof(MembID));
    qDebug()<<"Out File (" <<FileName << ") Preperation ....";

    getMemberID(MemberUID,MembID);

    QFile file(FileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug()<< FileName << " file Open failed";
        return -1;
    }

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0){
        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }

    char szShopId[16];
    memset(szShopId, 0, 16);
    getShopId(szShopId);

    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL){
        qDebug() << "File Not Found" << endl;
        return -1;
    }
    fprintf(fp,"%s","<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ser=\"http://service.fetch.rationcard/\">");
    fprintf(fp,"%s","<soapenv:Header/>");
    fprintf(fp,"%s","<soapenv:Body>");
    fprintf(fp,"%s","<ser:beneficiaryOTPRequestMobileAuth>");
    fprintf(fp,"<fpsId>%s</fpsId>",szShopId);
    fprintf(fp,"<fpsSessionId>%s</fpsSessionId>",SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp,"<password>%s</password>",HD_PASSWORD);
    fprintf(fp,"<existingRCNumber>%s</existingRCNumber>",ui->lblRCNO_Val->text().toAscii().data());
    fprintf(fp,"<mobile_no>%s</mobile_no>",otp_mobile);
    fprintf(fp,"<member_id>%s</member_id>",MembID);
    fprintf(fp,"<uid>%s</uid>",MemberUID);
    fprintf(fp,"%s","</ser:beneficiaryOTPRequestMobileAuth>");
    fprintf(fp,"%s","</soapenv:Body>");
    fprintf(fp,"%s","</soapenv:Envelope>");
    fclose(fp);
    return 0;
}

int Widget::getHeadMobileno(char *HeadMobNo)
{
    QSqlQuery query;
    query.prepare("select headmobileno from CustomerMaster");
    if(!query.exec()){
        qDebug() << query.lastError();
        return -1;
    }else{
        while(query.next()){
            strcpy(HeadMobNo, query.value(0).toString().toAscii().data());
        }
    }
    return 0;
}

int Widget::getMemberID(char *MemberUID,char *MembID)
{
    QSqlQuery query;
    query.prepare("select MemberId from CustomerFamily where MemberUID = ?");
    query.addBindValue(MemberUID);

    if(!query.exec()){
        qDebug() << query.lastError();
        return -1;
    }else{
        while(query.next()){
            strcpy(MembID, query.value(0).toString().toAscii().data());
        }
    }
    return 0;
}

int Widget::ParseMobileOTP(char *filename)
{
    QSqlQuery query;
    QString Qotp,QrefNo,QrespCode;
    query.exec("DELETE FROM MOBILEOTPRESPONSE");

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "In MOBILEOTPRESPONSE Response, Error Reading File";
        return -1;
    }

    if(file.size() == 0)
        return -1;

    QXmlStreamReader xml(&file);
    while(!xml.atEnd() && !xml.hasError()){
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartDocument)
            continue;
        if(token == QXmlStreamReader::StartElement){
            if(xml.name() == "otp"){
                xml.readNext();
                Qotp= xml.text().toString();
                //qDebug() << "Qotp : " << Qotp;
            }
            if(xml.name() == "refNo"){
                xml.readNext();
                QrefNo= xml.text().toString();
                qDebug() << "QrefNo : " << QrefNo;

                memset(gauth_transaction_code,0,sizeof(gauth_transaction_code));
                strcpy(gauth_transaction_code,QrefNo.toAscii().data());
            }
            if(xml.name() =="wOtpTimeOut"){
                xml.readNext();
                OtppopTimeOut= atoi(xml.text().toString().toAscii().data())*1000;

            }
            if(xml.name() == "respCode"){
                xml.readNext();
                QrespCode = xml.text().toString();
                qDebug() << "QrespCode : " << QrespCode;

                query.clear();
                query.prepare("INSERT INTO MOBILEOTPRESPONSE(mobotp, mobrefNo)VALUES(:mobotp, :mobrefNo)");
                query.bindValue(":mobotp", Qotp);
                query.bindValue(":mobrefNo", QrefNo);
                if(!query.exec())
                    qDebug() << query.lastError();
                else
                    qDebug()<<"MOBILEOTPRESPONSE Table Inserted"<<endl;
            }
        }
    }
    file.close();
    return 0;
}

int Widget::Mobile_OTP_Verify()
{
    char refNo[64];
    char FileName[64];
    int ret=0;

    QSqlQuery query;
    memset(refNo,0,sizeof(refNo));
    query.prepare("SELECT mobrefNo FROM MOBILEOTPRESPONSE");
    if(!query.exec()){
        qDebug() << query.lastError();
        return -1;
    }else{
        while(query.next()){
            strcpy(refNo, query.value(0).toString().toAscii().data());
        }
    }
    popup_msg *dlgPopup2 = new popup_msg(this);

    if(LangMode == ENGLISH)
        dlgPopup2->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..."));

    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();


    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/beneficiaryOTPVerifyMobileAuth_Request.xml");

    system("rm /mnt/jffs2/beneficiaryOTPVerifyMobileAuth_Request.xml");
    system("rm /mnt/jffs2/beneficiaryOTPVerifyMobileAuth_Response.xml");

    ret=beneficiaryOTPVerifyMobileAuth(FileName,refNo);
    if(ret < 0)
        return -1;
    qDebug()<<"ret form Prepare Out File : "<<ret;

    char catCmd[128];
    memset(catCmd, 0, sizeof(catCmd));
    sprintf(catCmd, "cat %s", FileName);
    system(catCmd);

    char cUrlName[256];
    memset(cUrlName, 0, sizeof(cUrlName));

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
    sleep(2);
    char cmdCurl[1024];
    memset(cmdCurl, 0, sizeof(cmdCurl));

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/beneficiaryOTPVerifyMobileAuth_Response.xml", responseTimedOutTimeInSec.toInt());
    qDebug() << "\nCurl Cmd : " << cmdCurl;

    int sysret = system(cmdCurl);
    qDebug() << "sysret = " << sysret;

    if(SystemCurlResponse(sysret, EPDS_SERVER) < 0){
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        return -1;
    }
    dlgPopup2->close();
    dlgPopup2->deleteLater();

    qDebug() << "/mnt/jffs2/beneficiaryOTPVerifyMobileAuth_Response.xml************";
    system("cat /mnt/jffs2/beneficiaryOTPVerifyMobileAuth_Response.xml");
    qDebug() << "***************************************************";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/beneficiaryOTPVerifyMobileAuth_Response.xml") != 0)
        return -1;
    if(ParsebeneficiaryOTPVerify((char *)"/mnt/jffs2/beneficiaryOTPVerifyMobileAuth_Response.xml") < 0)
        return -1;

    return 0;
}

int Widget::beneficiaryOTPVerifyMobileAuth(char *FileName,char *RefId)
{
    qDebug()<<"Out File (" <<FileName << ") Preperation ....";
    char MembID[16]="";
    memset(MembID,0,sizeof(MembID));
    getMemberID(MemberUID,MembID);
    QFile file(FileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug()<< FileName << " file Open failed";
        return -1;
    }
    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0){
        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }

    char szShopId[16];
    memset(szShopId, 0, 16);
    getShopId(szShopId);

    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL){
        qDebug() << "File Not Found" << endl;
        return -1;
    }
    fprintf(fp,"%s","<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ser=\"http://service.fetch.rationcard/\">");
    fprintf(fp,"%s","<soapenv:Header/>");
    fprintf(fp,"%s","<soapenv:Body>");
    fprintf(fp,"%s","<ser:beneficiaryOTPVerifyMobileAuth>");
    fprintf(fp,"<fpsId>%s</fpsId>",szShopId);
    fprintf(fp,"<fpsSessionId>%s</fpsSessionId>",SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp,"<password>%s</password>",HD_PASSWORD);
    fprintf(fp,"<existingRCNumber>%s</existingRCNumber>",Rcno.toAscii().data());
    fprintf(fp,"<member_id>%s</member_id>",MembID);
    fprintf(fp,"<mobile_no>%s</mobile_no>",otp_mobile);
    fprintf(fp,"<otp>%s</otp>",mobile_otp);
    fprintf(fp,"<otpRef>%s</otpRef>",RefId);
    fprintf(fp, "<uid>%s</uid>",MemberUID);
    fprintf(fp,"%s","</ser:beneficiaryOTPVerifyMobileAuth>");
    fprintf(fp,"%s","</soapenv:Body>");
    fprintf(fp,"%s","</soapenv:Envelope>");
    fclose(fp);

    return 0;
}

void Widget::on_btnMobile_Seeding_clicked()
{

    ui->leditcustrcno->clear();
    ui->leditcustmobno->clear();
    ui->leditmobupdateotp->clear();
    ui->leditcustrcno->setFocus();

    ui->stackedWidget->setCurrentIndex(47);
}

int Widget::Mobile_OTP_Calling()
{
    getcomTxn_type="O";
    qDebug()<<"mobile OTP Doing"<<endl;
    char Otp[12]="",FileName[625]="";
    memset(Otp,0,sizeof(Otp));
    memset(FileName,0,sizeof(FileName));

    memset(FileName, 0, sizeof(FileName));
    strcpy(FileName, "/mnt/jffs2/beneficiary-OTPRequestMobileAuth-Request.xml");

    system("rm /mnt/jffs2/beneficiary-OTPRequestMobileAuth-Request.xml");
    system("rm /mnt/jffs2/beneficiary-OTPRequestMobileAuth-Response.xml");

    if(prepare_beneficiaryOTPAUTHReq(FileName) < 0)
        return -1;

    popup_msg *dlgPopup2 = new popup_msg(this);

    if(LangMode == ENGLISH)
        dlgPopup2->ui->lblPopup->setText("Requesting OTP...\n Please Wait ...");
    else if(LangMode == 2)
        dlgPopup2->ui->lblPopup->setText(trUtf8("OTP केलिए रिक्वेस्ट .\nकृपया प्रतीक्षा करें.."));

    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    char catCmd[128];
    memset(catCmd, 0, sizeof(catCmd));
    sprintf(catCmd, "cat %s", FileName);
    system(catCmd);

    char cUrlName[256];
    memset(cUrlName, 0, sizeof(cUrlName));

    QSqlQuery query;
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

    sleep(2);
    char cmdCurl[1024];
    memset(cmdCurl, 0, sizeof(cmdCurl));

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", FileName, cUrlName, "/mnt/jffs2/beneficiary-OTPRequestMobileAuth-Response.xml", responseTimedOutTimeInSec.toInt());

    qDebug() << "\nCurl Cmd : " << cmdCurl;
    int sysret = system(cmdCurl);
    qDebug() << "sysret = " << sysret;

    if(SystemCurlResponse(sysret, EPDS_SERVER) < 0){
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        return -1;
    }
    dlgPopup2->close();
    dlgPopup2->deleteLater();

    qDebug() << "/mnt/jffs2/beneficiary-OTPRequestMobileAuth-Response.xml************";
    system("cat /mnt/jffs2/beneficiary-OTPRequestMobileAuth-Response.xml");
    qDebug() << "***************************************************";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/beneficiary-OTPRequestMobileAuth-Response.xml") != 0){
        return -1;
    }
    if(ParseMobileOTP("/mnt/jffs2/beneficiary-OTPRequestMobileAuth-Response.xml") < 0)
        return -1;

    memset(mobile_otp,0,sizeof(mobile_otp));
    if(OTP_Mobile_popup_New(Otp)==0){
        char sermobotp[16]="",sermobrefNo[64]="";
        memset(sermobotp,0,sizeof(sermobotp));
        memset(sermobrefNo,0,sizeof(sermobrefNo));
        strcpy(mobile_otp,Otp);
        if(Mobile_OTP_Verify()< 0)
            return -1;
    }
    else
        return -1;
    return 0;
}

int Widget::EYC_Manual_Calling()
{
    //    getcomTxn_type="M";
    getcomTxn_type="E";
    qDebug()<<"No aadhaar -Manual authentication-doing ekyc";
    char EkycUid[16]="";
    int ekycflag=0,ret=0;
    memset(EkycUid,0,sizeof(EkycUid));
    while(1){
        ekycflag=0;

        ret = Ekyc_uid_popup(EkycUid);
        qDebug() << "Ekyc_uid_popup Return value::" <<ret;

        if(ret == -1)
            return -1;

        if(ret == -2){
            continue;
        }
        else if(ret == 0){
            ekycflag=1;
            memset(memekycuid,0,sizeof(memekycuid));
            strcpy(memekycuid,EkycUid);
            break;
        }
        else if(ret == -1 && (strcmp(BenAuthMode_Manual,"N")== 0)){
            qDebug()<<"Manual flag"<<BenAuthMode_Manual;
            return -1;
        }
        else if(ret == -1 && (strcmp(BenAuthMode_Manual,"Y")== 0)){
            qDebug()<<"Manual flag"<<BenAuthMode_Manual;
            ekycflag=0;
            system("rm -rf /mnt/jffs2/PWDACKrequest.xml");
            if(PwdackRequest_Response("/mnt/jffs2/PWDACKrequest.xml")< 0)
                return -1;
            break;
        }
    }
    if(ekycflag == 1){
        EKYC_TEST_FLAG = 1;

        //        Member_EKYC_Status(1);
        int RetSts = Member_EKYC_Status(1);
        if(RetSts == -1)
            return -1;

        if((Qtxnflow == "B")||strcmp(BenAuthMode_Manual,"N")== 0){
            qDebug()<<"Qtxnflow="<<Qtxnflow<<"BenAuthMode_Manual"<<BenAuthMode_Manual<<endl;
            return -1;
        }

        if(Qtxnflow == "D")
        {
            //            getcomTxn_type = "D";
            DealerTxnFlag = 1; // Benificiary Dealer Txn
            DisplayDealerDetails();

            ui->ConsentFormcheckBox_3->setChecked(true);
            ui->stackedWidget->setCurrentIndex(20);
            //            return;
        }

        if(Qtxnflow == "M")
        {
            getcomTxn_type="M";
            DisplayCommDetails();
        }

        EKYC_TEST_FLAG = 0;
        //commented by santosh
        //        system("rm -rf /mnt/jffs2/PWDACKrequest.xml");
        //        if(PwdackRequest_Response("/mnt/jffs2/PWDACKrequest.xml") < 0)
        //            return -1;

    }
    return 0;
}

void Widget::Scan_barcodecam()
{
    qDebug()<<"scan barcode camera function called";

    char nodeName[30]="";
    char Excmd[50]="";
    char Incmd[50]="";
    int ret=0;

    int Camret = gl11_cam_control(1);
    qDebug()<<"Cam Pwron ret:"<<Camret;
    sleep(1);

    int bklret = gl11_lcdbklt_on();
    qDebug()<<"gl11 backlight on ret:"<<bklret;
    gl11_lcdbklt_time_duration(90);

    for(ret=0; ret<10; ret++){
        qDebug()<<ret<<"Identifying:----->";
        sprintf(Incmd,"udevadm info --name=/dev/video%d --attribute-walk | grep 37d0",ret);
        int Inret = system(Incmd);
        qDebug()<<"For UDEVADM system ret value is"<<Inret;

        sprintf(Incmd,"udevadm info --name=/dev/video%d --attribute-walk | grep f37d",ret);
        int Inret1 = system(Incmd);
        qDebug()<<"For UDEVADM system ret value is"<<Inret1;

        if( Inret == 0 || Inret1 == 0 ){
            gl11_internal_camflash(1);

            qDebug()<<"Internal Cam NODE IS Video "<<ret;
            qDebug()<<"Internal Camera";
            //			sprintf(nodeName,"zbarcam --prescale=500x400 /dev/video%d -Sdisable -Sqrcode.enable",ret);
            sprintf(nodeName,"zbarcam --prescale=500x400 /dev/video%d ",ret);
            system(nodeName);

            gl11_internal_camflash(0);
            break;
        }
        if(ret == 9)
            miscObj.DisplayMsgBox((char*)"Camera Not present");
    }

    QFile DFile("/root/decoded");

    if(!DFile.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->ldtRationCard->setText("FAILED");
    else{
        QTextStream DText(&DFile);
        QString barcodedata=DText.readAll();
        qDebug()<<"String RC no barcode :::"<<barcodedata;
        ui->ldtRationCard->setText(barcodedata);
    }
    int Camret1 = gl11_cam_control(0);
    qDebug()<<"Cam Pwroff ret:"<<Camret1;
}

void Widget::on_radioButton_scanbarcode_clicked()
{
    qDebug()<<"scan barcode clicked";
    ui->ldtRationCard->clear();
    ui->ldtRationCard->setReadOnly(1);
    if(ui->radioButton_scanbarcode->isChecked()){
        if(LangMode == 1)
            ui->lblRationCard->setText("Card No.");
        else
            ui->lblRationCard->setText(QString::fromUtf8("कार्ड नंबर"));
        cidType = "R";
        // function call for scanning
        Scan_barcodecam();
        if(ui->ldtRationCard->text().length() == 0){
            if(LangMode == 1){
                miscObj.DisplayWideMsgBox((char *)"Please Scan RC Number");
            }else
                miscObj.tel_DisplayWideMsgBox((char *)"कृपया आर.सी. नंबर स्कैन करे .");
            ui->ldtRationCard->clear();
            ui->ldtRationCard->setReadOnly(0);
            return;
        }
    }
}


int device_search()
{
    FILE *fp = NULL;
    struct stat sb;
    char *buf = NULL;
    const char *ptr;
    int ret = 0;

    if(!(system("cat /proc/bus/usb/devices | grep  \"f001\" > /mnt/jffs2/device_type")) || !(system("cat /proc/bus/usb/devices | grep \"3399\"  > /mnt/jffs2/device_type")))

        fp=fopen("/mnt/jffs2/device_type","r");
    if(fp==NULL){
        perror("fopen");
        return -1;
    }
    stat("/mnt/jffs2/device_type",&sb);
    buf=(char *)calloc(1,sb.st_size);
    if(buf==NULL){
        perror("calloc:");
        return -1;
    }
    fread(buf,1,sb.st_size,fp);
    ptr=strstr(buf,"3399");
    if(ptr!=NULL){
        ret = BIOMATIQUES;
        goto cleanup;
    }
    ptr=strstr(buf,"f001");
    if(ptr!=NULL){
        ret = IRITECH;
        goto cleanup;
    }else{
        ret = NO_DEVICE;
        goto cleanup;
    }
cleanup:
    free(buf);
    fclose(fp);
    return ret;
}

int Widget::Calling_Iritech_Iris(int iEyeSelect)
{
    int RetVal;

    qDebug() << "In Calling_Iritech_Iris()";

    memset(&iriTech_pid_Info,0,sizeof(iriTech_pData));
    memset(&eye_Capture1,0,sizeof(iriTech_eye_capture));
    memset(&iriTech_rdStatus1,0,sizeof(iriTech_rd_status));
    memset(iriTech_pid_Info.pid,0,15000);

    RetVal=Iritech_rd_service_discovery(&iriTech_rdStatus1);
    qDebug()<<"Iritech_rd_service_discovery"<<RetVal<<endl<<iriTech_rdStatus1.status<<endl;

    if(RetVal != 0)
        return -2;

    if(strcmp(iriTech_rdStatus1.status,"NOTREADY")==0)
        return -2;

    eye_Capture1.eye_Count=1;
    sprintf(eye_Capture1.pidVer,"%s","2.0");
    sprintf(eye_Capture1.env,"%s","P");
    eye_Capture1.Timeout = 15000;

    RetVal= Iritech_rd_capture(&iriTech_pid_Info,&eye_Capture1);
    qDebug()<<"Iritech_rd_capture RetVal="<<RetVal;

    if(atoi(iriTech_pid_Info.err_code) !=0)
    {
        qDebug()<<"Error="<<iriTech_pid_Info.err_info;
        CMisc misc;
        char errmessag[120]="";
        memset(errmessag,0,sizeof(errmessag));
        sprintf(errmessag,"%s-ERROR-%s",iriTech_pid_Info.err_info,iriTech_pid_Info.err_code);

        misc.DisplayWideMsgBox(errmessag);

        return -1;
    }
    //    if(atoi(iriTech_pid_Info.err_code) !=0)
    //        qDebug()<<"Error="<<iriTech_pid_Info.err_info;
    if(RetVal == -1 || RetVal == 0){
        if(iriTech_pid_Info.pid!= NULL){
            printf("\nerror code = %s\n",iriTech_pid_Info.err_code);
            printf("\nerror info = %s\n",iriTech_pid_Info.err_info);
            printf("\nnmPoints = %s\n",iriTech_pid_Info.nmPoints);
            printf("\nqScore = %s\n",iriTech_pid_Info.qScore);
            printf("\nci = %s\n",iriTech_pid_Info.ci);
            printf("\nSkey = %s\n",iriTech_pid_Info.Skey);
            printf("\nHmac = %s\n",iriTech_pid_Info.Hmac);
            printf("\npid type = %s\n",iriTech_pid_Info.type);
            printf("\npid = %s-----",iriTech_pid_Info.pid);
            printf("\ndc = %s\n",iriTech_pid_Info.iriTech_dev_data.dc);
            printf("\nmi = %s\n",iriTech_pid_Info.iriTech_dev_data.mi);
            printf("\nmc = %s\n",iriTech_pid_Info.iriTech_dev_data.mc);
            printf("\ndpId = %s\n",iriTech_pid_Info.iriTech_dev_data.dpId);
            printf("rdsId = %s\n",iriTech_pid_Info.iriTech_dev_data.rdsId);
            printf("\nrdsVer = %s\n",iriTech_pid_Info.iriTech_dev_data.rdsVer);
            printf("\n serialnumber = %s\n",iriTech_pid_Info.serialnumber);
            printf("\n#***********debug********************#\n");
        }
    }
    return 0;
}

int Widget::EKYC_Calling(int ekycflagcurrent)
{
    qDebug()<<"Doing ekyc";
    char EkycUid[16]="";
    int ekycflag=0,ret=0;
    memset(EkycUid,0,sizeof(EkycUid));

    while(1){
        ekycflag=0;
        ret = Ekyc_uid_popup(EkycUid);
        if(ret == -1)
            return -1;
        if(ret==-2){
            continue;
        }else if(ret == -1 && (strcmp(BenAuthMode_Manual,"N")== 0)){
            qDebug()<<"Manual flag"<<BenAuthMode_Manual;
            return -1;
        }else if(ret == -1 && (strcmp(BenAuthMode_Manual,"Y")== 0)){
            qDebug()<<"Manual flag"<<BenAuthMode_Manual;
            ekycflag=0;
            break;
        }else if(ret == 0){
            ekycflag=1;
            memset(memekycuid,0,sizeof(memekycuid));
            strcpy(memekycuid,EkycUid);

            //            Member_EKYC_Status(2);
            if(Member_EKYC_Status(2) < 0)
                return -1;

            if((Qtxnflow == "B")||strcmp(BenAuthMode_Manual,"N")== 0){
                qDebug()<<"Qtxnflow="<<Qtxnflow<<"BenAuthMode_Manual"<<BenAuthMode_Manual<<endl;
                return -1;
            }
            break;
        }
    }
    return 0;
}

int Widget::ParsePrintLastRecptResponse_balance(char *filename)
{
    qDebug()<<"ParsePrintLastRecptResponse_balance"<<endl;

    QSqlQuery query;
    query.exec("DELETE FROM PrintLastResp_balance");

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error Reading PrintLastResp File";
        return -1;
    }

    QXmlStreamReader xml_bal(&file);
    QString bQtycommodityNameEn,bQtybalacne,bQtycommodityNameLl;

    while(!xml_bal.atEnd() && !xml_bal.hasError())
    {
        QXmlStreamReader::TokenType token = xml_bal.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;
        if(token == QXmlStreamReader::StartElement){
            if(xml_bal.name() == "balacne"){
                xml_bal.readNext();
                bQtybalacne = xml_bal.text().toString();
                qDebug() << "balacne : " <<bQtybalacne;
            }
            if(xml_bal.name() == "commodityNameEn"){
                xml_bal.readNext();
                bQtycommodityNameEn= xml_bal.text().toString();
                qDebug() << "commodityNameEn : " <<bQtycommodityNameEn;
            }
            if(xml_bal.name() == "commodityNameLl"){
                xml_bal.readNext();
                bQtycommodityNameLl = xml_bal.text().toString();
                qDebug() << "commodityNameLl : " <<bQtycommodityNameLl;

                query.clear();
                query.prepare("INSERT INTO PrintLastResp_balance(bal_Qty,comm_Name,comm_Name_ll) VALUES(:bal_Qty,:comm_Name,:comm_Name_ll)");

                query.bindValue(":bal_Qty",bQtybalacne);
                query.bindValue(":comm_Name",bQtycommodityNameEn);
                query.bindValue(":comm_Name_ll",bQtycommodityNameLl);
                if(!query.exec()){
                    qDebug() << query.lastError();
                    return -1;
                }
                else
                    qDebug() << "Last Print Receipt_balance Inserted Successfully";
            }
        }
    }
    return 0;
}

void Widget::on_MinimizeButton_clicked()
{
    system("matchbox-remote -desktop");
    return;
}

void Widget::on_MinimizeButton_2_clicked()
{
    system("matchbox-remote -desktop");
    return;
}

void Widget::on_btnImpds_clicked()
{
    //    Testing
    //    QString impdsURL;
    //    impdsURL = "http://mnt/jffs2/impds.ini";
    //    QFile file("/mnt/jffs2/impds.ini");
    //    file.remove();
    //    if(file.open(QIODevice::ReadWrite | QIODevice::Text))
    //    {
    //        QTextStream out(&file);
    //        out << "[General]\n";
    //        out << "URL=" << impdsURL.trimmed() << "\n";
    //        out << "DataBase=/mnt/jffs2/db_commonpds_2_3.db";
    //        file.close();
    //    }
    //    Testing

    if(DealerLoginBy == 2)
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please login in online mode");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया ऑनलाइन मोड में लॉगिन करें ..");
        return;
    }

    popup_msg *dlgPopup2 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup2->ui->lblPopup->setText("Please Wait...");
    else
        dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..."));
    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    char szShopId[16];
    memset(szShopId, 0, sizeof(szShopId));
    getShopId(szShopId);

    char IMPDS_buff[120];
    memset(IMPDS_buff,0,sizeof(IMPDS_buff));
    //    sprintf(IMPDS_buff,"/usr/bin/impds %02d %s %s",__HARYANA__,szShopId,Qdist_code_impds.toAscii().data());
    sprintf(IMPDS_buff,"/usr/bin/impds %02d %s %s",stateCode.toInt(),szShopId,Qdist_code_impds.toAscii().data());

    system(IMPDS_buff);

    dlgPopup2->close();
    dlgPopup2->deleteLater();
}

void Widget::on_tableWidget_InspectorVerificationn_cellEntered(int row, int column)
{
    qDebug() <<"cell entered";
    qDebug() << "Roww::" << row << " & Column::" << column;
}

void Widget::on_tblCommIssue_cellClicked(int row, int column)
{
    //    if((partialOnlineOfflineStatus.toUpper() == "Y")||(DealerLoginBy == 2))
    //    {
    //        qDebug() << "Offline on_tblCommIssue_cellClicked " << row <<"\t"<< column;
    //    }
    //    else
    //    {
    //        qDebug() << "Online on_tblCommIssue_cellClicked " << row <<"\t"<< column;
    //        if(ui->tblCommIssue->item(row,8)->text() == "Y")
    //        {
    //            ui->btnGetWt->show();
    //            ui->comboCommMode->show();
    //        }
    //        else
    //        {
    //            ui->btnGetWt->hide();
    //            ui->comboCommMode->hide();
    //        }
    //    }

    if((PartialTxn != 1) && (DealerLoginBy != 2))
    {
        qDebug() << "Online on_tblCommIssue_cellClicked " << row <<"\t"<< column;
        if(ui->tblCommIssue->item(row,8)->text() == "Y")
        {
            ui->btnGetWt->show();
            ui->comboCommMode->show();
        }
        else
        {
            ui->btnGetWt->hide();
            ui->comboCommMode->hide();
        }
    }
}

void Widget::on_UploadDataButton_clicked()
{
    QSqlQuery query;
    GPRS objGprs;
    int RetVal;
    char TmpStr[1024];

    if(DealerLoginBy == 2)
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please login in online mode");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया ऑनलाइन मोड में लॉगिन करें ..");
        return;
    }

    if(objGprs.Check_Network_Available() < 0)
        return;

    query.clear();
    query.prepare("select count() from OfflineStockReceive where RecvdUploadSts='N'");

    if(!query.exec())
    {
        qDebug()<<"1.Txn not uploaded on_UploadDataButton_clicked ::"<<query.lastError();
    }
    if(query.next())
    {
        if(query.value(0).toString().toInt() > 0)
        {
            qDebug() << "Uploading Pending CB from OfflineStockReceive";
            popup_msg *dlgPopup2 = new popup_msg(this);
            dlgPopup2->ui->lblPopup->setText("Uploading Pending CB \n Please Wait ....");
            dlgPopup2->show();
            QCoreApplication::processEvents();
            QCoreApplication::processEvents();
            RetVal = ManualReceiveGdsUpload();
            dlgPopup2->close();
            dlgPopup2->deleteLater();
            if(RetVal != 0)
            {
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox((char *)"Offline Data Uploading Failed\nPlease Try again ..");
                else
                    miscObj.tel_DisplayWideMsgBox((char *)"ऑफ़लाइन डेटा अपलोडिंग विफल हुई \n कृपया पुनः प्रयास करें ..");
                return;
            }
        }
    }

    QString AllotmentMonth,AllotmentYear;
    query.clear();
    query.prepare("SELECT AllotMonth, AllotYear FROM PartialOnlineData");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            AllotmentMonth = query.value(0).toString();
            AllotmentYear  = query.value(1).toString();
        }
    }

    query.clear();
    query.prepare("select count() from BenfiaryTxn where TxnUploadSts = 'N'");

    if(!query.exec())
    {
        qDebug()<<"2.Txn not uploaded on_UploadDataButton_clicked ::"<<query.lastError();
    }
    if(query.next())
    {
        if(query.value(0).toString().toInt() > 0)
        {
            popup_msg *dlgPopup2 = new popup_msg(this);
            dlgPopup2->ui->lblPopup->setText("Uploading Pending Records \n Please Wait ....");
            dlgPopup2->show();
            QCoreApplication::processEvents();
            QCoreApplication::processEvents();
            RetVal = ManualServerUploadPartialTxns();
            dlgPopup2->close();
            dlgPopup2->deleteLater();
            if(RetVal == 0) //no txn exists
            {
                int ret = UpdateTransStatus();  //added ack for upload txns
                if(ret == 0)
                {
                    if(LangMode == 1)
                    {
                        memset(TmpStr,0x00,sizeof(TmpStr));

                        sprintf(TmpStr,"All offline txn records were uploaded to server\nAlloted Month&Year: %s-%s",AllotmentMonth.toStdString().c_str(),AllotmentYear.toStdString().c_str());
                        miscObj.DisplayWideMsgBox((char *)TmpStr);
                    }
                    else
                    {
                        memset(TmpStr,0x00,sizeof(TmpStr));
//                        miscObj.tel_DisplayWideMsgBox((char *)"सभी ऑफ़लाइन txn रिकॉर्ड सर्वर पर अपलोड किए गए थे");
                        sprintf(TmpStr,"सभी ऑफ़लाइन txn रिकॉर्ड सर्वर पर अपलोड किए गए थे\nAlloted Month&Year: %s-%s",AllotmentMonth.toStdString().c_str(),AllotmentYear.toStdString().c_str());
                        miscObj.DisplayWideMsgBox((char *)TmpStr);
                    }
                    return;
                }
                else
                {
                    if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Acknowledgement sending Failed\nPlease Try again ..");
                    else
                        miscObj.tel_DisplayWideMsgBox((char *)"ऑफ़लाइन डेटा अपलोडिंग विफल हुई \n कृपया पुनः प्रयास करें ..");
                    return;
                }
            }
            else // pending error txns exists
            {
                int ret = UpdateTransStatus();
                if(ret == 0)
                {
                    if(LangMode == 1)
                    {
//                        miscObj.DisplayWideMsgBox((char *)"All offline txn records were uploaded to server");
                        memset(TmpStr,0x00,sizeof(TmpStr));

                        sprintf(TmpStr,"All offline txn records were uploaded to server\nAlloted Month&Year: %s-%s",AllotmentMonth.toStdString().c_str(),AllotmentYear.toStdString().c_str());
                        miscObj.DisplayWideMsgBox((char *)TmpStr);
                    }
                    else
                    {
                        memset(TmpStr,0x00,sizeof(TmpStr));
//                        miscObj.tel_DisplayWideMsgBox((char *)"सभी ऑफ़लाइन txn रिकॉर्ड सर्वर पर अपलोड किए गए थे");
                        sprintf(TmpStr,"सभी ऑफ़लाइन txn रिकॉर्ड सर्वर पर अपलोड किए गए थे\nAlloted Month&Year: %s-%s",AllotmentMonth.toStdString().c_str(),AllotmentYear.toStdString().c_str());
                        miscObj.DisplayWideMsgBox((char *)TmpStr);
                    }
                    return;
                }
                else
                {
                    if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Offline Data Uploading Failed\nPlease Try again ..");
                    else
                        miscObj.tel_DisplayWideMsgBox((char *)"ऑफ़लाइन डेटा अपलोडिंग विफल हुई \n कृपया पुनः प्रयास करें ..");
                    return;
                }
            }
        }
        else
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Pending data unavailable");
            else
                miscObj.tel_DisplayWideMsgBox((char *)"लंबित डेटा अनुपलब्ध है");
            return;
        }
    }
}

//void Widget::on_BtnBenIRIS_clicked()
//{
//    qDebug()<<"Iris pressed"<<endl;
//    BenAuthSUCCESS = 0;
//    int iScannerStatus=device_search();
//    qDebug()<<"iScannerStatus"<<iScannerStatus;

//    if(iScannerStatus == -1)
//    {
//        if(LangMode==1)
//            pop_up((char *)"IRIS Scanner Not connected");
//        else if(LangMode==2)
//            pop_up((char *)"आईरिस स्कैनर जुड़ा नहीं है");

//        sleep(3);
//        return;
//    }

//    int row = 0;
//    char uid[36]="";
//    memset(uid,0,sizeof(uid));

//    row = ui->twdgtEntitlement->currentRow();
//    strcpy(uid,ui->twdgtEntitlement->item(row, 1)->text().toAscii().data());

//    ui->BtnBenIRIS->setDisabled(1);
//    Finger_Iris_Selection = 1;

//    on_pushButton_2_clicked();

//    Finger_Iris_Selection = 0;
//    ui->BtnBenIRIS->setEnabled(1);
//}

void Widget::on_BtnBenIRIS_clicked()
{
    qDebug()<<"Iris pressed"<<endl;
    BenAuthSUCCESS = 0;
    int iScannerStatus=device_search();
    qDebug()<<"iScannerStatus"<<iScannerStatus;

    if(iScannerStatus == -1)
    {
        if(LangMode==1)
            pop_up((char *)"IRIS Scanner Not connected");
        else if(LangMode==2)
            pop_up((char *)"आईरिस स्कैनर जुड़ा नहीं है");

        sleep(3);
        return;
    }

    int row = 0;
    char uid[36]="";
    memset(uid,0,sizeof(uid));

    row = ui->twdgtEntitlement->currentRow();
    strcpy(uid,ui->twdgtEntitlement->item(row, 1)->text().toAscii().data());

    ui->BtnBenIRIS->setDisabled(1);
    Finger_Iris_Selection = 1;

    on_pushButton_2_clicked();

//    Finger_Iris_Selection = 0;
    ui->BtnBenIRIS->setEnabled(1);

    iConsentFormBackFlag = 2;
    ui->ConsentCheckBox->setChecked(true);
    ConsentForm();
    ui->stackedWidget->setCurrentIndex(21);
}

int Widget::MinQtyChecking(QString user,QString minQty,float ErrorWeight)
{
    float usrqty=user.toFloat()*1000;
    qDebug()<<"Issue Qty in Gms"<<usrqty;
    float  minGms=minQty.toFloat()*1000;
    qDebug()<<"Min Qty in Gms"<<minGms;
    if(user.toFloat(0) > minQty.toFloat(0))
    {
        float rem=usrqty/minGms;
        rem=rem-(int)rem;
        float MaxQty=(usrqty+ErrorWeight)/minGms;
        MaxQty=MaxQty-(int)MaxQty;
        float MinQty=(usrqty-ErrorWeight)/minGms;
        MinQty=MinQty-(int)MinQty;
        if((rem==0.00)||(MaxQty==0.00)||(MinQty==0.00))
        {
            qDebug()<<"success";
            return 0 ;
        }else{
            qDebug()<<"failed";
            return -1;
        }
    }
}

void Widget::WeighRounding(QString MinQtyFlag,QString BalQty,QString minQty,QString IssueQtyIn,float ErrorWeight,QString &IssueQtyOut)
{
    float IssueQtyInF = IssueQtyIn.toFloat() * 1000;
    float BalQtyF = BalQty.toFloat() * 1000;
    float minQtyF = minQty.toFloat() * 1000;

    if((IssueQtyInF < (BalQtyF - ErrorWeight)) && (MinQtyFlag == "Y"))//min qty flag should check
    {
        float rem=IssueQtyInF/minQtyF;
        rem=rem-(int)rem;
        if(rem==0.00)
        {
            IssueQtyOut = IssueQtyIn;
            return;
        }
        else
        {
            float MaxQty=(IssueQtyInF+ErrorWeight)/minQtyF;
            MaxQty=MaxQty-(int)MaxQty;
            if(MaxQty == 0.00)
            {
                IssueQtyOut = QString::number((IssueQtyInF + ErrorWeight) / 1000);
                return;
            }
            else
            {
                float MinQty=(IssueQtyInF-ErrorWeight)/minQtyF;
                MinQty=MinQty-(int)MinQty;
                if(MinQty == 0.00)
                {
                    IssueQtyOut = QString::number((IssueQtyInF - ErrorWeight) / 1000);
                    return;
                }
                else
                {
                    IssueQtyOut = IssueQtyIn;
                    return;
                }
            }
        }
    }
    else
    {
        if (BalQtyF == IssueQtyInF)
        {
            IssueQtyOut = IssueQtyIn;
            return;
        }
        else if ((BalQtyF + ErrorWeight) == IssueQtyInF)
        {
            IssueQtyOut = QString::number((IssueQtyInF - ErrorWeight) / 1000);
            return;
        }
        else if ((BalQtyF - ErrorWeight) == IssueQtyInF)
        {
            IssueQtyOut = QString::number((IssueQtyInF + ErrorWeight) / 1000);
            return;
        }
        else
        {
            IssueQtyOut = IssueQtyIn;
            return;
        }
    }
}

void Widget::on_DetailsButton_clicked()
{
    QSqlQuery query;
    QString AllotmentMonth,AllotmentYear;
    query.prepare("SELECT AllotMonth, AllotYear FROM PartialOnlineData");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            AllotmentMonth = query.value(0).toString();
            AllotmentYear  = query.value(1).toString();
        }
    }

    char TmpStr[1024];
    memset(TmpStr,0x00,sizeof(TmpStr));

    sprintf(TmpStr,"Alloted Month&Year: %s-%s \nVersion:%s%s\n",AllotmentMonth.toStdString().c_str(),AllotmentYear.toStdString().c_str(),NEW_VERSION,SUB_VERSION);
    miscObj.DisplayWideMsgBox((char *)TmpStr);

}

void Widget::on_BtnSwavalamban_clicked()
{
    GPRS objGprs;

    if(DealerLoginBy == 2)
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please login in online mode");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया ऑनलाइन मोड में लॉगिन करें ..");
        return;
    }

    //swavalamban
//    QFile file1("/mnt/jffs2/swavalamban.ini");
//    if(file1.open(QIODevice::Text | QIODevice::Append))
//    {
//        QTextStream out(&file1);
//        out << "consent_header="<<stateNameEn;
//        file1.close();
//    }

    memset(SessionId, 0, sizeof(SessionId));
    database_queries qry;
    if(qry.getSessionId(SessionId) < 0)
    {
        miscObj.DisplayWideMsgBox((char *)"Session Id not found\nPlease try again");
        return;
    }

    if(objGprs.Check_Network_Available() < 0)
    {
        miscObj.DisplayWideMsgBox((char *)"Network unavailable");
        return;
    }

    char buff[256];
    memset(buff,0x00,sizeof(buff));

//    sprintf(buff,"/usr/bin/swavalamban %02d %s %s %s %s %s",stateCode.toInt(),ui->ldtShpId->text().toAscii().data(),Qdist_code.toAscii().data(),SessionId,QPassword.toAscii().data(),PmsMenuEn.toAscii().data());

    sprintf(buff,"/usr/bin/swavalamban %02d %s %s %s %s %s",stateCode.toInt(),ui->ldtShpId->text().toAscii().data(),Qdist_code_impds.toAscii().data(),SessionId,HD_PASSWORD,PmsMenuEn.toAscii().data());

    system(buff);
}

void Widget::on_BtnDisAgree_clicked()
{
    if(iConsentFormBackFlag == 1)
        ui->stackedWidget->setCurrentIndex(20);
    else if(iConsentFormBackFlag == 2)
        ui->stackedWidget->setCurrentIndex(5);
    else if(iConsentFormBackFlag == 3)
        ui->stackedWidget->setCurrentIndex(42);
    else if(iConsentFormBackFlag == 4)
    {
        ui->lEdit_InpsAuth->clear();
        ui->stackedWidget->setCurrentIndex(45);
    }
    else if(iConsentFormBackFlag == 5)
        ui->stackedWidget->setCurrentIndex(23);
    else if(iConsentFormBackFlag == 6)
        ui->stackedWidget->setCurrentIndex(50);
    else if(iConsentFormBackFlag == 7)
        ui->stackedWidget->setCurrentIndex(5);
    return;
}
