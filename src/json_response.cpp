#include "json_response.h"
#include "json.h"
#include <cstdio>
#include <cstring>
#include<iostream>
#include<fstream>
#include "QMessageBox"
#include "QDateTime"
#include <QDebug>
#include <QFile>
#include<QVariantMap>

#include<cstdlib>
#include<math.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

#include <unistd.h>
#include <QStringList>



JsonResponse::JsonResponse()
{
}
JsonResponse::~JsonResponse()
{
}
int JsonResponse :: ParseAppDownloadResp(char *FIleName, Auto_App_Download_Resp *respparams)
{

    //memset(str,0,sizeof(str));
    Json::Value root;
    Json::Reader reader;
    std::ifstream ReadData(FIleName);
    bool parsedSuccess = reader.parse(ReadData,root,false);
    //bool parsedSuccess = reader.parse(ResultData,root,false);
    if(not parsedSuccess) {
        return -2;
    }
    else
    {

        qDebug () << "parsing on the way";
        //const Json::Value PDSReceiveFPSTransactionHeader = root.get("PDSReceiveFPSTransactionHeader",0);
        //printf("Data Loaded \n");
        // Json::Value Response = PDSReceiveFPSTransactionHeader.get("",0);
        //printf("Response Loaded \n");

        respparams->fpsurl = objMisc.Convertstr(root["fpsDownloadURL"].asString());
        qDebug() <<respparams->fpsurl;
        //values retrieved from json response
        respparams->vesionnum= objMisc.Convertstr(root["versionNo"].asString());
        qDebug() << respparams->vesionnum;

        respparams->checksum= objMisc.Convertstr(root["checkSum"].asString());
        qDebug() << respparams->checksum;
        //QString Ack(acks);
        respparams->respcode = objMisc.Convertstr(root["respCode"].asString());
        qDebug() << respparams->respcode;

        respparams->respMsg = objMisc.Convertstr(root["respMessage"].asString());
        qDebug() <<respparams->respMsg;


        respparams->fpsid = objMisc.Convertstr(root["fpsId"].asString());
        qDebug() <<respparams->fpsid;

    }
    return 0;

}

int JsonResponse::ParseACKAppUpdateRespfile(char *FileName, char *crespCode, char *crespMessage)
{

    //memset(str,0,sizeof(str));
    Json::Value root;
    Json::Reader reader;
    std::ifstream ReadData(FileName);
    bool parsedSuccess = reader.parse(ReadData,root,false);
    //bool parsedSuccess = reader.parse(ResultData,root,false);
    if(not parsedSuccess) {
        return -2;
    }
    else
    {

        qDebug () << "parsing on the way";

         crespCode = objMisc.Convertstr(root["respCode"].asString());
        qDebug() << crespCode;

        crespMessage = objMisc.Convertstr(root["respMessage"].asString());
        qDebug() << crespMessage;

    }
    return 0;
}

int JsonResponse::parseStockDetailsResponseJson(char *FileName)
{

    QSqlQuery query;

    query.exec("DELETE FROM TruckChit_Table");
    query.exec("DELETE FROM Stock_Details");
    query.exec("DELETE FROM OrderNoTable");

    Json::Value root;
    Json::Reader reader;
    std::ifstream ReadData(FileName);
    char *crespCode;
    bool parsedSuccess = reader.parse(ReadData,root,false);

    if(not parsedSuccess)
    {
        return -2;
    }
    else
    {
        qDebug () << "parsing on the way";
        crespCode = objMisc.Convertstr(root["respCode"].asString());
        qDebug() << crespCode;

        Json::Value ApiTofetchFpsTruckChallandata = root.get("infoTCDetails",0);

        qDebug() << "size of truckchallan"<<ApiTofetchFpsTruckChallandata.size();

        if (ApiTofetchFpsTruckChallandata.size() > 0)
        {

            for(int j=0; j< ApiTofetchFpsTruckChallandata.size(); j++)
            {

                char * truckChitNo = objMisc.Convertstr(ApiTofetchFpsTruckChallandata[j]["truckChitNo"].asString());
                qDebug() << "truckChitNo ="<<truckChitNo;

                char *AllocationOrderNo =objMisc.Convertstr(ApiTofetchFpsTruckChallandata[j]["allocationOrderNo"].asString());

                qDebug() << "AllocationOrderNo ="<<AllocationOrderNo;

                char *NoOfComm ="";

                char *ChallanId
                        =objMisc.Convertstr(ApiTofetchFpsTruckChallandata[j]["challanId"].asString());

                char *truckNo =objMisc.Convertstr(ApiTofetchFpsTruckChallandata[j]["truckNo"].asString());

                char *AllotedMonth =objMisc.Convertstr(ApiTofetchFpsTruckChallandata[j]["allotedMonth"].asString());

                char *AllotedYear =objMisc.Convertstr(ApiTofetchFpsTruckChallandata[j]["allotedYear"].asString());


                //Insert Truck Details into DB
                query.prepare("INSERT INTO TruckChit_Table(truck_chit_no, do_ro_no, comm_length) VALUES(:truck_chit_no, :do_ro_no, :comm_length)");
                query.bindValue(":truck_chit_no", truckChitNo);
                query.bindValue(":do_ro_no", AllocationOrderNo);
                query.bindValue(":comm_length", NoOfComm);
                if(!query.exec())
                    qDebug() << "In TruckChit_Table = " << query.lastError();


                Json::Value dat1 = ApiTofetchFpsTruckChallandata[j];
                Json::Value details = dat1.get("tcCommDetails",0);
                qDebug()<<"size of Details :" << details.size();

                if (details.size() > 0)
                {
                    for(int i=0; i< details.size(); i++)
                    {
                        char *commCode = objMisc.Convertstr(details[i]["commCode"].asString());
                        qDebug() << "commCode :" <<commCode;
                        char *schemeID = objMisc.Convertstr(details[i]["schemeId"].asString());
                        qDebug() << "schemeID :" <<schemeID;


                        //char * KRA = details[i]["releasedQuantity"].asString();
                        //qDebug() << "#### ???? KRA :" << KRA;

                        double dobreleased=0;
                        double doballotment=0;

                        char *releasedQuantity = objMisc.Convertstr(details[i]["releasedQuantity"].asString());
                        qDebug() << "releasedQuantity :" <<releasedQuantity;

                        char *allotment = objMisc.Convertstr(details[i]["allotment"].asString());
                        qDebug() << "allotment" <<allotment ;

                        dobreleased=atof(releasedQuantity);
                        doballotment=atof(allotment);

                        char *commName =  objMisc.Convertstr(details[i]["commName"].asString());
                        qDebug() << "commName" <<commName ;

                        char *schemeName = objMisc.Convertstr(details[i]["schemeName"].asString());
                        qDebug() << "schemeName ="<<schemeName;  //added by vivek 04082017

                        //Convert Quintal to KGS
                        /*
                         fKRA = atof(KRA) * 100;
                         freleasedQuantity = atof(releasedQuantity) * 100;
                         fqtD = atof(qtD) * 100;
                          */

                        //Insert into DB
                        query.clear();
                        query.prepare("INSERT INTO Stock_Details(entryDate, AllocationOrderNo, truckChitNo, ChallanId, truckNo, NoOfComm, AllotedMonth, AllotedYear, commCode, commName,schemeID, KRA, releasedQuantity, qtD, schemeName) VALUES(:entryDate, :AllocationOrderNo, :truckChitNo, :ChallanId, :truckNo, :NoOfComm, :AllotedMonth, :AllotedYear, :commCode, :commName, :schemeID, :KRA, :releasedQuantity, :qtD, :schemeName)");

                        query.bindValue(":entryDate", "");
                        query.bindValue(":AllocationOrderNo", AllocationOrderNo);
                        query.bindValue(":truckChitNo", truckChitNo);
                        query.bindValue(":ChallanId", ChallanId);
                        query.bindValue(":truckNo", truckNo);
                        query.bindValue(":NoOfComm", "");
                        query.bindValue(":AllotedMonth", AllotedMonth);
                        query.bindValue(":AllotedYear", AllotedYear);
                        query.bindValue(":commCode", commCode);
                        query.bindValue(":commName", commName);
                        query.bindValue(":schemeID", schemeID);
                        query.bindValue(":KRA", "");
                        query.bindValue(":releasedQuantity",dobreleased);
                        query.bindValue(":qtD", doballotment);
                        query.bindValue(":schemeName", schemeName);

                        if(!query.exec())
                        {
                            qDebug() << query.lastError();
                            qDebug() << "Stock Details Insertion Failed";
                            return -1;
                        }
                        else
                            qDebug() << "Stock Details Insertion Successfully";

                    }

                }
            }
        }
    }
    return 0;
}

int JsonResponse::ParsingFirstConditionJson(char *FileName)
{

    Json::Value root;
    Json::Reader reader;
    std::ifstream ReadData(FileName);
    char *crespCode,*crespMessage;
    QString QrespCode;
    bool parsedSuccess = reader.parse(ReadData,root,false);

    if(not parsedSuccess)
    {
        return -2;
    }
    else
    {
        qDebug () << "parsing on the way";
        crespCode = objMisc.Convertstr(root["respCode"].asString());
        crespMessage=objMisc.Convertstr(root["respMessage"].asString());
        qDebug() << crespCode;
    }

    char dispbuff[256];
    memset(dispbuff, 0, sizeof(dispbuff));

    QrespCode = crespCode;
    if(QrespCode.isEmpty() || QrespCode.isNull())
    {
        sprintf(dispbuff, "%s", "Invalid Response...\nPlease Try Again...");
        objMisc.DisplayWideMsgBox((char *)dispbuff);

        return -1;
    }
    else if(QrespCode != "00")
    {
        sprintf(dispbuff, "%s(%s)",crespMessage ,QrespCode.toAscii().data());
        objMisc.DisplayWideMsgBox((char *)dispbuff);

        return -1;
    }

    return 0;
}

