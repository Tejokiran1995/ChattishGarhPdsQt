#ifndef PARSE_H
#define PARSE_H
#include "database.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QFontDatabase>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QAbstractItemView>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <QStringList>
//#include <QKeyEvent>
#include <QSqlRecord>
#include <QTableWidget>


#define FILE_EMPTY_RCVD -1
#define INVALID_BITMAP -1
#define SUCCESS 0
#define FILENEXISTS 1
#define FILEERROR -1
#define ERROR -1

class parse
{
public:
    parse();
    int parseCustMaster();
    int parseBenQuota();
    int parseItemMaster();
    int parseCustomerFamily();
    void deleteTables();
    void ParsingBankMaster();
    void ParsingBenMasterDetails(char *);
    //int HttpsCertDownload();
    void ParsingCashlessRRN(char *);
    int chkReversal(char *);
    //int parseNCR_HolderDetails(char *);
    void getTempRRN(char *);
};

#endif // PARSE_H
