#ifndef FINO_PRINT_H
#define FINO_PRINT_H

#include <QDialog>
#include <QByteArray>
#include <QObject>
#include <QImageReader>
#include <QVariant>
#include <QDebug>
#include <stdio.h>
#include<sys/stat.h>
#include <QFile>

#define FILE_EMPTY_RCVD -1
#define INVALID_BITMAP -1
#define SUCCESS 0
#define FILENEXISTS 1
#define FILEERROR -1
#define ERROR -1

#define LOGO_APGOVT 1
#define LOGO_CORP   2
#define LOGO_ICICI  4
#define LOGO_AXIS   5
#define LOGO_UNION  6
#define LOGO_BARODA 7
#define LOGO_ING    9
#define LOGO_INDIAN 10
#define LOGO_11     11
#define LOGO_DGB    12
#define LOGO_JANALAKSHMI    13
#define LOGO_ANDHRA         16
#define LOGO_JK             18
#define LOGO_PURVANCHAL     19
#define LOGO_UNITED         20
#define LOGO_DHANALAKSHMI   21
#define LOGO_KRISHNA        22
#define LOGO_GGB            23
#define LOGO_KOTAK          24
#define LOGO_UCO            25
#define LOGO_JK_GRAMIN      27
#define LOGO_NMB            28

#define LOGO_GURGOAN 96
#define LOGO_DECCAN  97
#define LOGO_PUNJAB  98
#define LOGO_FINO    99


class LogoPrint/* : public QDialog*/
{
    
public:

    void PrintLogo(int);
    void PrintBmpText();
    void createHexArray(int fileno,char *,bool);
    int takePrint(int fileno);
    int takeTextPrint(int fileno);
    int Check_bitmap(char *file_name);
    int Get_bitmap(char *buf,char *file_name);
    int prepare_bmp_image(unsigned char *data,int len);
    int prepare_bmp_text_image(unsigned char *data,int len);
    void get_bmp_file_name(int iLogoType, char *szLogoFile);
    int getLogoTypeFromDB();

};

#endif // MAINDIALOG_H
