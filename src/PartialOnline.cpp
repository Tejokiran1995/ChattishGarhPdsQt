///*For Txn upload report*/select (select count() from BenfiaryTxn) as Total_Txn,(select count() from BenfiaryTxn where TxnUploadSts='Y' and TxnType = 'O') as Online_Txn,(select count() from BenfiaryTxn where TxnType ='P' or TxnType ='Q') as PartialOfline_Txn,(select count() from BenfiaryTxn where  TxnUploadSts='Y' and TxnType='P' or TxnType ='Q') as Upload_Txn
#include "ui_popup_msg.h"
#include"ui_widget.h"
#include<widget.h>
#include<popup_msg.h>
#include<QDateTime>
#include<stdio.h>
#include<stdlib.h>
popup_msg *PopMsg;
extern QString DeviceMacId;
extern TxnUpload UploadData;
char *Convertstr(string str)
{
    char* cstr;
    string fname = str + '\0';
    cstr = new char [fname.size()+1];
    strcpy( cstr, fname.c_str());
    return cstr;
}
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}
int CurlRequest(char *Url,char *Data,int TIME_OUT)
{
    qDebug() << "CurlRequest ::" << Url;

    CURL *curl;
    FILE *fp;
    int res=-1;
    char errorbuf[300];
    memset(errorbuf,0x00,sizeof(errorbuf));
    //
    curl = curl_easy_init();
    if(curl)
    {
        struct curl_slist *header = NULL;
        header= curl_slist_append(header,"Accept:*/*");
        header= curl_slist_append(header,"Content-type:application/json");
        header= curl_slist_append(header,"Expect:");
        header= curl_slist_append(header,"charset=utf-8");

        curl_easy_setopt(curl, CURLOPT_URL,Url);
        qDebug()<<"Url :"<<Url;
        qDebug()<<"Data:"<<Data;
        fp = fopen("/tmp/Sample.txt","w");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);

        curl_easy_setopt(curl, CURLOPT_POST,true);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, Data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,strlen(Data));

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        //        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIME_OUT);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorbuf);
        res = curl_easy_perform(curl);

        printf("\nResponce error:%s\n",errorbuf);
        fprintf(stderr,"\n\nCurl Return Val:%d\n\n",res);

        curl_slist_free_all(header);
        curl_easy_cleanup(curl);
        fclose(fp);

        if(res == CURLE_OK)
        {
            return 0;
        }
        else
        {
            return res;
        }
    }
}
void Widget::DataDownloadError()
{
    if(LangMode == ENGLISH)
        PopMsg->ui->lblPopup->setText("Offline Data Downloading Failed\n      Please Try again ..");
    else if(LangMode == HINDI)
        PopMsg->ui->lblPopup->setText(trUtf8("ऑफ़लाइन डेटा डाउनलोडिंग विफल हुई \n कृपया पुनः प्रयास करें .."));

    PopMsg->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    sleep(3);
    PopMsg->close();
    PopMsg->deleteLater();
}


int Widget::GetOflineDbBkp()
{
    char UploadFileName[512],TmpStr[512];
    memset(UploadFileName,0x00,sizeof(UploadFileName));
    sprintf(UploadFileName,"PFile_%02d%02d%04d_%02d%02d%02d",QDateTime::currentDateTime().toString("dd").toInt(),QDateTime::currentDateTime().toString("MM").toInt(),QDateTime::currentDateTime().toString("yyyy").toInt(),QDateTime::currentDateTime().toString("hh").toInt(),QDateTime::currentDateTime().toString("mm").toInt(),QDateTime::currentDateTime().toString("ss").toInt());
    qDebug()<<"UploadFileName : "<<UploadFileName;

    memset(TmpStr,0x00,sizeof(TmpStr));
    sprintf(TmpStr,"cd /mnt/jffs2/;/usr/bin/sqlite3 /mnt/jffs2/db_commonpds_2_3.db \".dump BenfiaryTxn Pos_Ob\"  > /mnt/jffs2/%s.sql",UploadFileName);
    qDebug()<<"TmpStr:"<<TmpStr;
    FILE *fp = popen(TmpStr,"r");
    pclose(fp);

    memset(TmpStr,0x00,sizeof(TmpStr));
    sprintf(TmpStr,"cd /mnt/jffs2/;tar -zcvf /mnt/jffs2/PartialTxnBkp_%02d%02d%04d_%02d%02d%02d.tar.gz %s.sql",QDateTime::currentDateTime().toString("dd").toInt(),QDateTime::currentDateTime().toString("MM").toInt(),QDateTime::currentDateTime().toString("yyyy").toInt(),QDateTime::currentDateTime().toString("hh").toInt(),QDateTime::currentDateTime().toString("mm").toInt(),QDateTime::currentDateTime().toString("ss").toInt(),UploadFileName);
    qDebug()<<"TmpStr1:"<<TmpStr;
    fp = popen(TmpStr,"r");
    pclose(fp);

    memset(TmpStr,0x00,sizeof(TmpStr));
    sprintf(TmpStr,"rm /mnt/jffs2/PFile_*");
    qDebug()<<"TmpStr2:"<<TmpStr;
    fp = popen(TmpStr,"r");
    pclose(fp);

    QStringList FileList;
    string Str;
    fp = popen("ls -ltr /mnt/jffs2/PartialTxnBkp_*","r");
    FileList.clear();
    while(fgets(TmpStr,sizeof(TmpStr),fp))
    {
        TmpStr[strlen(TmpStr)-1]=NULL;
        Str.clear();
        qDebug()<<"TmpStr :"<<TmpStr;
        Str.append(TmpStr);
        qDebug()<<Str.substr(Str.find("/mnt/"),Str.size()).c_str();
        FileList.append(Str.substr(Str.find("/mnt/"),Str.size()).c_str());
    }
    pclose(fp);
    if(FileList.size()>5)
    {
        memset(TmpStr,0x00,sizeof(TmpStr));
        sprintf(TmpStr,"chmod -R 777 %s;rm -rf %s",FileList.at(0).toAscii().data(),FileList.at(0).toAscii().data());
        qDebug()<<"TmpStr :"<<TmpStr;
        system(TmpStr);
    }
    if(PartialTxnDeleteFlag.toUpper() == "Y")
    {
        QSqlQuery query;
        query.clear();
        if(!query.exec("delete from BenfiaryTxn"))
        {
            //qDebug() << "Pos_Ob Delete Table  Error:" << query.lastError();
            //DataDownloadError();
            //return -1;
        }

        query.clear();
        if(!query.exec("delete from OfflineStockReceive"))
        {
            //qDebug() << "OfflineStockReceive Delete Table  Error:" << query.lastError();
            //DataDownloadError();
            //return -1;
        }
    }
}

int Widget::UpdateDownLoadStatus()
{
    PopMsg = new popup_msg(this);
    if(LangMode == ENGLISH)
        PopMsg->ui->lblPopup->setText("Updating Downloading Status\n      Please Wait ..");
    else if(LangMode == HINDI)
        PopMsg->ui->lblPopup->setText(trUtf8("डाउनलोड स्थिति अपडेट कर रहा है \n कृपया प्रतीक्षा करें .."));
    PopMsg->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    Json::Value RequestedData;
    char FpsId[30];
    memset(FpsId,0x00,sizeof(FpsId));
    getShopId(FpsId);
    char TmpStr[500];

    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));
    miscObj.GetMachineId(cmachineID);

    QSqlQuery query;
    query.clear();
    query.prepare("select fpsSessionId from Dealer");
    query.exec();query.next();

    QSqlQuery query1;
    QString AllotmentMonth,AllotmentYear;
    query1.prepare("SELECT AllotMonth, AllotYear FROM PartialOnlineData");
    if(!query1.exec())
        qDebug() << query1.lastError();
    else
    {
        while(query1.next())
        {
            AllotmentMonth = query1.value(0).toString();
            AllotmentYear  = query1.value(1).toString();
        }
    }

    RequestedData["fpsId"] = FpsId;
    RequestedData["stateCode"] = stateCode.toStdString().c_str();
    RequestedData["token"] = RECEIVEGOODS_TOKEN;
    RequestedData["sessionId"] =query.value(0).toString().toAscii().data();
    RequestedData["terminalId"]=cmachineID;
    RequestedData["keyregisterDataDeleteStatus"]=PartialTxnDeleteFlag.toAscii().data();
    RequestedData["dataDownloadStatus"]="Y";
    RequestedData["distributionMonth"]=AllotmentMonth.toAscii().data();
    RequestedData["distributionYear"]=AllotmentYear.toAscii().data();

    if(PartialTxnDeleteFlag.toUpper() == "Y")
    {
        Json::Value CommWiseData;
        Json::Value ArrayList(Json::arrayValue);
        ArrayList.clear();
        query.clear();
        char TmpStr[512];
        memset(TmpStr,0x00,sizeof(TmpStr));
        sprintf(TmpStr,"select RcId,CommCode,TotQty,BalQty,SchemeId,IssuedQty,RecptId,commAmount,TotAmt,Rate,MemberName,DateTime,TxnType,AllotMonth,AllotYear,allocationType from BenfiaryTxn where TxnUploadSts ='N'");
        if(!query.exec(TmpStr))
        {
            qDebug()<<"Upload Query Error:"<<query.lastError();
        }
        while(query.next())
        {
            CommWiseData.clear();
            CommWiseData["rcId"]=query.value(0).toString().toAscii().data();
            CommWiseData["commCode"]=query.value(1).toString().toAscii().data();
            CommWiseData["totalEntitlement"]=query.value(2).toString().toAscii().data();
            CommWiseData["balanceEntitlement"]=query.value(3).toString().toAscii().data();
            CommWiseData["schemeId"]=query.value(4).toString().toAscii().data();
            CommWiseData["issueQty"]=query.value(5).toString().toAscii().data();
            CommWiseData["receiptId"]=query.value(6).toString().toAscii().data();
            CommWiseData["commAmount"]=query.value(7).toString().toAscii().data();
            CommWiseData["totalAmount"]=query.value(8).toString().toAscii().data();
            CommWiseData["commPrice"]=query.value(9).toString().toAscii().data();
            CommWiseData["headOfTheFamily"]=query.value(10).toString().toAscii().data();
            CommWiseData["transactionTime"]=query.value(11).toString().toAscii().data();
            CommWiseData["transMode"]=query.value(12).toString().toAscii().data();
            CommWiseData["allotedMonth"]=query.value(13).toString().toAscii().data();
            CommWiseData["allotedYear"]=query.value(14).toString().toAscii().data();
            CommWiseData["allocationType"]=query.value(15).toString().toAscii().data();
            ArrayList.append(CommWiseData);
        }
        RequestedData["fpsOfflineTransResponses"]=ArrayList;
    }

    /////////////////////////////////////////////////////////////
    RequestedData["totalRecords"]=0;
    RequestedData["uploadingRecords"]=0;
    RequestedData["pendingRecords"]=0;
    /////////////////////////////////////////////////////////////

    char *Ret=Convertstr(RequestedData.toStyledString());
    //    int RetVal = CurlRequest("http://164.100.65.97/ePosCommonService/eposCommon/dataDownloadACK",Ret);

    char URL[125];
    memset(URL,0x00,sizeof(URL));
//    sprintf(&URL[0],"%s/dataDownloadACK",OFFLINE_AND_RECEIVE_STOCK_URL);
    sprintf(&URL[0],"%sdataDownloadACK",Offline_and_ReceiveStock_URL.toAscii().data());
    int RetVal = CurlRequest(URL,Ret,300);

    PopMsg->close();
    PopMsg->deleteLater();
    if(RetVal!=0)
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox("Invalid Response\nPlease try again");
        else
            miscObj.tel_DisplayWideMsgBox("अमान्य प्रतिक्रिया\nकृपया पुन: प्रयास करें");
        return -1;
    }
    else
    {
        Json::Value root;
        root.clear();
        Json::Reader reader;
        std::ifstream ReadData("/tmp/Sample.txt");
        system("cat /tmp/Sample.txt");
        bool parsedSuccess = reader.parse(ReadData,root,false);
        ReadData.close();
        if(not parsedSuccess)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox("Invalid Response from Server\nPlease try again");
            else
                miscObj.tel_DisplayWideMsgBox("सर्वर से अमान्य प्रतिक्रिया\nकृपया पुन: प्रयास करें");

            return -1;
        }
        else
        {
            printf("\nrespCode    : %s\n",Convertstr(root["respCode"].asString()));
            printf("\nrespMessage : %s\n",Convertstr(root["respMessage"].asString()));
            if(atoi(Convertstr(root["respCode"].asString()))==0)
            {
                miscObj.DisplayWideMsgBox(Convertstr(root["respMessage"].asString()));
                GetOflineDbBkp(); //here BenfiaryTxn and OfflineStockReceive tables will delete when PartialTxnDeleteFlag == 'Y'
                return 0;
            }
            else
            {
//                miscObj.DisplayWideMsgBox(Convertstr(root["respMessage"].asString()));
                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"%s\nError Code :%s",Convertstr(root["respMessage"].asString()),Convertstr(root["respCode"].asString()));
                miscObj.DisplayWideMsgBox((char *)TmpStr);
                return -1;
            }
        }
    }
}

int Widget::DwnldPartialOnlineData()
{
    gl11_lcdbklt_on();
    PopMsg = new popup_msg(this);
    if(LangMode == ENGLISH)
        PopMsg->ui->lblPopup->setText("Downloading Offline Data\n      Please Wait ..");
    else if(LangMode == HINDI)
        PopMsg->ui->lblPopup->setText(trUtf8("ऑफ़लाइन डेटा डाउनलोड कर रहा है \n कृपया प्रतीक्षा करें .."));

    PopMsg->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    Json::Value RequestedData;
    char FpsId[30];
    memset(FpsId,0x00,sizeof(FpsId));
    getShopId(FpsId);

    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));
    miscObj.GetMachineId(cmachineID);

    QSqlQuery query;
    query.clear();
    query.prepare("select fpsSessionId from Dealer");
    query.exec();query.next();

    RequestedData["fpsId"] = FpsId;
    RequestedData["stateCode"] = stateCode.toStdString().c_str();
    RequestedData["token"] = RECEIVEGOODS_TOKEN;
    RequestedData["sessionId"] =query.value(0).toString().toAscii().data();//"0";
    RequestedData["terminalId"]=cmachineID;
    char *Ret=Convertstr(RequestedData.toStyledString());

    //    int RetVal = CurlRequest("http://164.100.65.97/ePosCommonService/eposCommon/getFpsOfflineData",Ret);

    char URL[125];
    memset(URL,0x00,sizeof(URL));
//    sprintf(&URL[0],"%s/getFpsOfflineData",OFFLINE_AND_RECEIVE_STOCK_URL);
    sprintf(&URL[0],"%sgetFpsOfflineData",Offline_and_ReceiveStock_URL.toAscii().data());

    int RetVal = CurlRequest(URL,Ret,300);

    PopMsg->close();
    PopMsg->deleteLater();

    if(RetVal!=0)
        return -1;

    query.clear();
    if(!query.exec("delete from KeyRegister")){
        qDebug() << "KeyRegister Delete Table  Error:" << query.lastError();
        //DataDownloadError();
        //return -1;
    }
    query.clear();
    if(!query.exec("delete from Pos_Ob")){
        //qDebug() << "Pos_Ob Delete Table  Error:" << query.lastError();
        //DataDownloadError();
        //return -1;
    }

    system("sqlite3 /mnt/jffs2/db_commonpds_2_3.db \"drop table commodityMaster\"");
    sync();

    query.clear();
    if(!query.exec("delete from commodityMaster")){
        //qDebug() << "Pos_Ob Delete Table  Error:" << query.lastError();
        //DataDownloadError();
        //return -1;
    }
    query.clear();
    if(!query.exec("delete from schemeMaster")){
        //qDebug() << "Pos_Ob Delete Table  Error:" << query.lastError();
        //DataDownloadError();
        //return -1;
    }
    //    query.clear();
    //    if(!query.exec("delete from OfflineStockReceive"))
    //    {
    //        //qDebug() << "OfflineStockReceive Delete Table  Error:" << query.lastError();
    //        //DataDownloadError();
    //        //return -1;
    //    } //Added - thread disabled

    query.clear();
    if(!query.exec("CREATE TABLE IF NOT EXISTS KeyRegister(rcId text,commNameEn text,commNameLl text,commCode text,totalEntitlement text,balanceEntitlement text,schemeId text,schemeName text,commPrice text,Unit text,memberNameLl text,memberNameEn text,AllotMonth text,AllotYear text,allocationType text,allotedMonth text,allotedYear text)")){
        qDebug() << "KeyRegister Table  Error:" << query.lastError();
        //DataDownloadError();
        //return -1;
    }
    query.clear();
    if(!query.exec("CREATE TABLE IF NOT EXISTS Pos_Ob(commCode text,commNameEn text,commNameLl text,closingBalance text)")){
        qDebug() << "KeyRegister Table  Error:" << query.lastError();
        //DataDownloadError();
        //return -1;
    }
    query.clear();

    qDebug() << "To create the table commodityMaster" ;
    if(!query.exec("CREATE TABLE IF NOT EXISTS commodityMaster(commCode text,commNameEn text,commNameLl text,measurmentUnit text,commonCommCode text)")){
        qDebug() << "KeyRegister Table  Error:" << query.lastError();
        //DataDownloadError();
        //return -1;
    }
    query.clear();
    if(!query.exec("CREATE TABLE IF NOT EXISTS schemeMaster(schemeId text,schemeName text)")){
        qDebug() << "KeyRegister Table  Error:" << query.lastError();
        //DataDownloadError();
        //return -1;
    }
    query.clear();
    if(!query.exec("CREATE TABLE IF NOT EXISTS BenfiaryTxn(FpsId text,RcId text,SchemeId text,CommCode text,TotQty text,BalQty text,IssuedQty text,Rate text,commAmount text,TotAmt text,RecptId text,MemberName text,DateTime text,TxnUploadSts text,TxnType text,AllotMonth text, AllotYear text,allocationType text)")){
        qDebug() << "BenfiaryTxn Table  Error:" << query.lastError();
        //DataDownloadError();
        //return -1;
    }
    query.clear();
    if(!query.exec("CREATE TABLE IF NOT EXISTS OfflineStockReceive(TruckChitNum text,Challan text,VechNum text,FpsId text,SchemeId text,CommCode text,RecvdQty text,Unit text,Month text,Year text,DateTime text,RecvdUploadSts text,RecvdMode text)")){
        qDebug() << "OfflineStockReceive Table  Error:" << query.lastError();
    }

    Json::Value root;
    root.clear();
    //QSqlQuery query;
    Json::Reader reader;
    std::ifstream ReadData("/tmp/Sample.txt");
    system("cat /tmp/Sample.txt");
    bool parsedSuccess = reader.parse(ReadData,root,false);
    ReadData.close();
    if(not parsedSuccess){
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox("Invalid Response from Server\nPlease try again");
        else
            miscObj.tel_DisplayWideMsgBox("सर्वर से अमान्य प्रतिक्रिया\nकृपया पुन: प्रयास करें");
        DataDownloadError();
        return -1;
    }
    else{
        printf("\nrespCode    : %s\n",Convertstr(root["respCode"].asString()));
        printf("\nrespMessage : %s\n",Convertstr(root["respMessage"].asString()));

        if(atoi(Convertstr(root["respCode"].asString()))==0)
        {
            Json::Value KeyRegisterData;
            KeyRegisterData.clear();
            KeyRegisterData = root.get("keyRegister",0);
            printf("\nSize : %d\n",KeyRegisterData.size());
            if(KeyRegisterData.size()==0){
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox("KeyRegister Data not found\nPlease try again");
                else
                    miscObj.tel_DisplayWideMsgBox("KeyRegister Data नहीं मिला\nकृपया पुन: प्रयास करें");
                DataDownloadError();
                return -2;
            }
            else{
                char FloatData[10];
                for(int i=0;i<KeyRegisterData.size();i++){
                    query.clear();
                    //allocationType,allotedMonth,allotedYear

                    query.prepare("insert into KeyRegister(rcId,commNameEn,commNameLl,commCode,totalEntitlement,balanceEntitlement,schemeId,schemeName,commPrice,Unit,memberNameLl,memberNameEn,AllotMonth,AllotYear,allocationType,allotedMonth,allotedYear) VALUES(:rcId,:commNameEn,:commNameLl,:commCode,:totalEntitlement,:balanceEntitlement,:schemeId,:schemeName,:commPrice,:Unit,:memberNameLl,:memberNameEn,:AllotMonth,:AllotYear,:allocationType,:allotedMonth,:allotedYear)");
                    query.bindValue(":rcId",Convertstr(KeyRegisterData[i]["rcId"].asString()));
                    query.bindValue(":commNameEn",Convertstr(KeyRegisterData[i]["commNameEn"].asString()));
                    query.bindValue(":commNameLl",QString::fromUtf8(Convertstr(KeyRegisterData[i]["commNameLl"].asString())));
                    query.bindValue(":commCode",Convertstr(KeyRegisterData[i]["commCode"].asString()));
                    memset(FloatData,0x00,sizeof(FloatData));
                    sprintf(FloatData,"%0.3f",QString(Convertstr(KeyRegisterData[i]["totalEntitlement"].asString())).toFloat());
                    query.bindValue(":totalEntitlement",FloatData);
                    memset(FloatData,0x00,sizeof(FloatData));
                    sprintf(FloatData,"%0.3f",QString(Convertstr(KeyRegisterData[i]["balanceEntitlement"].asString())).toFloat());
                    query.bindValue(":balanceEntitlement",FloatData);
                    query.bindValue(":schemeId",Convertstr(KeyRegisterData[i]["schemeId"].asString()));
                    query.bindValue(":schemeName",Convertstr(KeyRegisterData[i]["schemeName"].asString()));
                    memset(FloatData,0x00,sizeof(FloatData));
                    sprintf(FloatData,"%0.3f",QString(Convertstr(KeyRegisterData[i]["commPrice"].asString())).toFloat());
                    query.bindValue(":commPrice",FloatData);
                    query.bindValue(":Unit",Convertstr(KeyRegisterData[i]["measurmentUnit"].asString()));
                    query.bindValue(":memberNameLl",Convertstr(KeyRegisterData[i]["memberNameLl"].asString()));
                    query.bindValue(":memberNameEn",Convertstr(KeyRegisterData[i]["memberNameEn"].asString()));
                    query.bindValue(":AllotMonth",Convertstr(KeyRegisterData[i]["month"].asString()));
                    query.bindValue(":AllotYear",Convertstr(KeyRegisterData[i]["year"].asString()));
                    query.bindValue(":allocationType",Convertstr(KeyRegisterData[i]["allocationType"].asString()));
                    query.bindValue(":allotedMonth",Convertstr(KeyRegisterData[i]["allotedMonth"].asString()));
                    query.bindValue(":allotedYear",Convertstr(KeyRegisterData[i]["allotedYear"].asString()));

                    if(!query.exec()){
                        if(LangMode == 1)
                            miscObj.DisplayWideMsgBox("Invalid KeyRegister Data\nPlease try again");
                        else
                            miscObj.tel_DisplayWideMsgBox("अमान्य KeyRegister Data\nकृपया पुन: प्रयास करें");
                        qDebug() << query.lastError();
                        DataDownloadError();
                        return -3;
                    }
                }
            }
            Json::Value PosOb;
            PosOb.clear();
            PosOb = root.get("fpsCb",0);
            printf("\nSize : %d\n",PosOb.size());
            if(PosOb.size()==0){
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox("Pos Ob Data not found\nPlease try again");
                else
                    miscObj.tel_DisplayWideMsgBox("PosOb डेटा नहीं मिला\nकृपया पुन: प्रयास करें");
                DataDownloadError();
                return -2;
            }
            else{
                char FloatData[10];
                for(int i=0;i<PosOb.size();i++){
                    query.clear();
                    query.prepare("insert into Pos_Ob(commCode,commNameEn,commNameLl,closingBalance) VALUES(:commCode,:commNameEn,:commNameLl,:closingBalance)");
                    query.bindValue(":commCode",Convertstr(PosOb[i]["commCode"].asString()));
                    query.bindValue(":commNameEn",Convertstr(PosOb[i]["commNameEn"].asString()));
                    query.bindValue(":commNameLl",QString::fromUtf8(Convertstr(PosOb[i]["commNameLl"].asString())));
                    memset(FloatData,0x00,sizeof(FloatData));
                    sprintf(FloatData,"%0.3f",QString(Convertstr(PosOb[i]["closingBalance"].asString())).toFloat());
                    query.bindValue(":closingBalance",FloatData);
                    if(!query.exec()){
                        if(LangMode == 1)
                            miscObj.DisplayWideMsgBox("Invalid PosOb Data found\nPlease try again");
                        else
                            miscObj.tel_DisplayWideMsgBox("अमान्य PosOb डेटा\nकृपया पुन: प्रयास करें");
                        qDebug() << query.lastError();
                        DataDownloadError();
                        return -3;
                    }
                }
            }
            Json::Value CommCode;
            CommCode.clear();
            CommCode = root.get("commodityMaster",0);
            printf("\nSize : %d\n",PosOb.size());
            if(CommCode.size()==0){
                qDebug("\ncommodityMaster Data Not Found\n");
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox("Commodity Master Data not found\nPlease try again");
                else
                    miscObj.tel_DisplayWideMsgBox("कमोडिटी मास्टर डेटा नहीं मिला\nकृपया पुन: प्रयास करें");
                DataDownloadError();
                return -2;
            }
            else{
                for(int i=0;i<CommCode.size();i++){
                    query.clear();
                    query.prepare("insert into commodityMaster(commCode,commNameEn,commNameLl,measurmentUnit,commonCommCode) VALUES(:commCode,:commNameEn,:commNameLl,:measurmentUnit,:commonCommCode)");
                    query.bindValue(":commCode",Convertstr(CommCode[i]["commCode"].asString()));
                    query.bindValue(":commNameEn",Convertstr(CommCode[i]["commNameEn"].asString()));
                    query.bindValue(":commNameLl",QString::fromUtf8(Convertstr(CommCode[i]["commNameLl"].asString())));
                    query.bindValue(":measurmentUnit",Convertstr(CommCode[i]["measurmentUnit"].asString()));
                    query.bindValue(":commonCommCode",Convertstr(CommCode[i]["commonCommCode"].asString()));
                    if(!query.exec()){
                        if(LangMode == 1)
                            miscObj.DisplayWideMsgBox("Invalid Commodity Master Data\nPlease try again");
                        else
                            miscObj.tel_DisplayWideMsgBox("अमान्य Commodity Master डेटा\nकृपया पुन: प्रयास करें");
                        qDebug()<<"Insert commodityMaster Data";
                        qDebug() << query.lastError();
                        DataDownloadError();
                        return -3;
                    }
                }
            }
            Json::Value SchemId;
            SchemId.clear();
            SchemId = root.get("schemeMaster",0);
            printf("\nSize : %d\n",SchemId.size());
            if(SchemId.size()==0)
            {
                miscObj.DisplayWideMsgBox("Scheme Master Data not found");
                qDebug("\nScheme Master Data Not Found\n");
                DataDownloadError();
                return -2;
            }
            else
            {
                for(int i=0;i<SchemId.size();i++){
                    query.clear();
                    query.prepare("insert into schemeMaster(schemeId,schemeName) VALUES(:schemeId,:schemeName)");
                    query.bindValue(":schemeId",Convertstr(SchemId[i]["schemeId"].asString()));
                    query.bindValue(":schemeName",Convertstr(SchemId[i]["schemeName"].asString()));
                    if(!query.exec()){
                        if(LangMode ==1)
                            miscObj.DisplayWideMsgBox("Invalid Scheme Master Data\nPlease try again");
                        else
                            miscObj.tel_DisplayWideMsgBox("अमान्य Scheme Master डेटा\nकृपया पुन: प्रयास करें");
                        qDebug()<<"Insert schemeMaster Data";
                        qDebug() << query.lastError();
                        DataDownloadError();
                        return -3;
                    }
                }
            }

            //Added - thread disabled
            Json::Value Rgoods;
            Rgoods.clear();
            Rgoods = root.get("challanReceived",0);
            printf("\nSize : %d\n",Rgoods.size());
            if(Rgoods.size()==0)
            {
                qDebug("\nOffline Stock Receive Data Not Found\n");
                //            miscObj.DisplayWideMsgBox("Offline Stock Receive Data not found");
                //            DataDownloadError();
                //            return -2; //Commented -BSK.
            }
            else
            {
                for(int i=0;i<Rgoods.size();i++)
                {
                    query.clear();
                    query.prepare("insert into OfflineStockReceive(TruckChitNum,Challan,Month,Year,FpsId,DateTime,RecvdMode,CommCode,SchemeId,RecvdQty,Unit,RecvdUploadSts) VALUES(:TruckChitNum,:Challan,:Month,:Year,:FpsId,:DateTime,:RecvdMode,:CommCode,:SchemeId,:RecvdQty,:Unit,:RecvdUploadSts)");
                    query.bindValue(":TruckChitNum",Convertstr(Rgoods[i]["truckChitId"].asString()));
                    query.bindValue(":Challan",Convertstr(Rgoods[i]["challanId"].asString()));
                    query.bindValue(":Month",Convertstr(Rgoods[i]["month"].asString()));
                    query.bindValue(":Year",Convertstr(Rgoods[i]["year"].asString()));
                    query.bindValue(":FpsId",Convertstr(Rgoods[i]["fpsId"].asString()));
                    query.bindValue(":DateTime",Convertstr(Rgoods[i]["receivedDateTime"].asString()));
                    query.bindValue(":RecvdMode",Convertstr(Rgoods[i]["receivedMode"].asString()));
                    query.bindValue(":CommCode",Convertstr(Rgoods[i]["commCode"].asString()));
                    query.bindValue(":SchemeId",Convertstr(Rgoods[i]["schemeId"].asString()));
                    query.bindValue(":RecvdQty",Convertstr(Rgoods[i]["received"].asString()));
                    query.bindValue(":Unit",Convertstr(Rgoods[i]["units"].asString()));
                    query.bindValue(":RecvdUploadSts","Y");
                    if(!query.exec())
                    {
                        if(LangMode ==1)
                            miscObj.DisplayWideMsgBox("Invalid Recive Goods Master\nPlease try again");
                        else
                            miscObj.tel_DisplayWideMsgBox("अमान्य Recive Goods Master डेटा\nकृपया पुन: प्रयास करें");
                        qDebug()<<"Insert Recive Goods Master Data";
                        qDebug() << query.lastError();
                        DataDownloadError();

                        return -3;
                    }
                }
            }
        }
        else
        {
//            PopMsg->close();
//            PopMsg->deleteLater();
            char TmpStr[200];
            sprintf(TmpStr,"%s(%s)",Convertstr(root["respMessage"].asString()),Convertstr(root["respCode"].asString()));
            miscObj.DisplayWideMsgBox((char *)TmpStr);
//            miscObj.DisplayWideMsgBox(Convertstr(root["respMessage"].asString()));
            return -3;
        }
    }
//    PopMsg->close();
//    PopMsg->deleteLater();
    return 0;
}

int Widget::GetOfflineCommDetails()
{    
    qDebug()<<"In Partial Get comm Details";
    if(DealerLoginBy!=2)
    {
        popup_msg *dlgPopup1 = new popup_msg(this);
        if (LangMode == ENGLISH)
            dlgPopup1->ui->lblPopup->setText("No Connectivity\nGoing for Offline txn");
        else if(LangMode == HINDI)
            dlgPopup1->ui->lblPopup->setText(trUtf8("कनेक्टिविटी नहीं है\n ऑफ़लाइन लेनदेन प्रकार के लिए जा रहा है"));

        dlgPopup1->show();
        QCoreApplication::processEvents();
        QCoreApplication::processEvents();
        sleep(3);
        dlgPopup1->close();
        dlgPopup1->deleteLater();
    }
    PartialTxn = 1;
    iModuleType = 99;
    int row = 0;
    char buff[200];
    QSqlQuery query;
    query.clear();
    for(int k=ui->tblCommIssue->rowCount()-1;k>=0;k--)
    {
        ui->tblCommIssue->removeRow(k);
    }

//    if(LangMode == 1)
//        query.prepare("select a.commNameEn,a.totalEntitlement,a.commPrice,a.commCode,a.balanceEntitlement,a.Unit,b.closingBalance from KeyRegister a,Pos_Ob b where a.rcId=? and b.commCode=a.commCode");
//    else
//        query.prepare("select a.commNameLl,a.totalEntitlement,a.commPrice,a.commCode,a.balanceEntitlement,a.Unit,b.closingBalance from KeyRegister a,Pos_Ob b where a.rcId=? and b.commCode=a.commCode");

    if(LangMode == 1)
        //select a.commNameEn, totalEntitlement, a.commPrice, a.commCode, balanceEntitlement, a.Unit, closingBalance, b.commCode from (select rcid,a.commNameEn,schemeName,a.commCode,a.commPrice, commonCommCode, a.Unit, totalEntitlement, balanceEntitlement, b.commCode from KeyRegister a, commodityMaster b where a.commCode=b.commCode)a join Pos_Ob b on a.commonCommCode=b.commCode where rcid=? order by b.commcode
        query.prepare("select a.commNameEn, totalEntitlement, a.commPrice, a.commCode, balanceEntitlement, a.Unit, closingBalance, b.commCode, allocationType, allotedMonth, allotedYear from (select rcid,a.commNameEn,schemeName,a.commCode,a.commPrice, commonCommCode, a.Unit, totalEntitlement, balanceEntitlement, b.commCode, allocationType, allotedMonth, allotedYear from KeyRegister a, commodityMaster b where a.commCode=b.commCode)a join Pos_Ob b on a.commonCommCode=b.commCode where rcid=? order by b.commcode");
    else
        query.prepare("select a.commNameLl, totalEntitlement, a.commPrice, a.commCode, balanceEntitlement, a.Unit, closingBalance, b.commCode, allocationType, allotedMonth, allotedYear from (select rcid,a.commNameLl,schemeName,a.commCode,a.commPrice, commonCommCode, a.Unit, totalEntitlement, balanceEntitlement, b.commCode, allocationType, allotedMonth, allotedYear from KeyRegister a, commodityMaster b where a.commCode=b.commCode)a join Pos_Ob b on a.commonCommCode=b.commCode where rcid=? order by b.commcode");

    query.addBindValue(ui->ldtRationCard->text().toAscii().data());
    if(!query.exec())
    {
        qDebug()<<"In Partial Get comm Details Error";
        if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox("इस कार्ड के लिए वस्तु उपलब्ध नहीं है");
        else
            miscObj.DisplayWideMsgBox("No Commodities For This Card"); //Unable to get Commodity Details From Server...
        return -1;
    }
    while(query.next())
    {
        ui->tblCommIssue->insertRow(row);
        {
            if(LangMode == 1)
            {
                memset(buff,0,sizeof(buff));
                sprintf(buff, "%s(%s)", query.value(0).toString().toAscii().data(), query.value(1).toString().toAscii().data());
                ui->tblCommIssue->setItem(row, 0, new QTableWidgetItem(buff));
            }
            else
            {
                QString StringTmp;
                StringTmp.clear();
                StringTmp  = query.value(0).toString();
                StringTmp  += "(";
                StringTmp  += query.value(1).toString();
                StringTmp  += ")";
                ui->tblCommIssue->setItem(row, 0, new QTableWidgetItem(StringTmp));
            }
            ui->tblCommIssue->item(row, 0)->setFlags( ui->tblCommIssue->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tblCommIssue->item(row, 0)->setFlags(ui->tblCommIssue->item(row, 0)->flags() | Qt::ItemIsSelectable);
        }

        char iqty[32],prate[32]="";
        memset(iqty,0,sizeof(iqty));
        memset(prate,0,sizeof(prate));

        {
            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(4).toString().toAscii().data());
            qDebug()<<"1 Bal Qty = "<<buff;

            ui->tblCommIssue->setItem(row, 1, new QTableWidgetItem(buff));
            ui->tblCommIssue->item(row, 1)->setFlags(ui->tblCommIssue->item(row, 1)->flags() ^ Qt::ItemIsEditable);
            ui->tblCommIssue->item(row, 1)->setFlags(ui->tblCommIssue->item(row, 1)->flags() | Qt::ItemIsSelectable);
        }

        {   //Item Price
            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(2).toString().toAscii().data());
            qDebug()<<"2 Item Price = "<<buff;
            strcpy(prate,buff);

            ui->tblCommIssue->setItem(row, 2, new QTableWidgetItem(buff));
            ui->tblCommIssue->item(row, 2)->setFlags(ui->tblCommIssue->item(row, 2)->flags() ^ Qt::ItemIsEditable);
            ui->tblCommIssue->item(row, 2)->setFlags(ui->tblCommIssue->item(row, 2)->flags() | Qt::ItemIsSelectable);
        }

        QSqlQuery query1;
        float balanceEntitlement = 0.00;
        query1.clear();
        QString QueryStr = "SELECT sum(balanceEntitlement) FROM  Keyregister LEFT JOIN commodityMaster ON Keyregister.commCode = commodityMaster.CommCode where commodityMaster.commonCommCode = '" + query.value(7).toString() + "'";
        query1.prepare(QueryStr);

        qDebug() << query1.lastQuery();

        if(!query1.exec())
        {
            balanceEntitlement =  query.value(4).toString().toFloat();
        }
        else
        {
            while(query1.next())
            {
                balanceEntitlement =  query1.value(0).toFloat();
                qDebug() << "balanceEntitlement:" <<query1.value(0).toString();
            }
        }

        if(query.value(6).toString().toFloat() >= balanceEntitlement )
        {
            //issue qty
            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(4).toString().toAscii().data());
            qDebug()<<" Bal Qty as Issue Qty = "<<buff;
            strcpy(iqty,buff);

            ui->tblCommIssue->setItem(row, 3, new QTableWidgetItem(buff));
            ui->tblCommIssue->item(row, 3)->setData(Qt::BackgroundColorRole, QVariant(QColor(Qt::green)));

            //amount
            memset(buff,0,sizeof(buff));
            sprintf(buff,"%0.2f",atof(iqty)*atof(prate));

            ui->tblCommIssue->setItem(row, 4, new QTableWidgetItem(buff));
            ui->tblCommIssue->item(row, 4)->setFlags( ui->tblCommIssue->item(row,4)->flags() ^ Qt::ItemIsEditable);
            ui->tblCommIssue->item(row, 4)->setFlags(ui->tblCommIssue->item(row, 4)->flags() | Qt::ItemIsSelectable);
        }
        else
        {
            ui->tblCommIssue->setItem(row, 3, new QTableWidgetItem("0"));
            ui->tblCommIssue->setItem(row, 4, new QTableWidgetItem("0"));

            ui->tblCommIssue->item(row, 4)->setFlags( ui->tblCommIssue->item(row,4)->flags() ^ Qt::ItemIsEditable);
            ui->tblCommIssue->item(row, 4)->setFlags(ui->tblCommIssue->item(row, 4)->flags() | Qt::ItemIsSelectable);
        }

//        {
//            memset(buff,0,sizeof(buff));
//            strcpy(buff, query.value(4).toString().toAscii().data());
//            qDebug()<<" Bal Qty as Issue Qty = "<<buff;
//            strcpy(iqty,buff);
//            ui->tblCommIssue->setItem(row, 3, new QTableWidgetItem(buff));
//            ui->tblCommIssue->item(row, 3)->setData(Qt::BackgroundColorRole, QVariant(QColor(Qt::green)));
//            //            ui->tblCommIssue->item(0,3)->setFocus();
//        }
//        memset(buff,0,sizeof(buff));
//        sprintf(buff,"%0.2f",atof(iqty)*atof(prate));
//        {
//            ui->tblCommIssue->setItem(row, 4, new QTableWidgetItem(buff));
//            ui->tblCommIssue->item(row, 4)->setFlags( ui->tblCommIssue->item(row,4)->flags() ^ Qt::ItemIsEditable);
//            ui->tblCommIssue->item(row, 4)->setFlags(ui->tblCommIssue->item(row, 4)->flags() ^ Qt::ItemIsSelectable);
//        }

        {
            //Item Code
            memset(buff,0,sizeof(buff));
            strcpy(buff, query.value(3).toString().toAscii().data());
            ui->tblCommIssue->setItem(row, 5, new QTableWidgetItem(buff));
            ui->tblCommIssue->item(row, 5)->setFlags( ui->tblCommIssue->item(row,5)->flags() ^ Qt::ItemIsEditable);
            ui->tblCommIssue->item(row, 5)->setFlags(ui->tblCommIssue->item(row, 5)->flags() ^ Qt::ItemIsSelectable);
        }
        {
            ui->tblCommIssue->setItem(row, 6, new QTableWidgetItem(query.value(6).toString()));
            ui->tblCommIssue->item(row, 6)->setFlags( ui->tblCommIssue->item(row,6)->flags() ^ Qt::ItemIsEditable);
            ui->tblCommIssue->item(row, 6)->setFlags(ui->tblCommIssue->item(row, 6)->flags() ^ Qt::ItemIsSelectable);
        }

        ui->tblCommIssue->setItem(row, 10, new QTableWidgetItem(query.value(7).toString()));
        ui->tblCommIssue->setItem(row, 11, new QTableWidgetItem(query.value(8).toString()));
        ui->tblCommIssue->setItem(row, 12, new QTableWidgetItem(query.value(9).toString()));
        ui->tblCommIssue->setItem(row, 13, new QTableWidgetItem(query.value(10).toString()));

        ui->tblCommIssue->setRowHeight(row,20);
        row++;
    }

    if(row == 0)
    {
        if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox("इस कार्ड के लिए वस्तु उपलब्ध नहीं है");
        else
            miscObj.DisplayWideMsgBox("No Commodities for this Card"); //Unable to get Commodity Details From Server...

        return -1;
    }
    ui->tblCommIssue->setColumnWidth(0,120);
    ui->tblCommIssue->setColumnWidth(1,60);
    ui->tblCommIssue->setColumnWidth(2,60);
    ui->tblCommIssue->setColumnWidth(3,70);
    ui->tblCommIssue->setColumnWidth(4,60);
    ui->tblCommIssue->setColumnWidth(6,60);

    ui->tblCommIssue->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblCommIssue->verticalHeader()->hide();
    ui->tblCommIssue->setColumnHidden(5, true);
    ui->tblCommIssue->setColumnHidden(7, true);
    ui->tblCommIssue->setColumnHidden(8, true);
    ui->tblCommIssue->setColumnHidden(9, true);
    ui->tblCommIssue->setColumnHidden(10, true);
    ui->tblCommIssue->setColumnHidden(11, true);
    ui->tblCommIssue->setColumnHidden(12, true);
    ui->tblCommIssue->setColumnHidden(13, true);
    ui->tblCommIssue->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tblCommIssue->horizontalHeader()->setMinimumWidth(0);

    fTotalAmt = 0.00;
    fTotalQty = 0.00;
    ui->lblTotalAmtVal->setText("0.00");
    //ui->label_LapTime_4->clear();
    if(iMainTransaction == MDM_TRANSACTION || iMainTransaction == ICDS_TRANSACTION)
    {
        qDebug()<<"DISABLING DYNAMIC";
        ui->btnGetWt->hide();
        ui->comboCommMode->hide();
    }
    else
    {
        qDebug()<<"ENABLING DYNAMIC";
        //        ui->btnGetWt->show();
        //        ui->comboCommMode->show();
    }
    iModuleType = COMM_TXN;

    fTotalAmt = 0.00;
    fTotalQty = 0.00;
    int iNoOfRows = 0;
    int i = 0;

    iNoOfRows = ui->tblCommIssue->rowCount();
    for(i=0; i<iNoOfRows;i++)
    {
        if(ui->tblCommIssue->item(i, 4) != NULL)
        {
            fTotalAmt += atof(ui->tblCommIssue->item(i, 4)->text().toAscii().data());
        }
    }
    memset(buff, 0, sizeof(buff));
    sprintf(buff,"%7.2f",fTotalAmt);
    ui->lblTotalAmtVal->setText(buff);

    ui->lblRCNO_Val_2->setText(ui->ldtRationCard->text());
    ui->stackedWidget->setCurrentIndex(6);
    //return;
    // return 0;
}

void TxnUpload::run()
{
    ServerUploadPartialTxns();
}

void TxnUpload::ServerUploadPartialTxns()
{
    GetStateInfo();

    int SLEEPTIME=300;
    char TmpStr[500];

    int uploadingRecords=0,totalRecords=0,pendingRecords=0; //Offline-Onlie purpose

    while(1)
    {
        uploadingRecords = 0;
        qDebug()<<"In ServerUploadPartialTxns ";
        GPRS objGprs;
        if(objGprs.Check_GPRS_Available() < 0)
        {
            qDebug()<<"No Network";
            sleep(SLEEPTIME);
            continue ;
        }
        ReceiveGdsUpload(); //offline receive goods upload
        Json::Value UploadData,CommWiseData;
        QSqlQuery query;
        Json::Value ArrayList(Json::arrayValue);
        ArrayList.clear();
        UploadData.clear();
        char FpsId[30];
        memset(FpsId,0x00,sizeof(FpsId));
        QSqlQuery query1;
        query1.prepare("SELECT ShopID FROM  ShopOwnerDetails");
        if(!query1.exec())
            qDebug() << "SELECT ShopID FROM ShopOwnerDetails = " << query1.lastError();
        query1.next();
        //getShopId(FpsId);

        /*char cmachineID[16];
            memset(cmachineID, 0, sizeof(cmachineID));
            CMisc miscObj;
            miscObj.GetMachineId(cmachineID);*/

        query.clear();
        query.prepare("select fpsSessionId from Dealer");
        query.exec();query.next();

        UploadData["fpsId"]= query1.value(0).toString().toAscii().data();
        UploadData["stateCode"] = stateCode.toStdString().c_str();
        UploadData["token"]= RECEIVEGOODS_TOKEN;
        UploadData["sessionId"]=query.value(0).toString().toAscii().data();//"0";
        UploadData["terminalId"]=DeviceMacId.toAscii().data();
        query.clear();
        memset(TmpStr,0x00,sizeof(TmpStr));
        sprintf(TmpStr,"select RcId,CommCode,TotQty,BalQty,SchemeId,IssuedQty,RecptId,commAmount,TotAmt,Rate,MemberName,DateTime,TxnType,AllotMonth,AllotYear from BenfiaryTxn where TxnUploadSts ='N' limit 20");

        //query.prepare("select RcId,CommCode,TotQty,BalQty,SchemeId,IssuedQty,RecptId,commAmount,TotAmt,Rate,MemberName,DateTime,TxnType,AllotMonth,AllotYear from BenfiaryTxn where TxnUploadSts = ?");
        //query.addBindValue("N");

        if(!query.exec(TmpStr))
        {
            qDebug()<<"Upoad BenfiaryTxn Query Error:"<<query.lastError();
            sleep(SLEEPTIME);
            continue ; ;
        }
        while(query.next())
        {
            CommWiseData.clear();
            CommWiseData["rcId"]=query.value(0).toString().toAscii().data();
            CommWiseData["commCode"]=query.value(1).toString().toAscii().data();
            CommWiseData["totalEntitlement"]=query.value(2).toString().toAscii().data();
            CommWiseData["balanceEntitlement"]=query.value(3).toString().toAscii().data();
            CommWiseData["schemeId"]=query.value(4).toString().toAscii().data();
            CommWiseData["issueQty"]=query.value(5).toString().toAscii().data();
            CommWiseData["receiptId"]=query.value(6).toString().toAscii().data();
            CommWiseData["commAmount"]=query.value(7).toString().toAscii().data();
            CommWiseData["totalAmount"]=query.value(8).toString().toAscii().data();
            CommWiseData["commPrice"]=query.value(9).toString().toAscii().data();
            CommWiseData["headOfTheFamily"]=query.value(10).toString().toAscii().data();
            CommWiseData["transactionTime"]=query.value(11).toString().toAscii().data();
            CommWiseData["transMode"]=query.value(12).toString().toAscii().data();
            CommWiseData["month"]=query.value(13).toString().toAscii().data();
            CommWiseData["year"]=query.value(14).toString().toAscii().data();
            ArrayList.append(CommWiseData);
            uploadingRecords++;
        }
        UploadData["fpsOfflineTransResponses"]=ArrayList;
        query.prepare("select commCode,closingBalance from Pos_Ob");
        if(!query.exec())
        {
            qDebug()<<"Upoad CB Query Error:"<<query.lastError();
            sleep(SLEEPTIME);
            continue ; ;
        }
        ArrayList.clear();
        while(query.next())
        {
            CommWiseData.clear();
            CommWiseData["commCode"]=query.value(0).toString().toAscii().data();
            CommWiseData["closingBalance"]=query.value(1).toString().toAscii().data();
            ArrayList.append(CommWiseData);
        }
        UploadData["fpsCbs"]=ArrayList;
        char *Ret="";
        qDebug()<<"\n\n\nUploadData Size:"<<UploadData["fpsOfflineTransResponses"].size()<<"\n\n\n";
        if(UploadData["fpsOfflineTransResponses"].size()==0)
        {
            qDebug()<<"No Txn Were found for Upload";
            sleep(SLEEPTIME);
            continue ; ;
        }

        /////////////////////////////////////////////////////////////

        query.clear();
        query.exec("select count(*) from BenfiaryTxn where TxnType IN('P','Q')");
        query.next();
        totalRecords = query.value(0).toString().toInt(0);
        UploadData["totalRecords"]=totalRecords;
        UploadData["uploadingRecords"]=uploadingRecords;

        query.clear();
        query.exec("select count(*) from BenfiaryTxn where TxnType IN('P','Q') and TxnUploadSts ='N'");
        query.next();
        pendingRecords = query.value(0).toString().toInt(0);

        if(uploadingRecords < pendingRecords)
            UploadData["pendingRecords"]=(pendingRecords-uploadingRecords);
        else
            UploadData["pendingRecords"]=(uploadingRecords-pendingRecords);


        //        if(uploadingRecords > 0)
        //            UploadData["pendingRecords"]=(totalRecords-uploadingRecords);
        //        else
        //            UploadData["pendingRecords"]=uploadingRecords;

        /////////////////////////////////////////////////////////////

        Ret = Convertstr(UploadData.toStyledString());
        //qDebug()<<"Data :"<<Ret;
        //        int RetVal = CurlRequest("http://164.100.65.97/ePosCommonService/eposCommon/pushFpsOfflineData",Ret);

        char URL[125];
        memset(URL,0x00,sizeof(URL));
        sprintf(&URL[0],"%s/pushFpsOfflineData",OFFLINE_AND_RECEIVE_STOCK_URL);
//        sprintf(&URL[0],"%spushFpsOfflineData",Offline_and_ReceiveStock_URL.toAscii().data());
        int RetVal = CurlRequest(URL,Ret,300);

        if(RetVal==0)
        {
            Json::Value root;
            root.clear();
            //QSqlQuery query;
            Json::Reader reader;
            std::ifstream ReadData("/tmp/Sample.txt");
            system("cat /tmp/Sample.txt");
            bool parsedSuccess = reader.parse(ReadData,root,false);
            ReadData.close();
            if(not parsedSuccess)
            {
                qDebug()<<"Upload Status String  File not Found";
                sleep(SLEEPTIME);
                continue ;
            }
            else
            {
                printf("\nrespCode    : %s\n",Convertstr(root["respCode"].asString()));
                printf("\nrespMessage : %s\n",Convertstr(root["respMessage"].asString()));
                if(atoi(Convertstr(root["respCode"].asString()))==0)
                {
                    Json::Value UploadedData;
                    UploadedData.clear();
                    UploadedData = root.get("updatedReceipts",0);
                    printf("\nSize : %d\n",UploadedData.size());
                    if(UploadedData.size()==0)
                    {
                        sleep(SLEEPTIME);
                        continue ;
                    }
                    else
                    {
                        for(int i=0;i<UploadedData.size();i++)
                        {
                            query.clear();
                            memset(TmpStr,0x00,sizeof(TmpStr));
                            sprintf(TmpStr,"update BenfiaryTxn set TxnUploadSts = 'Y' where RecptId='%s' and RcId='%s' and CommCode='%s'",Convertstr(UploadedData[i]["receiptId"].asString()),Convertstr(UploadedData[i]["rcId"].asString()),Convertstr(UploadedData[i]["commCode"].asString()));
                            qDebug()<<"TmpStr Update Status:"<<TmpStr;
                            if(!query.exec(TmpStr))
                            {
                                qDebug()<<"Upoad Query Error:"<<query.lastError();
                                sleep(SLEEPTIME);
                                continue ; ;
                            }
                            continue;
                        }
                    }
                }
                else
                {
                    sleep(SLEEPTIME);
                    continue;
                }
            }
        }
        else
        {
            sleep(SLEEPTIME);
            continue;
        }
        //sleep(SLEEPTIME);
    }
}

int Widget::UpdateTransStatus()
{
//    int KeyRegTotalRecords=0,UploadedRecords=0;
    int totalRecords=0,uploadingRecords=0,pendingRecords=0;;
    int RetVal=-1;
    PopMsg = new popup_msg(this);
    PopMsg->ui->lblPopup->setText("Updating Transactions Status\n      Please Wait ..");

    PopMsg->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    Json::Value RequestedData;
    char FpsId[30];
    memset(FpsId,0x00,sizeof(FpsId));
    strcpy(FpsId,ui->ldtShpId->text().toAscii().data());

    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));
    miscObj.GetMachineId(cmachineID);

    QSqlQuery query;
    query.clear();
    query.prepare("select fpsSessionId from Dealer");
    query.exec();
    query.next();
    QString sessionIdQstr = query.value(0).toString();


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

    RequestedData["fpsId"] = FpsId;
    RequestedData["token"] = RECEIVEGOODS_TOKEN;
    RequestedData["sessionId"] = sessionIdQstr.toAscii().data();
    RequestedData["stateCode"] = stateCode.toStdString().c_str();
    RequestedData["terminalId"]=cmachineID;
    RequestedData["keyregisterDataDeleteStatus"]="U";
    RequestedData["dataDownloadStatus"]=partialOnlineOfflineStatus.toAscii().data();
    RequestedData["distributionMonth"]=AllotmentMonth.toAscii().data();
    RequestedData["distributionYear"]=AllotmentYear.toAscii().data();

    Json::Value CommWiseData;
    Json::Value ArrayList(Json::arrayValue);
    ArrayList.clear();
    query.clear();
    char TmpStr[512];
    memset(TmpStr,0x00,sizeof(TmpStr));
    sprintf(TmpStr,"select RcId,CommCode,TotQty,BalQty,SchemeId,IssuedQty,RecptId,commAmount,TotAmt,Rate,MemberName,DateTime,TxnType,AllotMonth,AllotYear,allocationType from BenfiaryTxn where TxnUploadSts ='N'");
    if(!query.exec(TmpStr))
    {
        qDebug()<<"Upload Query Error:"<<query.lastError();
    }
    while(query.next())
    {
        CommWiseData.clear();
        CommWiseData["rcId"]=query.value(0).toString().toAscii().data();
        CommWiseData["commCode"]=query.value(1).toString().toAscii().data();
        CommWiseData["totalEntitlement"]=query.value(2).toString().toAscii().data();
        CommWiseData["balanceEntitlement"]=query.value(3).toString().toAscii().data();
        CommWiseData["schemeId"]=query.value(4).toString().toAscii().data();
        CommWiseData["issueQty"]=query.value(5).toString().toAscii().data();
        CommWiseData["receiptId"]=query.value(6).toString().toAscii().data();
        CommWiseData["commAmount"]=query.value(7).toString().toAscii().data();
        CommWiseData["totalAmount"]=query.value(8).toString().toAscii().data();
        CommWiseData["commPrice"]=query.value(9).toString().toAscii().data();
        CommWiseData["headOfTheFamily"]=query.value(10).toString().toAscii().data();
        CommWiseData["transactionTime"]=query.value(11).toString().toAscii().data();
        CommWiseData["transMode"]=query.value(12).toString().toAscii().data();
//        CommWiseData["month"]=query.value(13).toString().toAscii().data();
//        CommWiseData["year"]=query.value(14).toString().toAscii().data();
        CommWiseData["allotedMonth"]=query.value(13).toString().toAscii().data();
        CommWiseData["allotedYear"]=query.value(14).toString().toAscii().data();
        CommWiseData["allocationType"]=query.value(15).toString().toAscii().data();

        ArrayList.append(CommWiseData);
        uploadingRecords++;
    }
    RequestedData["fpsOfflineTransResponses"]=ArrayList;

    /////////////////////////////////////////////////////////////

//    query.clear();
//    query.exec("select count(*) from KeyRegister");
//    query.next();
//    KeyRegTotalRecords = query.value(0).toString().toInt(0);

//    query.clear();
//    query.exec("select count(*) from BenfiaryTxn where TxnType = 'Y'");
//    query.next();
//    UploadedRecords = query.value(0).toString().toInt(0);

    query.clear();
    query.exec("select count(*) from BenfiaryTxn where TxnType IN('P','Q')");
    query.next();
    totalRecords = query.value(0).toString().toInt(0);
    RequestedData["totalRecords"]=totalRecords;
    RequestedData["uploadingRecords"]=uploadingRecords;

    query.clear();
    query.exec("select count(*) from BenfiaryTxn where TxnType IN('P','Q') and TxnUploadSts ='N'");
    query.next();
    pendingRecords = query.value(0).toString().toInt(0);

    if(uploadingRecords < pendingRecords)
        RequestedData["pendingRecords"]=(pendingRecords-uploadingRecords);
    else
        RequestedData["pendingRecords"]=(uploadingRecords-pendingRecords);

//    if(KeyRegTotalRecords != UploadedRecords)
//        RequestedData["fullDataUploadedStatus"]= "N";
//    else
//        RequestedData["fullDataUploadedStatus"]= "Y";

    if(PartiDataDownloadFlag.toUpper()=="Y")
        RequestedData["fullDataUploadedStatus"]= "Y";
    else
        RequestedData["fullDataUploadedStatus"]= "N";

    //    if(uploadingRecords > 0)
    //        RequestedData["pendingRecords"]=(totalRecords-uploadingRecords);
    //    else
    //        RequestedData["pendingRecords"]=uploadingRecords;;

    /////////////////////////////////////////////////////////////

    char *Ret=Convertstr(RequestedData.toStyledString());

    //    char cUrlName[256];
    //    memset(cUrlName, 0, sizeof(cUrlName));

    //    if(GeetingOfflineUrl(cUrlName))
    //    {
    //        miscObj.DisplayMsgBox((char *)"URL Not Found");
    //        return -1;
    //    }

    //    char OfflineUrl[1024];
    //    memset(OfflineUrl, 0x00, sizeof(OfflineUrl));
    //    sprintf(OfflineUrl,"%s/dataDownloadACK",cUrlName);
    //    if(uploadingRecords > 10)
    //        RetVal = CurlRequest(OfflineUrl,Ret,1800);
    //    else
    //        RetVal = CurlRequest(OfflineUrl,Ret,SERVER_TIME_OUT);
    //need take input

    //    RetVal = CurlRequest("http://164.100.65.97/ePosCommonService/eposCommon/dataDownloadACK",Ret);

    char URL[125];
    memset(URL,0x00,sizeof(URL));
//    sprintf(&URL[0],"%s/dataDownloadACK",OFFLINE_AND_RECEIVE_STOCK_URL);
    sprintf(&URL[0],"%sdataDownloadACK",Offline_and_ReceiveStock_URL.toAscii().data());
    RetVal = CurlRequest(URL,Ret,1800);

    PopMsg->close();
    PopMsg->deleteLater();
    if(RetVal!=0)
    {
        miscObj.DisplayWideMsgBox("Invalid Response\nPlease try again");
        return -1;
    }
    else
    {
        Json::Value root;
        root.clear();
        Json::Reader reader;
        std::ifstream ReadData("/tmp/Sample.txt");
        bool parsedSuccess = reader.parse(ReadData,root,false);
        ReadData.close();
        if(not parsedSuccess)
        {
            miscObj.DisplayWideMsgBox("Invalid Response from Server\nPlease try again");
            return -1;
        }
        else
        {
            printf("\nrespCode    : %s\n",Convertstr(root["respCode"].asString()));
            printf("\nrespMessage : %s\n",Convertstr(root["respMessage"].asString()));
            printf("deleteStatus : %s\n",Convertstr(root["deleteStatus"].asString()));
            if(atoi(Convertstr(root["respCode"].asString()))==0)
            {
                /////update TxnUploadSts here////////
                query.clear();
                QString UpdateQryStr;
                UpdateQryStr.clear();
                UpdateQryStr = "UPDATE BenfiaryTxn SET TxnUploadSts = 'Y'";
                qDebug() << "UPDATE BenfiaryTxnquery::" <<UpdateQryStr;
                query.exec(UpdateQryStr);
                /////////////////////////////////////-BSK.
                miscObj.DisplayWideMsgBox(Convertstr(root["respMessage"].asString()));
                if(strcmp(Convertstr(root["deleteStatus"].asString()),"Y") == 0)
                {
                    if(partialOnlineOfflineStatus.toUpper() == "N")
                    {
                        printf("deleteStatus : %s\n",Convertstr(root["deleteStatus"].asString()));
                        query.clear();
                        query.exec("drop table KeyRegister");

                        query.clear();
                        query.exec("drop table Pos_Ob");

                        query.clear();
                        query.exec("drop table commodityMaster");

                        query.clear();
                        query.exec("drop table schemeMaster");

                        query.clear();
                        query.exec("delete from BenfiaryTxn");
                    }
                }

                return 0;
            }
            else
            {
//                miscObj.DisplayWideMsgBox(Convertstr(root["respMessage"].asString()));
                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"%s\nError Code :%s",Convertstr(root["respMessage"].asString()),Convertstr(root["respCode"].asString()));
                miscObj.DisplayWideMsgBox((char *)TmpStr);
                return -1;
            }
        }
    }
}


int Widget::ManualServerUploadPartialTxns()
{
    int uploadingRecords =0,totalRecords=0,pendingRecords=0;
    char TmpStr[200];

    memset(TmpStr,0x00,sizeof(TmpStr));
    FILE *fp = popen("ping google.com -c 5|grep packets|awk {'print $7'}","r");
    fgets(TmpStr,sizeof(TmpStr),fp);
    qDebug() << "Ping Check In offline Login";
    fprintf(stderr,"%s",TmpStr);
    pclose(fp);
    qDebug()<<"Ping TmpStr:"<<TmpStr;
    if(strncmp(TmpStr,"0%",2) == 0)
    {
        qDebug()<<"Ping :"<<TmpStr << "sucess and Network available";
    }
    else
    {
        qDebug() << "ping failed Internet unavailable";
        return -2;
    }

    while(1)
    {
        uploadingRecords=0;
        GPRS objGprs;
        if(objGprs.Check_Network_Available() < 0)
        {
            return -1;
        }

        //        int Rval = ManualReceiveGdsUpload();
        //        if(Rval < 0)
        //        {
        //            return -1;
        //        } ////commenting thread upload as per dept.

        char TmpStr[500];
        qDebug()<<"In ManualServerUploadPartialTxns";
        Json::Value UploadData,CommWiseData;
        QSqlQuery query;

        Json::Value ArrayList(Json::arrayValue);
        ArrayList.clear();
        UploadData.clear();
        char FpsId[30];
        memset(FpsId,0x00,sizeof(FpsId));
        QSqlQuery query1;
        query1.prepare("SELECT ShopID FROM  ShopOwnerDetails");
        if(!query1.exec())
            qDebug() << "SELECT ShopID FROM ShopOwnerDetails = " << query1.lastError();
        query1.next();
        //getShopId(FpsId);

        char cmachineID[16];
        memset(cmachineID, 0, sizeof(cmachineID));

        CMisc miscObj;
        miscObj.GetMachineId(cmachineID);
        query.clear();
        query.prepare("select fpsSessionId from Dealer");
        query.exec();
        query.next();
        QString sessionIdQstr = query.value(0).toString();

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

        UploadData["fpsId"]= query1.value(0).toString().toAscii().data();
        UploadData["stateCode"] = stateCode.toStdString().c_str();
        UploadData["token"]= RECEIVEGOODS_TOKEN;
        UploadData["sessionId"]=sessionIdQstr.toAscii().data();//"0";
        UploadData["terminalId"]=cmachineID;
        UploadData["distributionMonth"]=AllotmentMonth.toAscii().data();
        UploadData["distributionYear"]=AllotmentYear.toAscii().data();
        query.clear();
        memset(TmpStr,0x00,sizeof(TmpStr));
        sprintf(TmpStr,"select RcId,CommCode,TotQty,BalQty,SchemeId,IssuedQty,RecptId,commAmount,TotAmt,Rate,MemberName,DateTime,TxnType,AllotMonth,AllotYear,allocationType from BenfiaryTxn where TxnUploadSts = 'N' limit 20");


        //query.prepare("select RcId,CommCode,TotQty,BalQty,SchemeId,IssuedQty,RecptId,commAmount,TotAmt,Rate,MemberName,DateTime,TxnType,AllotMonth,AllotYear,allocationType from BenfiaryTxn where TxnUploadSts = ?");
        //query.addBindValue("N");

        if(!query.exec(TmpStr))
        {
            qDebug()<<"Upoad Query Error:"<<query.lastError();
            return -1;
        }
        while(query.next())
        {
            CommWiseData.clear();
            CommWiseData["rcId"]=query.value(0).toString().toAscii().data();
            CommWiseData["commCode"]=query.value(1).toString().toAscii().data();
            CommWiseData["totalEntitlement"]=query.value(2).toString().toAscii().data();
            CommWiseData["balanceEntitlement"]=query.value(3).toString().toAscii().data();
            CommWiseData["schemeId"]=query.value(4).toString().toAscii().data();
            CommWiseData["issueQty"]=query.value(5).toString().toAscii().data();
            CommWiseData["receiptId"]=query.value(6).toString().toAscii().data();
            CommWiseData["commAmount"]=query.value(7).toString().toAscii().data();
            CommWiseData["totalAmount"]=query.value(8).toString().toAscii().data();
            CommWiseData["commPrice"]=query.value(9).toString().toAscii().data();
            CommWiseData["headOfTheFamily"]=query.value(10).toString().toAscii().data();
            CommWiseData["transactionTime"]=query.value(11).toString().toAscii().data();
            CommWiseData["transMode"]=query.value(12).toString().toAscii().data();
//            CommWiseData["month"]=query.value(13).toString().toAscii().data();
//            CommWiseData["year"]=query.value(14).toString().toAscii().data();

            CommWiseData["allotedMonth"]=query.value(13).toString().toAscii().data();
            CommWiseData["allotedYear"]=query.value(14).toString().toAscii().data();
            CommWiseData["allocationType"]=query.value(15).toString().toAscii().data();
            ArrayList.append(CommWiseData);
            uploadingRecords++;
        }
        UploadData["fpsOfflineTransResponses"]=ArrayList;
        query.prepare("select commCode,closingBalance from Pos_Ob");
        if(!query.exec())
        {
            qDebug()<<"Upoad Query Error:"<<query.lastError();
            return -1;
        }
        ArrayList.clear();
        while(query.next())
        {
            CommWiseData.clear();
            CommWiseData["commCode"]=query.value(0).toString().toAscii().data();
            CommWiseData["closingBalance"]=query.value(1).toString().toAscii().data();
            ArrayList.append(CommWiseData);
        }
        UploadData["fpsCbs"]=ArrayList;
        char *Ret="";
        qDebug()<<"\n\n\nUploadData Size:"<<UploadData["fpsOfflineTransResponses"].size()<<"\n\n\n";
        if(UploadData["fpsOfflineTransResponses"].size()==0)
        {
            qDebug()<<"No Txn Were found for Upload";
            return 0;
        }

        /////////////////////////////////////////////////////////////

        query.clear();
        query.exec("select count(*) from BenfiaryTxn where TxnType IN('P','Q')");
        query.next();
        totalRecords = query.value(0).toString().toInt(0);
        UploadData["totalRecords"]=totalRecords;
        UploadData["uploadingRecords"]=uploadingRecords;

        query.clear();
        query.exec("select count(*) from BenfiaryTxn where TxnType IN('P','Q') and TxnUploadSts ='N'");
        query.next();
        pendingRecords = query.value(0).toString().toInt(0);

        if(uploadingRecords < pendingRecords)
            UploadData["pendingRecords"]=(pendingRecords-uploadingRecords);
        else
            UploadData["pendingRecords"]=(uploadingRecords-pendingRecords);

        //        if(uploadingRecords > 0)
        //            UploadData["pendingRecords"]=(totalRecords-uploadingRecords);
        //        else
        //            UploadData["pendingRecords"]=uploadingRecords;

        /////////////////////////////////////////////////////////////

        Ret = Convertstr(UploadData.toStyledString());
        char URL[125];
        memset(URL,0x00,sizeof(URL));
        sprintf(&URL[0],"%spushFpsOfflineData",Offline_and_ReceiveStock_URL.toAscii().data());

        int RetVal;

        for(int Retry = 0 ; Retry < 3 ; Retry++)
        {
            qDebug() << "Attempt::" << Retry;
            RetVal = CurlRequest(URL,Ret,300);

            if(RetVal == 0 || RetVal == 28) //28 - Timed out
                break;
        }

        if(RetVal==0)
        {
            Json::Value root;
            root.clear();
            //QSqlQuery query;
            Json::Reader reader;
            std::ifstream ReadData("/tmp/Sample.txt");
            system("cat /tmp/Sample.txt");
            bool parsedSuccess = reader.parse(ReadData,root,false);
            ReadData.close();
            if(not parsedSuccess)
            {
                miscObj.DisplayWideMsgBox((char *)"Invalid Response From Server for Txn Upload");
                return -1;
            }
            else
            {
                printf("\nrespCode    : %s\n",Convertstr(root["respCode"].asString()));
                printf("\nrespMessage : %s\n",Convertstr(root["respMessage"].asString()));
                if(atoi(Convertstr(root["respCode"].asString()))==0)
                {
                    Json::Value PosOb;
                    PosOb.clear();
                    PosOb = root.get("fpsCb",0);
                    printf("\nSize : %d\n",PosOb.size());

                    if(PosOb.size()==0)
                    {
//                        if(LangMode == 1)
//                            miscObj.DisplayWideMsgBox("Pos Ob Data not found\nPlease try again");
//                        else
//                            miscObj.tel_DisplayWideMsgBox("PosOb डेटा नहीं मिला\nकृपया पुन: प्रयास करें");

//                        return -1;
                    }
                    else
                    {
                        query.clear();
                        if(!query.exec("delete from Pos_Ob"))
                        {
                            //qDebug() << "Pos_Ob Delete Table  Error:" << query.lastError();
                            //DataDownloadError();
                            //return -1;
                        }
                        char FloatData[10];
                        for(int i=0;i<PosOb.size();i++)
                        {
                            qDebug()<<"--------"<<Convertstr(PosOb[i]["commCode"].asString());
                            qDebug()<<"--------"<<Convertstr(PosOb[i]["commNameEn"].asString());

                            query.clear();
                            query.prepare("insert into Pos_Ob(commCode,commNameEn,commNameLl,closingBalance) VALUES(:commCode,:commNameEn,:commNameLl,:closingBalance)");
                            query.bindValue(":commCode",Convertstr(PosOb[i]["commCode"].asString()));
                            query.bindValue(":commNameEn",Convertstr(PosOb[i]["commNameEn"].asString()));
                            query.bindValue(":commNameLl",QString::fromUtf8(Convertstr(PosOb[i]["commNameLl"].asString())));
                            memset(FloatData,0x00,sizeof(FloatData));
                            sprintf(FloatData,"%0.3f",QString(Convertstr(PosOb[i]["closingBalance"].asString())).toFloat());
                            query.bindValue(":closingBalance",FloatData);
                            if(!query.exec())
                            {
//                                if(LangMode == 1)
//                                    miscObj.DisplayWideMsgBox("Invalid PosOb Data found\nPlease try again");
//                                else
//                                    miscObj.tel_DisplayWideMsgBox("अमान्य PosOb डेटा\nकृपया पुन: प्रयास करें");
//                                qDebug() << query.lastError();
//                                DataDownloadError();
                                return -1;
                            }
                        }
                    }

                    Json::Value UploadedData;
                    UploadedData.clear();
                    UploadedData = root.get("updatedReceipts",0);
                    printf("\nSize : %d\n",UploadedData.size());
                    if(UploadedData.size()==0)
                    {
                        miscObj.DisplayWideMsgBox((char *)"UpdatedReceipts Id not sent From Server for Txn Upload");
                        return -1;
                    }
                    else
                    {
                        for(int i=0;i<UploadedData.size();i++)
                        {
                            query.clear();
                            memset(TmpStr,0x00,sizeof(TmpStr));
                            sprintf(TmpStr,"update BenfiaryTxn set TxnUploadSts = 'Y' where RecptId='%s' and RcId='%s' and CommCode='%s'",Convertstr(UploadedData[i]["receiptId"].asString()),Convertstr(UploadedData[i]["rcId"].asString()),Convertstr(UploadedData[i]["commCode"].asString()));
                            qDebug()<<"TmpStr Update Status:"<<TmpStr;
                            if(!query.exec(TmpStr))
                            {
                                return -1;
                            }
                        }
                    }
                }
                else
                {
                    memset(TmpStr,0x00,sizeof(TmpStr));
                    sprintf(TmpStr,"%s\nError Code :%s",Convertstr(root["respMessage"].asString()),Convertstr(root["respCode"].asString()));
                    miscObj.DisplayWideMsgBox((char *)TmpStr);
                    return -1;
                }
            }
        }
        else
        {
            memset(TmpStr,0x00,sizeof(TmpStr));
            sprintf(TmpStr,"Unable to upload.Please check connectivity\nPlease Try again\nError Code:%d",RetVal);
            miscObj.DisplayWideMsgBox((char *)TmpStr);
            return -1;
        }
    }
    return 0;
}

int Widget::PartialOfflineTxnUpload()
{
    //FpsId,RcId,SchemeId,CommCode,TotQty,BalQty,IssuedQty,commAmount,TotAmt,RecptId,TxnUploadSts
    prn_open();
    if(prn_paperstatus() < 0)
    {
        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
        else if (LangMode == HINDI)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया पेपर रोल डालें");
        prn_close();
        return -1;
    }
    if(miscObj.Battery_check() != 0) //01072017
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद प्रिंट करने में असमर्थ. \n कृपया बैटरी चार्ज करें");
        prn_close();
        return -1;
    }
    prn_close();
    QSqlQuery query;
    char TmpStr[1024];
    char FpsId[30];
    memset(FpsId,0x00,sizeof(FpsId));
    getShopId(FpsId);

    memset(TxnId, 0, sizeof(TxnId));
    GetTransactionId(TxnId);
    qDebug()<<"Txn Id = "<<TxnId;


    memset(TmpStr,0x00,sizeof(TmpStr));
    for(int i=0;i<ui->tblCommIssue->rowCount();i++)
    {
        if(ui->tblCommIssue->item(i,3)->text().toFloat()>0.00)
        {
            memset(TmpStr,0x00,sizeof(TmpStr));
            sprintf(TmpStr,"select count() from KeyRegister where rcId='%s' and commCode='%s'",ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data());
            qDebug()<<"TmpStr  :"<<TmpStr;
            if(!query.exec(TmpStr))
            {

            }
            query.next();
            if(query.value(0).toInt()==0)
            {
                if(LangMode == ENGLISH)
                    miscObj.DisplayWideMsgBox((char *)"Commodity Balance Qty not available for this Entered Card");
                else if(LangMode == HINDI)
                    miscObj.tel_DisplayWideMsgBox((char *)"इस दर्ज कार्ड के लिए कमोडिटी बैलेंस मात्रा उपलब्ध नहीं है");

                return -1;
            }
        }
    }
    memset(TmpStr,0x00,sizeof(TmpStr));

//    for(int i=0;i<ui->tblCommIssue->rowCount();i++)
//    {
//        if(ui->tblCommIssue->item(i,3)->text().toFloat()>0.00)
//        {
//            memset(TmpStr,0x00,sizeof(TmpStr));
//            sprintf(TmpStr,"select count() from Pos_Ob where commCode='%s'",ui->tblCommIssue->item(i,5)->text().toAscii().data());
//            qDebug()<<"TmpStr  :"<<TmpStr;
//            if(!query.exec(TmpStr))
//            {

//            }
//            query.next();;
//            if(query.value(0).toInt()==0)
//            {
//                memset(TmpStr,0x00,sizeof(TmpStr));

//                if(LangMode ==ENGLISH)
//                {
//                    sprintf(TmpStr,"%s Commodity Closing balance not available for this shop",ui->tblCommIssue->item(i,0)->text().toAscii().data());
//                    miscObj.DisplayWideMsgBox((char *)TmpStr);
//                }
//                else
//                {

//                    QString Hintext= QString::fromUtf8("इस दुकान के लिए कमोडिटी समापन शेष उपलब्ध नहीं है");
//                    sprintf(TmpStr,"%s %s",ui->tblCommIssue->item(i,0)->text().toAscii().data(),Hintext.toAscii().data());
//                    miscObj.tel_DisplayWideMsgBox((char *)TmpStr);
//                }

//                return -1;
//            }
//        }
//    }

    memset(TmpStr,0x00,sizeof(TmpStr));
    for(int i=0;i<ui->tblCommIssue->rowCount();i++)
    {
        if(ui->tblCommIssue->item(i,3)->text().toFloat()>0.00)
        {
            memset(TmpStr,0x00,sizeof(TmpStr));

            //sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement-%0.3f where rcId='%s' and commCode='%s'",ui->tblCommIssue->item(i,3)->text().toFloat(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data());
            sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement-%0.3f where rcId='%s' and commCode='%s' and allocationType='%s' and allotedMonth='%s' and allotedYear='%s'",ui->tblCommIssue->item(i,3)->text().toFloat(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->tblCommIssue->item(i,11)->text().toAscii().data(),ui->tblCommIssue->item(i,12)->text().toAscii().data(),ui->tblCommIssue->item(i,13)->text().toAscii().data());

            qDebug()<<"TmpStr  :"<<TmpStr;
            if(!query.exec(TmpStr))
            {
                for(int j=0;j<i;j++)
                {
                    if(ui->tblCommIssue->item(j,3)->text().toFloat()>0.00)
                    {
                        memset(TmpStr,0x00,sizeof(TmpStr));
                        sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement+%0.3f where rcId='%s' and commCode='%s' and allocationType='%s' and allotedMonth='%s' and allotedYear='%s'",ui->tblCommIssue->item(j,3)->text().toFloat(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(j,5)->text().toAscii().data(),ui->tblCommIssue->item(i,11)->text().toAscii().data(),ui->tblCommIssue->item(i,12)->text().toAscii().data(),ui->tblCommIssue->item(i,13)->text().toAscii().data());
                        if(!query.exec(TmpStr))
                        {

                        }
                    }
                }
                qDebug()<<"Balance Updation Failed Query";
                return -1;
            }
        }
    }
    for(int i=0;i<ui->tblCommIssue->rowCount();i++)
    {
        if(ui->tblCommIssue->item(i,3)->text().toFloat()>0.00)
        {
            memset(TmpStr,0x00,sizeof(TmpStr));
            //sprintf(TmpStr,"update Pos_Ob set closingBalance = closingBalance-%0.3f where commCode='%s'",ui->tblCommIssue->item(i,3)->text().toFloat(),ui->tblCommIssue->item(i,5)->text().toAscii().data());

            sprintf(TmpStr,"update Pos_Ob set  closingBalance = closingBalance-%0.3f where commCode=(select commonCommCode from commodityMaster where commCode='%s')",ui->tblCommIssue->item(i,3)->text().toFloat(),ui->tblCommIssue->item(i,5)->text().toAscii().data()); //common com code

            qDebug()<<"TmpStr  :"<<TmpStr;
            if(!query.exec(TmpStr))
            {
                for(int j=0;j<ui->tblCommIssue->rowCount();j++)
                {
                    if(ui->tblCommIssue->item(j,3)->text().toFloat()>0.00)
                    {
                        memset(TmpStr,0x00,sizeof(TmpStr));
//                        sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement+%0.3f where rcId='%s' and commCode='%s'",ui->tblCommIssue->item(j,3)->text().toFloat(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(j,5)->text().toFloat());
                        sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement+%0.3f where rcId='%s' and commCode='%s' and allocationType='%s' and allotedMonth='%s' and allotedYear='%s'",ui->tblCommIssue->item(j,3)->text().toFloat(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(j,5)->text().toAscii().data(),ui->tblCommIssue->item(i,11)->text().toAscii().data(),ui->tblCommIssue->item(i,12)->text().toAscii().data(),ui->tblCommIssue->item(i,13)->text().toAscii().data());
                        if(!query.exec(TmpStr))
                        {

                        }
                    }
                }
                for(int j=0;j<i;j++)
                {
                    if(ui->tblCommIssue->item(j,3)->text().toFloat()>0.00)
                    {
                        memset(TmpStr,0x00,sizeof(TmpStr));
                        //sprintf(TmpStr,"update Pos_Ob set  closingBalance = closingBalance+%0.3f where commCode='%s'",ui->tblCommIssue->item(j,3)->text().toFloat(),ui->tblCommIssue->item(j,5)->text().toAscii().data());

                        sprintf(TmpStr,"update Pos_Ob set  closingBalance = closingBalance+%0.3f where commCode=(select commonCommCode from commodityMaster where commCode='%s')",ui->tblCommIssue->item(j,3)->text().toFloat(),ui->tblCommIssue->item(j,5)->text().toAscii().data());//common com code
                        if(!query.exec(TmpStr))
                        {

                        }
                    }
                }
                qDebug()<<"Closing Balanace Updation Failed Query";
                return -1;
            }
        }
    }
    displayTime();
    displayDate();
    char TxnType;
    if(DealerLoginBy == 2)
        TxnType = 'Q';
    else
        TxnType = 'P';
    for(int i=0;i<ui->tblCommIssue->rowCount();i++)
    {
        if(ui->tblCommIssue->item(i,3)->text().toFloat()>0.00)
        {
            memset(TmpStr,0x00,sizeof(TmpStr));

//            sprintf(TmpStr,"insert into BenfiaryTxn values('%s','%s',(select schemeId from KeyRegister where rcId = '%s' limit 1),'%s',(select totalEntitlement from KeyRegister where rcId = '%s' and commCode='%s'),(select balanceEntitlement from KeyRegister where rcId = '%s' and commCode='%s'),'%s','%s','%s','%s','%s',(select memberNameEn from KeyRegister where rcId = '%s' limit 1),'%s %s','N','%c',(select allotedMonth from KeyRegister where rcId = '%s' limit 1),(select allotedYear from KeyRegister where rcId = '%s' limit 1),(select allocationType from KeyRegister where rcId = '%s' limit 1))",FpsId,ui->ldtRationCard->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->tblCommIssue->item(i,3)->text().toAscii().data(),ui->tblCommIssue->item(i,2)->text().toAscii().data(),ui->tblCommIssue->item(i,4)->text().toAscii().data(),ui->lblTotalAmtVal->text().toAscii().data(),TxnId,ui->ldtRationCard->text().toAscii().data(),ui->labelDate->text().toAscii().data(),ui->labelTime->text().toAscii().data(),TxnType,ui->ldtRationCard->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data());
            sprintf(TmpStr,"insert into BenfiaryTxn values('%s','%s',(select schemeId from KeyRegister where rcId = '%s' limit 1),'%s',(select totalEntitlement from KeyRegister where rcId = '%s' and commCode='%s'),(select balanceEntitlement from KeyRegister where rcId = '%s' and commCode='%s'),'%s','%s','%s','%s','%s',(select memberNameEn from KeyRegister where rcId = '%s' limit 1),'%s %s','N','%c','%s','%s','%s')",FpsId,ui->ldtRationCard->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->tblCommIssue->item(i,3)->text().toAscii().data(),ui->tblCommIssue->item(i,2)->text().toAscii().data(),ui->tblCommIssue->item(i,4)->text().toAscii().data(),ui->lblTotalAmtVal->text().toAscii().data(),TxnId,ui->ldtRationCard->text().toAscii().data(),ui->labelDate->text().toAscii().data(),ui->labelTime->text().toAscii().data(),TxnType,ui->tblCommIssue->item(i,12)->text().toAscii().data(),ui->tblCommIssue->item(i,13)->text().toAscii().data(),ui->tblCommIssue->item(i,11)->text().toAscii().data());

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
                return -1;
            }
        }
    }
    //ServerUploadPartialTxns();
    getHeadings();
    Rcno.clear();
    Rcno=ui->ldtRationCard->text();
    if(iMainTransaction == PDS_TRANSACTION || iMainTransaction == ICDS_TRANSACTION || iMainTransaction == MDM_TRANSACTION || iMainTransaction == HAWKER_TRANSACTION) // PDS_CASHLESS_TRANSACTION diverted at function starting
    {
        char buff[64];
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "Transaction Successfull...\nPrinting, Please Wait...");
        gl11_lcdbklt_on();

        popup_msg *dlgPopup2 = new popup_msg(this);

        if(LangMode == 2)
            dlgPopup2->ui->lblPopup->setText(trUtf8("लेनदेन सफल... \n प्रिंटिंग कृपया प्रतीक्षा करें..."));
        else if(LangMode == 4)
            dlgPopup2->ui->lblPopup->setText(trUtf8("దయచేసి వేచి ఉండండి...ప్రింటింగ్ చేయబడుతుంది"));
        else
            dlgPopup2->ui->lblPopup->setText(buff);

        dlgPopup2->show();
        QCoreApplication::processEvents();
        QCoreApplication::processEvents();

        /*if(LangMode == 2)
            PrintTxnReceipt();
        else*/
        prn_open();
        PrintOfflineTxnRcpt();
        prn_close();

        dlgPopup2->close();
        dlgPopup2->deleteLater();
    }
    if(DealerLoginBy == 2)
    {
        memset(TmpStr,0x00,sizeof(TmpStr));
        FILE *fp = popen("ping google.com -c 5|grep packets|awk {'print $7'}","r");
        fgets(TmpStr,sizeof(TmpStr),fp);
        qDebug() << "Ping Check In offline Login";
//        while(fgets(TmpStr,sizeof(TmpStr),fp) != NULL) {
            fprintf(stderr,"%s",TmpStr);
//        }
        pclose(fp);
        qDebug()<<"Ping TmpStr:"<<TmpStr;
        if(strncmp(TmpStr,"0%",2)==0)
        {
            if(LangMode ==ENGLISH)
                miscObj.DisplayWideMsgBox((char *)"Network Avalible\nPlease login by dealer authentication");
            else if(LangMode == HINDI)
                miscObj.tel_DisplayWideMsgBox((char *)"नेटवर्क उपलब्ध \nडीलर प्रमाणीकरण द्वारा लॉगिन करें");
            PartialOnlineTimer->stop();
            ui->stackedWidget->setCurrentIndex(0);
            return 0;
        }
    }
    ui->ldtRationCard->setText("");
    ui->stackedWidget->setCurrentIndex(4);
}

int Widget::PrintOfflineTxnRcpt()
{
    char prn[512];
    LogoPrint logo;
    logo.PrintLogo(stateCode.toInt());
    QSqlQuery query;
    QString TmpString,str;
    if(LangMode == 1){

        str= stateReceiptHeaderEn;
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
        str.clear();

        str = "OFFLINE RECEIPT";
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
        str.clear();
    }
    else{
        TmpString.clear();
        TmpString= stateReceiptHeaderLl;
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_CENTER );
        TmpString.clear();

        TmpString= QString::fromUtf8("ऑफ़लाइन ")+QString::fromUtf8(Qpds_tran_tel.toAscii().data());
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_CENTER );
    }
    memset(prn, 0, sizeof(prn));
    strcpy(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

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

    memset(prn,0x00,sizeof(prn));
    sprintf(prn,"select ShopID,Name,(select memberNameEn from KeyRegister where rcId='%s' limit 1) as name,(select memberNameLl from KeyRegister where rcId='%s' limit 1) as llname,(select schemeName from KeyRegister where rcId='%s' limit 1) as shceme from ShopOwnerDetails",ui->ldtRationCard->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data());
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
        sprintf(prn,"Consumer Name: %s",query.value(2).toString().toAscii().data() );
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn,"Card No.     : %s/%s",ui->ldtRationCard->text().toAscii().data(),query.value(4).toString().toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn,"Transaction ID:%s",TxnId);
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn,"Txn Mode      :OFFLINE");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        TmpString.clear();
        TmpString= QString::fromUtf8("Date: ") + ui->labelDate->text() +"  "+ ui->labelTime->text();
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "AllotmentMonth  : %s", AllotmentMonth.toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "AllotmentYear   : %s", AllotmentYear.toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        strcpy(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        //sprintf(prn,"Commodity    Lifted  B.Qty  Rate  Price");
        sprintf(prn,"Commodity    Lifted  Rate  Price");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }else{
        TmpString.clear();
        TmpString= QString::fromUtf8("एफपीएस का नाम :") + query.value(1).toString();
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

        TmpString.clear();
        TmpString= QString::fromUtf8("एफपीएस आईडी  :") + query.value(0).toString();
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

        TmpString.clear();
        TmpString= QString::fromUtf8("ग्राहक    :") + query.value(2).toString();
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

        TmpString.clear();
        TmpString= QString::fromUtf8("कार्ड नंबर:") + ui->ldtRationCard->text() +"/"+query.value(4).toString();
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

        TmpString.clear();
        TmpString= QString::fromUtf8("लेनदेन आईडी:") + TxnId;
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

        TmpString.clear();
        TmpString= QString::fromUtf8("Txn मोड  :") + "OFFLINE";
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

        TmpString.clear();
        TmpString= QString::fromUtf8("तारीख: ") + ui->labelDate->text() +"  "+ ui->labelTime->text();
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),20,QString::fromUtf8(TmpString.toUtf8()),17,ALIGN_LEFT);

        str.clear();
        str= QString::fromUtf8("आवंटन माह :") + AllotmentMonth;
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

        str.clear();
        str= QString::fromUtf8("आबंटन वर्ष :") + AllotmentYear;
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_LEFT);

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
void Widget::on_Partial_DaDyRptBtn_clicked()
{
    QSqlQuery query;
    query.clear();
    char tmp[300];
    string TStr;
    TStr.clear();
    TStr.append(ui->dateEdit->text().toAscii().data());
    sprintf(tmp,"select count() from BenfiaryTxn where substr(DateTime,1,2)='%02d' and substr(DateTime,4,2)='%02d' and substr(DateTime,7,4)='%04d'",atoi(TStr.substr(0,2).c_str()),atoi(TStr.substr(3,2).c_str()),atoi(TStr.substr(6,4).c_str()));
    if(!query.exec(tmp))
    {
        if (LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Offline Data Not available");
        else if(LangMode == HINDI)
            miscObj.tel_DisplayWideMsgBox((char *)"ऑफ़लाइन डेटा उपलब्ध नहीं है");

        return;
    }
    query.next();
    if((query.value(0).toString().toAscii().data())==0)
    {

        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
        else if (LangMode == HINDI)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया पेपर रोल डालें");

        return;
    }

    ui->btnDtSalesView->hide();
    ui->tblSalesDaily->hide();
    ui->pushButton_7->hide();
    DayReportFlag = 2;//for Offline report
    ui->stackedWidget->setCurrentIndex(11);
}

void Widget::PrintOfflineDayReportBKP()
{
    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...");
    else
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..."));
    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    QSqlQuery CommQuery;
    int cnt=0;
    char Tmp[300],TmpQuery[1024],tmp[300];
    CommQuery.clear();
    CommQuery.prepare("select distinct(SchemeId),(select schemeName from KeyRegister where schemeId=a.schemeId) as SchName from KeyRegister a");
    if(!CommQuery.exec())
    {
        qDebug()<<"Comm code Query:"<<CommQuery.lastError();
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }
    memset(Tmp,0x00,sizeof(Tmp));
    memset(TmpQuery,0x00,sizeof(TmpQuery));
    sprintf(Tmp,"CommName    ");
    if(LangMode == 1)
        sprintf(TmpQuery,"select distinct(commCode),(select commNameEn from KeyRegister where CommCode=a.CommCode),");
    else
        sprintf(TmpQuery,"select distinct(commCode),(select commNameLl from KeyRegister where CommCode=a.CommCode),");
    string TStr;
    TStr.clear();
    TStr.append(ui->dateEdit->text().toAscii().data());
    while(CommQuery.next())
    {
        cnt++;
        memset(tmp,0x00,sizeof(tmp));
        sprintf(tmp,"%s ",CommQuery.value(1).toString().toAscii().data());
        strcat(Tmp,tmp);
        memset(tmp,0x00,sizeof(tmp));
        sprintf(tmp,"(select sum(IssuedQty) from BenfiaryTxn where CommCode=a.CommCode and SchemeId='%s' and substr(DateTime,1,2)='%02d' and substr(DateTime,4,2)='%02d' and substr(DateTime,7,4)='%04d'),",CommQuery.value(0).toString().toAscii().data(),atoi(TStr.substr(0,2).c_str()),atoi(TStr.substr(3,2).c_str()),atoi(TStr.substr(6,4).c_str()));
        strcat(TmpQuery,tmp);
    }
    strcat(Tmp," Tot");
    TmpQuery[strlen(TmpQuery)-1]='\0';

    memset(tmp,0x00,sizeof(tmp));


    sprintf(tmp,",(select sum(IssuedQty) from BenfiaryTxn where CommCode=a.CommCode and substr(DateTime,1,2)='%02d' and substr(DateTime,4,2)='%02d' and substr(DateTime,7,4)='%04d') from BenfiaryTxn a where substr(DateTime,1,2)='%02d' and substr(DateTime,4,2)='%02d' and substr(DateTime,7,4)='%04d'",atoi(TStr.substr(0,2).c_str()),atoi(TStr.substr(3,2).c_str()),atoi(TStr.substr(6,4).c_str()),atoi(TStr.substr(0,2).c_str()),atoi(TStr.substr(3,2).c_str()),atoi(TStr.substr(6,4).c_str()));
    strcat(TmpQuery,tmp);
    qDebug()<<"\n\nTmp:"<<Tmp;
    qDebug()<<"\n\nTmpQuery:"<<TmpQuery;

    prn_open();
    if(prn_paperstatus() < 0)
    {
        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Please Put Paper"); //कृपया पेपर रोल डालें
        else if(LangMode == HINDI)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया पेपर रोल डालें"); //
        prn_close();
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }

    if(miscObj.Battery_check() != 0) //01072017
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद प्रिंट करने में असमर्थ. \n कृपया बैटरी चार्ज करें");
        prn_close();
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;

    }
    char prn[512];
    LogoPrint objLogo;
    objLogo.PrintLogo(stateCode.toInt());

    QString str = stateReceiptHeaderEn;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
    str.clear();

    int LangMode = getLangMode();
    if(LangMode == 1)
    {
        str =  "OFFLINE";
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
        str.clear();

        str =  "DAY WISE SALE REPORT";
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
        str.clear();
    }
    else
    {
        QString TmpStr;
        TmpStr.clear();
        TmpStr = QString::fromUtf8("ऑफ़लाइन");
        prepareBmp(TmpStr, 22, TmpStr, 16, 0);

        TmpStr.clear();
        TmpStr = QString::fromUtf8("दिन बजाओ बिक्री रिपोर्ट");
        prepareBmp(TmpStr, 22, TmpStr, 16, 0);
    }

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    if(LangMode == 1)
    {
        memset(prn, 0, sizeof(prn));
        sprintf(prn, "   Date: %s        Time: %s", QDateTime::currentDateTime().toString("dd-MM-yyyy").toAscii().data(),QDateTime::currentDateTime().toString("hh:mm:ss").toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "   Shop ID: %s", ui->ldtShpId->text().toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "   Report Date: %s",ui->dateEdit->text().toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }
    else if(LangMode == 2)
    {
        QString tDateTime = QString::fromUtf8("दिनांक") + ":" + QDateTime::currentDateTime().toString("dd-MM-yyyy") + QString::fromUtf8("   समय") + ":" + QDateTime::currentDateTime().toString("hh:mm:ss");
        prepareBmp(tDateTime, 22, tDateTime, 16, ALIGN_LEFT);

        QString tShopNo  = QString::fromUtf8("एफपीएस आईडी : ")+ "" + ui->ldtShpId->text();
        prepareBmp(tShopNo, 22, tShopNo, 16, ALIGN_LEFT);   //

        tShopNo  = QString::fromUtf8("रिपोर्ट दिनांक : ")+ "" + ui->dateEdit->text();
        prepareBmp(tShopNo, 22, tShopNo, 16, ALIGN_LEFT);
    }

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);


    prn_write_text((unsigned char *)Tmp, strlen(Tmp), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    CommQuery.clear();
    if(!CommQuery.exec(TmpQuery))
    {
        qDebug()<<"Comm Error:"<<CommQuery.lastError();
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }
    QString PrintComm;
    while(CommQuery.next())
    {
        memset(TmpQuery,0x00,sizeof(TmpQuery));
        if(LangMode == 1)
        {
            sprintf(TmpQuery,"%-10s",CommQuery.value(1).toString().toAscii().data());

            for(int i=0;i<cnt+1;i++)
            {
                memset(Tmp,0x00,sizeof(Tmp));
                //sprintf(Tmp," %-3.3f",CommQuery.value(2+i).toString().toFloat());
                sprintf(Tmp," %-7.3f",CommQuery.value(2+i).toString().toFloat());
                strcat(TmpQuery,Tmp);
            }
            prn_write_text((unsigned char *)TmpQuery, strlen(TmpQuery), 0);
        }else{
            PrintComm.clear();
            PrintComm =  CommQuery.value(1).toString();
            if(PrintComm.size()<15){
                for(int j=PrintComm.size();j<15;j++)
                    PrintComm += " ";
            }
            for(int i=0;i<cnt+1;i++){
                memset(Tmp,0x00,sizeof(Tmp));
                //sprintf(Tmp," %-3.3f",CommQuery.value(2+i).toString().toFloat());
                sprintf(Tmp," %-7.3f",CommQuery.value(2+i).toString().toFloat());
                //strcat(TmpQuery,Tmp);
                PrintComm += Tmp;
            }
            prepareBmp(PrintComm, 22, PrintComm, 18, ALIGN_LEFT);
        }
    }
    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);


    if(LangMode == 1){
        memset(prn,0x00,sizeof(prn));
        sprintf(prn,"     Public Distribution Dept  ");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn,0x00,sizeof(prn));
        sprintf(prn," (Note : Quantity's in Kgs/Ltrs)");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }else{
        QString TmpString;
        TmpString.clear();
        TmpString= QString::fromUtf8("    सार्वजनिक वितरण विभाग:    ");
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),22,QString::fromUtf8(TmpString.toUtf8()),18,ALIGN_CENTER);

        TmpString.clear();
        TmpString= QString::fromUtf8("    (नोट: किग्रा/लीटर में मात्रा)    ");
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),22,QString::fromUtf8(TmpString.toUtf8()),18,ALIGN_CENTER);
    }

    prn_paper_feed(5);
    prn_close();
    dlgPopup1->close();
    dlgPopup1->deleteLater();


}


void Widget::PrintOfflineDayReport()
{
    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...");
    else
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..."));
    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    QSqlQuery CommQuery;
    char Tmp[300],TmpQuery[1024],tmp[300];
    string TStr;
    TStr.clear();
    TStr.append(ui->dateEdit->text().toAscii().data());

    memset(TmpQuery,0x00,sizeof(TmpQuery));
    sprintf(TmpQuery,"select sum(IssuedQty) as IssuedQty,a.commCode,AllotMonth,AllotYear,allocationType,commNameEn||'-'||AllotMonth as commNameEn,schemeName,a.schemeId from BenfiaryTxn a join commodityMaster b on a.commCode = b.commCode join schemeMaster c on a.schemeId = c.schemeId where substr(DateTime,1,2)='%02d' and substr(DateTime,4,2)='%02d' and substr(DateTime,7,4)='%4d' group by a.commCode,AllotMonth,AllotYear,allocationType,a.schemeId ",atoi(TStr.substr(0,2).c_str()),atoi(TStr.substr(3,2).c_str()),atoi(TStr.substr(6,4).c_str()));

    qDebug()<<"\n\nTmp:"<<TmpQuery;

    prn_open();
    if(prn_paperstatus() < 0)
    {
        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Please Put Paper"); //कृपया पेपर रोल डालें
        else if(LangMode == HINDI)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया पेपर रोल डालें"); //
        prn_close();
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }

    if(miscObj.Battery_check() != 0) //01072017
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद प्रिंट करने में असमर्थ. \n कृपया बैटरी चार्ज करें");
        prn_close();
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;

    }
    char prn[512];
    LogoPrint objLogo;
    objLogo.PrintLogo(stateCode.toInt());

    QString str = stateReceiptHeaderEn;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
    str.clear();

    int LangMode = getLangMode();
    if(LangMode == 1)
    {
        str =  "OFFLINE";
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
        str.clear();

        str =  "DAY WISE SALE REPORT";
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
        str.clear();
    }
    else
    {
        QString TmpStr;
        TmpStr.clear();
        TmpStr = QString::fromUtf8("ऑफ़लाइन");
        prepareBmp(TmpStr, 22, TmpStr, 16, 0);

        TmpStr.clear();
        TmpStr = QString::fromUtf8("दिन बजाओ बिक्री रिपोर्ट");
        prepareBmp(TmpStr, 22, TmpStr, 16, 0);
    }

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    if(LangMode == 1)
    {
        memset(prn, 0, sizeof(prn));
        sprintf(prn, "   Date: %s        Time: %s", QDateTime::currentDateTime().toString("dd-MM-yyyy").toAscii().data(),QDateTime::currentDateTime().toString("hh:mm:ss").toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "   Shop ID: %s", ui->ldtShpId->text().toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, "   Report Date: %s",ui->dateEdit->text().toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }
    else if(LangMode == 2)
    {
        QString tDateTime = QString::fromUtf8("दिनांक") + ":" + QDateTime::currentDateTime().toString("dd-MM-yyyy") + QString::fromUtf8("   समय") + ":" + QDateTime::currentDateTime().toString("hh:mm:ss");
        prepareBmp(tDateTime, 22, tDateTime, 16, ALIGN_LEFT);

        QString tShopNo  = QString::fromUtf8("एफपीएस आईडी : ")+ "" + ui->ldtShpId->text();
        prepareBmp(tShopNo, 22, tShopNo, 16, ALIGN_LEFT);   //

        tShopNo  = QString::fromUtf8("रिपोर्ट दिनांक : ")+ "" + ui->dateEdit->text();
        prepareBmp(tShopNo, 22, tShopNo, 16, ALIGN_LEFT);
    }

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    sprintf(prn, "CommName   Scheme    Total");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    CommQuery.clear();
    if(!CommQuery.exec(TmpQuery))
    {
        qDebug()<<"Comm Error:"<<CommQuery.lastError();
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }

    while(CommQuery.next())
    {
        memset(Tmp,0x00,sizeof(Tmp));
        if(LangMode == 1)
        {
            sprintf(Tmp,"%-8s  %-5s  %-7.3f",CommQuery.value(5).toString().toAscii().data(),CommQuery.value(6).toString().toAscii().data(),CommQuery.value(0).toFloat());
            prn_write_text((unsigned char *)Tmp, strlen(Tmp), 0);
        }else{
            sprintf(Tmp,"%-8s  %-5s  %-7.3f",CommQuery.value(5).toString().toAscii().data(),CommQuery.value(6).toString().toAscii().data(),CommQuery.value(0).toFloat());
            prn_write_text((unsigned char *)Tmp, strlen(Tmp), 0);
        }
    }
    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);


    if(LangMode == 1){
        memset(prn,0x00,sizeof(prn));
        sprintf(prn,"     Public Distribution Dept  ");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn,0x00,sizeof(prn));
        sprintf(prn," (Note : Quantity's in Kgs/Ltrs)");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }else{
        QString TmpString;
        TmpString.clear();
        TmpString= QString::fromUtf8("    सार्वजनिक वितरण विभाग:    ");
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),22,QString::fromUtf8(TmpString.toUtf8()),18,ALIGN_CENTER);

        TmpString.clear();
        TmpString= QString::fromUtf8("    (नोट: किग्रा/लीटर में मात्रा)    ");
        prepareBmp(QString::fromUtf8(TmpString.toUtf8()),22,QString::fromUtf8(TmpString.toUtf8()),18,ALIGN_CENTER);
    }

    prn_paper_feed(5);
    prn_close();
    dlgPopup1->close();
    dlgPopup1->deleteLater();


}



void Widget::on_PartialCurrStkRptBtn_clicked()
{
    QSqlQuery query;

    query.clear();
    if(!query.exec("select count() from Pos_Ob"))
    {
        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Offline Data Not available");
        else if(LangMode == HINDI)
            miscObj.tel_DisplayWideMsgBox((char *)"ऑफ़लाइन डेटा उपलब्ध नहीं है");
        return;
    }
    query.next();
    if(atoi(query.value(0).toString().toAscii().data())==0)
    {
        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Offline Data Not available");
        else if(LangMode == HINDI)
            miscObj.tel_DisplayWideMsgBox((char *)"ऑफ़लाइन डेटा उपलब्ध नहीं है");
        return;
    }

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait...");
    else
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..."));
    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    prn_open();
    if(prn_paperstatus() < 0)
    {
        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
        else if(HINDI)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया पेपर रोल डालें");
        prn_close();
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }

    if(miscObj.Battery_check() != 0) //01072017
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद प्रिंट करने में असमर्थ. \n कृपया बैटरी चार्ज करें");
        prn_close();
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }
    char prn[512];
    LogoPrint objLogo;
    objLogo.PrintLogo(stateCode.toInt());

    LangMode = getLangMode();

    QString str= stateReceiptHeaderEn;
    prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
    str.clear();

    if(LangMode == 1)
    {
        str= "OFFLINE";
        prepareBmp(QString::fromUtf8(str.toUtf8()),20,QString::fromUtf8(str.toUtf8()),17,ALIGN_CENTER );
        str.clear();

        memset(prn, 0, sizeof(prn));
        sprintf(prn, " Current Stock Report");
        prn_write_text((unsigned char *)prn, strlen(prn), 2);

        memset(prn, 0, sizeof(prn));

        sprintf(prn, " Date: %s        Time: %s", QDateTime::currentDateTime().toString("dd-MM-yyyy").toAscii().data(),QDateTime::currentDateTime().toString("hh:mm:ss").toAscii().data());
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
        sprintf(prn, " Report Type : PDS");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        memset(prn, 0, sizeof(prn));
        sprintf(prn, " Shop ID     : %s", ui->ldtShpId->text().toAscii().data());
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
    }
    else if(LangMode == 2)        ///Hindi Language moded addded by vivek 15052017
    {
        QString TmpStr;
        TmpStr.clear();
        TmpStr = QString::fromUtf8("ऑफ़लाइन");
        prepareBmp(TmpStr, 22, TmpStr, 16, 0);

        QString tHeader = QString::fromUtf8("  चालू स्टॉक रिपोर्ट");
        prepareBmp(tHeader, 20, tHeader, 16, ALIGN_CENTER);


        QString tDateTime = QString::fromUtf8("दिनांक") + ":" + QDateTime::currentDateTime().toString("dd-MM-yyyy").toAscii().data() + QString::fromUtf8("   समय") + ":" + QDateTime::currentDateTime().toString("hh:mm:ss").toAscii().data();
        prepareBmp(tDateTime, 20, tDateTime, 16, ALIGN_LEFT);

        QString tReportType;
        tReportType  = QString::fromUtf8("रिपोर्ट का प्रकार:  ")+ "" + QString::fromUtf8("पी.डी.एस.");
        prepareBmp(tReportType, 22, tReportType, 16, ALIGN_LEFT);

        QString tShopNo  = QString::fromUtf8("एफपीएस आईडी :  ")+ "" + ui->ldtShpId->text();
        prepareBmp(tShopNo, 20, tShopNo, 16, ALIGN_LEFT);   //
    }

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
//    sprintf(prn, "Name          Issued     CB");
    sprintf(prn, "Name          Issued     ");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);


    query.clear();
    if(LangMode == 1)
        query.prepare("select a.commNameEn,(select sum(IssuedQty) from BenfiaryTxn where commCode=a.commCode) as Issued,a.closingBalance from Pos_Ob a");
    else
        query.prepare("select a.commNameLl,(select sum(IssuedQty) from BenfiaryTxn where commCode=a.commCode) as Issued,a.closingBalance from Pos_Ob a");
    if(!query.exec())
    {

    }
    char TmpStr[100];
    QString Printcomm;
    while(query.next())
    {
        if(LangMode == 1)
        {
            memset(TmpStr,0x00,sizeof(TmpStr));
//            sprintf(TmpStr,"%-12s %-9.3f %-10s",query.value(0).toString().toAscii().data(),query.value(1).toString().toFloat(),query.value(2).toString().toAscii().data());
            sprintf(TmpStr,"%-12s %-9.3f",query.value(0).toString().toAscii().data(),query.value(1).toString().toFloat());

            prn_write_text((unsigned char *)TmpStr, strlen(TmpStr), 0);
        }
        else
        {
            Printcomm.clear();
            Printcomm = query.value(0).toString();
            if(Printcomm.size()<12)
            {
                for(int j=Printcomm.size();j<12;j++)
                    Printcomm += " ";
            }
            memset(TmpStr,0x00,sizeof(TmpStr));
//            sprintf(TmpStr,"  %-9.3f %-10s",query.value(1).toString().toFloat(),query.value(2).toString().toAscii().data());
            sprintf(TmpStr,"  %-9.3f",query.value(1).toString().toFloat());
            Printcomm += TmpStr;
            prepareBmp(Printcomm, 22, Printcomm, 18, ALIGN_LEFT);
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


    prn_paper_feed(5);

    prn_close();
    dlgPopup1->close();
    dlgPopup1->deleteLater();
}

int Widget::GetPartialTimeStatus()
{
    char TmpStr[200];
    QSqlQuery query;
    query.clear();
    if(!query.exec("select leftOfflineTime from PartialOnlineData limit 1"))
    {
        return -1;
    }
    query.next();
    memset(TmpStr,0x00,sizeof(TmpStr));
    if(query.value(0).toString().toInt()-1<=0)
        sprintf(TmpStr,"update PartialOnlineData set leftOfflineTime = '0'");
    else
        sprintf(TmpStr,"update PartialOnlineData set leftOfflineTime = leftOfflineTime-1");
    query.clear();
    qDebug()<<"TmpStr :"<<TmpStr;
    if(!query.exec(TmpStr))
    {
        qDebug()<<"Duration Update error:"<<query.lastError();
        return -1;
    }
    return 0;
}

int Widget::TxnAllotedBetweenTime()
{
    QDate OfflineStopDate;

    QSqlQuery query;
    query.clear();
    char TmpStr[200];
    if(!query.exec("select OfflineTxnTime,AllotMonth,pOfflineStoppedDate from PartialOnlineData"))
    {
        miscObj.DisplayWideMsgBox((char *)"Invalid Txn's Time");;
        return -1;
    }
    query.next();
    qDebug()<<"query :"<<query.value(0).toString();
    qDebug()<<"Start Time:"<<atoi(string(query.value(0).toString().toAscii().data()).substr(0,string(query.value(0).toString().toAscii().data()).find("AM")).c_str());
    if((QDateTime::currentDateTime().toString("hh").toInt()<atoi(string(query.value(0).toString().toAscii().data()).substr(0,string(query.value(0).toString().toAscii().data()).find("AM")).c_str()))&&(QDateTime::currentDateTime().toString("hh").toInt()>atoi(string(query.value(0).toString().toAscii().data()).substr(string(query.value(0).toString().toAscii().data()).find("-")+1,string(query.value(0).toString().toAscii().data()).find("PM")-string(query.value(0).toString().toAscii().data()).find("-")-1).c_str())))
    {
        memset(TmpStr,0x00,sizeof(TmpStr));
        if(LangMode == ENGLISH)
        {
            sprintf(TmpStr,"Txn's are allowed from %s\n",query.value(0).toString().toAscii().data());
            miscObj.DisplayWideMsgBox((char *)TmpStr);
        }
        else if(LangMode == HINDI)
        {
            QString Hintext=QString::fromUtf8("से लेन-देन की अनुमति है");
            sprintf(TmpStr," %s %s\n",query.value(0).toString().toAscii().data(),Hintext.toAscii().data());
            miscObj.tel_DisplayWideMsgBox((char *)TmpStr);
        }
        PartialOnlineTimer->stop();
        ui->stackedWidget->setCurrentIndex(0);
        return -1;
    }
    if((partialOnlineOfflineStatus.toUpper()=="Y")||(DealerLoginBy == 2))
    {
        QString Str;
        Str = query.value(2).toString();
        int StopMonth = Str.mid(5,2).toInt();
        qDebug()<<"1.StopMonth:: " << StopMonth <<"\tCurrent moth::" << QDateTime::currentDateTime().toString("MM").toInt();

//        if(query.value(1).toString().toInt()!= QDateTime::currentDateTime().toString("MM").toInt())
//        if(QDateTime::currentDateTime().toString("MM").toInt() != StopMonth)
//        if(QDateTime::currentDateTime().toString("MM").toInt() < StopMonth)
//        {
//            if(LangMode ==ENGLISH)
//                miscObj.DisplayWideMsgBox((char *)"Month Changed\nPlease login by authentication");
//            else if(LangMode == HINDI)
//                miscObj.tel_DisplayWideMsgBox((char *)"महीना बदल गया\nकृपया प्रमाणीकरण द्वारा लॉगिन करें");
//            return -1;
//        }

        qDebug()<<"pOfflineStoppedDate Str:"<<Str;
        OfflineStopDate = QDate::fromString(Str,"yyyy-MM-dd");
        QDate PresentDate(QDate::currentDate());
        int Days = PresentDate.daysTo(OfflineStopDate);
        qDebug() << "Remaining days to Stop::" << Days;
        if(Days < 0)
        {
            if(LangMode ==ENGLISH)
                miscObj.DisplayWideMsgBox((char *)"Month Changed\nPlease login by authentication");
            else if(LangMode == HINDI)
                miscObj.tel_DisplayWideMsgBox((char *)"महीना बदल गया\nकृपया प्रमाणीकरण द्वारा लॉगिन करें");
            return -1;
        }
    }
    if(DealerLoginBy == 2)
    {
        query.clear();
        if(!query.exec("select leftOfflineTime from PartialOnlineData limit 1"))
        {
            return -1;
        }
        query.next();
        memset(TmpStr,0x00,sizeof(TmpStr));
        if(query.value(0).toString().toInt()<=0)
        {
            if(LangMode == ENGLISH)
                miscObj.DisplayWideMsgBox((char *)"Offline time duration completed\nLogin by authentication");
            else if(LangMode == HINDI)
                miscObj.tel_DisplayWideMsgBox((char *)"ऑफ़लाइन समय अवधि पूरी हुई\nकृपया प्रमाणीकरण द्वारा लॉगिन करें");
            PartialOnlineTimer->stop();
            ui->stackedWidget->setCurrentIndex(0);
            return -1;
        }
    }
    return 0;
}

int Widget::LoginByPassword()
{    
    char TmpStr[200];
    QSqlQuery query;
    query.clear();
    QDate OfflineStopDate;

    if(!query.exec("select OfflineLogin,OffPassword,leftOfflineTime,OfflineTxnTime,AllotMonth,pOfflineStoppedDate from PartialOnlineData"))
    {
        qDebug() << "PartialOnlineData Last Error2::" <<query.lastError();
        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"No Network and Offline Login data not Available");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"कोई नेटवर्क और ऑफ़लाइन लॉगिन डेटा उपलब्ध नहीं है");
        return -1;
    }
    query.next();
    qDebug() <<"OfflineLogin::" <<query.value(0).toString().toUpper();
    if(query.value(0).toString().toUpper()!="Y")
    {
        qDebug() << "PartialOnlineData Last Error3::" <<query.lastError();
        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"No Network and Offline Login data not Available");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"कोई नेटवर्क और ऑफ़लाइन लॉगिन डेटा उपलब्ध नहीं है");
        return -1;
    }

    bool ok;
    QString LoginPasswd;
    if(LangMode == ENGLISH)
        LoginPasswd = QInputDialog::getText(this, tr("OFFLINE Login"),tr("Enter login Password"), QLineEdit::Password,"", &ok);
    else if(LangMode ==HINDI)
        LoginPasswd = QInputDialog::getText(this, tr("OFFLINE Login"), trUtf8("लॉगिन पासवर्ड दर्ज करें"), QLineEdit::Password,"", &ok);

    qDebug()<<"LoginPasswd:"<<LoginPasswd;

    if(query.value(1).toString().compare(LoginPasswd)!=0)
    {
        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Enter Valid Password");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"मान्य पासवर्ड दर्ज करें");
        return -1;
    }
    if((QDateTime::currentDateTime().toString("hh").toInt()<atoi(string(query.value(3).toString().toAscii().data()).substr(0,string(query.value(3).toString().toAscii().data()).find("AM")).c_str()))&&(QDateTime::currentDateTime().toString("hh").toInt()>atoi(string(query.value(3).toString().toAscii().data()).substr(string(query.value(3).toString().toAscii().data()).find("-")+1,string(query.value(3).toString().toAscii().data()).find("PM")-string(query.value(3).toString().toAscii().data()).find("-")-1).c_str())))
    {
        memset(TmpStr,0x00,sizeof(TmpStr));
        if(LangMode == ENGLISH)
        {
            sprintf(TmpStr,"Txn's are allowed from %s\n",query.value(3).toString().toAscii().data());
            miscObj.DisplayWideMsgBox((char *)TmpStr);
        }
        else if(LangMode == HINDI)
        {
            QString Hintext=QString::fromUtf8("से लेन-देन की अनुमति है");
            sprintf(TmpStr,"%s %s\n",query.value(3).toString().toAscii().data(),Hintext.toAscii().data());
            miscObj.tel_DisplayWideMsgBox((char *)TmpStr);
        }
        return -1;
    }

    QString Str;
    Str = query.value(5).toString();
    int StopMonth = Str.mid(5,2).toInt();
    qDebug()<<"2.StopMonth:: " << StopMonth <<"\tCurrent moth::" << QDateTime::currentDateTime().toString("MM").toInt();

//    if(query.value(4).toString().toInt()!= QDateTime::currentDateTime().toString("MM").toInt())
//    if(QDateTime::currentDateTime().toString("MM").toInt() != StopMonth)
//    if(QDateTime::currentDateTime().toString("MM").toInt() < StopMonth)
//    {
//        if(LangMode ==ENGLISH)
//            miscObj.DisplayWideMsgBox((char *)"Month Changed\nPlease login by authentication");
//        else
//            miscObj.tel_DisplayWideMsgBox((char *)"महीना बदल गया\nकृपया प्रमाणीकरण द्वारा लॉगिन करें");
//        return -1;
//    }

    qDebug()<<"pOfflineStoppedDate Str:"<<Str;
    OfflineStopDate = QDate::fromString(Str,"yyyy-MM-dd");
    QDate PresentDate(QDate::currentDate());
    int Days = PresentDate.daysTo(OfflineStopDate);
    qDebug() << "Remaining days to Stop::" << Days;
    if(Days < 0)
    {
        if(LangMode ==ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Month Changed\nPlease login by authentication");
        else if(LangMode == HINDI)
            miscObj.tel_DisplayWideMsgBox((char *)"महीना बदल गया\nकृपया प्रमाणीकरण द्वारा लॉगिन करें");
        return -1;
    }


    query.clear();
    if(!query.exec("select substr(a.lastlogindate,7,4),substr(a.lastlogindate,4,2),substr(a.lastlogindate,1,2) from PartialOnlineData a"))
    {

    }
    query.next();
    memset(TmpStr,0x00,sizeof(TmpStr));
    sprintf(TmpStr,"select julianday('now')-julianday('%04d-%02d-%02d')",query.value(0).toInt(),query.value(1).toInt(),query.value(2).toInt());
    qDebug()<<"TmpStr:"<<TmpStr;
    query.clear();
    if(!query.exec(TmpStr))
    {
        return -1;
    }
    query.next();
    qDebug()<<"query.value(0).toString().toInt() :"<<query.value(0).toString();
    if(atoi(query.value(0).toString().toAscii().data())!=0)
    {
        query.clear();
        if(!query.exec("update PartialOnlineData set leftOfflineTime = Duration"))
        {
            qDebug()<<"Duration Update error:"<<query.lastError();
            //return -1;
        }
    }
    memset(TmpStr,0x00,sizeof(TmpStr));
    sprintf(TmpStr,"update PartialOnlineData set lastlogindate = '%02d-%02d-%04d',lastlogintime='%02d:%02d:%02d'",QDateTime::currentDateTime().toString("dd").toInt(),QDateTime::currentDateTime().toString("MM").toInt(),QDateTime::currentDateTime().toString("yyyy").toInt(),QDateTime::currentDateTime().toString("hh").toInt(),QDateTime::currentDateTime().toString("mm").toInt(),QDateTime::currentDateTime().toString("ss").toInt());
    if(!query.exec(TmpStr))
    {
        return -1;
    }
    query.clear();
    if(!query.exec("select leftOfflineTime from PartialOnlineData"))
    {
        return -1;
    }
    query.next();
    if(query.value(0).toInt()<=0)
    {
        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Offline time duration completed\nLogin by authentication");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"ऑफ़लाइन समय अवधि पूरी हुई\nकृपया प्रमाणीकरण द्वारा लॉगिन करें");
        return -1;
    }
    query.clear();
    if(!query.exec("select (select count() from sqlite_master where type='table' and name = 'KeyRegister'),(select count() from sqlite_master where type='table' and name = 'Pos_Ob'),(select count() from sqlite_master where type='table' and name = 'BenfiaryTxn')"))
    {

    }
    query.next();
    qDebug()<<"query.value(0).toString().toInt() :"<<query.value(0).toString().toInt();
    qDebug()<<"query.value(1).toString().toInt() :"<<query.value(1).toString().toInt();
    qDebug()<<"query.value(2).toString().toInt() :"<<query.value(2).toString().toInt();
    if((query.value(0).toString().toInt()==0)&&(query.value(1).toString().toInt()==0)&&(query.value(2).toString().toInt()==0))
    {
        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Offline Data not Avaliable");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"ऑफ़लाइन डेटा उपलब्ध नहीं है");
        return -1;
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
    if(!query.exec("select (select count() from BenfiaryTxn) as Total_Txn,(select count() from BenfiaryTxn where TxnUploadSts='Y' and TxnType = 'O') as Online_Txn,(select count() from BenfiaryTxn where TxnType ='P' or TxnType ='Q') as PartialOfline_Txn,(select count() from BenfiaryTxn where  TxnUploadSts='Y' and TxnType IN('P','Q')) as Upload_Txn,(select count() from BenfiaryTxn where TxnUploadSts='N' and TxnType IN('P','Q'))as PendingOffline_Txn"))
    {
        qDebug()<<"Records Count query Failure";
    }
    if(query.next())
    {
        char TmpStr[1024];
        memset(TmpStr,0x00,sizeof(TmpStr));
        sprintf(TmpStr,"Total Txn Records : %d\nOnline Txn Records:%d\nOffline Txn Records:%d\nUploaded Records:%d\nPending Records:%d\nAlloted Month&Year: %s-%s",query.value(0).toInt(),query.value(1).toInt(),query.value(2).toInt(),query.value(3).toInt(),query.value(4).toInt(),AllotmentMonth.toStdString().c_str(),AllotmentYear.toStdString().c_str());
        miscObj.DisplayWideMsgBox((char *)TmpStr);
    }

    query.clear();
    int StateInfoCnt = 0;
    query.prepare("SELECT count(*) FROM StateInfo");
    if(!query.exec())
        qDebug() << "In get StateInfo::" << query.lastError();
    else
    {
        while(query.next())
        {
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
            while(query.next())
            {
                stateCode = query.value(0).toString();
                stateNameEn = query.value(1).toString();
                stateNameLl = query.value(2).toString();
                stateProfile = query.value(3).toString();
                stateReceiptHeaderEn = query.value(4).toString();
                stateReceiptHeaderLl = query.value(5).toString();
                statefpsId = query.value(6).toString();
                consentHeader = query.value(7).toString();
            }
        }
    }
    else
    {
        if(LangMode == ENGLISH)
            miscObj.DisplayWideMsgBox((char *)"Offline state info not Avaliable");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"Offline state info not Avaliable");
        return -1;
    }

    DealerLoginBy = 2;
    ui->btnAadhaarServices->setDisabled(true);
    //    ui->btnReceiveGoods->setDisabled(true);;
    ui->btn_inspector->setDisabled(true);
    ui->btn_BenVerify->setDisabled(true);
    ui->btnDailySales->setDisabled(true);
    ui->btnStockRegister->setDisabled(true);
    ui->BtnLastReciept->hide();

    ui->radioButton_UIDFLAG->hide();
    PartialOnlineTimer = new QTimer(this);
    connect(PartialOnlineTimer, SIGNAL(timeout()), this, SLOT(GetPartialTimeStatus()));
    PartialOnlineTimer->start(1000*60);
    ui->stackedWidget->setCurrentIndex(1);
}

void Widget::GetMsgScroll()
{
    /*QWidget *Base = new QWidget(ui->page_4);
    Base->setGeometry(10,42,300,35);
    QGridLayout *Area = new QGridLayout(Base);
    Scroler = new WidgetMarqueeLabel(this);
    Scroler->setTextFormat(Qt::RichText);
    Area->addWidget(Scroler);
    Scroler->setAlignment(Qt::AlignVCenter);
    char TmpStr[512];
    sprintf(TmpStr,"Your are Doing Txn's in Offline mode           Time slot for Offline Txn is 11:36:14 to 13:36:14 ");
    Scroler->setText(TmpStr);//Welcome to Public Distribution System
    Scroler->setSpeed(1);
    Scroler->setDirection(WidgetMarqueeLabel::RightToLeft);
    Scroler->setFont(QFont("Arial", 15,15));*/
}

void Widget::HideScroll()
{
    /*Scroler->close();
    Scroler->deleteLater();*/
}

//int Widget::GetTransactionId(char *TxnId)
//{
//    struct tm *tt;
//    time_t  t;
//    char buff[32];

//    time(&t);
//    tt=localtime(&t);

//    memset(buff, 0, sizeof(buff));

//    int tmp = 0;
//    tmp = (((tt->tm_year-100) - 10) * 366) + (tt->tm_yday+1);
//    qDebug()<<"first 4 digits = "<<tmp;

//    int time_sec = 0;
//    time_sec = (tt->tm_hour * 60 * 60) + (tt->tm_min * 60) + tt->tm_sec;

//    qDebug()<<"time sec = "<<time_sec;
//    qDebug()<<"Last 5 digits = "<<time_sec + 10000;

//    char cMachineID[16];
//    memset(cMachineID, 0, sizeof(cMachineID));
//    // CMisc miscObj;
//    miscObj.GetMachineId(cMachineID);

//    sprintf(TxnId, "%s%04d%05d", &cMachineID[6], tmp, time_sec+10000);
//    qDebug() <<"TxnID :"<<TxnId;

//    return 0;
//}

int Widget::GetTransactionId(char *TxnId)
{
    char cMachineID[16];
    memset(cMachineID, 0, sizeof(cMachineID));

    QDate Dt = QDate::currentDate();
    QString dateString = Dt.toString("yyyy");

    dateString = "0101" + dateString;
    QDate date = QDate::fromString(dateString, "ddMMyyyy");

    int JulianDay = Dt.toJulianDay() - date.toJulianDay();

    qDebug() << "Julian day:: " << JulianDay;

    QString TimeStamp = QTime::currentTime().toString("hhmmss");

    miscObj.GetMachineId(cMachineID);
    sprintf(TxnId, "%s%03d%s", &cMachineID[6], JulianDay, TimeStamp.toAscii().data());
    qDebug() <<"NewTxnID :"<<TxnId;

    return 0;
}

int Widget::getLangMode()
{
    char szLang[4];
    memset(szLang, 0, sizeof(szLang));

    QSqlQuery query;
    query.prepare("SELECT LangMode from Settings");
    if(!query.exec())
    {
        qDebug() << query.lastError();
        return -1;
    }
    else
    {
        while(query.next())
        {
            strcpy(szLang, query.value(0).toString().toAscii().data());
        }
    }
    qDebug()<<"Lang Mode : "<<szLang;
    return atoi(szLang);
}
int Widget::PingCheck()
{
    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Checking Network\nPlease Wait...");
    else
        dlgPopup1->ui->lblPopup->setText(trUtf8("नेटवर्क की जाँच हो रही है...\nकृपया प्रतीक्षा करें"));
    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    char TmpStr[5];
    memset(TmpStr,0x00,sizeof(TmpStr));
    FILE *fp = popen("ping google.com -c 5|grep packets|awk {'print $7'}","r");
    fgets(TmpStr,sizeof(TmpStr),fp);
    pclose(fp);
    qDebug()<<"Ping TmpStr:"<<TmpStr;
    dlgPopup1->close();
    dlgPopup1->deleteLater();
    if(strncmp(TmpStr,"0%",2)==0)
    {
        dlgPopup1 = new popup_msg(this);
        if(LangMode == 1)
            dlgPopup1->ui->lblPopup->setText("Network Available\nPlease try again...");
        else
            dlgPopup1->ui->lblPopup->setText(trUtf8("नेटवर्क उपलब्ध है\nकृपया पुन: प्रयास करें..."));
        dlgPopup1->show();
        QCoreApplication::processEvents();
        QCoreApplication::processEvents();
        sleep(2);
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return 0;
    }
    return -1;
}

int Widget::CheckOfflineTxnGetDetails()
{

    if(PingCheck()==0)
        return 0;
    if(partialOnlineOfflineStatus.toUpper() == "Y")
    {
        if(ui->radioButton_UIDFLAG->isChecked()==true)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"No Network\nPlease select \"Card No.\" and Enter \"Card No.\" for doing TXN in OFFLINE");
            else
                miscObj.tel_DisplayWideMsgBox((char *)"कोई नेटवर्क \n कृपया न चुनें \"कार्ड नंबर\" और ऑफ़लाइन होने के लिए TXN करने के लिए \"कार्ड नंबर\" दर्ज करें");
            ui->ldtRationCard->setText("");
            ui->radioButton_RCNOFLAG->setChecked(true);
            on_radioButton_RCNOFLAG_clicked(true);
            return -1;
        }
        GetOfflineCommDetails();
        return 0;
    }
}
int Widget::CheckOfflineTxnScanFp()
{

    if(PingCheck()==0)
        return 0;
    if(partialOnlineOfflineStatus.toUpper() == "Y")
    {
        if(ui->radioButton_UIDFLAG->isChecked()==true)
        {
            miscObj.DisplayMsgBox((char *)"No Network\nPlease select \"RC NO\" and Enter \"RC NO\" for doing TXN in OFFLINE");
            return -1;
        }
        GetOfflineCommDetails();
        return 0;
    }
}
int Widget::CheckOfflineTxnUpload()
{
    if(PingCheck()==0)
        return 0;
    if(partialOnlineOfflineStatus.toUpper() == "Y")
    {

        if(ui->radioButton_RCNOFLAG->isChecked()==true)
        {
        }
        else
        {
            ui->ldtRationCard->setText(ui->lblRCNO_Val->text());;
        }
        PartialOfflineTxnUpload();
        return 0;
    }
}
int Widget::CheckOfflineGetDealerDetails()
{
    if(PingCheck()==0)
        return 0;
    nLogout=0;
    LoginByPassword();
}

int Widget::CheckOfflineDealerAuth()
{
    QSqlQuery query;
    int RetVal;
    //PartiDataDownloadFlag = "Y";
    //PartialTxnDeleteFlag = "Y";

    if(CBOflineDownload())
         return -1;

    if(PartiDataDownloadFlag.toUpper()=="Y") //uploading data before downloadnig DKR here
    {
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
        if(!query.exec("select (select count() from BenfiaryTxn) as Total_Txn,(select count() from BenfiaryTxn where TxnUploadSts='Y' and TxnType = 'O') as Online_Txn,(select count() from BenfiaryTxn where TxnType ='P' or TxnType ='Q') as PartialOfline_Txn,(select count() from BenfiaryTxn where  TxnUploadSts='Y' and TxnType IN('P','Q')) as Upload_Txn,(select count() from BenfiaryTxn where TxnUploadSts='N' and TxnType IN('P','Q'))as PendingOffline_Txn"))
        {
            qDebug()<<"Records Count query Failure";
        }
        if(query.next())
        {
            char TmpStr[1024];
            memset(TmpStr,0x00,sizeof(TmpStr));
//            sprintf(TmpStr,"Total Txn Records : %d\nOnline Txn Records:%d\nOffline Txn Records:%d\nUploaded Records:%d\nPending Records:%d",query.value(0).toInt(),query.value(1).toInt(),query.value(2).toInt(),query.value(3).toInt(),query.value(4).toInt());

            sprintf(TmpStr,"Total Txn Records : %d\nOnline Txn Records:%d\nOffline Txn Records:%d\nUploaded Records:%d\nPending Records:%d\nAlloted Month&Year: %s-%s",query.value(0).toInt(),query.value(1).toInt(),query.value(2).toInt(),query.value(3).toInt(),query.value(4).toInt(),AllotmentMonth.toStdString().c_str(),AllotmentYear.toStdString().c_str());

            miscObj.DisplayWideMsgBox((char *)TmpStr);
        }

        query.clear();
        query.prepare("select count() from OfflineStockReceive where RecvdUploadSts='N'");

        if(!query.exec())
        {
            qDebug()<<"1.Txn not uploaded CheckOfflineDealerAuth() ::"<<query.lastError();
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
                    return -1;
                }
            }
        }

        query.clear();
        query.prepare("select count() from BenfiaryTxn where TxnUploadSts = 'N'");

        if(!query.exec())
        {
            qDebug()<<"Txn not uploaded";
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
                if(RetVal == -2)
                {
                    return -1; //No Internet
                }
                else if(RetVal == 0) //no txn exists
                {
                    int ret = UpdateTransStatus();  //added ack for upload txns
                    if(ret == 0)
                    {
                        if(LangMode == 1)
                            miscObj.DisplayWideMsgBox((char *)"All offline txn records were uploaded to server");
                        else
                            miscObj.tel_DisplayWideMsgBox((char *)"सभी ऑफ़लाइन txn रिकॉर्ड सर्वर पर अपलोड किए गए थे");
                        return 0;
                    }
                    else
                        return -1;
                }
                else // pending error txns exists
                {
                    int ret = UpdateTransStatus();
                    if(ret == 0)
                    {
                        if(LangMode == 1)
                            miscObj.DisplayWideMsgBox((char *)"All offline txn records were uploaded to server");
                        else
                            miscObj.tel_DisplayWideMsgBox((char *)"सभी ऑफ़लाइन txn रिकॉर्ड सर्वर पर अपलोड किए गए थे");
                        return 0;
                    }
                    else
                        return -1;
                    if(PartialTxnDeleteFlag.toUpper() == "Y")
                    {

                    }
                    else
                    {
                        return -1;
                    }
                }
            }
        }
        if(DwnldPartialOnlineData()) //DKR download
            return -1;
        if(UpdateDownLoadStatus()) //DKR AWC sending
            return -1;

        //        UploadData.start();  //thread running here
        //        UploadData.moveToThread(&UploadData); //commenting thread upload as per dept -BSK.
    }
    else
    {
        if(partialOnlineOfflineStatus.toUpper() == "Y")   // Thread Uploading Offline Shop txns
        {
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
            if(!query.exec("select (select count() from BenfiaryTxn) as Total_Txn,(select count() from BenfiaryTxn where TxnUploadSts='Y' and TxnType = 'O') as Online_Txn,(select count() from BenfiaryTxn where TxnType ='P' or TxnType ='Q') as PartialOfline_Txn,(select count() from BenfiaryTxn where  TxnUploadSts='Y' and TxnType IN('P','Q')) as Upload_Txn,(select count() from BenfiaryTxn where TxnUploadSts='N' and TxnType IN('P','Q'))as PendingOffline_Txn"))
            {
                qDebug()<<"Records Count query Failure";
            }
            if(query.next())
            {
                char TmpStr[1024];
                memset(TmpStr,0x00,sizeof(TmpStr));
//                sprintf(TmpStr,"Total Txn Records : %d\nOnline Txn Records:%d\nOffline Txn Records:%d\nUploaded Records:%d\nPending Records:%d",query.value(0).toInt(),query.value(1).toInt(),query.value(2).toInt(),query.value(3).toInt(),query.value(4).toInt());

                        sprintf(TmpStr,"Total Txn Records : %d\nOnline Txn Records:%d\nOffline Txn Records:%d\nUploaded Records:%d\nPending Records:%d\nAlloted Month&Year: %s-%s",query.value(0).toInt(),query.value(1).toInt(),query.value(2).toInt(),query.value(3).toInt(),query.value(4).toInt(),AllotmentMonth.toStdString().c_str(),AllotmentYear.toStdString().c_str());

                miscObj.DisplayWideMsgBox((char *)TmpStr);
            }
            //            UploadData.start();
            //            UploadData.moveToThread(&UploadData); //commenting thread upload as per dept -BSK.
        }
        else     //Offline to online convertion  if any Txn data exist then try to uploading forcefully
        {
            query.clear();
            query.prepare("select count() from OfflineStockReceive where RecvdUploadSts='N'");

            if(!query.exec())
            {
                qDebug()<<"2.Txn not uploaded CheckOfflineDealerAuth() ::"<<query.lastError();
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
                        return -1;
                    }
                }
            }

            query.clear();
            query.exec("select count(*) from BenfiaryTxn where TxnUploadSts ='N'");
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
                    if(RetVal == 0)
                    {
                        int ret = UpdateTransStatus();
                        if(ret == 0)
                        {
                            miscObj.DisplayWideMsgBox((char *)"All offline txn records were uploaded to server");
                            return 0;
                        }
                        else
                            return -1;
                    }
                    else
                    {
                        int ret = UpdateTransStatus();
                        if(ret == 0)
                        {
                            miscObj.DisplayWideMsgBox((char *)"All offline txn records were uploaded to server");
                            return 0;
                        }
                        else
                            return -1;
                    }
                }
                else
                {
                    if(partialOnlineOfflineStatus.toUpper() == "N")
                    {
                        query.clear();
                        query.exec("drop table KeyRegister");
                        query.clear();
                        query.exec("drop table Pos_Ob");
                        query.clear();
                        query.exec("drop table commodityMaster");
                        query.clear();
                        query.exec("drop table schemeMaster");
                        query.clear();
                        query.exec("delete from BenfiaryTxn");
                    }
                }
            }
        }
    }
    return 0;
}

int Widget::CheckRecviceGdsOffline()
{
    //    if(PingCheck()==0)
    //            return 0;
    ui->stackedWidget->setCurrentIndex(51);
}

void TxnUpload::GetStateInfo()
{
    int StateInfoCnt = 0;

    QSqlQuery query;
    query.prepare("SELECT count(*) FROM StateInfo");
    if(!query.exec())
        qDebug() << "In get StateInfo::" << query.lastError();
    else
    {
        while(query.next())
        {
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
            while(query.next())
            {
                stateCode = query.value(0).toString();
                stateNameEn = query.value(1).toString();
                stateNameLl = query.value(2).toString();
                stateProfile = query.value(3).toString();
                stateReceiptHeaderEn = query.value(4).toString();
                stateReceiptHeaderLl = query.value(5).toString();
                statefpsId = query.value(6).toString();
                consentHeader = query.value(7).toString();

            }
        }
    }
}

int TxnUpload::ReceiveGdsUpload()
{

    Json::Value UploadData,TmpData;
    Json::Value ArrayList(Json::arrayValue);

    ArrayList.clear();
    UploadData.clear();

    QSqlQuery query;
    query.clear();
    if(!query.exec("select FpsId,TruckChitNum,Challan,VechNum,SchemeId,CommCode,RecvdQty,Unit,Month,Year,DateTime from OfflineStockReceive where RecvdUploadSts='N'"))
    {

    }
    QSqlQuery query1;
    query1.prepare("Select (select ShopID FROM  ShopOwnerDetails),(select fpsSessionId from Dealer)");
    if(!query1.exec())
        qDebug() << "SELECT ShopID FROM ShopOwnerDetails = " << query1.lastError();
    query1.next();
    /*char cmachineID[16];
        memset(cmachineID, 0, sizeof(cmachineID));
        CMisc miscObj;
        miscObj.GetMachineId(cmachineID);*/
    if(query1.value(0).toString().size()==0)
    {
        qDebug()<<"Invalid Shop Number";

        return -1;
    }
    UploadData["fpsId"]=query1.value(0).toString().toAscii().data();
    UploadData["stateCode"] = stateCode.toStdString().c_str();
    UploadData["terminalId"]=DeviceMacId.toAscii().data();
    UploadData["token"]= RECEIVEGOODS_TOKEN;//"9f943748d8c1ff6ded5145c59d0b2ae7";
    UploadData["sessionId"]=query1.value(1).toString().toAscii().data();
    ArrayList.clear();
    while(query.next())
    {
        TmpData.clear();
        TmpData["truckChitId"]=query.value(1).toString().toAscii().data();
        TmpData["challanId"]=query.value(2).toString().toAscii().data();
        TmpData["truckNo"]=query.value(3).toString().toAscii().data();
        TmpData["schemeId"]=query.value(4).toString().toAscii().data();
        TmpData["commCode"]=query.value(5).toString().toAscii().data();
        TmpData["entryQuantity"]=query.value(6).toString().toAscii().data();
        TmpData["units"]=query.value(7).toString().toAscii().data();
        TmpData["month"]=query.value(8).toString().toAscii().data();
        TmpData["year"]=query.value(9).toString().toAscii().data();
        ArrayList.append(TmpData);
    }
    UploadData["truckChitRequest"]=ArrayList;
    char *Ret="";
    Ret = Convertstr(UploadData.toStyledString());
    //qDebug()<<"Data :"<<Ret;
    if(UploadData["truckChitRequest"].size()==0)
    {
        qDebug()<<"No Txn Were found for Upload";
        return 0;
    }

    //    int RetVal = CurlRequest("http://164.100.65.97/ePosCommonService/eposCommon/offlineReceiveStockUpdate",Ret);

    char URL[125];
    memset(URL,0x00,sizeof(URL));
    sprintf(&URL[0],"%s/offlineReceiveStockUpdate",OFFLINE_AND_RECEIVE_STOCK_URL);
//    sprintf(&URL[0],"%sofflineReceiveStockUpdate",Offline_and_ReceiveStock_URL.toAscii().data());
    int RetVal = CurlRequest(URL,Ret,300);

    char TmpStr[1024];
    if(RetVal==0)
    {
        Json::Value root;
        root.clear();
        //QSqlQuery query;
        Json::Reader reader;
        std::ifstream ReadData("/tmp/Sample.txt");
        bool parsedSuccess = reader.parse(ReadData,root,false);
        ReadData.close();
        if(not parsedSuccess)
        {
            qDebug()<<"Upload Status String  File not Found";

            return -1;
        }
        else
        {
            printf("\nrespCode    : %s\n",Convertstr(root["respCode"].asString()));
            printf("\nrespMessage : %s\n",Convertstr(root["respMessage"].asString()));
            if(atoi(Convertstr(root["respCode"].asString()))==0)
            {
                Json::Value UploadedData;
                UploadedData.clear();
                UploadedData = root.get("truckChitList",0);
                printf("\nSize : %d\n",UploadedData.size());
                if(UploadedData.size()==0)
                {
                    return -1;
                }
                else
                {
                    for(int i=0;i<UploadedData.size();i++)
                    {
                        query.clear();
                        memset(TmpStr,0x00,sizeof(TmpStr));
                        sprintf(TmpStr,"update OfflineStockReceive set RecvdUploadSts = 'Y' where TruckChitNum='%s' and Challan='%s' and CommCode='%s' and SchemeId='%s'",Convertstr(UploadedData[i]["truckChitId"].asString()),Convertstr(UploadedData[i]["challanId"].asString()),Convertstr(UploadedData[i]["commId"].asString()),Convertstr(UploadedData[i]["shcemeId"].asString()));
                        qDebug()<<"TmpStr Update Status:"<<TmpStr;
                        if(!query.exec(TmpStr))
                        {
                            qDebug()<<"Upoad Query Error:"<<query.lastError();
                            return -1;
                        }
                    }
                }
            }
            else
            {

                return -1;
            }
        }
    }
    else
    {
        return -1;
    }
}

int Widget::ManualReceiveGdsUpload()
{
    Json::Value UploadData,TmpData;
    Json::Value ArrayList(Json::arrayValue);

    ArrayList.clear();
    UploadData.clear();
    QSqlQuery query;
    query.clear();
    if(!query.exec("select FpsId,TruckChitNum,Challan,VechNum,SchemeId,CommCode,RecvdQty,Unit,Month,Year,DateTime from OfflineStockReceive where RecvdUploadSts='N'"))
    {
        return 0;
    }
    QSqlQuery query1;
    query1.prepare("Select (select ShopID FROM  ShopOwnerDetails),(select fpsSessionId from Dealer)");
    if(!query1.exec())
    {
        qDebug() << "SELECT ShopID FROM ShopOwnerDetails = " << query1.lastError();
        miscObj.DisplayWideMsgBox((char *)"Invalid Fps Id");
        return -1;

    }
    query1.next();
    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));
    CMisc miscObj;
    miscObj.GetMachineId(cmachineID);
    if(query1.value(0).toString().size()==0)
    {
        miscObj.DisplayWideMsgBox((char *)"Invalid Fps Id");
        return -1;
    }
    UploadData["fpsId"]=query1.value(0).toString().toAscii().data();
    UploadData["stateCode"] = stateCode.toStdString().c_str();
    UploadData["terminalId"]=cmachineID;
    UploadData["token"]= RECEIVEGOODS_TOKEN;//"9f943748d8c1ff6ded5145c59d0b2ae7";
    UploadData["sessionId"]=query1.value(1).toString().toAscii().data();
    ArrayList.clear();
    while(query.next())
    {
        TmpData.clear();
        TmpData["truckChitId"]=query.value(1).toString().toAscii().data();
        TmpData["challanId"]=query.value(2).toString().toAscii().data();
        TmpData["truckNo"]=query.value(3).toString().toAscii().data();
        TmpData["schemeId"]=query.value(4).toString().toAscii().data();
        TmpData["commCode"]=query.value(5).toString().toAscii().data();
        TmpData["entryQuantity"]=query.value(6).toString().toAscii().data();
        TmpData["units"]=query.value(7).toString().toAscii().data();
        TmpData["month"]=query.value(8).toString().toAscii().data();
        TmpData["year"]=query.value(9).toString().toAscii().data();
        ArrayList.append(TmpData);
    }
    UploadData["truckChitRequest"]=ArrayList;
    char *Ret="";
    Ret = Convertstr(UploadData.toStyledString());
    //qDebug()<<"Data :"<<Ret;
    if(UploadData["truckChitRequest"].size()==0)
    {
        qDebug()<<"No Txn Were found for Upload";
        return 0;
    }

    //    int RetVal = CurlRequest("http://164.100.65.97/ePosCommonService/eposCommon/offlineReceiveStockUpdate",Ret);

    char URL[125];
    memset(URL,0x00,sizeof(URL));
//    sprintf(&URL[0],"%s/offlineReceiveStockUpdate",OFFLINE_AND_RECEIVE_STOCK_URL);
    sprintf(&URL[0],"%sofflineReceiveStockUpdate",Offline_and_ReceiveStock_URL.toAscii().data());
    int RetVal = CurlRequest(URL,Ret,300);

    char TmpStr[1024];
    if(RetVal==0)
    {
        Json::Value root;
        root.clear();
        //QSqlQuery query;
        Json::Reader reader;
        std::ifstream ReadData("/tmp/Sample.txt");
        bool parsedSuccess = reader.parse(ReadData,root,false);
        ReadData.close();
        if(not parsedSuccess)
        {
            qDebug()<<"Upload Status String  File not Found";
            miscObj.DisplayWideMsgBox((char *)"Invalid Response for Receive Goods Upload");
            return -1;
        }
        else
        {
            printf("\nrespCode    : %s\n",Convertstr(root["respCode"].asString()));
            printf("\nrespMessage : %s\n",Convertstr(root["respMessage"].asString()));
            if(atoi(Convertstr(root["respCode"].asString()))==0)
            {
                Json::Value UploadedData;
                UploadedData.clear();
                UploadedData = root.get("truckChitList",0);
                printf("\nSize : %d\n",UploadedData.size());
                if(UploadedData.size()==0)
                {
                    miscObj.DisplayWideMsgBox((char *)"TruckChitList not found for Receive Goods Upload");
                    return -1;
                }
                else
                {
                    for(int i=0;i<UploadedData.size();i++)
                    {
                        query.clear();
                        memset(TmpStr,0x00,sizeof(TmpStr));
                        sprintf(TmpStr,"update OfflineStockReceive set RecvdUploadSts = 'Y' where TruckChitNum='%s' and Challan='%s' and CommCode='%s' and SchemeId='%s'",Convertstr(UploadedData[i]["truckChitId"].asString()),Convertstr(UploadedData[i]["challanId"].asString()),Convertstr(UploadedData[i]["commId"].asString()),Convertstr(UploadedData[i]["shcemeId"].asString()));
                        qDebug()<<"TmpStr Update Status:"<<TmpStr;
                        if(!query.exec(TmpStr))
                        {
                            qDebug()<<"Upoad Query Error:"<<query.lastError();
                            return -1;
                        }
                    }
                    return 0;
                }
            }
            else
            {
                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"Unable to upload.Please check connectivity\nPlease Try again\nError Code:%d",RetVal);
                miscObj.DisplayWideMsgBox((char *)TmpStr);
                return -1;
            }
        }
    }
    else
    {
        return -1;
    }
}


int Widget::PatchUpdateOfflineCheck()
{
//    return 0;
    QSqlQuery query;
    query.clear();
    if(!query.exec("select count() from BenfiaryTxn where TxnUploadSts='N' and TxnType IN('P','Q')"))
    {
        qDebug()<<"Records Count query Failure in Patch Update";
        qDebug()<<"Error:"<<query.lastError();
    }
    if(query.next()){
        qDebug()<<"cnt :"<<query.value(0).toInt();
        if(query.value(0).toInt()>0){
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Upload all pending Txn's and Try for Patch update");
            else
                miscObj.tel_DisplayWideMsgBox((char *)"सभी लंबित Txn को अपलोड करें और पैच अपडेट के लिए प्रयास करें");
            return -1;
        }else{
            return 0;
        }
    }
    return 0;
}

int Widget::CheckTxnDate()
{
    QDate OfflineStopDate;

    if((DealerLoginBy == 2)||(partialOnlineOfflineStatus.toUpper()=="Y"))
    {
        QSqlQuery query;
        query.clear();
        if(!query.exec("select pOfflineStoppedDate from PartialOnlineData")){
            return 0;
        }
        query.next();
        try{
            QString Str;
            Str = query.value(0).toString();
            qDebug()<<"pOfflineStoppedDate Str:"<<Str;
            OfflineStopDate = QDate::fromString(Str,"yyyy-MM-dd");
            QDate PresentDate(QDate::currentDate());
            int Days = PresentDate.daysTo(OfflineStopDate);
            qDebug() << "Remaining days to Stop::" << Days;
            if(Days < 0)
            {
                miscObj.DisplayWideMsgBox((char *)"PDS Distribution Stopped");
                return -1;
            }
        }catch(exception e){
            qDebug()<<"Exception;"<<e.what();
        }
    }
    return 0;
}

int Widget::CheckTxnStopDate(QString menu)
{
    qDebug()<<"---------------------->>"<<menu;
    if((DealerLoginBy == 2)||(partialOnlineOfflineStatus.toUpper()=="Y"))
    {
        if(menu == "P")
        {
            QSqlQuery query;
            query.clear();
            if(!query.exec("select pOfflineStoppedDate from PartialOnlineData"))
            {
                return 0;
            }
            query.next();
            try
            {
                string Str;
                Str.append(query.value(0).toString().toAscii().data());
                qDebug()<<"atoi(Str.substr(9,2).c_str(): "<<atoi(Str.substr(8,2).c_str());
                qDebug()<<"atoi(Str.substr(5,2).c_str(): "<<atoi(Str.substr(5,2).c_str());
                qDebug()<<"Current month: "<<QDateTime::currentDateTime().toString("MM").toInt();

                if((QDateTime::currentDateTime().toString("dd").toInt()>atoi(Str.substr(8,2).c_str())) || (QDateTime::currentDateTime().toString("MM").toInt() != atoi(Str.substr(5,2).c_str()) ))
                {
                    miscObj.DisplayWideMsgBox((char *)"PDS Distribution Stopped");
                    return -1;
                }
            }
            catch(exception e)
            {
                qDebug()<<"Exception;"<<e.what();
            }
        }
        else
        {
            QSqlQuery query;
            query.clear();
            char tmpstr[100];
            memset(tmpstr,0x00,sizeof(tmpstr));
            sprintf(tmpstr,"select Stop_Date from StoppedDate where Menu_Name ='%s'",menu.toAscii().data() );
            if(!query.exec(tmpstr))
            {
                return 0;
            }
            query.next();
            try
            {
                string Str;
                Str.append(query.value(0).toString().toAscii().data());
                qDebug()<<"atoi(Str.substr(9,2).c_str(): "<<atoi(Str.substr(8,2).c_str());
                qDebug()<<"atoi(Str.substr(5,2).c_str(): "<<atoi(Str.substr(5,2).c_str());
                qDebug()<<"Current month: "<<QDateTime::currentDateTime().toString("MM").toInt();

                if((QDateTime::currentDateTime().toString("dd").toInt()>atoi(Str.substr(8,2).c_str())) || (QDateTime::currentDateTime().toString("MM").toInt() != atoi(Str.substr(5,2).c_str()) ))
                {
                    miscObj.DisplayWideMsgBox((char *)"Distribution Stoped");
                    return -1;
                }
            }
            catch(exception e)
            {
                qDebug()<<"Exception;"<<e.what();
            }
        }
    }
    return 0;
}

int Widget::SendTxnCancelReason(QString & Reason,char ModuleType)
{
    PopMsg = new popup_msg(this);
    if(LangMode == ENGLISH)
        PopMsg->ui->lblPopup->setText("Sending Reason\n  Please Wait ..");
    else if(LangMode == HINDI)
        PopMsg->ui->lblPopup->setText(trUtf8("कारण भेजना \n कृपया प्रतीक्षा करें .."));
    PopMsg->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    Json::Value RequestedData;
    char FpsId[30];
    char TmpStr[500];
    memset(FpsId,0x00,sizeof(FpsId));
    getShopId(FpsId);

    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));
    miscObj.GetMachineId(cmachineID);

    QSqlQuery query;
    query.clear();
    query.prepare("select fpsSessionId from Dealer");
    query.exec();query.next();

    RequestedData["fpsId"] = FpsId;
    RequestedData["token"] = RECEIVEGOODS_TOKEN;

    //    if(iDealMemSelected == MEMBER_SELECTED)
    //        RequestedData["rcId"] = ui->lblRCNO_Val->text().toStdString().c_str();
    //    else
    //        RequestedData["rcId"] = FpsId;

    //    if(iDealMemSelected == MEMBER_SELECTED)
    //        RequestedData["rcId"] = ui->lblRCNO_Val->text().toStdString().c_str();
    //    else
    //        {
    if((iDealMemSelected == DEALER_SELECTED) || (iDealMemSelected == INSPECTOR_SELECTED))
        RequestedData["rcId"]=FpsId;
    else if((iMainTransaction == PDS_TRANSACTION) || (iMainTransaction == EKYC_TRANSACTION) || (iDealMemSelected == MEMBER_SELECTED))
        RequestedData["rcId"]=ui->lblRCNO_Val->text().toAscii().data();
    else if ((iMainTransaction == MDM_TRANSACTION) || (iMainTransaction == ICDS_TRANSACTION))
        RequestedData["rcId"]=ui->lineEditAnganwadiCardNo->text().toAscii().data();
    else
        RequestedData["rcId"]=FpsId;
    //        }

    if(iDealMemSelected == DEALER_SELECTED)
        RequestedData["modeOfService"]="D";
    else if(iDealMemSelected == BEN_VERIFICATION)
        RequestedData["modeOfService"]="B";
    else if(iMainTransaction == PDS_TRANSACTION)
        RequestedData["modeOfService"]="P";
    else if(iMainTransaction == MDM_TRANSACTION)
        RequestedData["modeOfService"]="S";
    else if(iMainTransaction == ICDS_TRANSACTION)
        RequestedData["modeOfService"]="A";
    else if(iMainTransaction == EKYC_TRANSACTION)
        RequestedData["modeOfService"]="US";
    else if(iMainTransaction == INSPECTOR_SELECTED)
        RequestedData["modeOfService"]="INSP";
    else
        RequestedData["modeOfService"]="O";

    if(ModuleType == 'R')
        RequestedData["moduleType"] = "R";
    else
        RequestedData["moduleType"] = "C";

    QStringList list = Reason.split("-");

    RequestedData["requestId"] = list.at(0).toStdString().c_str();
    RequestedData["requestValue"] = list.at(1).toStdString().c_str();
    RequestedData["timeStamp"] = QDateTime::currentDateTime().toString("ddMMyyyyhhmmss").toAscii().data();
    RequestedData["stateCode"] = stateCode.toStdString().c_str();
    RequestedData["sessionId"] =query.value(0).toString().toAscii().data();
    RequestedData["terminalId"]=cmachineID;

    char *Ret=Convertstr(RequestedData.toStyledString());

    char URL[125];
    memset(URL,0x00,sizeof(URL));
//    sprintf(&URL[0],"%s/reasonConsent",OFFLINE_AND_RECEIVE_STOCK_URL);
    sprintf(&URL[0],"%sreasonConsent",Offline_and_ReceiveStock_URL.toAscii().data());
    int RetVal = CurlRequest(URL,Ret,300);

    PopMsg->close();
    PopMsg->deleteLater();
    if(RetVal!=0)
    {
        miscObj.DisplayWideMsgBox("Invalid Response\nPlease try again");
        return -1;
    }
    else
    {
        Json::Value root;
        root.clear();
        Json::Reader reader;
        std::ifstream ReadData("/tmp/Sample.txt");
        bool parsedSuccess = reader.parse(ReadData,root,false);
        ReadData.close();
        if(not parsedSuccess)
        {
            miscObj.DisplayWideMsgBox("Invalid Response from Server\nPlease try again");
            return -1;
        }
        else
        {
            printf("\nrespCode    : %s\n",Convertstr(root["respCode"].asString()));
            printf("\nrespMessage : %s\n",Convertstr(root["respMessage"].asString()));
            if(atoi(Convertstr(root["respCode"].asString()))==0)
            {
                miscObj.DisplayWideMsgBox(Convertstr(root["respMessage"].asString()));
                return 0;
            }
            else
            {
//                miscObj.DisplayWideMsgBox(Convertstr(root["respMessage"].asString()));
//                return -1;
                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"%s\nError Code :%s",Convertstr(root["respMessage"].asString()),Convertstr(root["respCode"].asString()));
                miscObj.DisplayWideMsgBox((char *)TmpStr);
                return -1;
            }
        }
    }
}

//int Widget::CheckingPofflineTrans()
//{
//    int uploadingRecords =0,totalRecords=0,pendingRecords=0;
//    char TmpStr[500];
//    uploadingRecords=0;
//    totalRecords=0;
//    pendingRecords=0;

//    QSqlQuery query;

//    query.clear();
//    memset(TmpStr,0x00,sizeof(TmpStr));
//    query.clear();
//    sprintf(TmpStr,"select count(*) from BenfiaryTxn where TxnType IN('P','Q') and RcId=%s and TxnUploadSts = 'N'",ui->ldtRationCard->text().toAscii().data());
//    qDebug()<<"------------>"<<TmpStr;
//    query.exec(TmpStr);
//    query.next();
//    totalRecords = query.value(0).toString().toInt(0);
//    if(!totalRecords)
//    {
//        return 0;
//    }
//    qDebug()<<"In CheckingPofflineTrans";
//    Json::Value UploadData,CommWiseData;

//    Json::Value ArrayList(Json::arrayValue);
//    ArrayList.clear();
//    UploadData.clear();
//    char FpsId[30];
//    memset(FpsId,0x00,sizeof(FpsId));
//    QSqlQuery query1;
//    query1.prepare("SELECT ShopID FROM  ShopOwnerDetails");
//    if(!query1.exec())
//        qDebug() << "SELECT ShopID FROM ShopOwnerDetails = " << query1.lastError();
//    query1.next();
//    //getShopId(FpsId);

//    char cmachineID[16];
//    memset(cmachineID, 0, sizeof(cmachineID));

//    CMisc miscObj;
//    miscObj.GetMachineId(cmachineID);
//    query.clear();
//    query.prepare("select fpsSessionId from Dealer");
//    query.exec();query.next();

//    UploadData["fpsId"]= query1.value(0).toString().toAscii().data();
//    UploadData["token"]= RECEIVEGOODS_TOKEN;//"f55725bfae061106da03ce754dfcc41c";
//    UploadData["stateCode"] = stateCode.toStdString().c_str();
//    UploadData["sessionId"]=query.value(0).toString().toAscii().data();//"0";
//    UploadData["terminalId"]=cmachineID;

//    memset(TmpStr,0x00,sizeof(TmpStr));
//    sprintf(TmpStr,"select RcId,CommCode,TotQty,BalQty,SchemeId,IssuedQty,RecptId,commAmount,TotAmt,Rate,MemberName,DateTime,TxnType,AllotMonth,AllotYear from BenfiaryTxn where TxnUploadSts = 'N' and RcId=%s",ui->ldtRationCard->text().toAscii().data());

//    if(!query.exec(TmpStr))
//    {
//        qDebug()<<"Upoad Query Error:"<<query.lastError();
//        return -1;
//    }
//    while(query.next())
//    {
//        CommWiseData.clear();
//        CommWiseData["rcId"]=query.value(0).toString().toAscii().data();
//        CommWiseData["commCode"]=query.value(1).toString().toAscii().data();
//        CommWiseData["totalEntitlement"]=query.value(2).toString().toAscii().data();
//        CommWiseData["balanceEntitlement"]=query.value(3).toString().toAscii().data();
//        CommWiseData["schemeId"]=query.value(4).toString().toAscii().data();
//        CommWiseData["issueQty"]=query.value(5).toString().toAscii().data();
//        CommWiseData["receiptId"]=query.value(6).toString().toAscii().data();
//        CommWiseData["commAmount"]=query.value(7).toString().toAscii().data();
//        CommWiseData["totalAmount"]=query.value(8).toString().toAscii().data();
//        CommWiseData["commPrice"]=query.value(9).toString().toAscii().data();
//        CommWiseData["headOfTheFamily"]=query.value(10).toString().toAscii().data();
//        CommWiseData["transactionTime"]=query.value(11).toString().toAscii().data();
//        CommWiseData["transMode"]=query.value(12).toString().toAscii().data();
//        CommWiseData["month"]=query.value(13).toString().toAscii().data();
//        CommWiseData["year"]=query.value(14).toString().toAscii().data();
//        ArrayList.append(CommWiseData);
//    }
//    UploadData["fpsOfflineTransResponses"]=ArrayList;
//    query.prepare("select commCode,closingBalance from Pos_Ob");
//    if(!query.exec())
//    {
//        qDebug()<<"Upoad Query Error:"<<query.lastError();
//        return -1;
//    }
//    ArrayList.clear();
//    while(query.next())
//    {
//        CommWiseData.clear();
//        CommWiseData["commCode"]=query.value(0).toString().toAscii().data();
//        CommWiseData["closingBalance"]=query.value(1).toString().toAscii().data();
//        ArrayList.append(CommWiseData);
//    }
//    UploadData["fpsCbs"]=ArrayList;
//    char *Ret="";
//    qDebug()<<"\n\n\nUploadData Size:"<<UploadData["fpsOfflineTransResponses"].size()<<"\n\n\n";
//    if(UploadData["fpsOfflineTransResponses"].size()==0)
//    {
//        qDebug()<<"No Txn Were found for Upload";
//        return 0;
//    }
//    /////////////////////////////////////////////////////////////
//    totalRecords=0;
//    query.clear();
//    query.exec("select count(*) from BenfiaryTxn where TxnType IN('P','Q')");
//    query.next();
//    totalRecords = query.value(0).toString().toInt(0);
//    UploadData["totalRecords"]=totalRecords;
//    UploadData["uploadingRecords"]=uploadingRecords;

//    query.clear();
//    query.exec("select count(*) from BenfiaryTxn where TxnType IN('P','Q') and TxnUploadSts ='N'");
//    query.next();
//    pendingRecords = query.value(0).toString().toInt(0);

//    if(uploadingRecords < pendingRecords)
//        UploadData["pendingRecords"]=(pendingRecords-uploadingRecords);
//    else
//        UploadData["pendingRecords"]=(uploadingRecords-pendingRecords);

//    /////////////////////////////////////////////////////////////
//    Ret = Convertstr(UploadData.toStyledString());
//    //qDebug()<<"Data :"<<Ret;
////    int RetVal = CurlRequest("http://164.100.196.62/ePosCommonServiceMP1_5/eposCommon/pushFpsOfflineData",Ret,0);
//    char URL[125];
//    memset(URL,0x00,sizeof(URL));
//    sprintf(&URL[0],"%s/pushFpsOfflineData",OFFLINE_AND_RECEIVE_STOCK_URL);
//    int RetVal = CurlRequest(URL,Ret,300);

//    if(RetVal==0)
//    {
//        Json::Value root;
//        root.clear();
//        //QSqlQuery query;
//        Json::Reader reader;
//        system("cat /tmp/Sample.txt");
//        std::ifstream ReadData("/tmp/Sample.txt");
//        bool parsedSuccess = reader.parse(ReadData,root,false);
//        ReadData.close();
//        if(not parsedSuccess)
//        {
//            miscObj.DisplayWideMsgBox((char *)"Invalid Response From Server for Txn Upload");
//            return -1;
//        }
//        else
//        {
//            printf("\nrespCode    : %s\n",Convertstr(root["respCode"].asString()));
//            printf("\nrespMessage : %s\n",Convertstr(root["respMessage"].asString()));
//            if(atoi(Convertstr(root["respCode"].asString()))==0)
//            {
//                Json::Value PosOb;
//                PosOb.clear();
//                PosOb = root.get("fpsCb",0);
//                printf("\nSize : %d\n",PosOb.size());
//                if(PosOb.size()==0)
//                {
////                    if(LangMode == 1)
////                        miscObj.DisplayWideMsgBox("Pos Ob Data not found\nPlease try again");
////                    else
////                        miscObj.tel_DisplayWideMsgBox("PosOb डेटा नहीं मिला\nकृपया पुन: प्रयास करें");
////                    return -2; //commented -BSK.
//                }
//                else
//                {
//                    query.clear();
//                    if(!query.exec("delete from Pos_Ob"))
//                    {
//                        //qDebug() << "Pos_Ob Delete Table  Error:" << query.lastError();
//                        //DataDownloadError();
//                        //return -1;
//                    }
//                    char FloatData[10];
//                    for(int i=0;i<PosOb.size();i++)
//                    {
//                        qDebug()<<"--------"<<Convertstr(PosOb[i]["commCode"].asString());
//                        qDebug()<<"--------"<<Convertstr(PosOb[i]["commNameEn"].asString());

//                        query.clear();
//                        query.prepare("insert into Pos_Ob(commCode,commNameEn,commNameLl,closingBalance) VALUES(:commCode,:commNameEn,:commNameLl,:closingBalance)");
//                        query.bindValue(":commCode",Convertstr(PosOb[i]["commCode"].asString()));
//                        query.bindValue(":commNameEn",Convertstr(PosOb[i]["commNameEn"].asString()));
//                        query.bindValue(":commNameLl",QString::fromUtf8(Convertstr(PosOb[i]["commNameLl"].asString())));
//                        memset(FloatData,0x00,sizeof(FloatData));
//                        sprintf(FloatData,"%0.3f",QString(Convertstr(PosOb[i]["closingBalance"].asString())).toFloat());
//                        query.bindValue(":closingBalance",FloatData);
//                        if(!query.exec())
//                        {
//                            if(LangMode == 1)
//                                miscObj.DisplayWideMsgBox("Invalid PosOb Data found\nPlease try again");
//                            else
//                                miscObj.tel_DisplayWideMsgBox("अमान्य PosOb डेटा\nकृपया पुन: प्रयास करें");
//                            qDebug() << query.lastError();
////                            DataDownloadError();
//                            return -3;
//                        }
//                    }
//                }

//                Json::Value UploadedData;
//                UploadedData.clear();
//                UploadedData = root.get("updatedReceipts",0);
//                printf("\nSize : %d\n",UploadedData.size());
//                if(UploadedData.size()==0)
//                {
//                    miscObj.DisplayWideMsgBox((char *)"UpdatedReceipts Id not sent From Server for Txn Upload");
//                    return -1;
//                }
//                else
//                {
//                    for(int i=0;i<UploadedData.size();i++)
//                    {
//                        query.clear();
//                        memset(TmpStr,0x00,sizeof(TmpStr));
//                        sprintf(TmpStr,"update BenfiaryTxn set TxnUploadSts = 'Y' where RecptId='%s' and RcId='%s' and CommCode='%s'",Convertstr(UploadedData[i]["receiptId"].asString()),Convertstr(UploadedData[i]["rcId"].asString()),Convertstr(UploadedData[i]["commCode"].asString()));
//                        qDebug()<<"TmpStr Update Status:"<<TmpStr;
//                        if(!query.exec(TmpStr))
//                        {
//                            return -1;
//                        }
//                    }
//                }
//            }
//            else
//            {
//                memset(TmpStr,0x00,sizeof(TmpStr));
//                sprintf(TmpStr,"%s\nError Code :%s",Convertstr(root["respMessage"].asString()),Convertstr(root["respCode"].asString()));
//                miscObj.DisplayWideMsgBox((char *)TmpStr);
//                return -1;
//            }
//        }
//    }
//    else
//    {
//        memset(TmpStr,0x00,sizeof(TmpStr));
//        sprintf(TmpStr,"Unable to upload.Please check connectivity\nPlease Try again\nError Code:%d",RetVal);
//        miscObj.DisplayWideMsgBox((char *)TmpStr);
//        return -1;
//    }
//    return 0;
//}

int Widget::CheckingPofflineTrans()
{
    int uploadingRecords =0,totalRecords=0,pendingRecords=0;
    char TmpStr[500];
    uploadingRecords=0;
    totalRecords=0;
    pendingRecords=0;

    QSqlQuery query;

    query.clear();
    memset(TmpStr,0x00,sizeof(TmpStr));
    query.clear();
    sprintf(TmpStr,"select count(*) from BenfiaryTxn where TxnType IN('P','Q') and RcId = '%s' and TxnUploadSts = 'N'",ui->ldtRationCard->text().toAscii().data());
    qDebug()<<"------------>"<<TmpStr;
    query.exec(TmpStr);
    query.next();
    totalRecords = query.value(0).toString().toInt(0);

    qDebug()<<"In CheckingPofflineTrans";
    Json::Value UploadData,CommWiseData;

    Json::Value ArrayList(Json::arrayValue);
    ArrayList.clear();
    UploadData.clear();
    char FpsId[30];
    memset(FpsId,0x00,sizeof(FpsId));
    QSqlQuery query1;
    query1.prepare("SELECT ShopID FROM  ShopOwnerDetails");
    if(!query1.exec())
        qDebug() << "SELECT ShopID FROM ShopOwnerDetails = " << query1.lastError();
    query1.next();
    //getShopId(FpsId);

    QSqlQuery query2;
    QString AllotmentMonth,AllotmentYear;
    query2.prepare("SELECT AllotMonth, AllotYear FROM PartialOnlineData");
    if(!query2.exec())
        qDebug() << query2.lastError();
    else
    {
        while(query2.next())
        {
            AllotmentMonth = query2.value(0).toString();
            AllotmentYear  = query2.value(1).toString();
        }
    }

    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));

    CMisc miscObj;
    miscObj.GetMachineId(cmachineID);
    query.clear();
    query.prepare("select fpsSessionId from Dealer");
    query.exec();
    query.next();

    UploadData["fpsId"]= query1.value(0).toString().toAscii().data();
    UploadData["token"]= RECEIVEGOODS_TOKEN;
    UploadData["stateCode"] = stateCode.toStdString().c_str();
    UploadData["sessionId"]=query.value(0).toString().toAscii().data();//"0";
    UploadData["terminalId"]=cmachineID;
    UploadData["distributionMonth"]=AllotmentMonth.toAscii().data();
    UploadData["distributionYear"]=AllotmentYear.toAscii().data();

    qDebug()<<"totalRecords---->"<<totalRecords;
    if(totalRecords > 0)
    {
        memset(TmpStr,0x00,sizeof(TmpStr));
        sprintf(TmpStr,"select RcId,CommCode,TotQty,BalQty,SchemeId,IssuedQty,RecptId,commAmount,TotAmt,Rate,MemberName,DateTime,TxnType,AllotMonth,AllotYear,allocationType from BenfiaryTxn where TxnUploadSts = 'N' and RcId = '%s'",ui->ldtRationCard->text().toAscii().data());

        if(!query.exec(TmpStr))
        {
            qDebug()<<"Upoad Query Error:"<<query.lastError();
            return -1;
        }
        while(query.next())
        {
            CommWiseData.clear();
            CommWiseData["rcId"]=query.value(0).toString().toAscii().data();
            CommWiseData["commCode"]=query.value(1).toString().toAscii().data();
            CommWiseData["totalEntitlement"]=query.value(2).toString().toAscii().data();
            CommWiseData["balanceEntitlement"]=query.value(3).toString().toAscii().data();
            CommWiseData["schemeId"]=query.value(4).toString().toAscii().data();
            CommWiseData["issueQty"]=query.value(5).toString().toAscii().data();
            CommWiseData["receiptId"]=query.value(6).toString().toAscii().data();
            CommWiseData["commAmount"]=query.value(7).toString().toAscii().data();
            CommWiseData["totalAmount"]=query.value(8).toString().toAscii().data();
            CommWiseData["commPrice"]=query.value(9).toString().toAscii().data();
            CommWiseData["headOfTheFamily"]=query.value(10).toString().toAscii().data();
            CommWiseData["transactionTime"]=query.value(11).toString().toAscii().data();
            CommWiseData["transMode"]=query.value(12).toString().toAscii().data();
            CommWiseData["allotedMonth"]=query.value(13).toString().toAscii().data();
            CommWiseData["allotedYear"]=query.value(14).toString().toAscii().data();
            CommWiseData["allocationType"]=query.value(15).toString().toAscii().data();
            ArrayList.append(CommWiseData);
            uploadingRecords++;
        }
    }

    UploadData["fpsOfflineTransResponses"]=ArrayList;
    query.prepare("select commCode,closingBalance from Pos_Ob");
    query.exec();

    ArrayList.clear();
    while(query.next())
    {
        CommWiseData.clear();
        CommWiseData["commCode"]=query.value(0).toString().toAscii().data();
        CommWiseData["closingBalance"]=query.value(1).toString().toAscii().data();
        ArrayList.append(CommWiseData);
    }
    UploadData["fpsCbs"]=ArrayList;
    char *Ret="";
    qDebug()<<"\n\n\nUploadData Size:"<<UploadData["fpsOfflineTransResponses"].size()<<"\n\n\n";

    /////////////////////////////////////////////////////////////
    totalRecords=0;
    query.clear();
    query.exec("select count(*) from BenfiaryTxn where TxnType IN('P','Q')");
    query.next();
    totalRecords = query.value(0).toString().toInt(0);
    UploadData["totalRecords"]=totalRecords;
    UploadData["uploadingRecords"]=uploadingRecords;

    query.clear();
    query.exec("select count(*) from BenfiaryTxn where TxnType IN('P','Q') and TxnUploadSts ='N'");
    query.next();
    pendingRecords = query.value(0).toString().toInt(0);

    if(uploadingRecords < pendingRecords)
        UploadData["pendingRecords"]=(pendingRecords-uploadingRecords);
    else
        UploadData["pendingRecords"]=(uploadingRecords-pendingRecords);

    /////////////////////////////////////////////////////////////
    Ret = Convertstr(UploadData.toStyledString());

    char URL[125];
    memset(URL,0x00,sizeof(URL));
//    sprintf(&URL[0],"%s/pushFpsOfflineData",OFFLINE_AND_RECEIVE_STOCK_URL);
    sprintf(&URL[0],"%spushFpsOfflineData",Offline_and_ReceiveStock_URL.toAscii().data());

    int RetVal = CurlRequest(URL,Ret,300);

    if(RetVal==0)
    {
        Json::Value root;
        root.clear();
        //QSqlQuery query;
        Json::Reader reader;
        system("cat /tmp/Sample.txt");
        std::ifstream ReadData("/tmp/Sample.txt");
        bool parsedSuccess = reader.parse(ReadData,root,false);
        ReadData.close();
        if(not parsedSuccess)
        {
            miscObj.DisplayWideMsgBox((char *)"Invalid Response From Server for Txn Upload");
            return -1;
        }
        else
        {
            printf("\nrespCode    : %s\n",Convertstr(root["respCode"].asString()));
            printf("\nrespMessage : %s\n",Convertstr(root["respMessage"].asString()));
            if(atoi(Convertstr(root["respCode"].asString()))==0)
            {
                if(UploadData["fpsOfflineTransResponses"].size() !=0)
                {
                    Json::Value UploadedData;
                    UploadedData.clear();
                    UploadedData = root.get("updatedReceipts",0);
                    printf("\nSize : %d\n",UploadedData.size());
                    if(UploadedData.size()==0)
                    {
                        miscObj.DisplayWideMsgBox((char *)"UpdatedReceipts Id not sent From Server for Txn Upload");
                        return -1;
                    }
                    else
                    {
                        for(int i=0;i<UploadedData.size();i++)
                        {
                            query.clear();
                            memset(TmpStr,0x00,sizeof(TmpStr));
                            sprintf(TmpStr,"update BenfiaryTxn set TxnUploadSts = 'Y' where RecptId='%s' and RcId='%s' and CommCode='%s'",Convertstr(UploadedData[i]["receiptId"].asString()),Convertstr(UploadedData[i]["rcId"].asString()),Convertstr(UploadedData[i]["commCode"].asString()));
                            qDebug()<<"TmpStr Update Status:"<<TmpStr;
                            if(!query.exec(TmpStr))
                            {
                                return -1;
                            }
                        }
                    }
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"%s\nError Code :%s",Convertstr(root["respMessage"].asString()),Convertstr(root["respCode"].asString()));
                miscObj.DisplayWideMsgBox((char *)TmpStr);
                return -1;
            }
        }
    }
    else
    {
        memset(TmpStr,0x00,sizeof(TmpStr));
        sprintf(TmpStr,"Unable to upload.Please check connectivity\nPlease Try again\nError Code:%d",RetVal);
        miscObj.DisplayWideMsgBox((char *)TmpStr);
        return -1;
    }

    return 0;
}

int Widget::rtcDateValidation()
{
    int extentionYear,allotYear;
    int currentYear = QDateTime::currentDateTime().toString("yyyy").toInt(0);

    qDebug()<<"currentYear --"<<currentYear;
    if((currentYear > 2018) && (currentYear <2038))
    {
        if(DealerLoginBy == 2){
            QSqlQuery query;
            query.clear();
            if(!query.exec("select pOfflineStoppedDate, AllotYear from PartialOnlineData")){
                return -1;
            }
            query.next();
            try{
                string Str;
                Str.append(query.value(0).toString().toAscii().data());
                qDebug()<<"pOfflineStoppedDate --"<<query.value(0).toString().toAscii().data();

                qDebug()<<"atoi(Str.substr(9,2).c_str(): "<<atoi(Str.substr(8,2).c_str());
                qDebug()<<"atoi(Str.substr(5,2).c_str(): "<<atoi(Str.substr(5,2).c_str());
                qDebug()<<"atoi(Str.substr(1,4).c_str(): "<<atoi(Str.substr(0,4).c_str());

                extentionYear = atoi(Str.substr(0,4).c_str());

                allotYear     = query.value(1).toString().toInt(0);
                qDebug()<<"allotYear --"<<allotYear;
            }catch(exception e){
                qDebug()<<"Exception;"<<e.what();
            }

            if((currentYear == allotYear) || (currentYear == extentionYear)){
                qDebug()<< "we are in same year(offline)";
            }else{
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox((char *)"Please Check Date & Time");
                else if(LangMode == 2)
                    miscObj.tel_DisplayWideMsgBox((char *)"कृपया दिनांक और समय जांचें");
                return -1;
            }
        }
    }else{
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please Check Date & Time");
        else if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox((char *)"कृपया दिनांक और समय जांचें");
        return -1;
    }
    return 0;
}

int Widget::GetPrintPaperSts(QString &Out)
{
    QFile file("/mnt/jffs2/Flags.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    QTextStream in(&file);
    QString line = in.readLine();
    Out = line.trimmed();
    qDebug() <<"Print Paper Sts Flag frm file:"<<Out;

    return 0;
}

int Widget::ForwardDistrubutionValidation(int row, int column){
    float ItemPrice =0.00;
    char szItemPrice[32];

    float cbValidation = 0.0;
    if(ui->tblCommIssue->item(row, 3)->text().toFloat() > 0){
        QString commonCommCode =  ui->tblCommIssue->item(row, 10)->text();

        qDebug()<< "current commonCommCode  --- "<< commonCommCode;
        qDebug()<< " row Count --- "<< ui->tblCommIssue->rowCount();

        for(int i=0; i< ui->tblCommIssue->rowCount(); i++){
            qDebug()<< "commonCommCode      --- "<< ui->tblCommIssue->item(i, 10)->text();
            if(ui->tblCommIssue->item(i, 10)->text() == commonCommCode)
                cbValidation += atof(ui->tblCommIssue->item(i, 3)->text().toAscii().data());
        }
        qDebug()<< "Total Issue Qty  --- "<<cbValidation;
        qDebug()<< "CB               --- "<<ui->tblCommIssue->item(row, 6)->text().toFloat(0);
        if(cbValidation > ui->tblCommIssue->item(row, 6)->text().toFloat(0)){
            miscObj.DisplayWideMsgBox((char *)"Closing Balance Exceeded"); //hindi required

            if((ui->tblCommIssue->item(row, 11)->text() == "NA") || (ui->tblCommIssue->item(row, 11)->text() == "0")){
                ui->tblCommIssue->item(row,3)->setText("0");
            }else{
                for(int j = ui->tblCommIssue->rowCount() - 1; j > ui->tblCommIssue->currentRow() - 1; j--){
                    if(ui->tblCommIssue->item(j, 10)->text() == commonCommCode){
                        ui->tblCommIssue->item(j,3)->setText("0");
                    }
                }
            }
            return -1;
        }else{
            qDebug(" we are in less cb  validation ");
            qDebug()<< "current issue Qty --- "<<ui->tblCommIssue->item(row, 3)->text().toFloat(0);

            if((row+1) < ui->tblCommIssue->rowCount()){ //Added by Santosh.B
                qDebug()<< "Next issue Qty --- "<<ui->tblCommIssue->item(row+1, 3)->text().toFloat(0);
                if(ui->tblCommIssue->item(row, 10)->text() == ui->tblCommIssue->item(row+1, 10)->text()){
                    if(ui->tblCommIssue->item(row+1,3)->text().toFloat(0) > 0){
                        if((ui->tblCommIssue->item(row, 1)->text().toFloat(0) !=  ui->tblCommIssue->item(row, 3)->text().toFloat(0))){
                            ItemPrice = 0.00;
                            ItemPrice = atof(ui->tblCommIssue->item(row, column-1)->text().toAscii().data() ) * atof(ui->tblCommIssue->item(row, column)->text().toAscii().data());
                            memset(szItemPrice, 0, sizeof(szItemPrice));
                            sprintf(szItemPrice, "%0.2f", ItemPrice);
                            ui->tblCommIssue->setItem(row, 4, new QTableWidgetItem(szItemPrice));
                            ui->tblCommIssue->item(row, 4)->setFlags( ui->tblCommIssue->item(row,4)->flags() ^ Qt::ItemIsEditable);
                            ui->tblCommIssue->item(row, 4)->setFlags(ui->tblCommIssue->item(row, 4)->flags() ^ Qt::ItemIsSelectable);

                            for(int j = ui->tblCommIssue->rowCount() - 1; j > ui->tblCommIssue->currentRow(); j--){
                                if(ui->tblCommIssue->item(j, 10)->text() == commonCommCode){
                                    if(ui->tblCommIssue->item(j,3)->text().toFloat(0) > 0){
                                        ui->tblCommIssue->item(j,3)->setText("0");
                                    }
                                }
                            }
                            return -1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void Widget::on_GetCardStsBtn_clicked()
{
    if(ui->ldtRationCard->text().isEmpty())
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Please Enter RC Number");
        else
            miscObj.tel_DisplayMsgBox((char *)"कृपया कार्ड नंबर दर्ज करें.");

        ui->ldtRationCard->clear();
        return;
    }
    QSqlQuery query;
    bool IsValid = false;
    int row = 0;
    QList<QString> RcDetails;
    QVector< QList<QString> > RcDetailsVec;
    RcDetails.clear();

    qDebug() << "on_GetCardStsBtn_clicked";

    QString QueryStr;
//    QString StrTmp;
    QueryStr = "select commNameEn,totalEntitlement,balanceEntitlement,Unit from KeyRegister where rcID = '" +ui->ldtRationCard->text() +"'";

    if(!query.exec(QueryStr))
    {
        qDebug()<<"Upoad Query Error:"<<query.lastError();

        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"No Data found");
        else
            miscObj.tel_DisplayMsgBox((char *)"कोई डेटा नहीं मिला.");

        ui->ldtRationCard->clear();
        return;
    }

    for(int k=ui->RcDetailsOfflineTable->rowCount()-1;k>=0;k--)
        ui->RcDetailsOfflineTable->removeRow(k);

    while(query.next())
    {
        RcDetails.push_back(query.value(0).toString());
        RcDetails.push_back(query.value(2).toString());
        RcDetails.push_back(query.value(3).toString());

        RcDetailsVec.push_back(RcDetails);
        RcDetails.clear();

        IsValid = true;

        ui->RcDetailsOfflineTable->insertRow(row);

        ui->RcDetailsOfflineTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->RcDetailsOfflineTable->item(row, 0)->setFlags( ui->RcDetailsOfflineTable->item(row,0)->flags() ^ Qt::ItemIsEditable);
        ui->RcDetailsOfflineTable->item(row, 0)->setFlags(ui->RcDetailsOfflineTable->item(row, 0)->flags() | Qt::ItemIsSelectable);

        ui->RcDetailsOfflineTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->RcDetailsOfflineTable->item(row, 1)->setFlags( ui->RcDetailsOfflineTable->item(row,1)->flags() ^ Qt::ItemIsEditable);
        ui->RcDetailsOfflineTable->item(row, 1)->setFlags(ui->RcDetailsOfflineTable->item(row, 1)->flags() | Qt::ItemIsSelectable);

        ui->RcDetailsOfflineTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        ui->RcDetailsOfflineTable->item(row, 2)->setFlags( ui->RcDetailsOfflineTable->item(row,2)->flags() ^ Qt::ItemIsEditable);
        ui->RcDetailsOfflineTable->item(row, 2)->setFlags(ui->RcDetailsOfflineTable->item(row, 2)->flags() | Qt::ItemIsSelectable);

        ui->RcDetailsOfflineTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        ui->RcDetailsOfflineTable->item(row, 3)->setFlags( ui->RcDetailsOfflineTable->item(row,3)->flags() ^ Qt::ItemIsEditable);
        ui->RcDetailsOfflineTable->item(row, 3)->setFlags(ui->RcDetailsOfflineTable->item(row, 3)->flags() | Qt::ItemIsSelectable);

        row++;
    }

    if(!IsValid)
    {
        qDebug()<<"No Data found";

        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"No Data found");
        else
            miscObj.tel_DisplayMsgBox((char *)"कोई डेटा नहीं मिला.");

        ui->ldtRationCard->clear();
        return;
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(8);
        ui->RcDetailsCardNoLbl_2->setText(ui->ldtRationCard->text());
    }

//    StrTmp.clear();
//    StrTmp = "Balance Entitlements\n";

//    for(int i = 0 ;  i < RcDetailsVec.size() ; i++)
//    {
//        RcDetails = RcDetailsVec[i];
//        QString Str = RcDetails[0] + " - " +RcDetails[1]+ "(" + RcDetails[2] + ")\n";
//        StrTmp.append(Str);
//        RcDetails.clear();
//    }

//    miscObj.DisplayWideMsgBox(StrTmp.toAscii().data());
}

void Widget::on_BackFrmRcDetails_clicked()
{
//    ui->RcDetailsCardNoLbl_2->clear();
    ui->stackedWidget->setCurrentIndex(4);
}

int Widget::CBOflineDownload()
{
    QSqlQuery query;
    if(fpsCbDownloadStatus.toUpper() == "Y"){
        query.clear();
        if(!query.exec("select (select count() from BenfiaryTxn) as Total_Txn,(select count() from BenfiaryTxn where TxnUploadSts='Y' and TxnType = 'O') as Online_Txn,(select count() from BenfiaryTxn where TxnType ='P' or TxnType ='Q') as PartialOfline_Txn,(select count() from BenfiaryTxn where  TxnUploadSts='Y' and TxnType IN('P','Q')) as Upload_Txn,(select count() from BenfiaryTxn where TxnUploadSts='N' and TxnType IN('P','Q'))as PendingOffline_Txn"))
        {
            qDebug()<<"Records Count query Failure";
        }
        if(query.next())
        {
            char TmpStr[1024];
            memset(TmpStr,0x00,sizeof(TmpStr));
            sprintf(TmpStr,"Total Txn Records : %d\nOnline Txn Records:%d\nOffline Txn Records:%d\nUploaded Records:%d\nPending Records:%d",query.value(0).toInt(),query.value(1).toInt(),query.value(2).toInt(),query.value(3).toInt(),query.value(4).toInt());
            miscObj.DisplayWideMsgBox((char *)TmpStr);
        }
        query.clear();
        query.prepare("select count() from OfflineStockReceive where RecvdUploadSts='N'");
        if(!query.exec()){
            qDebug()<<"Txn not uploaded";
        }
        if(query.next())
        {
            if(query.value(0).toString().toInt()>0)
            {
                // uploading Offline Receive Goods
                popup_msg *dlgPopup2 = new popup_msg(this);
                dlgPopup2->ui->lblPopup->setText("Uploading Offline Receive Goods \n Please Wait ....");
                dlgPopup2->show();
                QCoreApplication::processEvents();
                QCoreApplication::processEvents();
                int RetVal = ManualReceiveGdsUpload();
                dlgPopup2->close();
                dlgPopup2->deleteLater();
                if(RetVal != 0)
                    return -1;
            }
        }
        query.clear();
        query.prepare("select count() from BenfiaryTxn where TxnUploadSts = ?");
        query.addBindValue("N");
        if(!query.exec()){
            qDebug()<<"Txn not uploaded";
        }
        if(query.next())
        {
            if(query.value(0).toString().toInt()>0)
            {
                popup_msg *dlgPopup2 = new popup_msg(this);
                dlgPopup2->ui->lblPopup->setText("Uploading Pending Records \n Please Wait ....");
                dlgPopup2->show();
                QCoreApplication::processEvents();
                QCoreApplication::processEvents();
                int RetVal = ManualServerUploadPartialTxns();
                dlgPopup2->close();
                dlgPopup2->deleteLater();
                if(RetVal == 0)
                {
                    int ret = UpdateTransStatus();
                    if(ret == 0){
                        if(LangMode == 1)
                            miscObj.DisplayWideMsgBox((char *)"All offline txn records were uploaded to server");
                        else
                            miscObj.tel_DisplayWideMsgBox((char *)"सभी ऑफ़लाइन txn रिकॉर्ड सर्वर पर अपलोड किए गए थे");
                    }else
                        return -1;
                }else{
                    int ret = UpdateTransStatus();
                    if(ret == 0){
                        if(LangMode == 1)
                            miscObj.DisplayWideMsgBox((char *)"All offline txn records were uploaded to server");
                        else
                            miscObj.tel_DisplayWideMsgBox((char *)"सभी ऑफ़लाइन txn रिकॉर्ड सर्वर पर अपलोड किए गए थे");
                    }else
                        return -1;

                    if(PartialTxnDeleteFlag.toUpper() == "Y"){

                    }else{
                        return -1;
                    }
                }
            }

            gl11_lcdbklt_on();
            PopMsg = new popup_msg(this);
            if(LangMode == ENGLISH)
                PopMsg->ui->lblPopup->setText("Downloading Offline CB Data\n      Please Wait ..");
            else if(LangMode == HINDI)
                PopMsg->ui->lblPopup->setText(trUtf8("ऑफ़लाइन CB डाउनलोड कर रहा है \n कृपया प्रतीक्षा करें .."));

            PopMsg->show();
            QCoreApplication::processEvents();
            QCoreApplication::processEvents();

            Json::Value RequestedData;
            char FpsId[30];
            memset(FpsId,0x00,sizeof(FpsId));
            getShopId(FpsId);

            char cmachineID[16];
            memset(cmachineID, 0, sizeof(cmachineID));
            miscObj.GetMachineId(cmachineID);

            QString AllotmentMonth,AllotmentYear;
            query.prepare("SELECT AllotMonth, AllotYear FROM PartialOnlineData");
            if(!query.exec())
                qDebug() << query.lastError();
            else{
                while(query.next()){
                    AllotmentMonth = query.value(0).toString();
                    AllotmentYear  = query.value(1).toString();
                }
            }

            query.clear();
            query.prepare("select fpsSessionId from Dealer");
            query.exec();
            query.next();

            RequestedData["fpsId"] = FpsId;
            RequestedData["stateCode"] = stateCode.toStdString().c_str();
            RequestedData["token"] = RECEIVEGOODS_TOKEN;
            RequestedData["sessionId"] =query.value(0).toString().toAscii().data();//"0";
            RequestedData["terminalId"]=cmachineID;
            RequestedData["allocationMonth"]=AllotmentMonth.toAscii().data();
            RequestedData["allocationYear"]=AllotmentYear.toAscii().data();

            char *Ret=Convertstr(RequestedData.toStyledString());
            QString URL = Offline_and_ReceiveStock_URL + "getCbUpdate";

            int RetVal = CurlRequest((char*)URL.toAscii().data(),Ret,300);
            if(RetVal!=0)
            {
                PopMsg->close();
                PopMsg->deleteLater();
                return -1;
            }
            PopMsg->close();
            PopMsg->deleteLater();

            query.clear();
            if(!query.exec("drop table Pos_Ob"))
            {
                qDebug() << "Pos_Ob Drop Table Error:" << query.lastError();
                system("sqlite3 /mnt/jffs2/db_commonpds_2_3.db \"drop table Pos_Ob\"");
                sync();
                sync();
                //DataDownloadError();
                //return -1;
            }

            query.clear();
            if(!query.exec("delete from Pos_Ob"))
            {
                qDebug() << "Pos_Ob delete Table Error:" << query.lastError();
            }

            query.clear();
            if(!query.exec("CREATE TABLE IF NOT EXISTS Pos_Ob(commCode text,commNameEn text,commNameLl text,closingBalance text)"))
            {
                qDebug() << "KeyRegister Table  Error:" << query.lastError();
                //DataDownloadError();
                //return -1;
            }

            system("cat /tmp/Sample.txt");
            system("cp /tmp/Sample.txt /tmp/Sample1.txt");
            Json::Value root;
            root.clear();
            //QSqlQuery query;
            Json::Reader reader;
            std::ifstream ReadData("/tmp/Sample.txt");
            bool parsedSuccess = reader.parse(ReadData,root,false);
            ReadData.close();
            if(not parsedSuccess){
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox("Invalid Response from Server\nPlease try again");
                else
                    miscObj.tel_DisplayWideMsgBox("सर्वर से अमान्य प्रतिक्रिया\nकृपया पुन: प्रयास करें");
                DataDownloadError();
                return -1;
            }else{
                printf("\n respCode      : %s\n",Convertstr(root["respCode"].asString()));
                printf("\n respMessage   : %s\n",Convertstr(root["respMessage"].asString()));
                printf("\n allocationMonth: %s\n",Convertstr(root["allocationMonth"].asString()));
                printf("\n allocationYear: %s\n",Convertstr(root["allocationYear"].asString()));

                if(atoi(Convertstr(root["respCode"].asString()))==0)
                {
                    Json::Value PosOb;
                    PosOb.clear();
                    PosOb = root.get("fpsCbs",0);
                    printf("\nSize : %d\n",PosOb.size());
                    if(PosOb.size()==0){
                        if(LangMode == 1)
                            miscObj.DisplayWideMsgBox("Pos Ob Data not found\nPlease try again");
                        else
                            miscObj.tel_DisplayWideMsgBox("PosOb डेटा नहीं मिला\nकृपया पुन: प्रयास करें");
                        DataDownloadError();
                        return -2;
                    }else{
                        char FloatData[10];
                        for(int i=0;i<PosOb.size();i++)
                        {
                            query.clear();
                            query.prepare("insert into Pos_Ob(commCode,commNameEn,commNameLl,closingBalance) VALUES(:commCode,:commNameEn,:commNameLl,:closingBalance)");
                            query.bindValue(":commCode",Convertstr(PosOb[i]["commCode"].asString()));
                            query.bindValue(":commNameEn",Convertstr(PosOb[i]["commNameEn"].asString()));
                            query.bindValue(":commNameLl",QString::fromUtf8(Convertstr(PosOb[i]["commNameLl"].asString())));
                            memset(FloatData,0x00,sizeof(FloatData));
                            sprintf(FloatData,"%0.3f",QString(Convertstr(PosOb[i]["closingBalance"].asString())).toFloat());
                            query.bindValue(":closingBalance",FloatData);
                            if(!query.exec())
                            {
                                if(LangMode == 1)
                                    miscObj.DisplayWideMsgBox("Invalid PosOb Data found\nPlease try again");
                                else
                                    miscObj.tel_DisplayWideMsgBox("अमान्य PosOb डेटा\nकृपया पुन: प्रयास करें");
                                qDebug() << query.lastError();
                                DataDownloadError();
                                return -3;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void Widget::HideConsent()
{
    ui->BtnConsentForm->hide();
    ui->BtnConsentForm_2->hide();
    ui->BtnConsentForm_4->hide();
    ui->BtnConsentForm_5->hide();
    ui->BtnConsentForm_6->hide();
    ui->BtnConsentForm_7->hide();
    ui->BtnConsentForm_8->hide();

    ui->ConsentFormcheckBox->hide();
    ui->ConsentFormcheckBox_3->hide();
    ui->ConsentFormcheckBox_4->hide();
    ui->ConsentFormcheckBox_5->hide();
    ui->ConsentFormcheckBox_6->hide();
    ui->ConsentFormcheckBox_7->hide();
    ui->ConsentFormcheckBox_8->hide();
}
