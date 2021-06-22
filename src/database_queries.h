#ifndef DATABASE_QUERIES_H
#define DATABASE_QUERIES_H

class database_queries
{
public:
    database_queries();

    void getShopId_fromdb(char*);
    int getLangMode();
    void getdistofficecode(char *);
    int getSessionId(char *);
    bool getRecvGoodsRuteOfficerStatus();
    bool getRecvGoodsDealerStatus();
    void Delete_Cashless_Record(char *);
    void GetStan(char *);
    void getCardAcceptorIdentificationCode(char *, char *);
    void getCardAcceptorName(char *);
    void GetAgentInfo(char *);
    void getRRN(char *);
    int getEJ_FileName(char *);
    void DeleteTxnsDB();
};

#endif // DATABASE_QUERIES_H
