#ifndef WIDGET_H
#define WIDGET_H

#include "include/compheader.h"
#include "database.h"
#include "defines.h"

#include<stdio.h>
#include<string.h>
#include <curl.h>
#include<curlver.h>
#include<easy.h>
#include<fstream>

#include<stdlib.h>
#include "json.h"
#include "json_request.h"
#include "json_response.h"

extern "C"
{
#include "bio.h"
#include "create_bmp.h"
#include "ejLog.h"
#include "http_client.h"
#include "BiomatiquesRDSClient.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include"iritech.h"
}

#include "misc.h"
#include "parse.h"
#include "modem.h"
#include "gprs.h"
#include "popup_msg.h"
#include "ui_popup_msg.h"
#include "popup_nothing.h"
#include "ui_popup_nothing.h"
#include "logo_print.h"
#include "finger.h"
#include "usb.h"
#include "ethernet.h"
#include "bil_pds_eod.pb-c.h"
#include "database_queries.h"

#define RESP_CODE_INDEX 10
#define ALIGN_CENTER 0
#define ALIGN_LEFT   1
#define ALIGN_RIGHT  2

//Declared for dynamic HTS and PASSWORD Value Vivek

static char DEL_PWD[64];

#define LEFT_INDEX      3//1
#define LEFT_LITTLE     4//2
#define LEFT_MIDDLE     5//3
#define LEFT_RING       6//4
#define LEFT_THUMB      7//5

#define RIGHT_INDEX     8//6
#define RIGHT_LITTLE    9//7
#define RIGHT_MIDDLE    10//8
#define RIGHT_RING      11//9
#define RIGHT_THUMB     12//10

#define UNKNOWN         99
#define VENDORID        "02"

#define OLD_VERSION         "2.1"
#define NEW_VERSION         "2.3"
#define SUB_VERSION         ".00"

#define SERVER_TIME_OUT         90
#define BFD_SERVER_TIME_OUT     60
#define BANK_SERVER_TIME_OUT    130

#define LEFT_EYE        1
#define RIGHT_EYE       2

#define BIOMATIQUES_RIGHT_EYE 1
#define BIOMATIQUES_LEFT_EYE  2
#define DEALER_SELECTED 7001
#define MEMBER_SELECTED 7002
#define ROUTE_OFFICER_SELECTED 7003
#define HAWKER_SELECTED 7004
#define INSPECTOR_SELECTED 7005
#define BEN_VERIFICATION 7006
#define KEROSENE_TXN    9011
#define STOCK_ENTRY     9012
#define KEROSENE_HWKR_TXN 9013
#define KEROSENE_HWKR_DIS 9014
#define COMM_TXN        2
#define DUMMY_TXN       99

#define NO_DEVICE 0
#define BIOMATIQUES 1
#define IRITECH 2

#define EPDS_SERVER     1
#define AUTH_SERVER     2

#define PDS_TRANSACTION     501
#define ICDS_TRANSACTION    502
#define PDS_CASHLESS_TRANSACTION 503
#define FEST_TRANSACTION    504
#define MDM_TRANSACTION     505
#define HAWKER_TRANSACTION  506
#define HAWKER_DOWNLOADING  507
#define RCV_GOODS_TRANSACTION 508
#define DEALER_DOWNLOADING  509
#define NCR_TRANSACTION     510
#define EKYC_TRANSACTION    511

#define PDS_URL "http://epos.nic.in/ePosServiceJDN2_3/jdCommoneposServiceRes?wsdl"
#define PDS_URL2 "http://aepds.dnh.gov.in/ePosServiceJDN2_3/jdCommoneposServiceRes?wsdl"

//#define PDS_URL "http://164.100.130.57/ePosServiceBR2_1/breposServiceRes?wsdl" //Test
//#define PDS_URL2 "http://123.63.38.215/ePosServiceBR2_1/breposServiceRes?wsdl"

//#define PDS_URL "http://aepds.dnh.gov.in/ePosServiceJDN2_3Test/jdCommoneposServiceRes?wsdl"
//#define PDS_URL2 " http://aepds.dnh.gov.in/ePosServiceJDN2_3Test/jdCommoneposServiceRes?wsdl"

#define HD_PASSWORD_INIT  "7797602c3da57f23e57a259b60358622"  //Common URL
#define HD_HTS_INIT        "111"

//#define OFFLINE_AND_RECEIVE_STOCK_URL "http://164.100.137.92/ePosCommonService/eposCommon/"
#define OFFLINE_AND_RECEIVE_STOCK_URL "http://164.100.65.97/ePosCommonService2_0/eposCommon"
#define STOCK__RECIEVE_URL "http://164.100.65.97/ePosCommonService2_0/eposCommon/getFpsStockDetails"
#define RECEIVEGOODS_TOKEN "9f943748d8c1ff6ded5145c59d0b2ae7"

#define TXN_STATUS_SUCCESS    0
#define TXN_STATUS_REVERSAL   1

#define TXN_SUCCESS 1
#define TXN_ERROR   2
#define TXN_PENDING 3

#define EJ_LOGS_SENT    0
#define EJ_LOGS_PENDING 1

#define NO_REVERSAL     5005

#define ISO_WD_TXN  6001
#define ISO_DP_TXN  6002
#define ISO_BI_TXN  6003
#define ISO_MS_TXN  6004
#define ISO_REVERSAL 6011
#define ISO_FUSION   6012

#define ENGLISH 1
#define HINDI   2
#define TELUGU  4

#include "Biomatiques.h"
#include "IrisCapture.h"
#include "data.h"
#include "EPI1000.h"

#include <iostream>

using namespace std;
namespace Ui {
class Widget;
}

enum TransactionMode { AEPS, LTS, RUPAY, IMPS, REVERSAL };

struct TransactionLog
{
    char cTempRRN[32];
    char cDate[16];
    char cTime[16];
    char cStan[16];
    char cPermRRN[64];
    int TxnMode;
    char cCustAadhaarNo[16];
    char cCustomerIIN[16];
    char cCustomerName[64];
    char cTxnAmount[32];
    char cAvailableBal[32];
    char cReversalStatus[8];
    char cResponseCode[8];
    //char cUIDAIAuthCode[64];
};

// Impds 23082018
enum STATE_CODES
{
    __JAMMU_AND_KASHMIR__=01,
    __HIMACHAL_PRADESH__,
    __PUNJAB__,
    __CHANDIGARH__,
    __UTTARANCHAL__,
    __HARYANA__,
    __DELHI__,
    __RAJASTHAN__,
    __UTTAR_PRADESH__,
    __BIHAR__,
    __SIKKIM__,
    __ARUNACHAL_PRADESH__,
    __NAGALAND__,
    __MANIPUR__,
    __MIZORAM__,
    __TRIPURA__,
    __MEGHALAYA__,
    __ASSAM__,
    __WEST_BENGAL__,
    __JHARKHAND__,
    __ORISSA__,
    __CHHATISGARH__,
    __MADHYA_PRADESH__,
    __GUJARAT__,
    __DAMAN_DIU__,
    __DADRA_NAGAR_HAVELI__,
    __MAHARASHTRA__,
    __ANDHRA_PRADESH__,
    __KARNATAKA__,
    __GOA__,
    __LAKSHADWEEP__,
    __KERALA__,
    __TAMIL_NADU__,
    __PONDICHERRY__,
    __ANDAMAN_NICOBAR_ISLANDS__,
    __TELANGANA__

};

class Widget : public QWidget, public IrisCapture,public JsonResponse
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    Database dbaccess;
    QString HindiNameComm;

    QString skey;
    char asciiBase64Enc[512];
    QString minqty_flag,weighAccuracyValueInGms;

    int CheckingPofflineTrans();
    int SendTxnCancelReason(QString & , char);
    int dotremoval(char *);
    void MemberDetails();
    int prepareEkycCardHolderReqFile(char *);
    int GetEkycCardHolderDetails(int);
    int parseEkycCardHolderDetails(char *);
    int PerformEKYC();
    QString Rcno;

    void EKYC_FP_Submit();
    int parseSeedingcCardHolderDetails(char *);

    QString QErrorDesc;
    QString QGatewayRRN;
    char szGatewayRRN[128];
    char szHelpDeskNo[16];
    int MinQtyChecking(QString,QString);
    int BenEnterQtyChecking(QString,QString,QString);
    int rsaEncryption(QString,QString);
    int CheckTxnDate();
    int CheckTxnStopDate(QString menu);
    QStringList MinQtyStr;

    int getRuteOfficerStatus();
    int CL_Conclusion(char *);
    int CL_ConclusionToServer(char *);
    int getCL_ConclusionReq(char *, char *);
    int getShopId(char *);
    int getEJ_Count();
    void getHeadings();
    QString Qpds_cl_tran_eng, Qpds_cl_tran_tel, Qpds_tran_eng, Qpds_tran_tel;

    void EKYC_Submit();
    int getCommLength(char *);
    int iLCD_Display;
    QString AvailedCards, TotalCards,receiptid;
    int parseCardsInfoDetails(char *);
    void parseCurrentStockReportRespDetailsToDisplay();
    void BFD_OPR();
    void PrintTxnReceiptEng();
    int TableNCR();
    void NCR_TxnUpdate();
    int prepareNCR_AuthReqFile(char *, char *, char *, char *);
    void ShowAlertMsg();
    int getAvailableBalance(char *, char *);
    int getFusionStatus();
    int getDealerFusionStatus(char *);
    int getUIDStatus();
    int FusionUpdateToServer(char *, char *, char *,char *);

    char FusionStatus[8];
    char DelAuthMode[8];
    char BenAuthMode_Finger[8];
    char BenAuthMode_Iris[8];
    char BenAuthMode_Manual[8];
    char BenAuthMode_Otp[8];

    void SetAppType(int);
    void SetAppType1(int);
    void SetLogo(QString);
    int rtcDateValidation();
    int GetPrintPaperSts(QString &);
    int ForwardDistrubutionValidation(int , int );
    int CBOflineDownload();

    int get_btnOK_EntitleStatus();
    int handleReversal(char *);
    void ReadSimSerialNo(char *);
    void PrintLastRecipt(char *);
    int iFinger_Flag;
    int Cashless_FP_Enroll();
    int CheckBankMasterData();

    void EnableDisableMenus();
    int ParseMenuDisplay(char *);
    int CallingMenuDrivenService();

    QString QDomainName;
    void PrintLastReciptResponse(char *);
    int ParsePrintLastRecptResponse(char *,char *);
    int prepareBFDPacket();
    QString QBFD_UID;
    void ShowDisplayMsg();
    int InsertCardTypesIntoDB(QString);
    QString QStockReportType;
    void UpdateURLs(QString);
    int UpdateDBTxnURL();
    void PrintCurrentStockReport();
    int iMainTransaction;
    int Calling_Biomatiques_Iris(int);
    char *buffIris;
    void OnFrameRecieve(RGBImage * i);
    void OnCaptureCompleted(BIOMTQS_ISO * ii);
    void OnCaptureCompletedImage(RGBImage * im,int kind,int * Points);

    int g_Capture_Var;
    int iFusionEnabledFlag;
    int SystemCurlResponse(int, int);
    int ParsingFirstCondition(char *);
    int ParsingFirstConditionFusion(char *);
    int ValidatingErrors(char *);
    int LangMode;
    int InsertIntoTxnMaster();
    void OprRS232();
    void OprUSB3();
    void insertWt(char *);
    int parseList();
    void ChangeUI_to_Eng();
    void ChangeUI_to_HINDI();
    void PrintTxnReceipt();
    void setLangMode(int);
    //int prnMonthlyReport();
    void prnDayWiseReport();
    void prepareBmp(QString , int , QString, int, int );
    void GetRTC_Params(char *, char *, char *, char *);
    int GetCustomerDetails();
    int getWM_MacId(char *);
    void setWM_MacId(char *);
    int getCommMode();
    void setSessionId(char *);
   // int send_getBFDDetails(QStringList, QStringList);
    //int parsegetbfddetails();
    int CheckingMandatoryFestComm();
    //int assumeCommName(char *, char * );
    //int GetSeqCommName(int, char *);
    void Print_ReceiveStock(int);
    int parseStockUpdateResp(char *);
    int GetDealerAuthFileName(char *);
    int prepareDealerReqFile(char *);
    int parseDealerDetails(char *);
    int parseAuthResponse(char *);
    int parseCardHolderDetails(char *);
    int parseupdateAndInsertServiceResponse(char *);
    int GetCardHolderDetails(int);
    int GetCardHolderFileName(char *);
    int prepareCardHolderReqFile(char *);
    int SendReceiveStockToServer_4_1();
    int SendReceiveStockToServer();
    void PrepareReportRequest();
    void parseCurrentStockReportRespDetails();
    int prepareInsertUpdateCommFile();
    int parsegetViewStockRecievedResponse();
    int SendTxnToServer();
    int AuthOwner();
    int AuthBen();
    int AuthBenRecvGoods();
    int prepareAuthReqFile(char *, char *, char *, char *);
    void DisplayBFDRespCode(char *, char *);
    void SetLanguage();
    int app_ver();
    void GetAppVersion(char *);
   // void prnTest();
    void PlayRupeesPaisa();
    void InitialiseBluetooth();
    void popupNothing();
    void ConsentForm();
    int Capture_Iris(int, char *);
    char * ReadIrisFile(char *, int);
    int Parse_Iris_Response(char *);
    int Cal_md5Checksum_Install();
    void RemovingUpdateFiles();
    int parseRouteOfficerResponse();
    int RecieveGoodsAuth();
    int DealerAuth();
    int check_http_error_code();
    void DisplayDealerDetails();
    void DisplayCommDetails();
    void HideConsent();
    QString QTokenNO;
    int iFestivalFlag;
    int iModuleType;
    int Finger_Iris_Selection;
    char gszDashedDt[16],gszDashedTm[16];
    char gRTC[32], gDATE[16], gTIME[16], gJULIAN_DAYS[8];
    char gauth_transaction_code[128];
    char gRoute_Off_UID[16];
    char gRoute_Off_auth_code[128];
    int iKOilPreviewFlag;
    int iDealMemSelected;
    int iConsentFormBackFlag;
    char buff_btId[32];
    char TxnId[32];
    char BT_DeviceName[64];
    char BT_DeviceID[64];
    int FP_Position;
    int CountFingers;
    int NoOfEnrollFingers;
    char IssueMonth[8];
    char RationCardId[25];
    char UID[16];
    char EID[32];
    char MemberUID[32];
    char MemberName[256];
    char ItemCode[16];
    char ItemName[32];
    char ItemQty[8];
    char SellingPrice[8];
    char Amount[8];
    char TxnDate[16];
    char TxnTime[16];
    char ReceiptNo[16];
    char DeliveryFlag[4];
    char PIN[8];
    char CardType[8];
    float fTotalAmt, fTotalQty;
    float ftotal_amount;
    char CustomerName[128];
    char DistrictCode[16];
    char MandalCode[16];
    char Panchayat[32];
    char Mandal[128];
    char District[128];
    char MemberCount[4];
    char Gender[8];
    int iUsbStatus;
    char SessionId[32];
    char memekycuid[32];
    char otp_mobile[16];
    char mobile_otp[16];

    QString Qdist_code_impds;
    QString pmsURL;
    QString g_pmsWadh;
    QString PmsMenuEn;
    QString getcomTxn_type;
    QString FestMsgEn;
    QString FestMsgTel;
    QString due_tag_display;
    QString due_tag_display_tel;
    QString previous_amount;
    QString SurveyStsFlag,SurveyMsgEn,SurveyMsgLl,SurveyMinQty;
    QString eKYCPrompt,irisStatus,paperRequiredFlag;

    int DealerTxnFlag;

    QComboBox* comboBox();
    int AutoAppUpdate(char *);
    QString QversionStatus;
    QString checkAppUpdate(char *);
    int AcknowledgementAppUpdate(char *, char *, char *); //New Parameter added for version 20062017 by vivek
    int CreateMd5checksumFile(char *);
    int systemret;
    CMisc miscObj;
    int getEkyc_MemberID(char *);
    //Inspection Module
    int preparegetInspectorDetailsReq(char *);
    int parsegetInspectorDetailsResp(char *);
    int showInspInchargeDetails();
    int showInspCommDetails();
    int showInspApprovals();
    int prepareinspPushCBDataReq(char *);
    int parseinspPushCBDataResp(char *);
    int getApprovestatus(char * , char *);
    int AuthInspector();
    int Submit_Inspector_Verfication();
    char cInspector_UID[50];
    char cCurrentApprovalValue[10]; //Inspector Approval Value
    int PrintTxnReceiptInspection();
    int PrintTxnReceiptInspectionHin();
    char cInspcTxnId[100];

    int parseAuthResponse_Insp(char *); //01072017
    int prepareAuthReqFile_Insp(char *, char *, char *, char *); //01072017

    //Transactio Modes
    QString cidType, cMode;

    //Recieve Goods Module
    int prepareStockDetailsReq(char *, char *, char*);
    int ConvertStockDetailsRespToJson(char *);
    int parseStockDetailsResponseJson(char *);
    int ParsingFirstConditionJson(char *);
    int ShowStockDetils(QString);
    int SetLineEditValidation();
    int SendReceiveStockTXNToServer();
    int preparegetRouteOfficerDtlsReq(char *);

    // Mobile Update  // added by vivek 01082017
    QString checkMobileUpdate(char *);
    QString MobileStatusMessage;
    int UpdateMobileNumber(char *);
    int prepareUpdateMobileNoReq(char *, char *);

    int pop_up(char *);

    // Aadhar 2.0
    int rdCallinit(int ); //RD 2.0 06/11/2017
    int rdCallinit_ekyc(int,char *);
    int parseStockInforUpdateResponse(char *);
    QString DYN_StockUpdate_TxnreceiptId;

    int getDealerAuthTypeANDFusion(int ,char *);
    int PwdackRequest_Response(char *);
    int ParsePWDACK(char *);
    int getBeneficiary_AuthTypeANDFusion(int,char *);
    int Member_EKYC_Status(int );
    int Ekyc_uid_popup(char *);
    void parseCurrentStockReportandinsert();

    int prepare_beneficiaryOTPReq(char *);
    int beneficiaryOTPVerify(char *,char *,char *);
    int ParsebeneficiaryOTP(char *);
    int ParsebeneficiaryOTPVerify(char *);

    //OTPAUTHMOBILE
    int prepare_beneficiaryOTPAUTHReq(char *);
    int mobileno_otp_popup(char *);
    int OTP_Mobile_popup(char *);
    int getHeadMobileno(char *);
    int getMemberID(char *,char *);
    int Mobile_OTP_Verify();

    int ParseMobileOTP(char *);
    int beneficiaryOTPVerifyMobileAuth(char *,char *);
    int Mobile_OTP_Calling();
    int EYC_Manual_Calling();

   void Scan_barcodecam();
   int OTP_Mobile_popup_New(char *);

   int Calling_Iritech_Iris(int);

   int stock_Receive();
   bool getRecvGoodsRuteOfficerStatus();
   int Get_BenVerify_CardHolderDetails();
   int prepareCardHolderReqFile_BenVerify(char *);
   int parseBenVerify_CardHolderDetails(char *);
   int Print_Beneficiarydetails(char *);
   int EKYC_Calling(int);

   int ParsePrintLastRecptResponse_balance(char *);
   int MdmTxnRcpt();

   int otp_popup_verify(QString );
   QString dealer_password;

   /*************Partial Online module************/
   QString GetSurveyWt();
   void DataDownloadError();
   int DwnldPartialOnlineData();
   int GetOfflineCommDetails();
   int PartialOfflineTxnUpload();
   int ManualServerUploadPartialTxns();
   int UpdateTransStatus();
   void GetMsgScroll();
   void HideScroll();
   int GetTransactionId(char *TxnId);
   int getLangMode();
   int LoginByPassword();
   int TxnAllotedBetweenTime();
   QTimer *PartialOnlineTimer;
   int PrintOfflineTxnRcpt();
   int UpdateDownLoadStatus();
   int GetOflineDbBkp();
   int CheckOfflineTxnGetDetails();
   int CheckOfflineTxnScanFp();
   int CheckOfflineTxnUpload();
   int PingCheck();
   int CheckOfflineGetDealerDetails();
   int CheckOfflineDealerAuth();

   int MinQtyChecking(QString user, QString minQty, float ErrorWeight);
   void WeighRounding(QString MinQtyFlag, QString BalQty, QString minQty, QString IssueQtyIn, float ErrorWeight, QString &IssueQtyOut);

   void PrintOfflineDayReport();
   void PrintOfflineDayReportBKP();
   int UrlPingCheck(char * cUrlName);
   int DayReportFlag;
   int PartialTxn;
   int DealerLoginBy;
   QString PartiDataDownloadFlag;
   QString partialOnlineOfflineStatus,PartialTxnDeleteFlag,fpsCbDownloadStatus,responseTimedOutTimeInSec;

   QString Qlogout;
   QTimer *logout_timer;
   int nLogout;

   QString consentHeader,stateCode,stateNameEn,stateNameLl,stateProfile,stateReceiptHeaderEn,stateReceiptHeaderLl,statefpsId;
   QString Offline_and_ReceiveStock_URL;
   typedef QList<QString> MapList;
   QVector<MapList> Reasons_Vec;

   bool eventFilter(QObject *, QEvent *);

    int Get_transactionID(char *);
    int CheckRecviceGdsOffline();
    void PrintRecvGoodsRecpt();
    int ManualReceiveGdsUpload();
    int PatchUpdateOfflineCheck();

   /*************Partial Online module************/
    int GetSchoolMemberDetails();
    int parseICDSMemberDetails(char *filename);
    int BenAuthSUCCESS;

    bool GotoConsentForm();
//
public slots:
    void setvalue1(QString);

Q_SIGNALS:
    void textChanged(const QString &);

private slots:

    void logout();
    void on_BtnAnganwadiMemDetails_clicked();

    int GetPartialTimeStatus();
    void on_RecvGoodsCommEntBack_clicked();
    void on_RecvGoodsCommEntSubmit_clicked();
    void on_RecvGoodsCommEntAdd_clicked();
    void on_RecvGoodsSubmitBtn_clicked();
    void on_RecvGoodsBackBtn_clicked();
    void RecvGoodsSelectCommD(QString Str);
    void RecvGoodsSelectSchemeType(QString Str);
    void RecvGoodsRecvQtyEntry(QString Str);

    void on_RecvGdsConfirmSubmit_clicked();
    void on_RecvGdsConfirmBack_clicked();

    void on_btnReports_clicked();
    void on_btnGotoHome_clicked();
    void on_btnAuthenticate_clicked();
    void on_btnBackEntitle_clicked();
    void on_btnOK_Entitle_clicked();
    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void BenficiaryAuth();
    void BenficiaryAuth_BKP();

    void on_btnCancelCommodity_clicked();

    void on_btnConfirmCommodity_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_btnGetWt_clicked();

    void on_ldtIssueQty_editingFinished();

    void on_btnBackSettings_clicked();

    void on_comboLangSel_currentIndexChanged(int index);

    void on_btnReceiveGoods_clicked();

    void on_tblCommIssue_cellChanged(int row, int column);

    void on_btnDailySales_clicked();

    void on_btnBackDt_clicked();

    void on_btnOkDt_clicked();

    void on_btnDtSalesView_clicked();

    void on_pushButton_7_clicked();

    void on_btnLogout_clicked();

    void on_btnQuitApp_clicked();

    void on_btnLogin_clicked();

    void on_btnUpdateSW_clicked();

    void on_btnOK_Settings_clicked();


    void on_ldtGetOwnDetails_clicked();

    void on_BtnBack_4_clicked();

    void on_BtnUSBUpdateApp_clicked();

    void on_BtnGPRSUpdateApp_clicked();

    void on_BtnDeviceSettings_clicked();

    void on_BtnBack_5_clicked();


    void on_btnSettings_2_clicked();

    void on_btnBack_clicked();

    void on_ldtRationCard_cursorPositionChanged(int arg1, int arg2);

    void on_BtnOK_2_clicked();

    void on_BtnConsentForm_2_clicked();

    void on_BtnConsentForm_clicked();

    void on_tblCommIssue_itemClicked(QTableWidgetItem *item);

    void on_BtnDealerScanFP_clicked();

    void on_comboTruckChitNo_activated(const QString &arg1);

    void on_BtnBack_7_clicked();

    void on_BtnNext_3_clicked();

    void on_BtnDealerRecGoods_clicked();

    void on_BtnRouteOfficer_clicked();

    void on_BtnBack_8_clicked();

    void on_BtnScanFinger_clicked();

    void on_BtnScanIris_clicked();

    void on_tableWidgetStockTable_cellChanged(int row, int column);

    void on_btnStockRegister_clicked();

    void on_BtnOK_3_clicked();

    void on_comboCommMode_activated(int index);

    void on_BtnPreview_clicked();

    void on_btnKerosene_clicked();

    void on_BtnHawker_clicked();

    void on_BtnPDS_clicked();

    void on_BtnBack_12_clicked();

    void on_comboTypeOfCard_activated(int index);
    void on_ldtShpId_cursorPositionChanged(int arg1, int arg2);
    void on_BtnLastReciept_clicked();
    void on_btnMobileUpdate_clicked();
    void on_BtnBack_18_clicked();
    void on_BtnOTP_clicked();
    void on_BtnBack_19_clicked();
    void on_BtnSubmit_3_clicked();
    void on_btnBackAadhaarServices_clicked();
    void on_btnAadhaarServices_clicked();
    void on_btnEKYC_Update_clicked();
    void on_btnSubmitEKYC_clicked();
    void on_btnBackEKYC_clicked();
    void on_ChkBox_Y_POI_POA_clicked(bool checked);
    void on_ChkBox_N_POI_POA_clicked(bool checked);
    void on_ChkBox_Y_Mob_eMail_clicked(bool checked);
    void on_ChkBox_N_Mob_eMail_clicked(bool checked);
    void on_btnBack_EKYC_clicked();
    void on_btnOK_EKYC_clicked();
    void on_btnEKYC_Back_clicked();
    void on_btnEKYC_FP_clicked();
    void on_btn_inspector_clicked();
    void on_btn_Back_inspectorDtls_clicked();
    void on_btn_FPAuth_inspectorDtls_clicked();
    void on_btn_Back_inspectorVerify_clicked();
    void on_btn_Submit_InspectorVerify_clicked();
    void on_radioButton_RCNOFLAG_clicked(bool checked);
    void on_radioButton_UIDFLAG_clicked(bool checked);
    void on_tableWidget_InspectorVerificationn_cellChanged(int row, int column);
    void on_radioButton_RCNOFLAGEKYC_clicked(bool checked);

    void on_btnScanFpInspAuth_clicked();
    void on_btnBack_InspAuth_clicked();
    void on_tableWidgetStockTable_3_cellChanged(int row, int column);
    void on_btnpoweroff_clicked();
    void kill_app();
    void displayDate();
    void displayTime();
    void set_RDIcon();
    void setsignalStrength();
    void updateBatStatus();
    void on_btncurrstkprint_clicked();
    void on_btncurrstkback_clicked();

    void on_BtnDealerIris_clicked();

    void on_btnmobupdateback_clicked();

    void on_btnmobupdatenext_clicked();

    void on_leditmobupdatebk_clicked();

    void on_leditmobupdatesub_clicked();

    void on_btnMobile_Seeding_clicked();

    void on_radioButton_scanbarcode_clicked();

    void on_btn_BenVerify_clicked();
    void on_btnBack_BenVerify_clicked();
    void on_btn_RCNOFLAGBenverify_clicked(bool checked);
    void on_btn_UIDFLAGBenverify_clicked(bool checked);
    void on_btnOK_BenVerify_clicked();
    void on_btn_BenVerifyBack__clicked();
    void on_BenVerifyScanFP_clicked();

    void on_radioButton_UIDFLAGEKYC_clicked();
    void on_Partial_DaDyRptBtn_clicked();

    void on_PartialCurrStkRptBtn_clicked();

    void on_btnMDM_clicked();

    //void on_BtnAnganwadiMemDetails_clicked();

    void on_BtnBack_13_clicked();

    void on_MinimizeButton_clicked();

    void on_MinimizeButton_2_clicked();

    void on_btnImpds_clicked();

    void on_BtnConsentForm_6_clicked();

    void on_BtnConsentForm_5_clicked();

    void on_BtnConsentForm_4_clicked();

//    void on_BtnConsentForm_3_clicked();

    void on_BtnConsentForm_7_clicked();

    void on_BtnConsentForm_8_clicked();

    void on_tableWidget_InspectorVerificationn_cellEntered(int row, int column);

    void on_tblCommIssue_cellClicked(int row, int column);

    void on_UploadDataButton_clicked();

    void on_OfflineReceiveGoodsButton_clicked();

    void on_BtnBenIRIS_clicked();

    void on_DetailsButton_clicked();

    void on_GetCardStsBtn_clicked();

    void on_BackFrmRcDetails_clicked();

    void on_BtnSwavalamban_clicked();

    void on_BtnDisAgree_clicked();

private:

    QString telItem;
    bool batBuzzStatus;
    QMovie *movie;

static  Npci__Pds__Eod__Protobuffer__Log *nPcieodlog;
    Ui::Widget *ui;


protected:

    QTime timeoutobj;
    //bool eventFilter(QObject *, QEvent *);

    void keyPressEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);

    JsonRequest *objJsonReq;
    JsonResponse *objJsonResp;

};
class TxnUpload:public QThread
{
    Q_OBJECT
    public:
    QString consentHeader,stateCode,stateNameEn,stateNameLl,stateProfile,stateReceiptHeaderEn,stateReceiptHeaderLl,statefpsId;

        explicit TxnUpload(QObject *parent = 0) : QThread(parent){}
        void run();
        void ServerUploadPartialTxns();
        int ReceiveGdsUpload();
        void GetStateInfo();
};
#endif // WIDGET_H





