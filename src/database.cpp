#include "include/database.h"

bool Database::createTable()
{
    bool ret = false;
    int insRet = 0;

    if(db.open())
    {
        QSqlQuery query;
        if(!query.exec("CREATE TABLE IF NOT EXISTS VersionMaintain(appver TEXT, status TEXT)"))
        {
            qDebug() << "1. " << query.lastError();
            return -1;
        }
        else
            query.exec("INSERT INTO VersionMaintain(appver, status) VALUES('1.0','Y')");

        if(!query.exec("CREATE TABLE IF NOT EXISTS CustomerMaster (RationCardId TEXT, CustomerName TEXT, address TEXT, LpgGasStatus TEXT, LpgType TEXT, CardType TEXT, MemberCount TEXT, OfficeName TEXT, RcId TEXT,  schemeId TEXT, type_card TEXT, znpnsbalance TEXT, headmobileno TEXT)"))
        {
            qDebug() << "2. " << query.lastError();
            return -1;
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS CustomerFamily (RationCardId TEXT, MemberNameEn TEXT, MemberUID TEXT, UID TEXT, bfd_1 TEXT, bfd_2 TEXT, bfd_3 TEXT, MemberFusion TEXT, UID_Status TEXT, MemberId TEXT, MemberNamell TEXT,Memberauth_finger TEXT,Memberauth_iris TEXT,Memberauth_manual TEXT,Memberauth_otp TEXT)"))
        {
            qDebug() << "3. " << query.lastError();
            return -1;
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS ItemMaster (ItemCode TEXT, ItemType TEXT, ItemNameEn TEXT, ItemNameTel TEXT, ItemPrice TEXT, Month TEXT, Year TEXT,MeasureUnit TEXT, PRIMARY KEY(ItemNameEn))"))
        {
            qDebug() << "4. " << query.lastError();
            return -1;
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS BenQuotaTable (RationCardId TEXT, CardHolderName TEXT, BalQty TEXT, MonthlyQuota TEXT, ItemCode TEXT, ItemType TEXT, ItemPrice TEXT, AvailedQty TEXT, MeasureUnit TEXT, RequiredQty TEXT, closingBal TEXT, commNamell TEXT,weighingFlag TEXT,minimumqty TEXT,allocationType TEXT,allotedMonth TEXT,allotedYear TEXT)"))
        {
            qDebug() << "5. " << query.lastError();
            return -1;
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS TransactionMaster (IssueMonth TEXT, RationCardId TEXT, UID TEXT, EID TEXT, MemberUID TEXT, ItemCode TEXT, ItemName TEXT, ItemQty TEXT, SellingPrice TEXT, Amount TEXT, TxnDate TEXT, TxnTime TEXT, ReceiptNo TEXT, DeliveryFlag TEXT, PIN TEXT, CardType TEXT, ShopID TEXT)"))
        {
            qDebug() << "6. " << query.lastError();
            return -1;
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS RationStockMaster (ItemCode TEXT, ItemName TEXT, ItemQty TEXT, Date TEXT,	BasicPrice TEXT, TotalCost TEXT, PIN TEXT)"))
        {
            qDebug() << "7. " << query.lastError();
            return -1;
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS RationOpeningStock (ItemCode TEXT, ItemName TEXT, ItemQty TEXT, Date TEXT,	BasicPrice TEXT, TotalCost TEXT, PIN TEXT)"))
        {
            qDebug() << "8. " << query.lastError();
            return -1;
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS Settings(LangMode TEXT, CommMode TEXT, WM_MacId TEXT, URL TEXT, CurrentDate TEXT, SCMURL TEXT, WeighStatus TEXT, update_app_url TEXT, flag_msg TEXT, fusion_attempts TEXT, helpline_number TEXT, message_eng TEXT, message_tel TEXT, route_off_enable TEXT, dealer_enable TEXT)")) // route_off_enable & dealer_enable added vivek 05082017
        {
            qDebug() << "9. " << query.lastError();
            return -1;
        }
        else
        {
            // CHANGED BY GMB (Lang mode 2)
            insRet = query.exec("INSERT INTO Settings(LangMode, CommMode, WM_MacId, URL, CurrentDate, SCMURL, WeighStatus, update_app_url, flag_msg, fusion_attempts, helpline_number, message_eng, message_tel, route_off_enable, dealer_enable) VALUES('1','1','0','http://epos.ap.gov.in/ePosService/rationcardservice?wsdl','00/00/0000', 'http://epos.ap.gov.in/ScmePosService/stockdetails?wsdl', '0', '0', '0', '0', '0', '0', '0','Y','N')"); //route_off_enable & dealer_enable Value added vivek 05082017
            if(!insRet)
            {
                qDebug()<<"9.1 : "<<query.lastError();
            }

        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS SESSION(SessionId TEXT)"))
        {
            qDebug() << "10. " << query.lastError();
            return -1;
        }
        else
            query.exec("INSERT INTO SESSION(SessionId) VALUES('0')");

        if(!query.exec("CREATE TABLE IF NOT EXISTS ShopOwnerDetails(ShopID TEXT, DealerURL TEXT, Name TEXT, UID TEXT, FpsID TEXT)"))
        {
            qDebug() << "11. " << query.lastError();
            return -1;
        }
        else
        {
            query.exec("INSERT INTO ShopOwnerDetails(ShopID, DealerURL, Name, UID, FpsId) VALUES ('0', '0', '0', '0', '0')");
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS Dealer(ownername TEXT, fpsuidno TEXT, del_bfd_1 TEXT, del_bfd_2 TEXT, del_bfd_3 TEXT, fpsSessionId TEXT, vendorName TEXT, login_time TEXT, dist_code TEXT, dealer_fusion TEXT, auth_type_del TEXT,dealer_type TEXT)"))
        {
            qDebug() << "12. " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS Nominee(nom_name TEXT, nom_uid TEXT, nominee_bfd_1 TEXT, nominee_bfd_2 TEXT, nominee_bfd_3 TEXT, nominee_fusion TEXT, auth_type_nominee TEXT)"))
        {
            qDebug() << "13. " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS StockDetails(alloted_month TEXT,alloted_year TEXT,commodityname TEXT,commoditycode TEXT,scheme_name TEXT,allotQty TEXT,dispatchQty TEXT,ROQuantity TEXT,scheme_code TEXT,commoditycount TEXT,received_quantity TEXT,truck_chit_no TEXT,commoditynamell TEXT)"))
        {
            qDebug() << "14. " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS TruckChitTable(truck_chit_no TEXT, do_ro_no TEXT, comm_length TEXT,dispatchId TEXT,PRIMARY KEY(truck_chit_no))"))//Added comm_length in 4.2 version as per NIC requirement
        {
            qDebug() << "15. " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS RouteOfficerDetails(mobile TEXT, name TEXT, uid TEXT)"))
        {
            qDebug() << "16. " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS getViewStockRecieved(alloted_month TEXT, alloted_year TEXT, comm TEXT, commoditycode TEXT, dispatch_id TEXT, do_ro_no TEXT, entry_date TEXT, KRA TEXT, OB TEXT, QI TEXT, qaD TEXT, qnD TEXT, qtD TEXT, received_quantity TEXT, released_quantity TEXT, stock_received TEXT, truck_chit_no TEXT, truck_no TEXT)"))
        {
            qDebug() << "17. " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS getKeroseneDtls(alloted_month TEXT, alloted_qty TEXT, alloted_year TEXT, cb TEXT, comm_id TEXT, comm_name TEXT, issued TEXT, ob TEXT, recv_qty TEXT, total_recieved TEXT, total_stock TEXT)"))
        {
            qDebug() << "18. " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS ICDSCustomerFamily(member_id TEXT, member_name TEXT, member_uid TEXT)"))
        {
            qDebug() << "19. " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS CardTypes(type TEXT)"))
        {
            qDebug() << "20. " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS BFD(fp_position TEXT, nfiq TEXT)"))
        {
            qDebug() << "21. " << query.lastError();
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS PrintLastResp(bal_qty TEXT, carry_over TEXT, comm_name TEXT, member_name TEXT, reciept_id TEXT, retail_price TEXT, tot_amount TEXT, total_qty TEXT, txn_time TEXT, uid_refer_no TEXT,rcId TEXT,comm_name_ll TEXT,scheme TEXT,member_name_ll TEXT,availedFps TEXT,commAmount TEXT)"))
        {
            qDebug() << "22. " << query.lastError();
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS BFDRanks(fp_position TEXT, Rank TEXT)"))
        {
            qDebug() << "23. " << query.lastError();
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS BankMasterTable(AgentID TEXT, AgentName TEXT, BankID TEXT, BankName TEXT, BankURL TEXT, ftp_password TEXT, ftp_url TEXT, ftp_userid TEXT, PinCode TEXT, StateCode TEXT, TerminalID TEXT, VendorID TEXT, EJ_URL TEXT)"))
        {
            qDebug() << "24. " << query.lastError();
            return -1;
        }
        else
        {
            query.exec("INSERT INTO BankMasterTable(AgentID, AgentName, BankID, BankName, BankURL, ftp_password, ftp_url, ftp_userid, PinCode, StateCode, TerminalID, VendorID, EJ_URL) VALUES ('0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0')");
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS MenuTable(Service TEXT, Slno TEXT, Status TEXT)"))
        {
            qDebug() << "25. " << query.lastError();
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS BenBankTable(BankName TEXT, IIN TEXT)"))
        {
            qDebug() << "26. " << query.lastError();
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS CashlessParams(RRN TEXT)"))
        {
            qDebug() << "27. " << query.lastError();
        }
        else
        {
            query.exec("INSERT INTO CashlessParams(RRN) VALUES ('0')");
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS CustomerTxn(PriAccNum TEXT, ProcessCode TEXT, TxnAmt TEXT, Stan TEXT, LocalTxnTime TEXT, LocalTxnDate TEXT, MerType TEXT, PosEntryMode TEXT, PosCondCode TEXT, AcqIIC TEXT, TempRRN TEXT, CardAccTerID TEXT, CardAccIDC TEXT, CardAccLoc TEXT, AgentInfo TEXT, CurrCode TEXT, ActionDate TEXT, OriginalDataEle TEXT, InterfaceVerNo TEXT, PermanentRRN TEXT, CustAadhaarNo TEXT, CustIIN TEXT, CustName TEXT, AvailBal TEXT, TransactionMode TEXT, ResponseCode TEXT, DateFormat TEXT, TimeFormat TEXT, EJFlag TEXT, TransactionStatus TEXT, TransactionType TEXT, GatewayRRN TEXT, UIDAIAuthRefCode TEXT)"))
        {
            qDebug() << "28. " << query.lastError();
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS LastTxnStatus(Date TEXT, TempRRN TEXT, Txn_Status TEXT)"))
        {
            qDebug() << "29. " << query.lastError();
        }
        else
        {
            query.exec("INSERT INTO LastTxnStatus(Date, TempRRN, Txn_Status) VALUES ('0', '0', '0')");
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS ErrorCodes(Code TEXT, Description TEXT)"))
        {
            qDebug() << "29. " << query.lastError();
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS FestDetails(CommodityCode TEXT, CommodityName TEXT, Price TEXT, Quantity TEXT, Unit TEXT)"))
        {
            qDebug() << "30. " << query.lastError();
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS Headers(pds_cl_tran_eng TEXT, pds_cl_tran_tel TEXT, pds_tran_eng TEXT, pds_tran_tel TEXT)"))
        {
            qDebug() << "30. " << query.lastError();
        }
        else
        {

            insRet = query.exec("INSERT INTO Headers(pds_cl_tran_eng, pds_cl_tran_tel, pds_tran_eng, pds_tran_tel) VALUES('CASHLESS RECEIPT','0','RECEIPT','0')");
            if(!insRet)
            {
                qDebug()<<"4.2 : "<<query.lastError();
            }
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS InspApprovalsDtls(approveKey TEXT, approveValue TEXT)"))
        {
            qDebug() << "InspApprovalsDtls : " << query.lastError();
            return -1;
        }
        qDebug() << "Database Opened" << endl;

        if(!query.exec("CREATE TABLE IF NOT EXISTS InspCommDtls(closingBalance TEXT, commCode TEXT, commNameEn TEXT, commNamell TEXT)"))
        {
            qDebug() << "InspCommDtls : " << query.lastError();
            return -1;
        }
       /**** Service Changed Below InspList and InspNominee need to remove 01072017 vivek ****/

        if(!query.exec("CREATE TABLE IF NOT EXISTS InspList(incharge_mobile TEXT,incharge_name TEXT, incharge_uid TEXT, insp_incharge_bfd1 TEXT, insp_incharge_bfd2 TEXT, insp_incharge_bfd3 TEXT, insp_incharge_fusion TEXT)"))
        {
            qDebug() << "InspList : " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS InspNominee(insp_nom_mobile TEXT,insp_nom_name TEXT, insp_nom_uid TEXT, insp_nom_bfd1 TEXT, insp_nom_bfd2 TEXT, insp_nom_bfd3 TEXT, insp_nom_fusion TEXT)"))
        {
            qDebug() << "InspNominee. " << query.lastError();
            return -1;
        }
    /**** Service Changed Below InspList and InspNominee need to remove 01072017 vivek ***/

        if(!query.exec("CREATE TABLE IF NOT EXISTS TransactionModes(idType TEXT, idValue TEXT, opeValue TEXT, oprMode TEXT)"))
        {
            qDebug() << "TransactionModes. " << query.lastError();
            return -1;
        }
        //Recieve Goods added by vivek 27062017
        if(!query.exec("CREATE TABLE IF NOT EXISTS Stock_Details(entryDate TEXT, AllocationOrderNo TEXT, truckChitNo TEXT, ChallanId TEXT, truckNo TEXT, NoOfComm TEXT, AllotedMonth TEXT, AllotedYear TEXT, commCode TEXT, commName TEXT,schemeID TEXT, KRA TEXT, releasedQuantity TEXT, qtD TEXT, schemeName TEXT)"))
        {
            qDebug() << "Stock_Details. " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS TruckChit_Table(truck_chit_no TEXT, do_ro_no TEXT, comm_length TEXT,  PRIMARY KEY(truck_chit_no))"))
        {
            qDebug() << "15. " << query.lastError();
            return -1;
        }

        if(!query.exec("CREATE TABLE IF NOT EXISTS InspDetails(inspectorName TEXT,inspectorDesignation TEXT)"))
        {
            qDebug() << "InspDetails : " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS OTPRESPONSE(member_id TEXT,otp TEXT,refNo TEXT)"))
        {
            qDebug() << "OTPRESPONSE : " << query.lastError();
            return -1;
        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS MOBILEOTPRESPONSE(mobotp TEXT, mobrefNo TEXT)"))
        {
            qDebug() << "MOBILEOTPRESPONSE : " << query.lastError();
            return -1;

        }
        if(!query.exec("CREATE TABLE IF NOT EXISTS PrintLastResp_balance(bal_Qty TEXT,comm_Name TEXT,comm_Name_ll TEXT)"))
        {
            qDebug() << "22. " << query.lastError();
            return -1;
        }

        query.clear();
        if(!query.exec("CREATE TABLE IF NOT EXISTS PartialOnlineData(OffPassword text,OfflineLogin text,OfflineTxnTime text,Duration text,leftOfflineTime text,lastlogindate text,lastlogintime text,lastlogoutdate text,lastlogouttime text,AllotMonth text,AllotYear text,pOfflineStoppedDate text)"))
        {
            qDebug() << "22. " << query.lastError();
            return -1;
        }

        query.clear();
        if(!query.exec("CREATE TABLE IF NOT EXISTS StateInfo(stateCode TEXT, stateNameEn TEXT, stateNameLl TEXT, stateProfile TEXT, stateReceiptHeaderEn TEXT, stateReceiptHeaderLl TEXT, statefpsId TEXT,consentHeader TEXT,consentHeaderLl TEXT)"))
        {
            qDebug() << "StateInfo : " << query.lastError();
            return -1;
        }
        qDebug() << "Database Opened" << endl;

        ret = true;
    }

    return ret;
}

bool Database::openDB()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/mnt/jffs2/db_commonpds_2_3.db");

    return db.open();
}

bool Database::closeDB()
{
    db.close();
    return true;
}

bool Database::createDB()
{
    QFile filename;
    filename.setFileName("/mnt/jffs2/db_commonpds_2_3.db");
    if(filename.exists() == false)
    {
        if(openDB())
        {
            createTable();
            return true;
        }
        else
        {
            QMessageBox::critical(0,"Cannot Open Database","Unable to establish a database connection",QMessageBox::Cancel);
            return false;
        }
    }
    else
    {
        openDB();
    }
    return true;
}

void Database::BEGIN()
{
    qDebug()<<"ENROLLMENT BEGIN";
    QSqlQuery query;
    query.exec("BEGIN");
}

void Database::ROLLBACK()
{
    qDebug()<<"ENROLLMENT ROLLBACKED";

    QSqlQuery query;
    query.exec("ROLLBACK");
}

void Database::COMMIT()
{
    qDebug()<<"ENROLLMENT COMMITED";
    QSqlQuery query;
    query.exec("COMMIT");
}



