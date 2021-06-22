#include "include/parse.h"
#include "include/widget.h"


parse::parse()
{
}
void parse::deleteTables()
{
    QSqlQuery query;
    query.exec("DELETE FROM CustomerMaster");
    query.exec("DELETE FROM BenQuotaTable");
    query.exec("DELETE FROM ItemMaster");
    query.exec("DELETE FROM CustomerFamily");
}

int parse::parseCustMaster()
{
    int ret;
    qDebug()<<"CustemerMaster Parsing....";
    QFile file("/mnt/jffs2/CustomerMaster.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"CustomerMaster.csv file Open failed";
        return -1;
    }

    QTextStream in(&file);
    QString line;

    QSqlQuery query;

    while(!in.atEnd())
    {
        line = in.readLine();
        qDebug() << "Line = " << line << endl;

        QStringList fields = line.split(",");
        query.prepare("INSERT INTO CustomerMaster(RationCardId, CustomerName, DistrictCode, MandalCode, UID, EID, CardType, Panchayat, Mandal, District, MemberCount, Gender, PIN) values(:RationCardId, :CustomerName, :DistrictCode, :MandalCode, :UID, :EID, :CardType, :Panchayat, :Mandal, :District, :MemberCount, :Gender, :PIN)");
        query.bindValue(":RationCardId",fields.at(0));
        query.bindValue(":CustomerName",fields.at(1));
        query.bindValue(":DistrictCode",fields.at(2));
        query.bindValue(":MandalCode",fields.at(3));
        query.bindValue(":UID",fields.at(4));
        query.bindValue(":EID",fields.at(5));//.toUtf8().data());
        query.bindValue(":CardType",fields.at(6));
        query.bindValue(":Panchayat",fields.at(7));//.toUtf8().data());
        query.bindValue(":Mandal",fields.at(8));//.toUtf8().data());
        query.bindValue(":District",fields.at(9));//.toUtf8().data());
        query.bindValue(":MemberCount",fields.at(10));
        query.bindValue(":Gender",fields.at(11));
        query.bindValue(":PIN",fields.at(12));
        ret = query.exec();
        if(ret == false){ qDebug()<<query.lastError();return -1;}
    }
    file.close();
    return 0;
}

int parse :: parseBenQuota()
{
    int ret;
    qDebug()<<"BenQuota Parsing....";
    QFile file("/mnt/jffs2/BenQuota.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    QTextStream in(&file);
    QString line;

    QSqlQuery query;

    while(!in.atEnd())
    {
        line = in.readLine();
        qDebug() << "Line = " << line << endl;

        QStringList fields = line.split(",");


        query.prepare("INSERT INTO BenQuotaTable (RationCardId, CardHolderName, BalQty, Month, Year, MonthlyQuota, ItemCode, ItemType, ItemPrice) values( :RationCardId, :CardHolderName, :BalQty, :Month, :Year, :MonthlyQuota, :ItemCode, :ItemType, :ItemPrice)");
        query.bindValue(":RationCardId",fields.at(0));
        query.bindValue(":CardHolderName",fields.at(1));
        query.bindValue(":UID",fields.at(2));
        query.bindValue(":Month",fields.at(3));
        query.bindValue(":Year",(fields.at(4)));
        query.bindValue(":MonthlyQuota",fields.at(5));
        query.bindValue(":ItemCode",fields.at(6));
        query.bindValue(":ItemType",(fields.at(7)));
        query.bindValue(":ItemPrice",fields.at(8));

        ret = query.exec();
        if(ret == false){ qDebug()<<query.lastError();return -1;}
    }
    file.close();

    return 0;
}



int parse :: parseItemMaster()
{
    int ret;
    qDebug()<<"ItemMaster Parsing....";
    QFile file("/mnt/jffs2/ItemMaster.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    QTextStream in(&file);
    QString line;

    QSqlQuery query;

    while(!in.atEnd())
    {
        line = in.readLine();
        qDebug() << "Line = " << line << endl;

        QStringList fields = line.split(",");

        query.prepare("INSERT INTO ItemMaster(ItemCode, ItemType, ItemNameEn, ItemNameTel, ItemPrice, Month, Year) values(:ItemCode, :ItemType, :ItemNameEn, :ItemNameTel, :ItemPrice, :Month, :Year)");
        query.bindValue(":ItemCode",fields.at(0));
        query.bindValue(":ItemType",fields.at(1));
        query.bindValue(":ItemNameEn",fields.at(2));
        query.bindValue(":ItemNameTel",fields.at(3));
        query.bindValue(":ItemPrice",fields.at(4));
        query.bindValue(":Month",(fields.at(5)));
        query.bindValue(":Year",(fields.at(6)));

        ret = query.exec();
        if(ret == false){ qDebug()<<query.lastError();return -1;}
    }
    file.close();

    return 0;
}



int parse :: parseCustomerFamily()
{
    int ret;
    qDebug()<<"CustomerFamily parsing..";
    QFile file("/mnt/jffs2/CustomerFamily.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    QTextStream in(&file);
    QString line;

    QSqlQuery query;

    while(!in.atEnd())
    {
        line = in.readLine();
        qDebug() << "Line = " << line << endl;

        QStringList fields = line.split(",");

        query.prepare("INSERT INTO CustomerFamily (MemberNo	, RationCardId, MemberNameEn , MemberNameTel, MemberUID , MemberEID , MemGender , MemberRel , UID) values(:MemberNo, :RationCardId, :MemberNameEn , :MemberNameTel, :MemberUID , :MemberEID , :MemGender , :MemberRel , :UID)");
        query.bindValue(":MemberNo",fields.at(0));
        query.bindValue(":RationCardId",fields.at(1));
        query.bindValue(":MemberNameEn",fields.at(2));
        query.bindValue(":MemberNameTel",fields.at(3));
        query.bindValue(":MemberUID",(fields.at(4)));
        query.bindValue(":MemberEID",fields.at(5));//trUtf8(fields.at(4).toUtf8().data())
        query.bindValue(":MemGender",fields.at(6).data());
        query.bindValue(":MemberRel",fields.at(7));
        query.bindValue(":UID",fields.at(8));

        ret = query.exec();
        if(ret == false){ qDebug()<<query.lastError();return -1;}
    }
    file.close();

    return 0;
}

void parse ::ParsingBankMaster()
{
    QString Qagent_Id, Qagent_Name, Qbank_Id, Qbank_name;
    QString Qbank_url, Qpin_Code, Qstate_Code, Qterminal_Id, Qvendor_Id, Qej_url;
    QString Qftp_password, Qftp_url, Qftp_userid;
    QString QErrorCode, QErrorDesc;

    QSqlQuery query;

    query.exec("DELETE FROM ErrorCodes");

    QFile file("/mnt/jffs2/GetBankMasterDetails_Resp.xml");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In ParsingBankMaster, Error Reading File";
        return;
    }

    if(file.size() == 0)
        return;

    QXmlStreamReader xml(&file);

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "agent_Id")
            {
                xml.readNext();
                Qagent_Id = xml.text().toString();
                qDebug() << "Qagent_Id : " << Qagent_Id;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET AgentID=?");
                query.addBindValue(Qagent_Id);
                if(!query.exec())
                    qDebug() << query.lastError();
            }

            if(xml.name() == "agent_Name")
            {
                xml.readNext();
                Qagent_Name = xml.text().toString();
                qDebug() << "Qagent_Name : " << Qagent_Name;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET AgentName=?");
                query.addBindValue(Qagent_Name);
                if(!query.exec())
                    qDebug() << query.lastError();
            }

            if(xml.name() == "bank_Id")
            {
                xml.readNext();
                Qbank_Id = xml.text().toString();
                qDebug() << "Qbank_Id : " << Qbank_Id;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET BankID=?");
                query.addBindValue(Qbank_Id);
                if(!query.exec())
                    qDebug() << query.lastError();
            }

            if(xml.name() == "bank_name")
            {
                xml.readNext();
                Qbank_name = xml.text().toString();
                qDebug() << "Qbank_name : " << Qbank_name;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET BankName=?");
                query.addBindValue(Qbank_name);
                if(!query.exec())
                    qDebug() << query.lastError();
            }

            if(xml.name() == "bank_url")
            {
                xml.readNext();
                Qbank_url = xml.text().toString();
                qDebug() << "Qbank_url : " << Qbank_url;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET BankURL=?");
                query.addBindValue(Qbank_url);
                if(!query.exec())
                    qDebug() << query.lastError();
            }
            if(xml.name() == "ej_url")
            {
                xml.readNext();
                Qej_url = xml.text().toString();
                qDebug() << "Qej_url : " << Qej_url;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET EJ_URL=?");
                query.addBindValue(Qej_url);
                if(!query.exec())
                    qDebug() << query.lastError();
            }

            if(xml.name() == "ftp_password")
            {
                xml.readNext();
                Qftp_password = xml.text().toString();
                qDebug() << "Qftp_password = " << Qftp_password;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET ftp_password=?");
                query.addBindValue(Qftp_password);
                if(!query.exec())
                    qDebug() << query.lastError();
            }

            if(xml.name() == "ftp_url")
            {
                xml.readNext();
                Qftp_url = xml.text().toString();
                qDebug() << "Qftp_url = " << Qftp_url;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET ftp_url=?");
                query.addBindValue(Qftp_url);
                if(!query.exec())
                    qDebug() << query.lastError();
            }

            if(xml.name() == "ftp_userid")
            {
                xml.readNext();
                Qftp_userid = xml.text().toString();
                qDebug() << "Qftp_userid = " << Qftp_userid;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET ftp_userid=?");
                query.addBindValue(Qftp_userid);
                if(!query.exec())
                    qDebug() << query.lastError();

            }

            if(xml.name() == "pin_Code")
            {
                xml.readNext();
                Qpin_Code = xml.text().toString();
                qDebug() << "Qpin_Code : " << Qpin_Code;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET PinCode=?");
                query.addBindValue(Qpin_Code);
                if(!query.exec())
                    qDebug() << query.lastError();
            }

            if(xml.name() == "state_Code")
            {
                xml.readNext();
                Qstate_Code = xml.text().toString();
                qDebug() << "Qstate_Code : " << Qstate_Code;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET StateCode=?");
                query.addBindValue(Qstate_Code  );
                if(!query.exec())
                    qDebug() << query.lastError();
            }

            if(xml.name() == "terminal_Id")
            {
                xml.readNext();
                Qterminal_Id = xml.text().toString();
                qDebug() << "Qterminal_Id : " << Qterminal_Id;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET TerminalID=?");
                query.addBindValue(Qterminal_Id);
                if(!query.exec())
                    qDebug() << query.lastError();
            }

            if(xml.name() == "vendor_Id")
            {
                xml.readNext();
                Qvendor_Id = xml.text().toString();
                qDebug() << "Qvendor_Id : " << Qvendor_Id;

                query.clear();
                query.prepare("UPDATE BankMasterTable SET VendorID=?");
                query.addBindValue(Qvendor_Id);
                if(!query.exec())
                    qDebug() << query.lastError();
            }

            if(xml.name() == "error_code")
            {
                xml.readNext();
                QErrorCode = xml.text().toString();
                qDebug() << "QErrorCode = " << QErrorCode;

            }

            if(xml.name() == "error_desc")
            {
                xml.readNext();
                QErrorDesc = xml.text().toString();
                qDebug() << "QErrorDesc = " << QErrorDesc;

                query.clear();
                query.prepare("INSERT INTO ErrorCodes (Code, Description) VALUES(:Code, :Description)");
                query.bindValue(":Code", QErrorCode);
                query.bindValue(":Description", QErrorDesc);
                if(!query.exec())
                    qDebug() << "In Inserting ErrorCodes = " << query.lastError();
            }
        }
    }

    file.close();
    return;
}

void parse::ParsingBenMasterDetails(char *filename)
{
    QSqlQuery query;
    QString Qbank_name, Qiin_number;

    query.exec("DELETE FROM BenBankTable");

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In ParsingBenMasterDetails, Error Reading File";
        return;
    }

    if(file.size() == 0)
        return;

    QXmlStreamReader xml(&file);

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "bank_name")
            {
                xml.readNext();
                Qbank_name = xml.text().toString();
                qDebug() << "Qbank_name : " << Qbank_name;
            }

            if(xml.name() == "iin_number")
            {
                xml.readNext();
                Qiin_number = xml.text().toString();
                qDebug() << "Qiin_number : " << Qiin_number;

                query.clear();
                query.prepare("INSERT INTO BenBankTable(BankName, IIN) VALUES(:BankName, :IIN)");
                query.bindValue(":BankName", Qbank_name);
                query.bindValue(":IIN", Qiin_number);
                if(!query.exec())
                    qDebug() << query.lastError();
            }
        }
    }

    return;
}

void parse::ParsingCashlessRRN(char *filename)
{
    CMisc miscObj;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "In ParsingCashlessRRN = " << filename;
        return;
    }

    if(file.size() == 0)
        return;

    QString Qrrn_number;
    QSqlQuery query;
    QXmlStreamReader xml(&file);

    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if(token == QXmlStreamReader::StartDocument)
            continue;

        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "rrn_number")
            {
                xml.readNext();
                Qrrn_number = xml.text().toString();
                qDebug() << "Qrrn_number : " << Qrrn_number;

                query.prepare("UPDATE CashlessParams SET RRN=?");
                query.addBindValue(Qrrn_number);
                if(!query.exec())
                    qDebug() << query.lastError();

                char buff[128];
                memset(buff, 0, sizeof(buff));
                sprintf(buff, "RRN From NIC: %s", Qrrn_number.toAscii().data());
                miscObj.DisplayWideMsgBox((char *)buff);
            }
        }
    }

    file.close();

    return;
}


int parse::chkReversal(char *TempRRN)
{
    QSqlQuery query;
    QString QSDate, QTempRRN, QTxnStatus;

    query.prepare("SELECT Date, TempRRN, Txn_Status FROM LastTxnStatus");
    if(!query.exec())
        qDebug() <<  query.lastError();
    else
    {
        while(query.next())
        {
            QSDate      = query.value(0).toString();
            QTempRRN    = query.value(1).toString();
            QTxnStatus  = query.value(2).toString();
        }
    }

    qDebug() << "Date : "<<QSDate;
    qDebug()<<"RRN : "<<QTempRRN;
    qDebug()<<"Txn Status : "<<QTxnStatus;

    if(QTxnStatus.toInt(0) == TXN_STATUS_SUCCESS)
        return -1;

    strcpy(TempRRN, QTempRRN.toAscii().data());

    return 0;
}


void parse::getTempRRN(char *TempRRN)//IND
{
    QSqlQuery query;
    QString QSDate, QTempRRN, QTxnStatus;

    query.prepare("SELECT Date, TempRRN, Txn_Status FROM LastTxnStatus");
    if(!query.exec())
        qDebug() <<  query.lastError();
    else
    {
        while(query.next())
        {
            QSDate      = query.value(0).toString();
            QTempRRN    = query.value(1).toString();
            QTxnStatus  = query.value(2).toString();
        }
    }
    qDebug()<<"Temp RRN : "<<QTempRRN;

    strcpy(TempRRN, QTempRRN.toAscii().data());

}

