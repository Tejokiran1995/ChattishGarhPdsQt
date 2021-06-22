#include"include/widget.h"
#include"ui_widget.h"
#include"misc.h"
#include"logo_print.h"
int RecvGoodsRowCnt=0;
void Widget::on_RecvGoodsCommEntBack_clicked()
{
        ui->RecvGoodsTruckChitNoLEdit->setText("");
        ui->RecvGoodsVechNoLEdit->setText("");
        ui->stackedWidget->setCurrentIndex(51);
}

void Widget::on_RecvGoodsCommEntSubmit_clicked()
{        
        for(int i=ui->RecvGdsConfirmTable->rowCount()-1;i>=0;i--)
        {
                ui->RecvGdsConfirmTable->removeRow(i);
        }
        ui->RecvGdsConfirmTable->clear();        
        if(LangMode == 1)
                    ui->RecvGdsConfirmTable->setHorizontalHeaderLabels(QString("Commodity;Scheme;Recv Qty;Unit").split(";"));      
        else
                    ui->RecvGdsConfirmTable->setHorizontalHeaderLabels(QString(QString::fromUtf8("Commodity;Scheme;Recv Qty;Unit")).split(";"));        
        ui->RecvGdsConfirmTable->horizontalHeader()->setStyleSheet("QHeaderView { font-size: 12pt;color: rgb(85, 85, 0); }");;

        int row=0;
        for(int i=0;i<ui->RecvGdsCommDEntryTable->rowCount();i++)
        {
                QComboBox *RecvCommData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,0);
                QComboBox *RecvSchemeData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,1);
                QLineEdit *RecvQtyData = (QLineEdit *)ui->RecvGdsCommDEntryTable->cellWidget(i,2);
                if((RecvQtyData->text().toFloat()>0)&&((RecvCommData->currentText().compare("SELECT COMMODITY")!=0)&&((RecvSchemeData->currentText().compare("SELECT SCHEME TYPE")!=0))))
                {
                        ui->RecvGdsConfirmTable->insertRow(row);
                        {
                                ui->RecvGdsConfirmTable->setItem(row,0,new QTableWidgetItem(RecvCommData->currentText()));
                                ui->RecvGdsConfirmTable->item(row, 0)->setFlags( ui->RecvGdsConfirmTable->item(row,0)->flags() ^ Qt::ItemIsEditable);
                                ui->RecvGdsConfirmTable->item(row, 0)->setFlags( ui->RecvGdsConfirmTable->item(row,0)->flags() | Qt::ItemIsSelectable);
                        }
                        {
                                ui->RecvGdsConfirmTable->setItem(row,1,new QTableWidgetItem(RecvSchemeData->currentText()));
                                ui->RecvGdsConfirmTable->item(row, 1)->setFlags( ui->RecvGdsConfirmTable->item(row,1)->flags() ^ Qt::ItemIsEditable);
                                ui->RecvGdsConfirmTable->item(row, 1)->setFlags( ui->RecvGdsConfirmTable->item(row,1)->flags() | Qt::ItemIsSelectable);
                        }
                        {
                                ui->RecvGdsConfirmTable->setItem(row,2,new QTableWidgetItem(RecvQtyData->text()));
                                ui->RecvGdsConfirmTable->item(row, 2)->setFlags( ui->RecvGdsConfirmTable->item(row,2)->flags() ^ Qt::ItemIsEditable);
                                ui->RecvGdsConfirmTable->item(row, 2)->setFlags( ui->RecvGdsConfirmTable->item(row,2)->flags() | Qt::ItemIsSelectable);
                        }
                        {
                                ui->RecvGdsConfirmTable->setItem(row,3,new QTableWidgetItem(ui->RecvGdsCommDEntryTable->item(i,3)->text()));
                                ui->RecvGdsConfirmTable->item(row, 3)->setFlags( ui->RecvGdsConfirmTable->item(row,3)->flags() ^ Qt::ItemIsEditable);
                                ui->RecvGdsConfirmTable->item(row, 3)->setFlags( ui->RecvGdsConfirmTable->item(row,3)->flags() | Qt::ItemIsSelectable);
                        }
                        ui->RecvGdsConfirmTable->setRowHeight(row,25);
                        row++;
                }
        }
        ui->RecvGdsConfirmTable->setColumnWidth(0,120);
        ui->RecvGdsConfirmTable->setColumnWidth(1,70);
        ui->RecvGdsConfirmTable->setColumnWidth(2,80);
        ui->RecvGdsConfirmTable->setColumnWidth(3,50);
        ui->RecvGdsConfirmTable->verticalHeader()->hide();
        ui->RecvGdsConfirmTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->RecvGdsConfirmTable->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->RecvGdsConfirmTable->selectRow(0);
        //qDebug()<<"ui->RecvGdsConfirmTable->rowCount() :"<<ui->RecvGdsConfirmTable->rowCount();
        if(ui->RecvGdsConfirmTable->rowCount()==0)
        {
                if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Please enter the received Quantity");
                else
                        miscObj.tel_DisplayWideMsgBox((char *)"प्राप्त मात्रा दर्ज करें");
                return;
        }
        ui->stackedWidget->setCurrentIndex(53);
}

void Widget::on_RecvGoodsCommEntAdd_clicked()
{
        QSqlQuery query;
        query.clear();
        //query.prepare("select count() from off_commodity_code a,off_economic_scheme b");
        //query.prepare("select count(distinct(a.comm_code))*count(distinct(a.scheme_id)) from off_pos_ob_month a");
        query.prepare("select count(distinct(a.commCode))*count(distinct(b.schemeId)) from commodityMaster a,schemeMaster b");
        if(!query.exec())
        {
                qDebug()<<query.lastError();
        }
        query.next();
        QLineEdit *RecvQtyData = (QLineEdit *)ui->RecvGdsCommDEntryTable->cellWidget(RecvGoodsRowCnt,2);
        if(RecvQtyData->text().toFloat()<=0.00)
        {
                if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Please Enter the Receive Quantity and try to add");
                else
                        miscObj.tel_DisplayWideMsgBox((char *)"कृपया मात्रा प्राप्त करें और जोड़ने का प्रयास करें");
                return;
        }
        qDebug()<<"RecvGoodsRowCnt :"<<RecvGoodsRowCnt;
        RecvGoodsRowCnt++;
        if(RecvGoodsRowCnt<atoi(query.value(0).toString().toAscii().data()))
        {
                ui->RecvGdsCommDEntryTable->showRow(RecvGoodsRowCnt);
        }
        else
        {
                RecvGoodsRowCnt--;
        }
}

void Widget::RecvGoodsSelectCommD(QString Str)
{
        string Tmp=Str.toStdString();
        int x = atoi(Tmp.substr(0,Tmp.find("-")).c_str());
        int y = atoi(Tmp.substr(Tmp.find("-")+1,Tmp.size()).c_str());

        QComboBox *RecvCommData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(x,0);        
        QComboBox *RecvSchemeData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(x,1);
        QLineEdit *RecvQtyData = (QLineEdit *)ui->RecvGdsCommDEntryTable->cellWidget(x,2);
        if((RecvCommData->currentText().compare("SELECT COMMODITY")==0))
        {
                if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Please select Commodity");
                else
                        miscObj.tel_DisplayWideMsgBox((char *)"कृपया कमोडिटी का चयन करें");
                return;
        }
        for(int i=0;i<=RecvGoodsRowCnt;i++)
        {

                QComboBox    *RecvCommDataTmp = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,0);
                if(i==x)
                    continue;
                if((RecvCommData->currentText().compare(RecvCommDataTmp->currentText())==0))
                {                        
                        QComboBox *RecvSchemDataTmp = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,1);                        
                        if(RecvSchemeData->currentText().compare(RecvSchemDataTmp->currentText())==0)
                        {
                                RecvCommData->setCurrentIndex(0);
                                RecvQtyData->setText("0");
                        }
                }
        }
        QSqlQuery query;
        query.clear();
        query.prepare("select measurmentUnit  from commodityMaster where commNameEn=?");
        query.addBindValue(RecvCommData->currentText());
        if(!query.exec())
        {
            qDebug()<<query.lastError();
        }
        query.next();
        ui->RecvGdsCommDEntryTable->item(x,3)->setText(query.value(0).toString());
}

void Widget::RecvGoodsSelectSchemeType(QString Str)
{
        string Tmp=Str.toStdString();
        int x = atoi(Tmp.substr(0,Tmp.find("-")).c_str());
        int y = atoi(Tmp.substr(Tmp.find("-")+1,Tmp.size()).c_str());        
        QComboBox *RecvCommData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(x,0);        
        QComboBox *RecvSchemeData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(x,1);
        if((RecvSchemeData->currentText().compare("SELECT SCHEME TYPE")==0))
        {
                if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Please select Scheme Type");
                else
                        miscObj.tel_DisplayWideMsgBox((char *)"कृपया योजना प्रकार का चयन करें");
                return;
        }
        /*if(x<RecvGoodsRowCnt)
        {
                for(int i=RecvGoodsRowCnt;i>x;i--)
                {
                        QComboBox *RecvCommDataTmp = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,0);
                        QComboBox *RecvSchemeDataTmp = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,1);
                        QLineEdit *RecvQtyDataTmp = (QLineEdit *)ui->RecvGdsCommDEntryTable->cellWidget(i,2);
                        RecvCommDataTmp->setCurrentIndex(0);
                        RecvSchemeDataTmp->setCurrentIndex(0);
                        RecvQtyDataTmp->setText("0");

                        ui->RecvGdsCommDEntryTable->hideRow(i);
                        RecvGoodsRowCnt--;
                }
                //return;
        }*/
        //qDebug()<<"Curr:"<<RecvSchemeData->currentText();
        for(int i=0;i<=RecvGoodsRowCnt;i++)
        {
                QComboBox *RecvSchmeDataTmp = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,1);                
                if(i==x)
                    continue;
                if((RecvSchemeData->currentText().compare(RecvSchmeDataTmp->currentText())==0))
                {
                        QComboBox *RecvCommDataTmp = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,0);                        
                        if(RecvCommData->currentText().compare(RecvCommDataTmp->currentText())==0)
                        {
                                RecvSchemeData->setCurrentIndex(0);
                        }
                }
        }
}

void Widget::RecvGoodsRecvQtyEntry(QString Str)
{
        string Tmp=Str.toStdString();
        int x = atoi(Tmp.substr(0,Tmp.find("-")).c_str());
        int y = atoi(Tmp.substr(Tmp.find("-")+1,Tmp.size()).c_str());

        QComboBox *RecvCommData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(x,0);        
        QComboBox *RecvSchemeData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(x,1);        
        QLineEdit *RecvQtyData = (QLineEdit *)ui->RecvGdsCommDEntryTable->cellWidget(x,y);
        if((RecvSchemeData->currentText().compare("SELECT SCHEME TYPE")==0)||(RecvCommData->currentText().compare("SELECT COMMODITY")==0))
        {
                    if(LangMode == 1)
                            miscObj.DisplayWideMsgBox((char *)"Please select Commodity and Scheme Type and Enter the Recv Quanty");
                    else
                            miscObj.tel_DisplayWideMsgBox((char *)"कृपया वस्तु और योजना प्रकार चयन करें और प्राप्त मात्रा दर्ज करें");
                    RecvQtyData->setText("");
                    return;
        }
}

void Widget::on_RecvGoodsSubmitBtn_clicked()
{
        if((ui->RecvGoodsTruckChitNoLEdit->text().size()<5)||(ui->RecvGoodsTruckChitNoLEdit->text().size()>15))
        {
                if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Invalid Truck chit Number");
                else
                        miscObj.tel_DisplayWideMsgBox((char *)"अमान्य ट्रक चिट संख्या !");
                return;
        }
        if((ui->RecvGoodsVechNoLEdit->text().size()<5)||(ui->RecvGoodsVechNoLEdit->text().size()>10))
        {
                if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Invalid Vechile Number");
                else
                        miscObj.tel_DisplayWideMsgBox((char *)"अमान्य वाहन संख्या !");
                return;
        }
        popup_msg *dlgPopup1 = new popup_msg(this);
        if(LangMode == 1)
                dlgPopup1->ui->lblPopup->setText("Getting Data \nPlease Wait");
        else
                dlgPopup1->ui->lblPopup->setText(trUtf8("डेटा प्राप्त हो रहा है\n कृपया प्रतीक्षा करें!"));


        QSqlQuery query,query1,query2;
        for(int i=ui->RecvGdsCommDEntryTable->rowCount()-1;i>=0;i--)
        {
                ui->RecvGdsCommDEntryTable->removeRow(i);
        }
        ui->RecvGdsCommDEntryTable->clear();
        if(LangMode == 1)
                    ui->RecvGdsCommDEntryTable->setHorizontalHeaderLabels(QString("Commodity;Scheme;Recv Qty;Unit").split(";"));        
        else
                    ui->RecvGdsCommDEntryTable->setHorizontalHeaderLabels(QString(QString::fromUtf8("Commodity;Scheme;Recv Qty;Unit")).split(";"));

        ui->RecvGdsCommDEntryTable->horizontalHeader()->setStyleSheet("QHeaderView { font-size: 12pt;color: rgb(85, 85, 0); }");;
        query2.clear();
        //query2.prepare("select count() from off_commodity_code a,off_economic_scheme b");
        query2.prepare("select count(distinct(a.commCode))*count(distinct(b.schemeId)) from commodityMaster a,schemeMaster b");
        if(!query2.exec())
        {
                qDebug()<<query2.lastError();
                dlgPopup1->close();
                dlgPopup1->deleteLater();
                if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Please Download the Data");
                else
                        miscObj.tel_DisplayWideMsgBox((char *)"कृपया डेटा डाउनलोड करें!");
                return;
        }
        RecvGoodsRowCnt=0;
        int row=0;
        query2.next();
        //qDebug()<<"atoi(query2.value(0).toString().toAscii().data() :"<<atoi(query2.value(0).toString().toAscii().data());
        QSignalMapper *RecvQtyMapper = new QSignalMapper();
        QSignalMapper *SelCommMapper = new QSignalMapper();
        QSignalMapper *SelSchmeMapper = new QSignalMapper();
        while(row<atoi(query2.value(0).toString().toAscii().data()))
        {
                ui->RecvGdsCommDEntryTable->insertRow(row);
                query.clear();
                query1.clear();
                //query.prepare("select distinct(comm_name_eng) from off_commodity_code");
                query.prepare("select a.commNameEn from commodityMaster a where a.commCode IN(select distinct(commCode) from commodityMaster)");

                //query1.prepare("select distinct(scheme_desc) from off_economic_scheme");
                query1.prepare("select a.schemeName from schemeMaster a where a.schemeId IN(select distinct(schemeId) from schemeMaster)");
                if((!query.exec())||(!query1.exec()))
                {
                        qDebug()<<query.lastError();
                        qDebug()<<query1.lastError();
                }
                {
                        QComboBox *SelectComm = new QComboBox();
                        SelectComm->addItem("SELECT COMMODITY");
                        while(query.next())
                        {
                                SelectComm->addItem(query.value(0).toString());
                        }
                        ui->RecvGdsCommDEntryTable->setCellWidget(row,0,SelectComm);
                        SelCommMapper->setMapping(SelectComm,QString("%1-%2").arg(row).arg(2));
                        connect(SelectComm,SIGNAL(activated(QString)),SelCommMapper,SLOT(map()));
                }
                {
                        QComboBox *SelectSchemeType = new QComboBox();
                        SelectSchemeType->addItem("SELECT SCHEME TYPE");
                        while(query1.next())
                        {
                                SelectSchemeType->addItem(query1.value(0).toString());
                        }
                        ui->RecvGdsCommDEntryTable->setCellWidget(row,1,SelectSchemeType);
                        SelSchmeMapper->setMapping(SelectSchemeType,QString("%1-%2").arg(row).arg(2));
                        connect(SelectSchemeType,SIGNAL(activated(QString)),SelSchmeMapper,SLOT(map()));
                }
                {
                        QLineEdit *RecvQty = new QLineEdit("0");
                        RecvQty->setValidator(new QDoubleValidator(0, 5, 3, RecvQty));
                        ui->RecvGdsCommDEntryTable->setCellWidget(row,2,RecvQty);
                        RecvQtyMapper->setMapping(RecvQty,QString("%1-%2").arg(row).arg(2));
                        connect(RecvQty,SIGNAL(textEdited(QString)),RecvQtyMapper,SLOT(map()));
                }
                {
                        ui->RecvGdsCommDEntryTable->setItem(row,3,new QTableWidgetItem(""));
                        ui->RecvGdsCommDEntryTable->item(row, 3)->setFlags( ui->RecvGdsCommDEntryTable->item(row,3)->flags() ^ Qt::ItemIsEditable);
                        ui->RecvGdsCommDEntryTable->item(row, 3)->setFlags( ui->RecvGdsCommDEntryTable->item(row,3)->flags() | Qt::ItemIsSelectable);
                }
                ui->RecvGdsCommDEntryTable->setRowHeight(row,25);
                if(row!=0)
                {
                        ui->RecvGdsCommDEntryTable->hideRow(row);
                        //ui->RecvGdsCommDEntryTable->showRow(RecvGoodsRowCnt);
                }
                row++;
        }
        connect(RecvQtyMapper, SIGNAL(mapped(const QString &)),this, SLOT(RecvGoodsRecvQtyEntry(QString)));
        connect(SelCommMapper, SIGNAL(mapped(const QString &)),this, SLOT(RecvGoodsSelectCommD(QString)));
        connect(SelSchmeMapper, SIGNAL(mapped(const QString &)),this, SLOT(RecvGoodsSelectSchemeType(QString)));
        ui->RecvGdsCommDEntryTable->setColumnWidth(0,120);
        ui->RecvGdsCommDEntryTable->setColumnWidth(1,70);
        ui->RecvGdsCommDEntryTable->setColumnWidth(2,80);
        ui->RecvGdsCommDEntryTable->setColumnWidth(3,50);
        ui->RecvGdsCommDEntryTable->verticalHeader()->hide();
        ui->RecvGdsCommDEntryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->RecvGdsCommDEntryTable->setSelectionMode(QAbstractItemView::SingleSelection);
        //ui->RecvGdsCommDEntryTable->selectRow(0);
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        ui->stackedWidget->setCurrentIndex(52);
}

void Widget::on_RecvGoodsBackBtn_clicked()
{
        ui->RecvGoodsTruckChitNoLEdit->clear();
        ui->RecvGoodsVechNoLEdit->clear();
        ui->stackedWidget->setCurrentIndex(1);
}
void Widget::PrintRecvGoodsRecpt()
{
        char TmpStr[1000];
        QString TmpString;
        QSqlQuery query;
        popup_msg *dlgPopup1 = new popup_msg(this);
        if(LangMode == 1)
                dlgPopup1->ui->lblPopup->setText("Printing Receive Goods Receipt\nPlease Wait ..");
        else
                dlgPopup1->ui->lblPopup->setText(trUtf8(" सामान रसीद प्रिंट हो रही है\nकृपया प्रतीक्षा करें .."));

        LogoPrint logo;
        logo.PrintLogo();
        //if((LanguageSelected == ENGLISHLANG)||(LanguageSelected == HINDILANG))
        {
                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"   Department of Food,Civil Supplies ");
                prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);


                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"Receive Goods Receipt");
                prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),2);

                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"------------------------------------------");
                prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);

                query.clear();
                query.prepare("select del_name,fps_id from off_common_fps");
                if(!query.exec())
                {
                    qDebug()<<query.lastError();
                }
                query.next();
                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"FPS Owner Name : %s",query.value(0).toString().toAscii().data());
                prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);


                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"FPS ID.        : %s",query.value(1).toString().toAscii().data());
                prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);

                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"TRUCK CHIT NO  : %s",ui->RecvGoodsTruckChitNoLEdit->text().toAscii().data());
                prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);

                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"VECHILE NO     : %s",ui->RecvGoodsVechNoLEdit->text().toAscii().data());
                prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);

                //GetDateandTime();
                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"Date : %02d/%02d/%04d      Time : %02d:%02d:%02d",QDateTime::currentDateTime().toString("dd").toInt(),QDateTime::currentDateTime().toString("MM").toInt(),QDateTime::currentDateTime().toString("yyyy").toInt(),QDateTime::currentDateTime().toString("hh").toInt(),QDateTime::currentDateTime().toString("mm").toInt(),QDateTime::currentDateTime().toString("ss").toInt());
                prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);

                query.clear();
                query.prepare("select month,year from off_pos_ob_month limit 1");
                if(!query.exec())
                {
                    qDebug()<<query.lastError();
                }
                query.next();

                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"Allocation Month/Year : %02d/%04d",query.value(0).toString().toInt(),query.value(1).toString().toInt());
                prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);

                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"                                    ");
                prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);

                memset(TmpStr,0x00,sizeof(TmpStr));
                sprintf(TmpStr,"Commodity(Unit)      Scheme   R.Qty");
                prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);
        }
        memset(TmpStr,0x00,sizeof(TmpStr));
        sprintf(TmpStr,"------------------------------------------");
        prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);

        char TmpData[50];
        for(int i=0;i<ui->RecvGdsCommDEntryTable->rowCount();i++)
        {
                QComboBox *RecvCommData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,0);
                QComboBox *RecvSchemeData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,1);
                QLineEdit *RecvQtyData = (QLineEdit *)ui->RecvGdsCommDEntryTable->cellWidget(i,2);
                if((RecvQtyData->text().toFloat()>0)&&((RecvCommData->currentText().compare("SELECT COMMODITY")!=0)&&((RecvSchemeData->currentText().compare("SELECT SCHEME TYPE")!=0))))
                {
                        memset(TmpData,0x00,sizeof(TmpData));
                        sprintf(TmpData,"%s(%s)",RecvCommData->currentText().toAscii().data(),ui->RecvGdsCommDEntryTable->item(i,3)->text().toAscii().data());
                        memset(TmpStr,0x00,sizeof(TmpStr));
                        sprintf(TmpStr,"%-21s %-7s %-0.3f",TmpData,RecvSchemeData->currentText().toAscii().data(),RecvQtyData->text().toFloat());
                        prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);
                }
        }
        memset(TmpStr,0x00,sizeof(TmpStr));
        sprintf(TmpStr,"------------------------------------------");
        prn_write_text((unsigned char *)TmpStr,strlen(TmpStr),0);

        prn_paper_feed(5);

        prn_close();
        dlgPopup1->close();
        dlgPopup1->deleteLater();
        ui->RecvGoodsTruckChitNoLEdit->setText("");
        ui->RecvGoodsVechNoLEdit->setText("");
        ui->stackedWidget->setCurrentIndex(51);
}

void Widget::on_RecvGdsConfirmSubmit_clicked()
{
        char TmpStr[500];
        prn_open();
        CMisc objMisc;
        if(objMisc.Battery_check() < 0)
        {
                prn_close();
                if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"Unable To Print Recipt due to Low Battery. \n Please charge the Battery");
                else
                        miscObj.tel_DisplayWideMsgBox((char *)"बैटरी कम है...\nकृपया चार्जर कनेक्ट कीजिये");
                return;
        }
        if(prn_paperstatus()<0)
        {
                prn_close();
                if(LangMode == 1)
                        miscObj.DisplayWideMsgBox((char *)"please place paper");
                else
                        miscObj.tel_DisplayWideMsgBox((char *)"कृपया कागज डालें");
                 return;
        }
        QSqlQuery query;
        for(int i=0;i<ui->RecvGdsCommDEntryTable->rowCount();i++)
        {
                QComboBox *RecvCommData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,0);
                QComboBox *RecvSchemeData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,1);
                QLineEdit *RecvQtyData = (QLineEdit *)ui->RecvGdsCommDEntryTable->cellWidget(i,2);
                if((RecvQtyData->text().toFloat()>0)&&((RecvCommData->currentText().compare("SELECT COMMODITY")!=0)&&((RecvSchemeData->currentText().compare("SELECT SCHEME TYPE")!=0))))
                {
                        memset(TmpStr,0x00,sizeof(TmpStr));
                        //sprintf(TmpStr,"select count() from Pos_Ob where comm_code = (select comm_code from off_commodity_code where comm_name_eng = '%s') and scheme_id = (select scheme_id from off_economic_scheme where scheme_desc = '%s')",RecvCommData->currentText().toAscii().data(),RecvSchemeData->currentText().toAscii().data());
                        sprintf(TmpStr,"select count() from Pos_Ob where commCode = (select commCode from commodityMaster where commNameEn = '%s')",RecvCommData->currentText().toAscii().data());
                        qDebug()<<TmpStr;
                        query.clear();
                        query.prepare(TmpStr);
                        if(!query.exec())
                        {
                                memset(TmpStr,0x00,sizeof(TmpStr));
                                sprintf(TmpStr,"%s(%s) Commodity not allowed for Receiving ",RecvCommData->currentText().toAscii().data(),RecvSchemeData->currentText().toAscii().data());
                                miscObj.DisplayWideMsgBox((char *)TmpStr);
                                prn_close();
                                return;
                        }
                        query.next();
                        if(atoi(query.value(0).toString().toAscii().data())<=0)
                        {
                                memset(TmpStr,0x00,sizeof(TmpStr));
                                sprintf(TmpStr,"%s(%s) Commodity not allowed for Receiving ",RecvCommData->currentText().toAscii().data(),RecvSchemeData->currentText().toAscii().data());
                                miscObj.DisplayWideMsgBox((char *)TmpStr);
                                prn_close();
                                return;
                        }
                }
        }
        for(int i=0;i<ui->RecvGdsCommDEntryTable->rowCount();i++)
        {
                QComboBox *RecvCommData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,0);
                QComboBox *RecvSchemeData = (QComboBox *)ui->RecvGdsCommDEntryTable->cellWidget(i,1);
                QLineEdit *RecvQtyData = (QLineEdit *)ui->RecvGdsCommDEntryTable->cellWidget(i,2);
                if((RecvQtyData->text().toFloat()>0)&&((RecvCommData->currentText().compare("SELECT COMMODITY")!=0)&&((RecvSchemeData->currentText().compare("SELECT SCHEME TYPE")!=0))))
                {

                        memset(TmpStr,0x00,sizeof(TmpStr));
                        sprintf(TmpStr,"update Pos_Ob set closingBalance=closingBalance+%0.2f where commCode = (select commCode from commodityMaster where commNameEn = '%s')",RecvQtyData->text().toFloat(),RecvCommData->currentText().toAscii().data());
                        qDebug()<<TmpStr;
                        query.clear();
                        query.prepare(TmpStr);                        
                        if((!query.exec())||(query.numRowsAffected()==0))
                        {
                                qDebug()<<"Stock Receive :"<<query.lastError();
                                memset(TmpStr,0x00,sizeof(TmpStr));
                                sprintf(TmpStr,"%s(%s) Commodity not allowed for Receiving ",RecvCommData->currentText().toAscii().data(),RecvSchemeData->currentText().toAscii().data());
                                miscObj.DisplayWideMsgBox((char *)TmpStr);
                                prn_close();
                                return;
                        }
                        qDebug()<<"numRowsAffected :"<<query.numRowsAffected();
                        memset(TmpStr,0x00,sizeof(TmpStr));

                        sprintf(TmpStr,"insert into off_stock_receipt values('%s','%s','%s','%s',(select commCode from commodityMaster where commNameEn = '%s'),(select schemeId from schemeMaster where schemeName = '%s'),'%s',(select distinct(month) from KeyRegister limit 1),(select distinct(year) from KeyRegister limit 1),'%02d-%02d-%04d %02d:%02d:%02d')",ui->RecvGoodsTruckChitNoLEdit->text().toAscii().data(),ui->RecvGoodsVechNoLEdit->text().toAscii().data(),ui->ldtShpId->text().toAscii().data(),RecvQtyData->text().toAscii().data(),RecvCommData->currentText().toAscii().data(),RecvSchemeData->currentText().toAscii().data(),RecvQtyData->text().toAscii().data(),QDateTime::currentDateTime().toString("dd").toInt(),QDateTime::currentDateTime().toString("MM").toInt(),QDateTime::currentDateTime().toString("yyyy").toInt(),QDateTime::currentDateTime().toString("hh").toInt(),QDateTime::currentDateTime().toString("mm").toInt(),QDateTime::currentDateTime().toString("ss").toInt());
                        qDebug()<<TmpStr;


                        query.clear();
                        query.prepare(TmpStr);
                        if(!query.exec())
                        {
                                qDebug()<<"Stock Receive :"<<query.lastError();
                                prn_close();
                                return;
                        }
                }
        }
        PrintRecvGoodsRecpt();
}


void Widget::on_RecvGdsConfirmBack_clicked()
{        
        ui->stackedWidget->setCurrentIndex(52);
}

