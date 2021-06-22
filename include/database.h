#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QMessageBox>

class Database
{
public:
    bool openDB();
    bool createTable();
    bool closeDB();
    bool createDB();
    void BEGIN();
    void ROLLBACK();
    void COMMIT();

private:
    QSqlDatabase db;
    //Database dbaccess;
};


#endif // DATABASE_H
