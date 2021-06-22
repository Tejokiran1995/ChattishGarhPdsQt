#ifndef IRIS_H
#define IRIS_H

#include <QWidget>
#include <QDebug>

#include <QFile>

extern "C"
{
#include "gl11.h"

#include "iris_biometric_api.h"
#include "Iddk2000Defs.h"
//#include "IddkBase.h"

}



class cIris : public QWidget
{
    Q_OBJECT
public:
    explicit cIris(QWidget *parent = 0);

    int Iris_Initialisation();
    int Scan_Iris_ISO(char *);
    void Iris_Uninitialised();


signals:
    
public slots:
    
};

#endif // IRIS_H
