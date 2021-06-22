#include "include/widget.h"
#include "ui_widget.h"
#include "include/misc.h"
#include "include/usb.h"
#include "include/gprs.h"
#include "include/database.h"
#include "include/popup_msg.h"
#include "include/popup_nothing.h"
#include "include/ui_popup_msg.h"
#include "include/ui_popup_nothing.h"

#include<cstdlib>
#include<math.h>
#include<json_request.h>
#include<json.h>
#include <cstdio>
#include <cstring>
#include<iostream>
#include<fstream>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>


extern char  HD_PASSWORD[64];
extern char  HD_HTS[8];

extern "C"
{
#include"include/gl11.h"
}

void Widget::on_btnReceiveGoods_clicked()
{
//    if(DealerLoginBy ==2){
//        ui->RecvGoodsVechNoLEdit->hide();
//        ui->RecvGoodsVechNoLbl->hide();
//        CheckRecviceGdsOffline();
//        return;
//    }
    ui->tableWidgetStockTable_3->setColumnHidden(3,true);
    ui->tableWidgetStockTable_3->setColumnHidden(6,true);
    ui->tableWidgetStockTable_3->setColumnHidden(7,true);

    iMainTransaction = PDS_TRANSACTION;

    GPRS objGprs;
    QSqlQuery query;

    if(objGprs.Check_Network_Available() < 0)
        return;

    popup_msg *dlgPopup = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup->ui->lblPopup->setText("Please Wait...");
    else if(LangMode == 2)
        dlgPopup->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें..."));
    else if(LangMode == 5)
            dlgPopup->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करा"));
    dlgPopup->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    int RetVal = stock_Receive();
    dlgPopup->close();
    dlgPopup->deleteLater();
    //if(RetVal == -2)
    //        CheckRecviceGdsOffline();


    if(objJsonResp->ParsingFirstConditionJson((char *)"/mnt/jffs2/feast_response.json") < 0)
        return;

    if(objJsonResp->parseStockDetailsResponseJson((char *)"/mnt/jffs2/feast_response.json") < 0)
        return;

    query.clear();
    query.prepare("SELECT truck_chit_no FROM TruckChit_Table");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        ui->comboTruckChitNo->clear();
        ui->comboTruckChitNo->addItem("Select TruckChit No");

        while(query.next())
        {
            ui->comboTruckChitNo->addItem(query.value(0).toString(),query.at());
        }
    }

    ui->tableWidgetStockTable_3->setColumnWidth(0,80);
    ui->tableWidgetStockTable_3->setColumnWidth(1,60);
    ui->tableWidgetStockTable_3->setColumnWidth(2,50);
    ui->tableWidgetStockTable_3->setColumnWidth(3,50);
    ui->tableWidgetStockTable_3->setColumnWidth(4,70);
    ui->tableWidgetStockTable_3->setColumnWidth(5,70);
    ui->tableWidgetStockTable_3->setColumnWidth(6,70);
    ui->tableWidgetStockTable_3->setColumnWidth(7,50);

    ui->tableWidgetStockTable_3->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetStockTable_3->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetStockTable_3->verticalHeader()->hide();
    ui->tableWidgetStockTable_3->showMaximized();
    ui->tableWidgetStockTable_3->clearContents();

    for(int i = ui->tableWidgetStockTable_3->rowCount(); i > 0; i--)
    {
        ui->tableWidgetStockTable_3->removeRow(i-1);
    }

    ui->labelRO->clear();
    ui->label_TruckNo->clear();

    ui->stackedWidget->setCurrentIndex(22);
    return;
}

int Widget::stock_Receive()
{
    char jsonads_Request[625]="";
    char jsonads_Response[625]="";
    char jsonads_command[625]="";

    char szShopId[16];
    memset(szShopId, 0, sizeof(szShopId));
    getShopId(szShopId);

    Json::Value JStr;

    memset(jsonads_Request,0,sizeof(jsonads_Request));
    memset(jsonads_Response,0,sizeof(jsonads_Response));
    memset(jsonads_command,0,sizeof(jsonads_command));

    strcpy(jsonads_Request,"/mnt/jffs2/feast_request.json");
    strcpy(jsonads_Response,"/mnt/jffs2/feast_response.json");

    system("rm -f /mnt/jffs2/feast_request.json");
    system("rm -f /mnt/jffs2/feast_response.json");

    JStr["fps_id"] =szShopId; //"158500500003";
    JStr["token"] = RECEIVEGOODS_TOKEN;
    JStr["mode"]   = "PDS";
    JStr["stateCode"] = stateCode.toStdString().c_str();

    QFile file(jsonads_Request);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug()<<"NO input File";
        return -1;
    }

    Json::StyledWriter styledWriter;
    string str = styledWriter.write(JStr);

    QTextStream out(&file);
    out<<str.c_str();
    file.close();

    system("cat /mnt/jffs2/feast_request.json");

//    sprintf(jsonads_command,"curl -k -X POST --header \"Content-Type:application/json;\" --data @%s %s -o %s -m 75",jsonads_Request,STOCK__RECIEVE_URL,jsonads_Response);

    sprintf(jsonads_command,"curl -k -X POST --header \"Content-Type:application/json;\" --data @%s %sgetFpsStockDetails -o %s -m 75",jsonads_Request,Offline_and_ReceiveStock_URL.toAscii().data(),jsonads_Response);

    qDebug() << "getFpsStockDetails::" << jsonads_command;

    int Retval = system(jsonads_command);
    if(SystemCurlResponse(Retval, EPDS_SERVER) < 0){
        return -2;
    }
    system("cat /mnt/jffs2/feast_response.json");
    return 0;
}

void Widget::on_comboTruckChitNo_activated(const QString &arg1)
{
    if(arg1 == "Select TruckChit No")
    {
        qDebug() << arg1;
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT do_ro_no FROM TruckChit_Table WHERE truck_chit_no=?");
    query.addBindValue(arg1);
    if(!query.exec())
        qDebug() << "In on_comboTruckChitNo_activated = " << query.lastError();
    else
    {
        while(query.next())
        {
            ui->labelRO->setText(query.value(0).toString());
        }
    }

    query.clear();

    query.prepare("SELECT truckNo FROM Stock_Details WHERE truckChitNo=?");

    query.addBindValue(arg1);
    if(!query.exec())
        qDebug() << "In on_comboTruckChitNo_activated_1= " << query.lastError();
    else
    {
        while(query.next())
        {
            ui->label_TruckNo->setText(query.value(0).toString());
            qDebug() << "TruckNo : " << query.value(0).toString();
        }
    }
    ShowStockDetils(arg1);

    return;
}

int Widget::ShowStockDetils(QString CurrentCombooxItem)
{
    qDebug()<<"::::::ShowStockDetils:::::::";
    qDebug() << "CurrentCombooxItem" <<CurrentCombooxItem;

    QSqlQuery query;
    int row  = 0;
    query.clear();

    query.prepare("SELECT COUNT(commCode) FROM Stock_Details WHERE truckChitNo= ?");
    query.addBindValue(CurrentCombooxItem);

    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            row = query.value(0).toInt(0);
        }
    }

    qDebug()<<"no of Rows :"<<row;
    ui->tableWidgetStockTable_3->setRowCount(row);

    qDebug()<<"before quesry"<<endl;
    query.clear();

    query.prepare("SELECT commName, schemeName, KRA, qtD, releasedQuantity, commCode, schemeID,AllotedMonth,AllotedYear FROM Stock_Details WHERE truckChitNo=?");

    query.addBindValue(CurrentCombooxItem);
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        row = 0;
        while(query.next())
        {
            qDebug()<<"0.CommName";
            //0.CommName
            ui->tableWidgetStockTable_3->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            ui->tableWidgetStockTable_3->item(row, 0)->setFlags( ui->tableWidgetStockTable_3->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetStockTable_3->item(row, 0)->setFlags( ui->tableWidgetStockTable_3->item(row,0)->flags() | Qt::ItemIsSelectable);

            //1 schemeName
            ui->tableWidgetStockTable_3->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
            ui->tableWidgetStockTable_3->item(row, 1)->setFlags( ui->tableWidgetStockTable_3->item(row,1)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetStockTable_3->item(row, 1)->setFlags( ui->tableWidgetStockTable_3->item(row,1)->flags() | Qt::ItemIsSelectable);

            //allot=qtd

            ui->tableWidgetStockTable_3->setItem(row, 2, new QTableWidgetItem(query.value(3).toString()));
            ui->tableWidgetStockTable_3->item(row, 2)->setFlags( ui->tableWidgetStockTable_3->item(row,2)->flags() ^ Qt::ItemIsEditable);
            ui->tableWidgetStockTable_3->item(row, 2)->setFlags( ui->tableWidgetStockTable_3->item(row,2)->flags() | Qt::ItemIsSelectable);

            //releasedQuantity //dispatched
            ui->tableWidgetStockTable_3->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));

            ui->tableWidgetStockTable_3->item(row, 4)->setFlags( ui->tableWidgetStockTable_3->item(row,4)->flags() ^ Qt::ItemIsEditable);
            ui->tableWidgetStockTable_3->item(row, 4)->setFlags( ui->tableWidgetStockTable_3->item(row,4)->flags() | Qt::ItemIsSelectable);


            //Received
            ui->tableWidgetStockTable_3->setItem(row, 5, new QTableWidgetItem("0"));    //Delivered Qty
            ui->tableWidgetStockTable_3->item(row, 5)->setData(Qt::BackgroundColorRole, QVariant(QColor(Qt::green)));
        //    ui->tableWidgetStockTable_3->item(row, 5)->setFlags( ui->tableWidgetStockTable_3->item(row,5)->flags() | Qt::ItemIsEditable);


            //commCode
            ui->tableWidgetStockTable_3->setItem(row, 6, new QTableWidgetItem(query.value(5).toString()));
            ui->tableWidgetStockTable_3->item(row, 6)->setFlags( ui->tableWidgetStockTable_3->item(row,6)->flags() ^ Qt::ItemIsEditable);
            ui->tableWidgetStockTable_3->item(row, 6)->setFlags( ui->tableWidgetStockTable_3->item(row,6)->flags() | Qt::ItemIsSelectable);

            //schemeID
            ui->tableWidgetStockTable_3->setItem(row, 7, new QTableWidgetItem(query.value(6).toString()));
            ui->tableWidgetStockTable_3->item(row, 7)->setFlags( ui->tableWidgetStockTable_3->item(row,7)->flags() ^ Qt::ItemIsEditable);
            ui->tableWidgetStockTable_3->item(row, 7)->setFlags( ui->tableWidgetStockTable_3->item(row,7)->flags() | Qt::ItemIsSelectable);

            //AllotedMonth
            ui->tableWidgetStockTable_3->setItem(row, 8, new QTableWidgetItem(query.value(7).toString()));
            ui->tableWidgetStockTable_3->item(row, 8)->setFlags( ui->tableWidgetStockTable_3->item(row,8)->flags() ^ Qt::ItemIsEditable);
            ui->tableWidgetStockTable_3->item(row, 8)->setFlags( ui->tableWidgetStockTable_3->item(row,8)->flags() | Qt::ItemIsSelectable);

            //AllotedYear
            ui->tableWidgetStockTable_3->setItem(row, 9, new QTableWidgetItem(query.value(8).toString()));
            ui->tableWidgetStockTable_3->item(row, 9)->setFlags( ui->tableWidgetStockTable_3->item(row,9)->flags() ^ Qt::ItemIsEditable);
            ui->tableWidgetStockTable_3->item(row, 9)->setFlags( ui->tableWidgetStockTable_3->item(row,9)->flags() | Qt::ItemIsSelectable);

            row++;
        }
    }
    /* Hiding tableWidgetStockTable_3 columns*/

    ui->tableWidgetStockTable_3->setColumnHidden(3,true); //RO QTY
    ui->tableWidgetStockTable_3->setColumnHidden(6,true); //schemeCode
    ui->tableWidgetStockTable_3->setColumnHidden(7,true); //schemeID
    ui->tableWidgetStockTable_3->setColumnHidden(8,true);
    ui->tableWidgetStockTable_3->setColumnHidden(9,true);

    return 0;
}

void Widget::on_BtnBack_7_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    return;
}

void Widget::on_BtnNext_3_clicked()
{
    iModuleType = STOCK_ENTRY;

    if(ui->comboTruckChitNo->currentIndex() == 0){
        miscObj.DisplayWideMsgBox((char *)"Please Select Truck Chit No");
        return;
    }

    int receiveflag=0;
    int row = ui->tableWidgetStockTable_3->rowCount();

    for(int i = 0; i<row; i++){
        if(atof(ui->tableWidgetStockTable_3->item(i,5)->text().toAscii().data()) > 0)
            receiveflag++;
    }
    if(receiveflag == 0)
    {
        char buff[128];
        memset(buff, 0, sizeof(buff));
        sprintf(buff,"%s", "Please Issue Atleast one Receive");
        miscObj.DisplayWideMsgBox((char *)buff);
        return;
    }

    int rows = ui->tableWidgetStockTable_3->rowCount();
    if(rows > 0)
    {
        for(int i = 0; i < rows; i++)
        {
            qDebug() << ui->tableWidgetStockTable_3->item(i, 7)->text().toInt(0);

            if(ui->tableWidgetStockTable_3->item(i, 7)->text().isEmpty()||ui->tableWidgetStockTable_3->item(i, 7)->text().isNull())
            {
                char buff[128];
                memset(buff, 0, sizeof(buff));
                if(LangMode == 1)
                {
                    sprintf(buff, "Please Enter Delivered Qty For \"%s\"-\"%s\"", ui->tableWidgetStockTable_3->item(i, 0)->text().toAscii().data(),ui->tableWidgetStockTable_3->item(i, 1)->text().toAscii().data());
                    miscObj.DisplayWideMsgBox((char *)buff);

                }
                else if(LangMode == 2)
                {
                    QString str;
                    str = "कृपया " +ui->tableWidgetStockTable_3->item(i, 0)->text() +" के लिए मात्रा दर्ज करें"+"-"+ui->tableWidgetStockTable_3->item(i, 1)->text();
                    miscObj.tel_DisplayWideMsgBox((char *)str.toAscii().data());
                }
                else if(LangMode == 5)
                {

                        QString str;
                        str = "कृपया " +ui->tableWidgetStockTable_3->item(i, 0)->text() +" साठी मात्रा प्रविष्ट करा"+"-"+ui->tableWidgetStockTable_3->item(i, 1)->text();
                        miscObj.tel_DisplayWideMsgBox((char *)str.toAscii().data());

                }
                return;
            }
        }
    }

    for(int i = ui->tableWidgetRecieveGoodsAuth->rowCount(); i > 0; i--)
    {
        ui->tableWidgetRecieveGoodsAuth->removeRow(i-1);
    }
    ui->BtnDealerRecGoods->setEnabled(true);
    ui->BtnDealerRecGoods->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(255, 85, 0);");

    ui->BtnRouteOfficer->setDisabled(true);
    ui->BtnRouteOfficer->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(255, 255, 255);");

    ui->tableWidgetRecieveGoodsAuth->hideColumn(1);
    ui->tableWidgetRecieveGoodsAuth->hideColumn(2);

    ui->ConsentFormcheckBox_4->setChecked(true);
    ui->stackedWidget->setCurrentIndex(23);

    return;
}

void Widget::on_BtnBack_8_clicked()
{
    if(iMainTransaction == HAWKER_TRANSACTION || iMainTransaction == HAWKER_DOWNLOADING)
    {
        qDebug()<<"Module type is not KEROSENE_TXN and KEROSENE_HWKR_TXN \n Page goes to 22";
        ui->stackedWidget->setCurrentIndex(38);//22);
    }
    else if(iMainTransaction == RCV_GOODS_TRANSACTION)
    {
        ui->stackedWidget->setCurrentIndex(22);
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
    return;
}

void Widget::on_BtnDealerRecGoods_clicked()
{
    char cmaskUID[15]="";
    QTime tim;
    tim.restart();
    tim.start();

    iDealMemSelected = DEALER_SELECTED;

    int tableCount = 0;

    QSqlQuery query;
    query.prepare("SELECT COUNT(ownername) FROM Dealer");
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
            miscObj.DisplayWideMsgBox((char *)"Dealer Details is not registered at AePDS server...");
        else
            miscObj.tel_DisplayWideMsgBox((char *)"डीलर विवरण सर्वर पर पंजीकृत नहीं हैं");
        return;
    }
    ui->tableWidgetRecieveGoodsAuth->showMaximized();
    ui->tableWidgetRecieveGoodsAuth->clearContents();
    ui->tableWidgetRecieveGoodsAuth->setRowCount(tableCount);

    int row = 0;
    query.clear();
    query.prepare("SELECT ownername, fpsuidno, del_bfd_1, del_bfd_2, del_bfd_3,dealer_type FROM Dealer");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            ui->tableWidgetRecieveGoodsAuth->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            ui->tableWidgetRecieveGoodsAuth->item(row, 0)->setFlags( ui->tableWidgetRecieveGoodsAuth->item(row,0)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetRecieveGoodsAuth->item(row, 0)->setFlags( ui->tableWidgetRecieveGoodsAuth->item(row,0)->flags() | Qt::ItemIsSelectable);

            memset(cmaskUID, 0, sizeof(cmaskUID));
            strcpy(cmaskUID,query.value(1).toString().toAscii().data());
            // memset(cmaskUID,'X',8);
            qDebug()<<"UID == " <<query.value(1).toString().toAscii().data() <<"MASK ==" <<cmaskUID;
            ui->tableWidgetRecieveGoodsAuth->setItem(row, 1, new QTableWidgetItem (cmaskUID));
            ui->tableWidgetRecieveGoodsAuth->item(row, 1)->setFlags( ui->tableWidgetRecieveGoodsAuth->item(row,1)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetRecieveGoodsAuth->item(row, 1)->setFlags( ui->tableWidgetRecieveGoodsAuth->item(row,1)->flags() | Qt::ItemIsSelectable);

            ui->tableWidgetRecieveGoodsAuth->setItem(row, 3, new QTableWidgetItem (query.value(1).toString()));
            ui->tableWidgetRecieveGoodsAuth->item(row, 3)->setFlags( ui->tableWidgetRecieveGoodsAuth->item(row,3)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetRecieveGoodsAuth->item(row, 3)->setFlags( ui->tableWidgetRecieveGoodsAuth->item(row,3)->flags() | Qt::ItemIsSelectable);

            ui->tableWidgetRecieveGoodsAuth->setItem(row, 4, new QTableWidgetItem (query.value(5).toString()));
            ui->tableWidgetRecieveGoodsAuth->item(row, 4)->setFlags( ui->tableWidgetRecieveGoodsAuth->item(row,4)->flags() ^ Qt::ItemIsEditable);//For Non Editable of the field of the table
            ui->tableWidgetRecieveGoodsAuth->item(row, 4)->setFlags( ui->tableWidgetRecieveGoodsAuth->item(row,4)->flags() | Qt::ItemIsSelectable);

            row++;
        }
    }

    ui->tableWidgetRecieveGoodsAuth->setColumnWidth(0,200);
    ui->tableWidgetRecieveGoodsAuth->setColumnWidth(1,150);
    ui->tableWidgetRecieveGoodsAuth->setColumnWidth(2,200);

    ui->tableWidgetRecieveGoodsAuth->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetRecieveGoodsAuth->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetRecieveGoodsAuth->verticalHeader()->hide();
    ui->tableWidgetRecieveGoodsAuth->selectRow(0);

    ui->tableWidgetRecieveGoodsAuth->hideColumn(1);
    ui->tableWidgetRecieveGoodsAuth->hideColumn(2);

    return;
}

void Widget::on_BtnRouteOfficer_clicked()
{
    iDealMemSelected = ROUTE_OFFICER_SELECTED;

    char cmaskUID[15]="";
    //Check_mplayer_Exists();

    GPRS objGprs;

    popup_msg *dlgPopup2 = new popup_msg(this);

    if(LangMode == 1)
        dlgPopup2->ui->lblPopup->setText("Downloading Route-Officer Details...");
    else if(LangMode == 2 ||LangMode == 5)
        dlgPopup2->ui->lblPopup->setText(trUtf8("रूट-अधिकारी विवरण डाउनलोड हो रहा है"));

    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    if(objGprs.Check_Network_Available() < 0)
    {
        dlgPopup2->close();
        dlgPopup2->deleteLater();
        return;
    }

    systemret = system("rm /mnt/jffs2/RouteOfficerReq.xml");
    systemret = system("rm /mnt/jffs2/RouteOfficerResp.xml");

    int ret =           preparegetRouteOfficerDtlsReq("/mnt/jffs2/RouteOfficerReq.xml");

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

    //ui->tableWidgetRecieveGoodsAuth->setRowCount(row);
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
                ui->tableWidgetRecieveGoodsAuth->setRowCount(1);
                //   ui->tableWidgetRecieveGoodsAuth->setRowCount(1);

                ui->tableWidgetRecieveGoodsAuth->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
                ui->tableWidgetRecieveGoodsAuth->item(row, 0)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row, 0)->flags() ^ Qt::ItemIsEditable);
                ui->tableWidgetRecieveGoodsAuth->item(row,0)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row,0)->flags() | Qt::ItemIsSelectable);

                memset(cmaskUID, 0, sizeof(cmaskUID));
                strcpy(cmaskUID,query.value(1).toString().toAscii().data());
                memset(cmaskUID, 'X', 8);
                ui->tableWidgetRecieveGoodsAuth->setItem(row, 1, new QTableWidgetItem(cmaskUID));
                ui->tableWidgetRecieveGoodsAuth->item(row, 1)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row, 1)->flags() ^ Qt::ItemIsEditable);
                ui->tableWidgetRecieveGoodsAuth->item(row,1)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row,1)->flags() | Qt::ItemIsSelectable);

                ui->tableWidgetRecieveGoodsAuth->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
                ui->tableWidgetRecieveGoodsAuth->item(row, 2)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row, 2)->flags() ^ Qt::ItemIsEditable);
                ui->tableWidgetRecieveGoodsAuth->item(row,2)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row,2)->flags() | Qt::ItemIsSelectable);

                ui->tableWidgetRecieveGoodsAuth->setItem(row, 3, new QTableWidgetItem(query.value(1).toString()));
                ui->tableWidgetRecieveGoodsAuth->item(row, 3)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row, 3)->flags() ^ Qt::ItemIsEditable);
                ui->tableWidgetRecieveGoodsAuth->item(row,3)->setFlags(ui->tableWidgetRecieveGoodsAuth->item(row,3)->flags() | Qt::ItemIsSelectable);

                row++;
            }
        }
    }

    ui->tableWidgetRecieveGoodsAuth->setColumnWidth(0,150);
    ui->tableWidgetRecieveGoodsAuth->setColumnWidth(1,150);

    ui->tableWidgetRecieveGoodsAuth->setColumnWidth(2,100);

    ui->tableWidgetRecieveGoodsAuth->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetRecieveGoodsAuth->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetRecieveGoodsAuth->verticalHeader()->hide();
    ui->tableWidgetRecieveGoodsAuth->showMaximized();
    ui->tableWidgetRecieveGoodsAuth->selectRow(0);

    return;
}

void Widget::on_BtnScanFinger_clicked()
{
    iConsentFormBackFlag = 5;
    ui->ConsentCheckBox->setChecked(true);
    ConsentForm();
    ui->stackedWidget->setCurrentIndex(21);

    return;
}

//void Widget::on_BtnScanFinger_clicked()
//{
//    if(!ui->ConsentFormcheckBox_4->isChecked())
//    {
////        miscObj.DisplayWideMsgBox((char *)"Please accept consent form");
//        if(LangMode == 1)
//            miscObj.DisplayWideMsgBox((char *)"Please accept consent form");
//        else if(LangMode == 2)
//            miscObj.tel_DisplayWideMsgBox("कृपया सहमति प्रपत्र स्वीकार करें");

//        QString ConsentReasonStr = "0-N";
//        if(SendTxnCancelReason(ConsentReasonStr,'C') < 0)
//            return;
//        return;
//    }

//    Finger_Iris_Selection = 1;  //Finger Scanner

//    int recicegoods_rows=ui->tableWidgetRecieveGoodsAuth->rowCount();
//    qDebug()<<"recicegoods_rows"<<recicegoods_rows;

//    if(recicegoods_rows==0)
//        return;

//    if(RecieveGoodsAuth() == 0)
//    {
//        /* Check Printing Condition*/
//        prn_open();
//        if(prn_paperstatus() < 0)
//        {
//            if(LangMode == 1)
//                miscObj.DisplayWideMsgBox((char *)"Please Put Paper");
//            else if(LangMode == 2)
//                miscObj.tel_DisplayWideMsgBox((char *)"कृपया कागज डालें");
//            else if(LangMode == 5)
//                miscObj.tel_DisplayWideMsgBox((char *)"कृपया पेपर ठेवा");
//            prn_close();
//            return;
//        }
//        if(miscObj.Battery_check() != 0) //01072017
//        {
//            if(LangMode == 1)
//                miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
//            else if(LangMode == 2)
//                miscObj.tel_DisplayWideMsgBox((char *)"कम बैटरी के कारण रसीद प्रिंट करने में असमर्थ. \n कृपया बैटरी चार्ज करें");
//            else if(LangMode == 5)
//                 miscObj.tel_DisplayWideMsgBox((char *)"कमी बॅटरीमुळे प्रवेश मुद्रित करण्यात अक्षम. \n कृपया बॅटरी चार्ज करा");
//            prn_close();
//            return;

//        }
//        prn_close();

//        if(iDealMemSelected == DEALER_SELECTED)
//        {
//            if(getRecvGoodsRuteOfficerStatus() != true)
//            {
//                qDebug()<<"IF RO only true";
//                if(SendReceiveStockTXNToServer() == 0) //New Code Added by vivek
//                {
//                    Print_ReceiveStock(1);
//                }
//                //Clear the Widgets after success txn
//                ui->comboTruckChitNo->clear();
//                ui->label_TruckNo->clear();
//                ui->labelRO->clear();
//                ui->tableWidgetStockTable_3->clearContents();

//                ui->stackedWidget->setCurrentIndex(1);
//                return;

//            }
//            else
//            {
//                // IF Dealer and RO Both are True
//                ui->BtnDealerRecGoods->setDisabled(true);
//                ui->BtnDealerRecGoods->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(255, 255, 255);"); // white
//                ui->BtnRouteOfficer->setEnabled(true);
//                ui->BtnRouteOfficer->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(255, 85, 0);"); // orange
//                ui->tableWidgetRecieveGoodsAuth->clearContents();

//                for(int i = ui->tableWidgetRecieveGoodsAuth->rowCount(); i > 0; i--)
//                {
//                    ui->tableWidgetRecieveGoodsAuth->removeRow(i-1);
//                }
//            }

//        }
//        if(iDealMemSelected ==  ROUTE_OFFICER_SELECTED)
//        {
//            qDebug()<<"IF RO and Dealer or Dealer only True";
//            if(SendReceiveStockTXNToServer() == 0) //New Code Added by vivek
//            {
//                Print_ReceiveStock(2);
//            }
//            ui->comboTruckChitNo->clear();
//            ui->label_TruckNo->clear();
//            ui->labelRO->clear();
//            ui->tableWidgetStockTable_3->clearContents();
//            ui->stackedWidget->setCurrentIndex(1);
//            return;
//        }

//    }
//    return;
//}

int Widget::preparegetRouteOfficerDtlsReq(char *FileName)
{
    qDebug()<<"*** INSIDE preparegetRouteOfficerDtlsReq =="<< FileName;
    FILE *fp;
    fp = fopen(FileName,"w");
    if(fp == NULL)
    {
        qDebug() << "preparegetRouteOfficerDtlsReq File Not Found" << endl;
        return -1;
    }

    char szShopId[16];
    memset(szShopId, 0, sizeof(szShopId));
    getShopId(szShopId);

    char userType[4];
    memset(userType, 0, sizeof(userType));
    strcpy(userType, "R");

    memset(SessionId, 0, sizeof(SessionId));

    database_queries obj;
    if(obj.getSessionId(SessionId) < 0)
    {
        miscObj.DisplayMsgBox("Session Id not found\nPlease try again");
        return -1;
    }

    qDebug() << "SessionId" << SessionId;

    fprintf(fp, "%s", "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    fprintf(fp, "%s","<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://service.fetch.rationcard/\">");
    fprintf(fp, "<%s>", "SOAP-ENV:Body");
    fprintf(fp, "<%s>", "ns1:getRoutOffDetail");
    fprintf(fp, "<fpsId>%s</fpsId>", szShopId);
    fprintf(fp, "<userType>%s</userType>", userType);
    fprintf(fp, "<fpsSessionId>%s</fpsSessionId>", SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>",stateCode.toStdString().c_str());
    fprintf(fp, "<password>%s</password>", HD_PASSWORD);
    fprintf(fp, "</%s>", "ns1:getRoutOffDetail");
    fprintf(fp, "</%s>", "SOAP-ENV:Body");
    fprintf(fp, "</%s>", "SOAP-ENV:Envelope");
    fclose(fp);

    return 0;
}

int Widget::parseRouteOfficerResponse()
{
    QSqlQuery query;

    query.exec("DELETE FROM RouteOfficerDetails");

    QFile file("/mnt/jffs2/RouteOfficerResp.xml");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error Reading Param File";
        return -1;
    }

    QXmlStreamReader xml(&file);

    QString Qroute_off_mobile, Qroute_off_name, Qroute_off_uid;
    QString Qroute_off_1_mobile, Qroute_off_1_name, Qroute_off_1_uid;

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {

            if(xml.name() == "route_off_mobile")
            {
                xml.readNext();
                Qroute_off_mobile = xml.text().toString();
                qDebug() << "Qroute_off_mobile : " << Qroute_off_mobile;
            }
            else if(xml.name() == "route_off_name")
            {
                xml.readNext();
                Qroute_off_name = xml.text().toString();
                qDebug() << "Qroute_off_name : " << Qroute_off_name;
            }
            else if(xml.name() == "route_off_uid")
            {
                xml.readNext();
                Qroute_off_uid = xml.text().toString();
                qDebug() << "Qroute_off_uid : " << Qroute_off_uid;

                query.clear();
                query.prepare("INSERT INTO RouteOfficerDetails(mobile, name, uid) VALUES(:mobile, :name, :uid)");
                query.bindValue(":mobile", Qroute_off_mobile);
                query.bindValue(":name", Qroute_off_name);
                query.bindValue(":uid", Qroute_off_uid);
                if(!query.exec())
                {
                    qDebug() << "In Route Officer Parsing : " << query.lastError();
                    return -1;
                }
                else
                {
                    qDebug() << "Route Officer Details Inserted Successfully";
                }
            }
            if(xml.name() == "route_off_1_mobile")
            {
                xml.readNext();
                Qroute_off_1_mobile = xml.text().toString();
                qDebug() << "Qroute_off_1_mobile : " << Qroute_off_1_mobile;
            }
            else if(xml.name() == "route_off_1_name")
            {
                xml.readNext();
                Qroute_off_1_name = xml.text().toString();
                qDebug() << "Qroute_off_1_name : " << Qroute_off_1_name;
            }
            else if(xml.name() == "route_off_1_uid")
            {
                xml.readNext();
                Qroute_off_1_uid = xml.text().toString();
                qDebug() << "Qroute_off_1_uid : " << Qroute_off_1_uid;

                query.clear();
                query.prepare("INSERT INTO RouteOfficerDetails(mobile, name, uid) VALUES(:mobile, :name, :uid)");
                query.bindValue(":mobile", Qroute_off_1_mobile);
                query.bindValue(":name", Qroute_off_1_name);
                query.bindValue(":uid", Qroute_off_1_uid);
                if(!query.exec())
                {
                    qDebug() << "In Route Officer Parsing-1: " << query.lastError();
                    return -1;
                }
                else
                {
                    qDebug() << "Route Officer Details Inserted Successfully-1";
                }
            }

        }
    }
    return 0;
}

int Widget::RecieveGoodsAuth()
{
    int iRowCount = 0;
    int i = 0;

    iRowCount = ui->tableWidgetRecieveGoodsAuth->rowCount();

    qDebug() << "No Of Row in tableWidgetRecieveGoodsAuth = " << iRowCount;
    for(int i = 0; i < iRowCount; i++)
    {
        if(ui->tableWidgetRecieveGoodsAuth->item(i,0)->isSelected() == true)
            break;
    }

    if(i == iRowCount)
    {
        if(iDealMemSelected == DEALER_SELECTED)
        {
            if(LangMode == 1)
                miscObj.DisplayMsgBox((char *)"Please Select Dealer");
            else
                miscObj.tel_DisplayWideMsgBox((char *)"कृपया डीलर का चयन करें");
        }
        else if(iDealMemSelected == ROUTE_OFFICER_SELECTED)
        {
            if(LangMode == 1)
                miscObj.DisplayMsgBox((char *)"Please Select Route Officer");
            else
                miscObj.tel_DisplayWideMsgBox((char *)"कृपया रूट अधिकारी का चयन करें");
        }

        return -1;
    }

    int row = 0;
    row = ui->tableWidgetRecieveGoodsAuth->currentRow();
    qDebug() << "***********Current Row No = " << row;
//    ui->tableWidgetDealNomDetails->setCurrentCell(row,0);

    qDebug() << "Member Name = " << ui->tableWidgetRecieveGoodsAuth->item(row, 0)->text().toAscii().data();
    qDebug() << "Member UID =  " << ui->tableWidgetRecieveGoodsAuth->item(row, 3)->text().toAscii().data();

    if( iDealMemSelected == ROUTE_OFFICER_SELECTED )
    {
        qDebug() << "Member Mobile = " << ui->tableWidgetRecieveGoodsAuth->item(row, 2)->text().toAscii().data();

        memset(gRoute_Off_UID, 0, sizeof(gRoute_Off_UID));
        strcpy(gRoute_Off_UID, ui->tableWidgetRecieveGoodsAuth->item(row, 3)->text().toAscii().data());
        qDebug()<<"ROUTE_OFFICER_SELECTED :"<<"gRoute_Off_UID :" <<gRoute_Off_UID;
    }
    else if( getRecvGoodsRuteOfficerStatus() != true)
    {
        memset(gRoute_Off_UID, 0, sizeof(gRoute_Off_UID));
        strcpy(gRoute_Off_UID, "0");

    }
    memset(MemberUID, 0, sizeof(MemberUID));
    strcpy(MemberUID, ui->tableWidgetRecieveGoodsAuth->item(row, 3)->text().toAscii().data());

    if(Finger_Iris_Selection == 1)
    {
        Finger_Iris_Selection = 0;

//        int ret = AuthBen();
        int ret = AuthBenRecvGoods();

        qDebug()<<"Ret Auth Ben : "<<ret;
        if(ret < 0)
        {
            return -1;
        }
        else
        {
            gl11_lcdbklt_on();

            if(iDealMemSelected == DEALER_SELECTED)
            {
                if(LangMode == 1)
                    miscObj.TimerMessageBox((char *)"Dealer Authentication Success", 1);
                else
                    miscObj.Tel_TimerMessageBox((char *)"डीलर प्रमाणीकरण सफल", 1);

            }
            else if(iDealMemSelected == ROUTE_OFFICER_SELECTED)
            {
                if(LangMode == 1)
                    miscObj.TimerMessageBox((char *)"Route Officer Authentication Success", 1);
                else
                    miscObj.Tel_TimerMessageBox((char *)"रूट अधिकारी प्रमाणीकरण सफल", 1);
            }

        }
    }
    else
    {
        Finger_Iris_Selection = 0;

        int i = 1;
        while(1)
        {
            if((i == 1) || (i == 3) || (i == 5))
            {
                int ret = Capture_Iris(LEFT_EYE, MemberUID);
                if(ret == 0)
                {
                    if(iDealMemSelected == DEALER_SELECTED)
                    {
                        if(LangMode == 1)
                            miscObj.TimerMessageBox((char *)"Dealer Authentication Success", 1);
                        else
                            miscObj.Tel_TimerMessageBox((char *)"डीलर प्रमाणीकरण सफल", 1);
                    }
                    else if(iDealMemSelected == ROUTE_OFFICER_SELECTED)
                    {
                        if(LangMode == 1)
                            miscObj.TimerMessageBox((char *)"Dealer Authentication Success", 1);
                        else
                            miscObj.Tel_TimerMessageBox((char *)"डीलर प्रमाणीकरण सफल", 1);
                    }
                    else if(iDealMemSelected == HAWKER_SELECTED)
                    {
                        if(LangMode == 1)
                            miscObj.TimerMessageBox((char *)"Hawker Authentication Success", 1);
                        else
                            miscObj.Tel_TimerMessageBox((char *)"हॉकर प्रमाणीकरण सफल", 1);
                    }

                    break;
                }
                else if(ret == -2)
                    return -1;
            }
            else if((i == 2) || (i == 4) || (i == 6))
            {
                int ret = Capture_Iris(RIGHT_EYE, MemberUID);
                if(ret == 0)
                {
                    if(iDealMemSelected == DEALER_SELECTED)
                    {
                        if(LangMode == 1)
                            miscObj.TimerMessageBox((char *)"Dealer Authentication Success", 1);
                        else
                            miscObj.Tel_TimerMessageBox((char *)"डीलर प्रमाणीकरण सफल", 1);
                    }
                    else if(iDealMemSelected == ROUTE_OFFICER_SELECTED)
                    {
                        if(LangMode == 1)
                            miscObj.TimerMessageBox((char *)"Dealer Authentication Success", 1);
                        else
                            miscObj.Tel_TimerMessageBox((char *)"डीलर प्रमाणीकरण सफल", 1);
                    }
                    else if(iDealMemSelected == HAWKER_SELECTED)
                    {
                        if(LangMode == 1)
                            miscObj.TimerMessageBox((char *)"Hawker Authentication Success", 1);
                        else
                            miscObj.Tel_TimerMessageBox((char *)"हॉकर प्रमाणीकरण सफल", 1);
                    }

                    break;
                }
                else if(ret == -2)
                    return -1;
            }

            if(i == 6)
            {
                if(LangMode == 1)
                    miscObj.DisplayWideMsgBox((char *)"Exceeded Attempts for Iris Authentication...\nPlease Opt for Scan-FP");
                else
                    miscObj.tel_DisplayWideMsgBox((char *)"आईरिस प्रमाणीकरण के प्रयासों की सीमा  को पार कर लिया गया है  ... \nकृपया स्कैन-एफपी विकल्प चुनें");
                return -1;
            }

            i++;
        }
    }

    return 0;
}

int Widget::SendReceiveStockTXNToServer()
{
    QSqlQuery query;

    systemret = system("rm /mnt/jffs2/StockUpdateReq.xml");
    systemret = system("rm /mnt/jffs2/StockUpdateResp.xml");

    FILE *fp;
    fp = fopen("/mnt/jffs2/StockUpdateReq.xml","w");
    if(fp == NULL)
    {
        qDebug() << "/mnt/jffs2/StockUpdateReq.xml File Not Found" << endl;
        return -1;
    }

    int iRecvGoodsCount = 0;
    iRecvGoodsCount = getCommLength(ui->comboTruckChitNo->currentText().toAscii().data());
    qDebug()<<"iRecvGoodsCount = "<<iRecvGoodsCount;

    char dispachId[64];
    memset(dispachId, 0, sizeof(dispachId));
    query.clear();
    query.prepare("SELECT ChallanId FROM Stock_Details WHERE truckChitNo=? AND AllocationOrderNo=?");   //AllocationOrderNo is do_ro_No, ChallanId is dispacth ID
    query.addBindValue(ui->comboTruckChitNo->currentText().toAscii().data());
    query.addBindValue(ui->labelRO->text().toAscii().data());
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            strcpy(dispachId, query.value(0).toString().toAscii().data());
        }
    }

    char szShopId[16];
    memset(szShopId, 0, sizeof(szShopId));
    getShopId(szShopId);

    char cdeviceId[15]; // added by vivek 28082017
    memset(cdeviceId, 0, sizeof(cdeviceId));
    miscObj.GetMachineId(cdeviceId);

    fprintf(fp, "%s", "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    fprintf(fp, "%s", "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://service.fetch.rationcard/\">");
    fprintf(fp, "<%s>", "SOAP-ENV:Body");
    fprintf(fp, "<%s>", "ns1:stockInfoUpdate");
    fprintf(fp, "<%s>", "Stock_Entry");
    fprintf(fp, "<deviceId>%s</deviceId>", cdeviceId);
    fprintf(fp, "<dispatchId>%s</dispatchId>", dispachId);
    fprintf(fp, "<do_ro_no>%s</do_ro_no>", ui->labelRO->text().toAscii().data());
    fprintf(fp, "<noOfComm>%d</noOfComm>", iRecvGoodsCount);

    if(getRecvGoodsRuteOfficerStatus() == true) // new condition 02122017
    {
        fprintf(fp, "<route_off_auth>%s</route_off_auth>", gRoute_Off_auth_code); // Route Officer
        fprintf(fp, "<route_uid>%s</route_uid>", gRoute_Off_UID); // Route officer
    }
    else
    {
        fprintf(fp, "<route_off_auth>%s</route_off_auth>", gauth_transaction_code); // Dealer
        fprintf(fp, "<route_uid>%s</route_uid>", MemberUID); // Dealer
    }

    fprintf(fp, "<shopNo>%s</shopNo>", szShopId);

    memset(SessionId, 0, sizeof(SessionId));
    database_queries obj;

    if(obj.getSessionId(SessionId) < 0)
    {
        miscObj.DisplayMsgBox("Session Id not found\nPlease try again");
        return -1;
    }

    int rows = ui->tableWidgetStockTable_3->rowCount();

    for(int i = 0; i<rows; i++)
    {
            fprintf(fp, "<%s>", "stockNewBean");
            char buf1[32];
            memset(buf1, 0, sizeof(buf1));
            QStringList list;
            list = ui->tableWidgetStockTable_3->item(i,0)->text().split('(', QString::SkipEmptyParts);
            qDebug() << "LIST ====" << list;
            strcpy(buf1, list.at(0).trimmed().toAscii().data());

            fprintf(fp, "<commCode>%s</commCode>", ui->tableWidgetStockTable_3->item(i,6)->text().toAscii().data());
            fprintf(fp, "<commName>%s</commName>", ui->tableWidgetStockTable_3->item(i,0)->text().toAscii().data());
            fprintf(fp, "<KRA>%s</KRA>", ui->tableWidgetStockTable_3->item(i,2)->text().toAscii().data()); //Allot
            fprintf(fp, "<receiveQty>%s</receiveQty>", ui->tableWidgetStockTable_3->item(i,5)->text().toAscii().data()); //DelvQty
            fprintf(fp, "<releasedQty>%s</releasedQty>", ui->tableWidgetStockTable_3->item(i,4)->text().toAscii().data()); //ROQTY
            fprintf(fp,"<shemeId>%s</shemeId>", ui->tableWidgetStockTable_3->item(i,7)->text().toAscii().data());
            fprintf(fp,"<allotedMonth>%s</allotedMonth>",ui->tableWidgetStockTable_3->item(i,8)->text().toAscii().data());
            fprintf(fp,"<allotedYear>%s</allotedYear>",ui->tableWidgetStockTable_3->item(i,9)->text().toAscii().data());
            fprintf(fp, "</%s>", "stockNewBean");
    }

    fprintf(fp, "<truckChitNo>%s</truckChitNo>", ui->comboTruckChitNo->currentText().toAscii().data());
    fprintf(fp, "<truckNo>%s</truckNo>", ui->label_TruckNo->text().toAscii().data());
    fprintf(fp, "</%s>", "Stock_Entry");
    fprintf(fp, "<password>%s</password>", HD_PASSWORD);
    fprintf(fp, "<fpsSessionId>%s</fpsSessionId>",SessionId);
    fprintf(fp, "<stateCode>%s</stateCode>", stateCode.toAscii().data());
    fprintf(fp, "</%s>", "ns1:stockInfoUpdate");
    fprintf(fp, "</%s>", "SOAP-ENV:Body");
    fprintf(fp, "</%s>", "SOAP-ENV:Envelope");
    fclose(fp);

    qDebug() << "******** /mnt/jffs2/StockUpdateReq.xml *******";
    systemret = system("cat /mnt/jffs2/StockUpdateReq.xml");
    qDebug() << "***************************************************";

    GPRS objGprs;

    popup_msg *dlgPopup1 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup1->ui->lblPopup->setText("Please Wait... \n Uploading Recieved Stock Data to Server...");
    else if(LangMode == 2)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें... \n सर्वर में प्राप्त स्टॉक डेटा अपलोड हो रहा है"));
    else if(LangMode == 5)
        dlgPopup1->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करा ... \n प्राप्त झालेली स्टॉक डेटा सर्व्हरवर अपलोड करीत आहे."));
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
    memset(cUrlName, 0, sizeof(cUrlName));

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

    sprintf(cmdCurl, "curl -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", "/mnt/jffs2/StockUpdateReq.xml", cUrlName,  "/mnt/jffs2/StockUpdateResp.xml", responseTimedOutTimeInSec.toInt());
    qDebug()<<"\nCurl Cmd : "<<cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, EPDS_SERVER) < 0)
    {
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

    //QSqlQuery query;
    char TmpStr[1024];
    query.clear();
    if(!query.exec("select (select count() from sqlite_master where type='table' and name = 'KeyRegister'),(select count() from sqlite_master where type='table' and name = 'Pos_Ob'),(select count() from sqlite_master where type='table' and name = 'BenfiaryTxn')"))
    {

    }
    query.next();
    if((query.value(0).toString().toInt()==1)&&(query.value(1).toString().toInt()==1)&&(query.value(2).toString().toInt()==1))
    {
            for(int i = 0;i<ui->tableWidgetStockTable_3->rowCount();i++)
            {
                    memset(TmpStr,0x00,sizeof(TmpStr));
                    sprintf(TmpStr,"update Pos_Ob set closingBalance=closingBalance+%0.2f where commCode = '%s'",ui->tableWidgetStockTable_3->item(i,4)->text().toFloat(),ui->tableWidgetStockTable_3->item(i,6)->text().toAscii().data());
                    qDebug()<<TmpStr;
                    query.clear();
                    query.prepare(TmpStr);
                    if((!query.exec())||(query.numRowsAffected()==0))
                    {
                    }
                    qDebug()<<"numRowsAffected :"<<query.numRowsAffected();

                    memset(TmpStr,0x00,sizeof(TmpStr));
                    sprintf(TmpStr,"insert into OfflineStockReceive values('%s','%s','%s','%s','%s','%s','%s','%s',(select distinct(AllotMonth) from KeyRegister limit 1),(select distinct(AllotYear) from KeyRegister limit 1),'%02d-%02d-%04d %02d:%02d:%02d','Y','O')",ui->comboTruckChitNo->currentText().toAscii().data(),dispachId,ui->label_TruckNo->text().toAscii().data(),ui->ldtShpId->text().toAscii().data(),ui->tableWidgetStockTable_3->item(i,7)->text().toAscii().data(),ui->tableWidgetStockTable_3->item(i,6)->text().toAscii().data(),ui->tableWidgetStockTable_3->item(i,4)->text().toAscii().data(),"",QDateTime::currentDateTime().toString("dd").toInt(),QDateTime::currentDateTime().toString("MM").toInt(),QDateTime::currentDateTime().toString("yyyy").toInt(),QDateTime::currentDateTime().toString("hh").toInt(),QDateTime::currentDateTime().toString("mm").toInt(),QDateTime::currentDateTime().toString("ss").toInt());
                    qDebug()<<TmpStr;
                    query.clear();
                    query.prepare(TmpStr);
                    if(!query.exec())
                    {

                    }
            }
    }
    parseStockInforUpdateResponse((char *)"/mnt/jffs2/StockUpdateResp.xml");

    return 0;
}

void Widget::Print_ReceiveStock(int printflag)
{
    int iCount = 0;
    char prn[256];
    QSqlQuery qry;
    char cShopID[16];

    qry.clear();
    qry.prepare("SELECT ShopID FROM ShopOwnerDetails");
    if(!qry.exec())
        qDebug() << qry.lastError();
    else
    {
        while(qry.next())
        {
            memset(cShopID, 0, sizeof(cShopID));
            strcpy(cShopID, qry.value(0).toString().toAscii().data());
        }
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

    prn_open();

    iCount =ui->tableWidgetStockTable_3->rowCount();
    qDebug() << "********iCount = " << iCount;

    database_queries obj;
    int LangMode = obj.getLangMode();

    int j=printflag;
    for(int i = 0; i < printflag; i++)
    {

        LogoPrint objLogo;
        objLogo.PrintLogo(stateCode.toInt());

        if(LangMode == 1)
        {
            if(i == 0)
            {
                memset(prn, 0, sizeof(prn));
                sprintf(prn, "%s" ,"    Dealer Copy");
                prn_write_text((unsigned char *)prn, strlen(prn), 2);
            }
            else
            {
                memset(prn, 0, sizeof(prn));
                sprintf(prn, "%s" ,"Route Officer Copy");
                prn_write_text((unsigned char *)prn, strlen(prn), 2);
            }

            memset(prn, 0, sizeof(prn));
            sprintf(prn, "%s" ,"      RECEIPT");
            prn_write_text((unsigned char *)prn, strlen(prn), 2);

            memset(prn, 0, sizeof(prn));
            sprintf(prn, "   Date: %s        Time: %s", gszDashedDt, gszDashedTm);
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));
            strcpy(prn, "------------------------------------------");
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));

            sprintf(prn, "Receipt No  : %s", DYN_StockUpdate_TxnreceiptId.toAscii().data()); // 16112017
            prn_write_text((unsigned char *)prn, strlen(prn), 0);


            memset(prn, 0, sizeof(prn));
            sprintf(prn, "Shop ID     : %s", cShopID);
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));
            sprintf(prn, "Truck Chit No: %s", ui->comboTruckChitNo->currentText().toAscii().data());
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));
            sprintf(prn, "RO          : %s", ui->labelRO->text().toAscii().data());
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));
            sprintf(prn, "Truck No    : %s", ui->label_TruckNo->text().toAscii().data());
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));
            strcpy(prn,"------------------------------------------");
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));
            sprintf(prn,  "%s", "ItemName     Sch     Dispt   Recv(kgs/ltr)");
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            memset(prn, 0, sizeof(prn));
            strcpy(prn,"------------------------------------------");
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            for(int j = 0; j < iCount; j++)
            {
                memset(prn, 0, sizeof(prn));
                sprintf(prn, "%-10s %-5s %0.3f  %0.3f", ui->tableWidgetStockTable_3->item(j,0)->text().toAscii().data(),ui->tableWidgetStockTable_3->item(j,1)->text().toAscii().data(), ui->tableWidgetStockTable_3->item(j,4)->text().toFloat(0), ui->tableWidgetStockTable_3->item(j,5)->text().toFloat(0));
                prn_write_text((unsigned char *)prn, strlen(prn), 0);
            }
        }
        else
        {
            if(i == 0)
            {
                QString tRouteOffCopy = QString::fromUtf8("रूट अधिकारी की कॉपी");
                prepareBmp(tRouteOffCopy, 26, tRouteOffCopy, 26, ALIGN_CENTER);
            }
            else
            {
                QString tDealerCopy = QString::fromUtf8("   डीलर कॉपी");
                prepareBmp(tDealerCopy, 26, tDealerCopy, 26, ALIGN_CENTER);
            }

            QString tRaseedu = QString::fromUtf8("रसीद");
            prepareBmp(tRaseedu, 26, tRaseedu, 26, ALIGN_CENTER);

            QString tDateTime = QString::fromUtf8("दिनांक") + ":" + gszDashedDt + QString::fromUtf8("   समय") + ":" + gszDashedTm;
            prepareBmp(tDateTime, 22, tDateTime, 16, ALIGN_CENTER);

            memset(prn, 0, sizeof(prn));
            strcpy(prn, "------------------------------------------");
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            QString tRaseeduNo  = QString::fromUtf8("रसीद संख्या ")+ ": " + DYN_StockUpdate_TxnreceiptId.toAscii().data(); // 16112017
            prepareBmp(tRaseeduNo, 20, tRaseeduNo, 16, ALIGN_LEFT); // 16112017

            QString tShopID     = QString::fromUtf8("एफपीएस नंबर   ")+ "" + cShopID;
            prepareBmp(tShopID, 22, tShopID, 16, ALIGN_LEFT);

            QString tTruckChitNo     = QString::fromUtf8("ट्रक चिट संख्या:   ")+ "" + ui->comboTruckChitNo->currentText().toAscii().data();
            prepareBmp(tTruckChitNo, 22, tTruckChitNo, 16, ALIGN_LEFT);

            QString tRO     = QString::fromUtf8("आरओ   ")+ "" + ui->labelRO->text().toAscii().data();
            prepareBmp(tRO, 22, tRO, 16, ALIGN_LEFT);

            QString tTRuckNo     = QString::fromUtf8("ट्रक नंबर   ")+ "" + ui->label_TruckNo->text().toAscii().data();
            prepareBmp(tTRuckNo, 22, tTRuckNo, 16, ALIGN_LEFT);

            memset(prn, 0, sizeof(prn));
            strcpy(prn,"------------------------------------------");
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            QString tVastuvu_Header = QString::fromUtf8("वस्तु, ") +QString::fromUtf8(" योजना , ")+ QString::fromUtf8(" भेजें, ") + QString::fromUtf8("प्राप्त स्टॉक")+QString::fromUtf8("kgs/ltr");

            prepareBmp(tVastuvu_Header, 22, tVastuvu_Header, 16, ALIGN_CENTER);

            memset(prn, 0, sizeof(prn));
            strcpy(prn,"------------------------------------------");
            prn_write_text((unsigned char *)prn, strlen(prn), 0);

            for(int j = 0; j < iCount; j++)
            {
                memset(prn, 0, sizeof(prn));
                sprintf(prn, "%-10s  %-5s %0.3f  %0.3f", ui->tableWidgetStockTable_3->item(j,0)->text().toAscii().data(),ui->tableWidgetStockTable_3->item(j,1)->text().toAscii().data(), ui->tableWidgetStockTable_3->item(j,4)->text().toFloat(0), ui->tableWidgetStockTable_3->item(j,5)->text().toFloat(0));
                prn_write_text((unsigned char *)prn, strlen(prn), 0);
            }
        }

        memset(prn, 0, sizeof(prn));
        strcpy(prn, "------------------------------------------");
        prn_write_text((unsigned char *)prn, strlen(prn), 0);
        prn_paper_feed(5);
    }

    prn_close();
    return;
}

int Widget::parseStockInforUpdateResponse(char *filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In ParsingFirstCondition = " << filename;
        return -1;
    }

    if(file.size() == 0)
        return -1;

    QString QreceiptId;

    QXmlStreamReader xml(&file);

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "receiptId")
            {
                xml.readNext();
                QreceiptId = xml.text().toString();
                qDebug() << "receiptId : " << QreceiptId;
            }
        }
    }
    // DYN_StockUpdate_TxnreceiptId value assignment
    DYN_StockUpdate_TxnreceiptId.clear();
    DYN_StockUpdate_TxnreceiptId = QreceiptId;
    file.close();

    return 0;
}

bool Widget::getRecvGoodsRuteOfficerStatus()
{
    char sts[4];
    memset(sts, 0, sizeof(sts));

    QSqlQuery query;
    query.prepare("SELECT route_off_enable from Settings");
    if(!query.exec())
    {
        qDebug() << query.lastError();
        return false;
    }
    else
    {
        while(query.next())
        {
            strcpy(sts, query.value(0).toString().toAscii().data());
        }
    }
    query.clear();

    qDebug()<<"Status(RO) : "<<sts;
    if(strcmp(sts, "N") == 0)
    {
        return false;
    }
    return true;
}
