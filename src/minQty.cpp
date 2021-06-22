#include "include/widget.h"

#include <stdio.h>
#include <string.h>

extern "C"{
    void Write_RunningApp_info(char *ApplicationType,char *ApplicationName,char *Version);
}

int Widget::MinQtyChecking(QString minQty,QString enterQty)
{
    float dividend, divisor, quotient;

    divisor = minQty.toFloat(0);
    if(divisor == 0.0)
    {
        return 0;
    }
    dividend = enterQty.toFloat(0);
    qDebug()<<"--------minQty---->"<<divisor;
    qDebug()<<"--------enterQty---->"<<dividend;

    quotient = dividend / divisor;
    char qty[100];
    int cnt=0,dummy=0;
    sprintf(qty,"%0.3f",quotient);
    printf("qty----------->%s\n",qty);
    for(int i=0;i<strlen(qty);i++)
    {
        if(qty[i] == '.')
        {
            for(int j=i+1;j<strlen(qty);j++)
            {
                dummy++;
                if((qty[j] != '0')) {
                    if (qty[j] == '9')
                    {
                    }
                    else
                        cnt++;
                    if((dummy == 3) && (cnt == 1))
                    {
                        if(qty[j] > '6')
                            printf("------cnt-------%d\n",cnt);
                        else
                            cnt=0;
                    }
                    if((dummy == 2) && (cnt == 1))
                    {
                        if(qty[j] == '1')
                            cnt =0;

                    }
                }
            }

            if(cnt == 0)
            {
                qDebug("-----ok----\n");
                return 0;
            }
            else {
                qDebug("------bad-------\n");
                return 1;
            }
        }
    }

}
int Widget::BenEnterQtyChecking(QString BalQty,QString user,QString minQty) //+- 0.005 checking
{
    //Added BalQty -BSK.
    if(BalQty.toFloat(0) < minQty.toFloat(0))
        minQty = BalQty;
    //Added BalQty -BSK.

    if(user.toFloat(0) > minQty.toFloat(0))
    {
        float diff = user.toFloat(0) - minQty.toFloat(0);
        qDebug()<<"-----Dif----->"<<diff;

        char buff[10];
        sprintf(buff,"%0.3f",diff);

        if((strcmp(buff,"0.005") ==0) || (strcmp(buff,"-0.005") == 0) || (strcmp(buff,"0.000") == 0) || (diff > 0.005))
            return 0;
        else
            return -1;
    }
    else
    {
        float diff = minQty.toFloat(0) - user.toFloat(0);
        qDebug()<<"-----Diff----->"<<diff;

        char buff[10];
        sprintf(buff,"%0.3f",diff);

        if((strcmp(buff,"0.005") ==0) || (strcmp(buff,"-0.005") == 0) || (strcmp(buff,"0.000") == 0) || (diff < 0.005))
            return 0;
        else
            return -1;
    }
}


void Write_RunningApp_info(char *ApplicationType,char *ApplicationName,char *Version)
{
    char App_info_file[560];
    FILE *fp=NULL;
    memset(App_info_file,0,sizeof(App_info_file));

    sprintf(App_info_file,"mkdir -p /etc/vision/RHMS/RunningApps/%s/%s",ApplicationType,ApplicationName);
    system(App_info_file);
    memset(App_info_file,0,sizeof(App_info_file));

    sprintf(App_info_file,"/etc/vision/RHMS/RunningApps/%s/%s/app.info",ApplicationType,ApplicationName);


    fp = fopen(App_info_file,"w");
    if ( fp == NULL)
    {
        fprintf(stderr,"%s file not found \n",App_info_file);
        return;
    }


    fprintf(fp,"ApplicationType:%s\nApplicationName:%s\nVersion:%s\n",ApplicationType,ApplicationName,Version);
    fprintf(stdout,"App info file = %s\nApplicationType:%s\nApplicationName:%s\nVersion:%s\n",App_info_file,ApplicationType,ApplicationName,Version);

    fclose(fp);

    return;
}
