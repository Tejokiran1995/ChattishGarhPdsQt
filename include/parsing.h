#ifndef PARSING_H
#define PARSING_H

#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QtXml/QXmlStreamReader>
#include <QString>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomText>
#include <QDomNodeList>

//#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

class parsing : public QMainWindow
{
    Q_OBJECT
public:
    explicit parsing(QWidget *parent = 0);
    void parseWsdlResponseData();

signals:
    
public slots:
    
};

#endif // PARSING_H
