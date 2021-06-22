#include "json_request.h"
#include "json.h"
#include <stdio.h>
#include <string.h>
#include <QtDebug>
#include <iostream>
#include <fstream>
//#include "iengine_ansi_iso.h"
#include <dirent.h>
#include <QtCore>
#include <fcntl.h>
extern "C"{
//#include "gl11.h"
#include <stdlib.h>
#include<stdio.h>
#include<string.h>
//#include"tfmapi.h"
//#include"tfmtypes.h"
//#include"tfmerror.h"
//#include <bclib.h>
}

using std::string;
//using qt

JsonRequest::JsonRequest()
{
}
JsonRequest::~JsonRequest()
{
}

char *JsonRequest :: PrepareAppDownloadRequestFile( char *Filename, char *cVerno, char *cPass, char *ckey, char *cvendorid)
{  
    int ret = 0;
    if (objMisc.fileExists(Filename) ){objMisc.fileDelete(Filename);}
    std::ofstream file_id;
    file_id.open(Filename);
    CMisc misc;
    char machineid[15];
    memset(machineid,0,sizeof(machineid));
    misc.GetMachineId((char *)machineid);
    //file.open(QIODevice::ReadWrite);
    char* Ret="";
    Json::Value JStr;
    Json::Value ArrayList(Json::arrayValue);

    JStr["versionNo"] = cVerno;    //"1.0"        // Hardcoded Vivek
    JStr["password"] =  cPass;            //"d690186b26dd093ab32b5e22a13f6c39";  // Hardcoded Vivek
    JStr["key"] = ckey;                      //"457";
    JStr["terminalID"] = machineid;
    JStr["vendorId"] = cvendorid;                 //"02";           // Hardcoded Vivek
    Json::StyledWriter styledWriter;
    file_id << styledWriter.write(JStr);
    file_id.close();
    ret = system("cat /root/Downloads/reqjson1.txt");
    Ret = objMisc.Convertstr(JStr.toStyledString());
    qDebug() << Ret;
    return Ret;

}
int JsonRequest::PrepareACKAppUpdateReqfile(char *Filename,char *cstatus, char*cfpsId, char*machineID, char *cversionNo) //New Parameter added for version 20062017 by vivek
{
    int ret = 0;
    if (objMisc.fileExists(Filename) ){objMisc.fileDelete(Filename);}
    std::ofstream file_id;
    file_id.open(Filename);
    CMisc misc;
    char machineid[15];
    memset(machineid,0,sizeof(machineid));
    misc.GetMachineId((char *)machineid);
    //file.open(QIODevice::ReadWrite);
    char* Ret="";
    Json::Value JStr;
    Json::Value ArrayList(Json::arrayValue);

    JStr["status"] = cstatus;
    JStr["fpsId"] =  cfpsId;
//    JStr["stateCode"] = stateCode.toStdString().c_str();
    JStr["terminalId"] = machineID; //Changed by vivek 20062017
    JStr["versionNo"] = cversionNo; //New Parameter added for version 20062017 by vivek
    Json::StyledWriter styledWriter;
    file_id << styledWriter.write(JStr);
    file_id.close();
    ret = system("cat /root/Downloads/reqjson1.txt");
    Ret = objMisc.Convertstr(JStr.toStyledString());
    qDebug() << Ret;
    return 0;
}
char *JsonRequest :: JSON_REQUEST_HAR()
{
    //if (fileExists("/root/Downloads/reqjson1.txt") ){fileDelete("/root/Downloads/reqjson1.txt");}
    //    std::ofstream file_id;
    //    file_id.open("/root/Downloads/reqjson1.txt");
    //    //file.open(QIODevice::ReadWrite);

    //    char* Ret="";
    //    char latitude = '1';
    //    Json::Value JStr;
    //   // Json::Value JStr(Json::arrayValue);
    //    Json::Value ArrayList(Json::arrayValue);

    //    Json::Value JStr1;


    //    JStr["pDSReceiveFPSHeader"]["fps"] = "111111111111";
    //    JStr["pDSReceiveFPSHeader"]["lat"] = latitude;
    //    JStr["pDSReceiveFPSHeader"]["lon"] = "1";
    //    JStr["pDSReceiveFPSHeader"]["cc"] = "1";
    //    JStr["pDSReceiveFPSHeader"]["mac"] = "78:45:C4:B4:AA:71";
    //    JStr["pDSReceiveFPSHeader"]["ver"] = "1.0";
    //    JStr["pDSReceiveFPSHeader"]["reqts"] = "13-07-2016 10:10:10:123";
    //    JStr["pDSReceiveFPSHeader"]["req"] = "dfwl";
    //    JStr["pDSReceiveFPSHeader"]["bid"] = "1";
    //    JStr["pDSReceiveFPSHeader"]["reccnt"] = "1";
    //    JStr["pDSReceiveFPSHeader"]["totcnt"] = "1";

    //    JStr1["orderno"]="1";
    //    JStr1["txnid"]="11111111111111118282151231316543132236.018";
    //    JStr1["txndate"]="13-07-2016";
    //    JStr1["rcno"]="111111111111";
    //    JStr1["rctype"]="31";
    //    JStr1["commcode"]="1";
    //    JStr1["memid"]="11111111111101";
    //    JStr1["authstatus"]="Y";
    //    JStr1["authcode"]="1";
    //    JStr1["lqty"]="1";
    //    JStr1["munit"]="1";
    //    JStr1["price"]="1";
    //    JStr1["month"]="7";
    //    JStr1["year"]="2016";
    //    ArrayList.append(JStr1);
    //    JStr["pDSReceiveFPSTransaction_Detail"] = ArrayList;





    //JStr["pDSReceiveFPSTransaction: null"];

    //    JStr["pDSReceiveFPSHeader"]["orderno"]="1";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["txnid"]="11111111111111118282151231316543132222.018";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["txndate"]="13-07-2016";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["rcno"]="111111111111";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["rctype"]="31";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["commcode"]="1";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["memid"]="11111111111101";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["authstatus"]="Y";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["authcode"]="1";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["lqty"]="1";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["munit"]="1";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["price"]="1";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["month"]="7";
    //    JStr["pDSReceiveFPSTransaction_Detail"]["year"]="2016";

    //JStr["PDSReceiveFPSTransaction"]["fps"] = "111111111111";

    //    JStr2["orderno"]= "1";
    //    JStr2 ["txnid"]="11111111111111111111111111111113132223.018",
    //    JStr2["txndate"]= "13-07-2016";
    //    JStr2["rcno"]= "111111111111";
    //    JStr2["rctype"]= "31";
    //    JStr2["commcode"]= "1";
    //    JStr2["memid"]= "11111111111101";
    //    JStr2["authstatus"]= "Y";
    //    JStr2["authcode"]= "1";
    //    JStr2["lqty"]= "1";
    //    JStr2["munit"]= "1";
    //    JStr2["price"]= "1";
    //    JStr2["month"]= "7";
    //    JStr2["year"]= "2016";
    //    JStr["PDSReceiveFPSTransaction"] = JStr2; //commented
    //  JStr["pDSReceiveFPSHeader"]=JStr1;

    //    Json::StyledWriter styledWriter;
    //    file_id << styledWriter.write(JStr);
    //    //file_id << styledWriter.write(JStr1);
    //    //file_id << styledWriter.writeWithIndent();
    //    file_id.close();
    //    system("cat /root/Downloads/reqjson1.txt");
    //    Ret = Convertstr(JStr.toStyledString());
    //    qDebug() << Ret;
    //    return Ret;
    return "";
}
