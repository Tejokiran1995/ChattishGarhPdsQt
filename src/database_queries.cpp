
#include "include/database_queries.h"
#include "include/widget.h"


database_queries::database_queries()
{
}


void database_queries::getShopId_fromdb(char *ShopId)
{
    QSqlQuery query;
    query.prepare("SELECT ShopID FROM  ShopOwnerDetails");
    if(!query.exec())
        qDebug() << "SELECT ShopID FROM ShopOwnerDetails = " << query.lastError();
    else
    {
        while(query.next())
        {
            strcpy(ShopId, query.value(0).toString().toAscii().data());
        }
    }
}


int database_queries::getLangMode()
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

void database_queries::getdistofficecode(char *distofficecode)
{
    QSqlQuery query;
    query.clear();
    query.prepare("SELECT type FROM CardTypes");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            strcpy(distofficecode, query.value(0).toString().toAscii().data());

        }
    }

}

int database_queries::getSessionId(char *szSId)
{
    memset(szSId, 0, sizeof(szSId));

    QSqlQuery query;

    query.prepare("SELECT fpsSessionId FROM Dealer");
    if(!query.exec())
    {
        qDebug() << query.lastError();
        return -1;
    }
    else
    {
        while(query.next())
        {
            strcpy(szSId, query.value(0).toString().toAscii().data());
        }
    }
    qDebug()<<"SESSION ID : "<<szSId;
    return 0;
}

bool database_queries::getRecvGoodsRuteOfficerStatus()
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

bool database_queries::getRecvGoodsDealerStatus()
{
    char sts[4];
    memset(sts, 0, sizeof(sts));

    QSqlQuery query;
    query.prepare("SELECT dealer_enable from Settings");
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

    qDebug()<<"Status(Dealer) : "<<sts;
    if(strcmp(sts, "N") == 0)
    {
        return false;
    }
    return true;
}

void database_queries::Delete_Cashless_Record(char *buff)
{
    QSqlQuery query;

    query.prepare("DELETE FROM CustomerTxn WHERE TempRRN = ?");
    query.addBindValue(buff);
    if(!query.exec())
        qDebug() << query.lastError();

    return;
}

void database_queries::GetStan(char *dummystan)
{
    char cTermID[16];
    char dummyTermID[16];

    CMisc miscObj;
    miscObj.GetMachineId(cTermID);

    memset(dummyTermID,0,sizeof(dummyTermID));
    strncpy(dummyTermID,&cTermID[3],3);
    qDebug() << "1. " << dummyTermID;

    struct tm tt;
    int currentMinute = 0, currentSecond = 0;

    memset(&tt,0,sizeof(struct tm));
    gl11_getrtc(&tt); //no need to chnage for RD
    currentMinute = tt.tm_min;
    currentSecond = tt.tm_sec;
    qDebug() << "currentMinute = " << currentMinute << " " << "currentSecond = " << currentSecond;

    int CalcVal = ((currentMinute * 60) + currentSecond)/4;
    qDebug() << "CalcVal = " << CalcVal;

    sprintf(dummystan,"%s%03d",dummyTermID, CalcVal);
    qDebug() << "New Stan = " << dummystan;

    return;
}

void database_queries::getCardAcceptorIdentificationCode(char *buff, char *MachineId)
{
    QSqlQuery query;
    QString QBankId, QVendorId;
    query.prepare("SELECT BankID, VendorID FROM BankMasterTable");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            QBankId   = query.value(0).toString();
            QVendorId =  query.value(1).toString();
        }
    }
    sprintf(buff, "%03s%02s%10s", QBankId.toAscii().data(), QVendorId.toAscii().data(), MachineId);
    return;
}

void database_queries::getCardAcceptorName(char *buff)
{
    sprintf(buff, "%s", "AIROLI                 MUMBAI       MHIN");
    return;
}

void database_queries::GetAgentInfo(char *buff)
{
    QSqlQuery query;
    QString QAgentId;
    QString QVendorId;
    query.prepare("SELECT AgentID, VendorID FROM BankMasterTable");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            QAgentId   = query.value(0).toString();
            QVendorId  = query.value(1).toString();
        }
    }
    sprintf(buff, "%s|%s", QAgentId.toAscii().data(), "02");//Hard Coded
    return;
}

void database_queries::getRRN(char *buff)
{
    QSqlQuery query;
    QString RRN;

    query.prepare("SELECT RRN FROM CashlessParams");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            RRN   = query.value(0).toString();
        }
    }
    sprintf(buff, "%s", RRN.toAscii().data());
    return;
}

int database_queries::getEJ_FileName(char *EJ_File)
{
    struct tm *tt;
    time_t  t;
    CMisc miscObj;

    time(&t);
    tt=localtime(&t);

    char RTC[16];
    memset(RTC, 0, sizeof(RTC));
    sprintf(RTC,"%02d%02d%02d%02d%02d%02d",tt->tm_mday,(tt->tm_mon+1),(tt->tm_year+1900)%100, tt->tm_hour, tt->tm_min, tt->tm_sec);

    char szMachineID[16], szAgentId[16];
    memset(szMachineID, 0, sizeof(szMachineID));
    memset(szAgentId, 0, sizeof(szAgentId));
    miscObj.GetMachineId(szMachineID);
    miscObj.GetAgentId(szAgentId);

    sprintf(EJ_File, "/mnt/jffs2/EJlogs_%s_%s_%s.txt", szMachineID, szAgentId, RTC);

    return 0;
}

void database_queries::DeleteTxnsDB()
{
    int iCount = 0;

    QSqlQuery query;
    query.prepare("SELECT COUNT(RationCardId) FROM TransactionMaster");
    if(!query.exec())
        qDebug() << query.lastError();
    else
    {
        while(query.next())
        {
            iCount = query.value(0).toInt(0);
        }
    }
    if(iCount > 5000)
    {
        query.clear();
        query.exec("DELETE FROM TransactionMaster");
    }
    return;
}
