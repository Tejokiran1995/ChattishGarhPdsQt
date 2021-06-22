#include "ui_popup_msg.h"
#include"ui_widget.h"
#include<widget.h>
#include<popup_msg.h>
#include<QDateTime>
#include<stdio.h>
#include<stdlib.h>
popup_msg *PopMsg;
QTimer *PartialOnlineTimer;
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
int CurlRequest(char *Url,char *Data)
{
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
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120);
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
    PopMsg->ui->lblPopup->setText("Offline Data Downloading Failed\n      Please Try again ..");
    PopMsg->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    sleep(3);
    PopMsg->close();
    PopMsg->deleteLater();
}

int Widget::DwnldPartialOnlineData()
{
        gl11_lcdbklt_on();

        PopMsg = new popup_msg(this);
        PopMsg->ui->lblPopup->setText("Downloading Offline Data\n      Please Wait ..");
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
        query.prepare("select fps_seesion_Id from Dealer");
        query.exec();query.next();

        RequestedData["fpsId"] = FpsId;
        RequestedData["token"] = "f55725bfae061106da03ce754dfcc41c";

        RequestedData["sessionId"] =query.value(0).toString().toAscii().data();//"0";
        RequestedData["terminalId"]=cmachineID;
        char *Ret=Convertstr(RequestedData.toStyledString());

        int RetVal = CurlRequest("http://164.100.132.87/ePosCommonServiceBR/eposCommon/getFpsOfflineData",Ret);
        if(RetVal!=0)
                return -1;

        query.clear();
        if(!query.exec("delete from KeyRegister"))
        {
            qDebug() << "KeyRegister Delete Table  Error:" << query.lastError();
            //DataDownloadError();
            //return -1;
        }
        query.clear();
        if(!query.exec("delete from Pos_Ob"))
        {
            //qDebug() << "Pos_Ob Delete Table  Error:" << query.lastError();
            //DataDownloadError();
            //return -1;
        }
        query.clear();
        if(!query.exec("CREATE TABLE IF NOT EXISTS KeyRegister(rcId text,commNameEn text,commNameLl text,commCode text,totalEntitlement text,balanceEntitlement text,schemeId text,schemeName text,commPrice text,Unit text,memberNameLl text,memberNameEn text)"))
        {
            qDebug() << "KeyRegister Table  Error:" << query.lastError();
            //DataDownloadError();
            //return -1;
        }
        query.clear();
        if(!query.exec("CREATE TABLE IF NOT EXISTS Pos_Ob(commCode text,commNameEn text,commNameLl text,closingBalance text)"))
        {
            qDebug() << "KeyRegister Table  Error:" << query.lastError();
            //DataDownloadError();
            //return -1;
        }        
        query.clear();
        if(!query.exec("CREATE TABLE IF NOT EXISTS BenfiaryTxn(FpsId text,RcId text,SchemeId text,CommCode text,TotQty text,BalQty text,IssuedQty text,Rate text,commAmount text,TotAmt text,RecptId text,MemberName text,DateTime text,TxnUploadSts text)"))
        {
            qDebug() << "BenfiaryTxn Table  Error:" << query.lastError();
            //DataDownloadError();
            //return -1;
        }
        Json::Value root;
        root.clear();
        //QSqlQuery query;
        Json::Reader reader;
        std::ifstream ReadData("/tmp/Sample.txt");
        bool parsedSuccess = reader.parse(ReadData,root,false);
        ReadData.close();
        if(not parsedSuccess)
        {
            qDebug()<<"FpsMonthlyPdsData File not Found";
            DataDownloadError();
            return -1;
        }
        else
        {
            printf("\nrespCode    : %s\n",Convertstr(root["respCode"].asString()));
            printf("\nrespMessage : %s\n",Convertstr(root["respMessage"].asString()));

            Json::Value KeyRegisterData;
            KeyRegisterData.clear();
            KeyRegisterData = root.get("keyRegister",0);
            printf("\nSize : %d\n",KeyRegisterData.size());
            if(KeyRegisterData.size()==0)
            {
                qDebug("\n1:Key Register Data Not Found\n");
                DataDownloadError();
                return -2;
            }
            else
            {
                    for(int i=0;i<KeyRegisterData.size();i++)
                    {
                            query.clear();
                            query.prepare("insert into KeyRegister(rcId,commNameEn,commNameLl,commCode,totalEntitlement,balanceEntitlement,schemeId,schemeName,commPrice,Unit,memberNameLl,memberNameEn) VALUES(:rcId,:commNameEn,:commNameLl,:commCode,:totalEntitlement,:balanceEntitlement,:schemeId,:schemeName,:commPrice,:Unit,:memberNameLl,:memberNameEn)");
                            query.bindValue(":rcId",Convertstr(KeyRegisterData[i]["rcId"].asString()));
                            query.bindValue(":commNameEn",Convertstr(KeyRegisterData[i]["commNameEn"].asString()));
                            query.bindValue(":commNameLl",QString::fromUtf8(Convertstr(KeyRegisterData[i]["commNameLl"].asString())));
                            query.bindValue(":commCode",Convertstr(KeyRegisterData[i]["commCode"].asString()));
                            query.bindValue(":totalEntitlement",QString(Convertstr(KeyRegisterData[i]["totalEntitlement"].asString())).toFloat());
                            query.bindValue(":balanceEntitlement",Convertstr(KeyRegisterData[i]["balanceEntitlement"].asString()));
                            query.bindValue(":schemeId",Convertstr(KeyRegisterData[i]["schemeId"].asString()));
                            query.bindValue(":schemeName",Convertstr(KeyRegisterData[i]["schemeName"].asString()));
                            query.bindValue(":commPrice",Convertstr(KeyRegisterData[i]["commPrice"].asString()));
                            query.bindValue(":Unit",Convertstr(KeyRegisterData[i]["measurmentUnit"].asString()));
                            query.bindValue(":memberNameLl",Convertstr(KeyRegisterData[i]["memberNameLl"].asString()));
                            query.bindValue(":memberNameEn",Convertstr(KeyRegisterData[i]["memberNameEn"].asString()));
                            if(!query.exec())
                            {
                                qDebug()<<"Insert Key Register Data";
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
            if(PosOb.size()==0)
            {
                qDebug("\n1:PosOb Data Not Found\n");
                DataDownloadError();
                return -2;
            }
            else
            {
                    for(int i=0;i<PosOb.size();i++)
                    {
                            query.clear();
                            query.prepare("insert into Pos_Ob(commCode,commNameEn,commNameLl,closingBalance) VALUES(:commCode,:commNameEn,:commNameLl,:closingBalance)");
                            query.bindValue(":commCode",Convertstr(PosOb[i]["commCode"].asString()));
                            query.bindValue(":commNameEn",Convertstr(PosOb[i]["commNameEn"].asString()));
                            query.bindValue(":commNameLl",QString::fromUtf8(Convertstr(PosOb[i]["commNameLl"].asString())));
                            query.bindValue(":closingBalance",Convertstr(PosOb[i]["closingBalance"].asString()));
                            if(!query.exec())
                            {
                                qDebug()<<"Insert PosOb Data";
                                qDebug() << query.lastError();
                                DataDownloadError();
                                return -3;
                            }

                    }
            }
        }
        PopMsg->close();
        PopMsg->deleteLater();
        return 0;
}

int Widget::GetOfflineCommDetails()
{    
    qDebug()<<"In Partial Get comm Details";
    if(DealerLoginBy!=2)
    {
            popup_msg *dlgPopup1 = new popup_msg(this);
            dlgPopup1->ui->lblPopup->setText("No Connectivity\nGoing for Offline txn");
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
    //query.prepare("select ItemType, MonthlyQuota, ItemPrice, ItemCode, BalQty, AvailedQty, MeasureUnit, RequiresdQty, closingBal from BenQuotaTable where RationCardId=?");
    query.prepare("select a.commNameEn,a.totalEntitlement,a.commPrice,a.commCode,a.balanceEntitlement,a.Unit,b.closingBalance from KeyRegister a,Pos_Ob b where a.rcId=? and b.commCode=a.commCode");
    query.addBindValue(ui->ldtRationCard->text().toAscii().data());
    if(!query.exec())
    {
        qDebug()<<"In Partial Get comm Details Error";
        if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox("इस कार्ड के लिए वस्तु उपलब्ध नहीं है");
        else if(LangMode == 4)
            miscObj.tel_DisplayWideMsgBox("ఈ కార్డు కి వస్తువులు లేవు"); //సర్వర్ నుండి వస్తువులు వివరాలు లభించలేదు
        else
            miscObj.DisplayMsgBox("No Commodities For This Card"); //Unable to get Commodity Details From Server...

        return -1;
    }
    while(query.next())
    {
        ui->tblCommIssue->insertRow(row);
        {
                memset(buff,0,sizeof(buff));
                sprintf(buff, "%s(%s)", query.value(0).toString().toAscii().data(), query.value(1).toString().toAscii().data());

                ui->tblCommIssue->setItem(row, 0, new QTableWidgetItem(buff));
                ui->tblCommIssue->item(row, 0)->setFlags( ui->tblCommIssue->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
                ui->tblCommIssue->item(row, 0)->setFlags(ui->tblCommIssue->item(row, 0)->flags() | Qt::ItemIsSelectable);
        }
        {
                memset(buff,0,sizeof(buff));
                strcpy(buff, query.value(4).toString().toAscii().data());
                qDebug()<<"1 Bal Qty = "<<buff;

                ui->tblCommIssue->setItem(row, 1, new QTableWidgetItem(buff));
                ui->tblCommIssue->item(row, 1)->setFlags(ui->tblCommIssue->item(row, 1)->flags() ^ Qt::ItemIsEditable);
                ui->tblCommIssue->item(row, 1)->setFlags(ui->tblCommIssue->item(row, 1)->flags() ^ Qt::ItemIsSelectable);
        }
        {
                //Item Price
                memset(buff,0,sizeof(buff));
                strcpy(buff, query.value(2).toString().toAscii().data());
                qDebug()<<"2 Item Price = "<<buff;

                ui->tblCommIssue->setItem(row, 2, new QTableWidgetItem(buff));
                ui->tblCommIssue->item(row, 2)->setFlags(ui->tblCommIssue->item(row, 2)->flags() ^ Qt::ItemIsEditable);
                ui->tblCommIssue->item(row, 2)->setFlags(ui->tblCommIssue->item(row, 2)->flags() ^ Qt::ItemIsSelectable);
        }
        {
                ui->tblCommIssue->setItem(row, 3, new QTableWidgetItem("0"));
                ui->tblCommIssue->item(row, 3)->setData(Qt::BackgroundColorRole, QVariant(QColor(Qt::green)));
        }
        {
                ui->tblCommIssue->setItem(row, 4, new QTableWidgetItem("0"));                
                ui->tblCommIssue->item(row, 4)->setFlags( ui->tblCommIssue->item(row,4)->flags() ^ Qt::ItemIsEditable);
                ui->tblCommIssue->item(row, 4)->setFlags(ui->tblCommIssue->item(row, 4)->flags() ^ Qt::ItemIsSelectable);
        }
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
        ui->tblCommIssue->setRowHeight(row,20);
        row++;
    }

    if(row == 0)
    {
        if(LangMode == 2)
            miscObj.tel_DisplayWideMsgBox("इस कार्ड के लिए वस्तु उपलब्ध नहीं है");
        else if(LangMode == 4)
            miscObj.tel_DisplayWideMsgBox("ఈ కార్డు కి వస్తువులు లేవు"); //సర్వర్ నుండి వస్తువులు వివరాలు లభించలేదు
        else
            miscObj.DisplayMsgBox("No Commodities For This Card"); //Unable to get Commodity Details From Server...

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

    fTotalAmt = 0.00;
    fTotalQty = 0.00;
    ui->lblTotalAmtVal->setText("0.00");
    ui->label_LapTime_4->clear();
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
    int SLEEPTIME=200;
    char TmpStr[500];
    while(1)
    {
            qDebug()<<"In ServerUploadPartialTxns ";
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
                    //sleep(SLEEPTIME);
                    //continue ;
            }
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
            query.prepare("select fps_seesion_Id from Dealer");
            query.exec();query.next();

            UploadData["fpsId"]= query1.value(0).toString().toAscii().data();
            UploadData["token"]= "f55725bfae061106da03ce754dfcc41c";
            UploadData["sessionId"]=query.value(0).toString().toAscii().data();//"0";
            UploadData["terminalId"]=cmachineID;
            query.clear();
            query.prepare("select RcId,CommCode,TotQty,BalQty,SchemeId,IssuedQty,RecptId,commAmount,TotAmt,Rate,MemberName,DateTime from BenfiaryTxn where TxnUploadSts = ?");
            query.addBindValue("N");

            if(!query.exec())
            {
                qDebug()<<"Upoad Query Error:"<<query.lastError();
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
                    ArrayList.append(CommWiseData);
            }
            UploadData["fpsOfflineTransResponses"]=ArrayList;
            query.prepare("select commCode,closingBalance from Pos_Ob");
            if(!query.exec())
            {
                qDebug()<<"Upoad Query Error:"<<query.lastError();
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
            Ret = Convertstr(UploadData.toStyledString());
            qDebug()<<"Data :"<<Ret;
            int RetVal = CurlRequest("http://164.100.132.87/ePosCommonServiceBR/eposCommon/pushFpsOfflineData",Ret);
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
                            sleep(SLEEPTIME);
                            continue ; ;
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
                                                    sprintf(TmpStr,"update BenfiaryTxn set TxnUploadSts = 'Y' where RecptId='%s' and RcId='%s'",Convertstr(UploadedData[i]["receiptId"].asString()),Convertstr(UploadedData[i]["rcId"].asString()));
                                                    qDebug()<<"TmpStr Update Status:"<<TmpStr;
                                                    if(!query.exec(TmpStr))
                                                    {
                                                        qDebug()<<"Upoad Query Error:"<<query.lastError();
                                                        sleep(SLEEPTIME);
                                                        continue ; ;
                                                    }
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

            }
            sleep(SLEEPTIME);
        }
}

int Widget::PartialOfflineTxnUpload()
{
    //FpsId,RcId,SchemeId,CommCode,TotQty,BalQty,IssuedQty,commAmount,TotAmt,RecptId,TxnUploadSts
    prn_open();
    if(prn_paperstatus() < 0)
    {
        miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
        return -1;
    }
    if(miscObj.Battery_check() != 0) //01072017
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
        else
            miscObj.DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद प्रिंट करने में असमर्थ. \n कृपया बैटरी चार्ज करें");
        return -1;
    }
    QSqlQuery query;
    char TmpStr[512];
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
                    if(query.value(0).toInt()==0)
                    {
                            miscObj.DisplayWideMsgBox((char *)"Commodity Balance Qty not available for this Entered Card");
                            return -1;
                    }
            }
    }
    memset(TmpStr,0x00,sizeof(TmpStr));
    for(int i=0;i<ui->tblCommIssue->rowCount();i++)
    {
            if(ui->tblCommIssue->item(i,3)->text().toFloat()>0.00)
            {
                    memset(TmpStr,0x00,sizeof(TmpStr));
                    sprintf(TmpStr,"select count() from Pos_Ob where commCode='%s'",ui->tblCommIssue->item(i,5)->text().toAscii().data());
                    qDebug()<<"TmpStr  :"<<TmpStr;
                    if(!query.exec(TmpStr))
                    {

                    }
                    if(query.value(0).toInt()==0)
                    {
                            memset(TmpStr,0x00,sizeof(TmpStr));
                            sprintf(TmpStr,"%s Commodity Closing balance not available for this shop",ui->tblCommIssue->item(i,0)->text().toAscii().data());
                            miscObj.DisplayWideMsgBox((char *)TmpStr);
                            return -1;
                    }
            }
    }
    memset(TmpStr,0x00,sizeof(TmpStr));
    for(int i=0;i<ui->tblCommIssue->rowCount();i++)
    {
            if(ui->tblCommIssue->item(i,3)->text().toFloat()>0.00)
            {
                    memset(TmpStr,0x00,sizeof(TmpStr));
                    sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement-%0.3f where rcId='%s' and commCode='%s'",ui->tblCommIssue->item(i,3)->text().toFloat(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data());
                    qDebug()<<"TmpStr  :"<<TmpStr;
                    if(!query.exec(TmpStr))
                    {
                            for(int j=0;j<i;j++)
                            {
                                    if(ui->tblCommIssue->item(j,3)->text().toFloat()>0.00)
                                    {
                                            memset(TmpStr,0x00,sizeof(TmpStr));
                                            sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement+%0.3f where rcId='%s' and commCode='%s'",ui->tblCommIssue->item(j,3)->text().toFloat(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(j,5)->text().toAscii().data());
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
                    sprintf(TmpStr,"update Pos_Ob set closingBalance = closingBalance-%0.3f where commCode='%s'",ui->tblCommIssue->item(i,3)->text().toFloat(),ui->tblCommIssue->item(i,5)->text().toAscii().data());
                    qDebug()<<"TmpStr  :"<<TmpStr;
                    if(!query.exec(TmpStr))
                    {
                            for(int j=0;j<ui->tblCommIssue->rowCount();j++)
                            {
                                    if(ui->tblCommIssue->item(j,3)->text().toFloat()>0.00)
                                    {
                                            memset(TmpStr,0x00,sizeof(TmpStr));
                                            sprintf(TmpStr,"update KeyRegister set  balanceEntitlement = balanceEntitlement+%0.3f where rcId='%s' and commCode='%s'",ui->tblCommIssue->item(j,3)->text().toFloat(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(j,5)->text().toFloat());
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
                                            sprintf(TmpStr,"update Pos_Ob set  closingBalance = closingBalance+%0.3f where commCode='%s'",ui->tblCommIssue->item(j,3)->text().toFloat(),ui->tblCommIssue->item(j,5)->text().toAscii().data());
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
    for(int i=0;i<ui->tblCommIssue->rowCount();i++)
    {
            if(ui->tblCommIssue->item(i,3)->text().toFloat()>0.00)
            {
                    memset(TmpStr,0x00,sizeof(TmpStr));
                    sprintf(TmpStr,"insert into BenfiaryTxn values('%s','%s',(select schemeId from KeyRegister where rcId = '%s' limit 1),'%s',(select totalEntitlement from KeyRegister where rcId = '%s' and commCode='%s'),(select balanceEntitlement from KeyRegister where rcId = '%s' and commCode='%s'),'%s','%s','%s','%s','%s',(select memberNameEn from KeyRegister where rcId = '%s' limit 1),'%s %s','N')",FpsId,ui->ldtRationCard->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->ldtRationCard->text().toAscii().data(),ui->tblCommIssue->item(i,5)->text().toAscii().data(),ui->tblCommIssue->item(i,3)->text().toAscii().data(),ui->tblCommIssue->item(i,2)->text().toAscii().data(),ui->tblCommIssue->item(i,4)->text().toAscii().data(),ui->lblTotalAmtVal->text().toAscii().data(),TxnId,ui->ldtRationCard->text().toAscii().data(),ui->labelDate->text().toAscii().data(),ui->labelTime->text().toAscii().data());
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
            PrintTxnReceiptEng();
            prn_close();

            dlgPopup2->close();
            dlgPopup2->deleteLater();
    }    
    ui->ldtRationCard->setText("");
    ui->stackedWidget->setCurrentIndex(4);
}


void Widget::on_Partial_DaDyRptBtn_clicked()
{
        ui->btnDtSalesView->hide();
        ui->tblSalesDaily->hide();
        ui->pushButton_7->hide();
        DayReportFlag = 2;//for Offline report
        ui->stackedWidget->setCurrentIndex(11);
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
        int cnt=0;
        char Tmp[200],TmpQuery[712],tmp[200];
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
        sprintf(TmpQuery,"select distinct(commCode),(select commNameEn from KeyRegister where CommCode=a.CommCode),");;
        while(CommQuery.next())
        {
                cnt++;
                memset(tmp,0x00,sizeof(tmp));
                sprintf(tmp,"%s ",CommQuery.value(1).toString().toAscii().data());
                strcat(Tmp,tmp);
                memset(tmp,0x00,sizeof(tmp));
                sprintf(tmp,"(select sum(IssuedQty) from BenfiaryTxn where CommCode=a.CommCode and SchemeId='%s'),",CommQuery.value(0).toString().toAscii().data());
                strcat(TmpQuery,tmp);
        }
        strcat(Tmp," Tot");
        TmpQuery[strlen(TmpQuery)-1]='\0';

        memset(tmp,0x00,sizeof(tmp));
        string TStr;
        TStr.clear();
        TStr.append(ui->dateEdit->text().toAscii().data());

        sprintf(tmp,",(select sum(IssuedQty) from BenfiaryTxn where CommCode=a.CommCode) from BenfiaryTxn a where substr(DateTime,1,2)='%02d' and substr(DateTime,4,2)='%02d' and substr(DateTime,7,4)='%04d'",atoi(TStr.substr(0,2).c_str()),atoi(TStr.substr(3,2).c_str()),atoi(TStr.substr(6,4).c_str()));
        strcat(TmpQuery,tmp);
        qDebug()<<"\n\nTmp:"<<Tmp;
        qDebug()<<"\n\nTmpQuery:"<<TmpQuery;

        prn_open();
        if(prn_paperstatus() < 0)
        {
            miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
            dlgPopup1->close();
            dlgPopup1->deleteLater();
            return;
        }

        if(miscObj.Battery_check() != 0) //01072017
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
            else
                miscObj.DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद प्रिंट करने में असमर्थ. \n कृपया बैटरी चार्ज करें");
            dlgPopup1->close();
            dlgPopup1->deleteLater();
            return;

        }
        char prn[512];
        LogoPrint objLogo;
        objLogo.PrintLogo();

        int LangMode = getLangMode();
        memset(prn, 0, sizeof(prn));
        sprintf(prn, "OFFLINE DAY REPORT");
        prn_write_text((unsigned char *)prn, strlen(prn), 2);

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
            prepareBmp(tDateTime, 22, tDateTime, 16, ALIGN_CENTER);

            QString tShopNo  = QString::fromUtf8("दुकान नं  ")+ "" + ui->ldtShpId->text();
            prepareBmp(tShopNo, 22, tShopNo, 16, ALIGN_LEFT);   //
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
        while(CommQuery.next())
        {
                memset(TmpQuery,0x00,sizeof(TmpQuery));
                sprintf(TmpQuery,"%-10s",CommQuery.value(1).toString().toAscii().data());
                for(int i=0;i<cnt+1;i++)
                {
                        memset(Tmp,0x00,sizeof(Tmp));
                        sprintf(Tmp," %-3.3f",CommQuery.value(2+i).toString().toFloat());
                        strcat(TmpQuery,Tmp);
                }
                prn_write_text((unsigned char *)TmpQuery, strlen(TmpQuery), 0);
        }
        memset(prn, 0, sizeof(prn));
        sprintf(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);

        prn_paper_feed(5);
        prn_close();
        dlgPopup1->close();
        dlgPopup1->deleteLater();


}

void Widget::on_PartialCurrStkRptBtn_clicked()
{
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
        miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }

    if(miscObj.Battery_check() != 0) //01072017
    {
        if(LangMode == 1)
            miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
        else
            miscObj.DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद प्रिंट करने में असमर्थ. \n कृपया बैटरी चार्ज करें");
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        return;
    }
    char prn[512];
    LogoPrint objLogo;
    objLogo.PrintLogo();

    LangMode = getLangMode();

    if(LangMode == 1)
    {
        memset(prn, 0, sizeof(prn));
        sprintf(prn, "       OFFLINE");
        prn_write_text((unsigned char *)prn, strlen(prn), 2);

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
        QString tHeader = QString::fromUtf8("  चालू स्टॉक रिपोर्ट");
        prepareBmp(tHeader, 20, tHeader, 16, ALIGN_CENTER);


        QString tDateTime = QString::fromUtf8("दिनांक") + ":" + QDateTime::currentDateTime().toString("dd-MM-yyyy").toAscii().data() + QString::fromUtf8("   समय") + ":" + QDateTime::currentDateTime().toString("hh:mm:ss").toAscii().data();
        prepareBmp(tDateTime, 20, tDateTime, 16, ALIGN_CENTER);

        QString tReportType;
        tReportType  = QString::fromUtf8("रिपोर्ट का प्रकार:  ")+ "" + QString::fromUtf8("पी.डी.एस.");
        prepareBmp(tReportType, 22, tReportType, 16, ALIGN_LEFT);

        QString tShopNo  = QString::fromUtf8("दुकान नं :  ")+ "" + ui->ldtShpId->text();
        prepareBmp(tShopNo, 20, tShopNo, 16, ALIGN_LEFT);   //
    }

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "Name        Issued   CB");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    QSqlQuery query;
    query.clear();
    query.prepare("select a.commNameEn,(select sum(IssuedQty) from BenfiaryTxn where commCode=a.commCode) as Issued,a.closingBalance from Pos_Ob a");
    if(!query.exec())
    {

    }
    char TmpStr[100];

    while(query.next())
    {
            memset(TmpStr,0x00,sizeof(TmpStr));
            sprintf(TmpStr,"%-12s %-7.3f %-7.3f",query.value(0).toString().toAscii().data(),query.value(1).toString().toFloat(),query.value(2).toString().toFloat());
            prn_write_text((unsigned char *)TmpStr, strlen(TmpStr), 0);

    }
    memset(prn, 0, sizeof(prn));
    sprintf(prn, "------------------------------------------");
    prn_write_text((unsigned char *)prn, strlen(prn), 0);

    prn_paper_feed(5);

    prn_close();
    dlgPopup1->close();
    dlgPopup1->deleteLater();
}
//int Widget::GetPartialTimeStatus()
//{
//    qDebug()<<"In Partial Time Status:";
//    char TmpStr[512];
//    QSqlQuery query;
//    query.clear();

//    /*memset(TmpStr,0x00,sizeof(TmpStr));
//    sprintf(TmpStr,"select julianday('now')-julianday('(select substr(lastlogindate,7,4) from PartialOnlineData)-(select substr(lastlogindate,4,2) from PartialOnlineData)-(select substr(lastlogindate,1,2) from PartialOnlineData)')");

//    if(!query.exec(TmpStr))
//    {
//        return -1;
//    }
//    query.next();
//    if(query.value(0).toString().toInt()!=0)
//    {
//        miscObj.DisplayWideMsgBox((char *)"Day Changed Kindly re-login");
//        ui->stackedWidget->setCurrentIndex(0);
//        return -1;
//    }*/
//    query.clear();
//    if(!query.exec("select substr(lastlogindate,1,2),substr(lastlogindate,4,2),substr(lastlogindate,7,4),substr(lastlogintime,1,2),substr(lastlogintime,4,2),substr(lastlogintime,7,2),leftOfflineTime from PartialOnlineData limit 1"))
//    {
//        return -1;
//    }
//    query.next();
//    memset(TmpStr,0x00,sizeof(TmpStr));
//    sprintf(TmpStr,"SELECT time(strftime('%s','%04d-%02d-%02d %02d:%02d:%02d')-strftime('%s','%04d-%02d-%02d %02d:%02d:%02d'),'unixepoch')","%s",QDateTime::currentDateTime().toString("yyyy").toInt(),QDateTime::currentDateTime().toString("MM").toInt(),QDateTime::currentDateTime().toString("dd").toInt(),QDateTime::currentDateTime().toString("hh").toInt(),QDateTime::currentDateTime().toString("mm").toInt(),QDateTime::currentDateTime().toString("ss").toInt(),"%s",query.value(2).toString().toInt(),query.value(1).toString().toInt(),query.value(0).toString().toInt(),query.value(3).toString().toInt(),query.value(4).toString().toInt(),query.value(5).toString().toInt());

//    int TotalDurationLimit = query.value(6).toString().toInt();

//    query.clear();
//    if(!query.exec(TmpStr))
//    {
//        return -1;
//    }
//    query.next();
//    string tmpString;
//    tmpString.clear();

//    tmpString.append(query.value(0).toString().toAscii().data());
//    int logedInTime = (atoi(tmpString.substr(0,2).c_str())*60)+(atoi(tmpString.substr(3,2).c_str()));
//    qDebug()<<"Curret Time:"<<tmpString.c_str();
//    qDebug()<<"Total Duration :"<<TotalDurationLimit;
//    qDebug()<<"logedInTime :"<<logedInTime;

//    memset(TmpStr,0x00,sizeof(TmpStr));
//    if((TotalDurationLimit-logedInTime)<=0)
//            sprintf(TmpStr,"update PartialOnlineData set leftOfflineTime = '0'");
//    else
//            sprintf(TmpStr,"update PartialOnlineData set leftOfflineTime = leftOfflineTime-%d",logedInTime);

//    query.clear();
//    qDebug()<<"TmpStr :"<<TmpStr;;
//    if(!query.exec(TmpStr))
//    {
//        qDebug()<<"Duration Update error:"<<query.lastError();
//        return -1;
//    }
//    miscObj.DisplayWideMsgBox((char *)"Offline time duration completed\nLogin by authentication");
//    return -1;

//    if(!query.exec("select leftOfflineTime,OfflineTxnTime from PartialOnlineData"))
//    {
//        return -1;
//    }
//    query.next();
//    if(query.value(0).toInt()<=0)
//    {
//            miscObj.DisplayWideMsgBox((char *)"Offline time duration completed\nLogin by authentication");
//            return -1;
//    }
//    /*
//    query.clear();
//    if(!query.exec("select leftOfflineTime,OfflineTxnTime from PartialOnlineData"))
//    {
//        return -1;
//    }
//    query.next();
//    if((atoi(string(query.value(1).toString().toAscii().data()).substr(0,2).c_str())<QDateTime::currentDateTime().toString("hh").toInt())&&(atoi(string(query.value(3).toString().toAscii().data()).substr(0,2).c_str())>QDateTime::currentDateTime().toString("hh").toInt()))
//    {
//            memset(TmpStr,0x00,sizeof(TmpStr));
//            sprintf(TmpStr,"Txn's are allowed from %s\n",query.value(3).toString().toAscii().data());
//            miscObj.DisplayWideMsgBox((char *)TmpStr);
//            return -1;
//    }*/
//}
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
    QSqlQuery query;
    query.clear();
    char TmpStr[200];
    if(!query.exec("select OfflineTxnTime from PartialOnlineData"))
    {
        miscObj.DisplayWideMsgBox((char *)"Invalid Txn's Time");
        return -1;
    }
    query.next();
    qDebug()<<"query :"<<query.value(0).toString();
    qDebug()<<"Start Time:"<<atoi(string(query.value(0).toString().toAscii().data()).substr(0,string(query.value(0).toString().toAscii().data()).find("AM")).c_str());
    if((QDateTime::currentDateTime().toString("hh").toInt()<atoi(string(query.value(0).toString().toAscii().data()).substr(0,string(query.value(0).toString().toAscii().data()).find("AM")).c_str()))&&(QDateTime::currentDateTime().toString("hh").toInt()>atoi(string(query.value(0).toString().toAscii().data()).substr(string(query.value(0).toString().toAscii().data()).find("-")+1,string(query.value(0).toString().toAscii().data()).find("PM")-string(query.value(0).toString().toAscii().data()).find("-")-1).c_str())))
    {
            memset(TmpStr,0x00,sizeof(TmpStr));
            sprintf(TmpStr,"Txn's are allowed from %s\n",query.value(0).toString().toAscii().data());
            miscObj.DisplayWideMsgBox((char *)TmpStr);
            ui->stackedWidget->setCurrentIndex(0);
            return -1;
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
                    miscObj.DisplayWideMsgBox((char *)"Offline time duration completed\nLogin by authentication");
                    ui->stackedWidget->setCurrentIndex(0);
                    return -1;
            }
    }
    return 0;
}

int Widget::LoginByPassword()
{
    if(access("/mnt/jffs2/Hybrid.txt",F_OK))
    {
        return -1;
    }
    QSqlQuery query;
    query.clear();

    char TmpStr[200];
    memset(TmpStr,0x00,sizeof(TmpStr));
    sprintf(TmpStr,"select OfflineLogin from PartialOnlineData");

    if(!query.exec("select OfflineLogin,OffPassword,leftOfflineTime,OfflineTxnTime from PartialOnlineData"))
    {
        return -1;
    }
    query.next();
    if(query.value(0).toString().toUpper()!="Y")
            return -1;

    bool ok;
    QString LoginPasswd = QInputDialog::getText(this, tr("Login"),tr("Enter login Password"), QLineEdit::Normal,"", &ok);
    qDebug()<<"LoginPasswd:"<<LoginPasswd;

    if(query.value(1).toString().compare(LoginPasswd)!=0)
    {
            miscObj.DisplayWideMsgBox((char *)"Enter Valid Password");
            return -1;
    }
    if(query.value(2).toInt()<=0)
    {
            miscObj.DisplayWideMsgBox((char *)"Offline time duration completed\nLogin by authentication");
            return -1;
    }

    if((QDateTime::currentDateTime().toString("hh").toInt()<atoi(string(query.value(3).toString().toAscii().data()).substr(0,string(query.value(3).toString().toAscii().data()).find("AM")).c_str()))&&(QDateTime::currentDateTime().toString("hh").toInt()>atoi(string(query.value(3).toString().toAscii().data()).substr(string(query.value(3).toString().toAscii().data()).find("-")+1,string(query.value(3).toString().toAscii().data()).find("PM")-string(query.value(3).toString().toAscii().data()).find("-")-1).c_str())))
    {
            memset(TmpStr,0x00,sizeof(TmpStr));
            sprintf(TmpStr,"Txn's are allowed from %s\n",query.value(3).toString().toAscii().data());
            miscObj.DisplayWideMsgBox((char *)TmpStr);

            return -1;
    }
    memset(TmpStr,0x00,sizeof(TmpStr));

    sprintf(TmpStr,"update PartialOnlineData set lastlogindate = '%02d-%02d-%04d',lastlogintime='%02d:%02d:%02d'",QDateTime::currentDateTime().toString("dd").toInt(),QDateTime::currentDateTime().toString("MM").toInt(),QDateTime::currentDateTime().toString("yyyy").toInt(),QDateTime::currentDateTime().toString("hh").toInt(),QDateTime::currentDateTime().toString("mm").toInt(),QDateTime::currentDateTime().toString("ss").toInt());

    if(!query.exec(TmpStr))
    {
        return -1;
    }
    DealerLoginBy = 2;

    //DwnldPartialOnlineData();

    ui->btnAadhaarServices->setDisabled(true);
    ui->btnReceiveGoods->setDisabled(true);
    ui->btn_inspector->setDisabled(true);
    ui->btn_BenVerify->setDisabled(true);
    ui->btnDailySales->setDisabled(true);
    ui->btnStockRegister->setDisabled(true);;;
    PartialOnlineTimer = new QTimer(this);

    connect(PartialOnlineTimer, SIGNAL(timeout()), this, SLOT(GetPartialTimeStatus()));

    PartialOnlineTimer->start(1000);

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
int Widget::GetTransactionId(char *TxnId)
{
    struct tm *tt;
    time_t  t;
    char buff[32];

    time(&t);
    tt=localtime(&t);

    memset(buff, 0, sizeof(buff));

    int tmp = 0;
    tmp = (((tt->tm_year-100) - 10) * 366) + (tt->tm_yday+1);
    qDebug()<<"first 4 digits = "<<tmp;

    int time_sec = 0;
    time_sec = (tt->tm_hour * 60 * 60) + (tt->tm_min * 60) + tt->tm_sec;

    qDebug()<<"time sec = "<<time_sec;
    qDebug()<<"Last 5 digits = "<<time_sec + 10000;

    char cMachineID[16];
    memset(cMachineID, 0, sizeof(cMachineID));
    // CMisc miscObj;
    miscObj.GetMachineId(cMachineID);

    sprintf(TxnId, "%s%04d%05d", &cMachineID[6], tmp, time_sec+10000);
    qDebug() <<"TxnID :"<<TxnId;

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
/*void Widget::run()
{

}*/
