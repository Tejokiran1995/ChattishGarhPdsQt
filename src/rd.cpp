#include "widget.h"
#include "ui_widget.h"
#include "iritech.h"

extern "C"
{
    int device_search();
}

/*
info dev_info;
pData pid_info;
rd_status rdStatus;
device_data dev_details;

discoveryiris  temp;
discoveryiris *t;
//discovery *t;
dev_infoiris temp_dev_info;
dev_infoiris *tt;
captureiris temp_capture;
captureiris *ttt;

discoveryiris * BiomatiquesRDS_Discovery();
dev_infoiris * BiomatiquesRDS_DeviceInfo(discoveryiris * );
captureiris * BiomatiquesRDS_Capture(discoveryiris * ,pidopts );
*/
//iriTech_rd

iriTech_rd_status iriTech_rdStatus1;
iriTech_eye_capture eye_Capture1;

iriTech_pData iriTech_pid_Info;

int Iritech_rd_service_discovery(iriTech_rd_status *iriTech_rdStatus1);
int Iritech_rd_capture(iriTech_pData *iriTech_pidInfo,iriTech_eye_capture *eye_Capture1);

int Widget::Capture_Iris(int iEyeSelect, char *cUID)
{
    int iSelectIrisScanner=device_search();

    qDebug()<<"IRIS device"<<iSelectIrisScanner;

    popup_msg *dlgPopup1 = new popup_msg(this);

    if(iEyeSelect == LEFT_EYE)
    {
        miscObj.AudioTest((char *)"172");


        if(LangMode ==1)
            dlgPopup1->ui->lblPopup->setText("Please Place IRIS Scanner\nnear \"Left Eye\"");
        else
            dlgPopup1->ui->lblPopup->setText(trUtf8("आईरिएस स्कैनर \" बाएं आंख \" के पास रखें."));
    }
    else if(iEyeSelect == RIGHT_EYE)
    {
        miscObj.AudioTest((char *)"173");

        if(LangMode ==1)
            dlgPopup1->ui->lblPopup->setText("Please Place IRIS Scanner\nnear \"Right Eye\"");
        else
            dlgPopup1->ui->lblPopup->setText(trUtf8("आईरिएस स्कैनर \" दाहिने आंख \" के पास रखें"));
    }

    dlgPopup1->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    GPRS objGprs;
    if(objGprs.Check_Network_Available() < 0)
    {
        dlgPopup1->close();
        dlgPopup1->deleteLater();

        return -1;
    }

    if(iSelectIrisScanner == BIOMATIQUES)
    {
        int ret = Calling_Biomatiques_Iris(iEyeSelect);

        if(ret == -1)
        {
            dlgPopup1->close();
            dlgPopup1->deleteLater();
            return -1;
        }
        if(ret == -2)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox("IRIS RD IS NOT READY");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"आईरिएस आरडी शुरू नहीं हुई ");

            dlgPopup1->close();
            dlgPopup1->deleteLater();

            return -2;

        }
        if(ret == -3)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox("Start the Biomatiques RD Service");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"बायोमेटिकेस आरडी सेवा शुरू करें");

            dlgPopup1->close();
            dlgPopup1->deleteLater();

            return -2;
        }

        dlgPopup1->close();
        dlgPopup1->deleteLater();
    }
    /*
    else if(iSelectIrisScanner == IRITECH)
    {
        int ret=Calling_Iritech_Iris(iEyeSelect);

        qDebug()<<"ret IRitech"<<ret<<endl;

        if(ret == -1)
        {
            dlgPopup1->close();
            dlgPopup1->deleteLater();
            return -1;

        }
        if(ret == -2)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox("IRIS RD IS NOT READY");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"आईरिएस आरडी शुरू नहीं हुई ");

            dlgPopup1->close();
            dlgPopup1->deleteLater();

            return -2;
        }
        if(ret == -3)
        {
            if(LangMode == 1)
                miscObj.DisplayWideMsgBox("Start the IRITECH RD Service");
            else if(LangMode == 2)
                miscObj.tel_DisplayWideMsgBox((char *)"IRITECH आरडी सेवा शुरू करें");

            dlgPopup1->close();
            dlgPopup1->deleteLater();

            return -2;
        }
        dlgPopup1->close();
        dlgPopup1->deleteLater();

    }
    */

    miscObj.AudioTest((char *)"182");
    popup_msg *dlgPopup2 = new popup_msg(this);
    if(LangMode == 1)
        dlgPopup2->ui->lblPopup->setText("Please Wait...\nAuthenticating Iris...");
    else if(LangMode == 2)
        dlgPopup2->ui->lblPopup->setText(trUtf8("कृपया प्रतीक्षा करें ... \n आँख का प्रामणीकरण हो रहा है ."));


    dlgPopup2->show();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();

    sleep(2); //Kept For Audio

    char buff[64];
    memset(buff, 0, sizeof(buff));
    char cSessionId[16];
    memset(cSessionId, 0, sizeof(cSessionId));
    database_queries qry;
    if(qry.getSessionId(cSessionId) < 0)
    {
        dlgPopup2->close();
        dlgPopup2->deleteLater();

        miscObj.DisplayMsgBox((char *)"Session Id not found\nPlease try again");
        return -1;
    }

    char cDealMembuff[32];
    memset(cDealMembuff, 0, sizeof(cDealMembuff));
    if(iDealMemSelected == DEALER_SELECTED)
    {

        strcpy(cDealMembuff, ui->ldtShpId->text().toAscii().data());
    }
    else if(iDealMemSelected == MEMBER_SELECTED)
    {
        strcpy(cDealMembuff,ui->lblRCNO_Val->text().toAscii().data());
    }
    else
    {
        strcpy(cDealMembuff, ui->ldtShpId->text().toAscii().data());
    }
    char cmachineID[16];
    memset(cmachineID, 0, sizeof(cmachineID));
    miscObj.GetMachineId(cmachineID);

    FILE *fp;
    fp = fopen("/mnt/jffs2/IrisReq.xml","w");
    if(fp == NULL)
    {
        qDebug() << "File Not Found" << endl;
        return -1;
    }

    char ServiceIris[64];
    memset(ServiceIris, 0, sizeof(ServiceIris));

    if(iMainTransaction == NCR_TRANSACTION)
    {
        strcpy(ServiceIris, "getDTRSecureIRISAuthenticate");

        fprintf(fp, "%s", "<?xml version='1.0' encoding='UTF-8' standalone='no' ?><soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ser=\"http://service.fetch.rationcard/\">");
        fprintf(fp, "<%s/>", "soapenv:Header");
        fprintf(fp, "<%s>", "soapenv:Body");
        fprintf(fp, "<%s>", "ser:getDTRSecureIRISAuthenticate");
        fprintf(fp, "<fps_seesion_Id>%s</fps_seesion_Id>", cSessionId);
        fprintf(fp, "<Shop_no>%s</Shop_no>", ui->ldtShpId->text().toAscii().data());
        fprintf(fp, "<uidNumber>%s</uidNumber>", cUID);
        fprintf(fp, "<udc>%s</udc>", cmachineID);
        fprintf(fp, "<%s>", "auth_packet");
       // fprintf(fp, "<certificateIdentifier>%s</certificateIdentifier>", bufExpiryStr);
        fprintf(fp, "<dataType>%s</dataType>", "X");
        //fprintf(fp, "<encHmac>%s</encHmac>", hmac);
        fprintf(fp, "<secure_agency>%s</secure_agency>", "CivilSupplies");
        fprintf(fp, "<secure_agencyCode>%s</secure_agencyCode>", "1/100");
        //fprintf(fp, "<secure_pid>%s</secure_pid>", pload);//secure_temp
        //fprintf(fp, "<sessionKey>%s</sessionKey>", encryptedSessKey);
        fprintf(fp, "</%s>", "auth_packet");
        fprintf(fp, "</%s>", "ser:getDTRSecureIRISAuthenticate");
        fprintf(fp, "</%s>", "soapenv:Body");
        fprintf(fp, "</%s>", "soapenv:Envelope");
    }
    else if(iSelectIrisScanner == BIOMATIQUES)
    {
        strcpy(ServiceIris, "getAuthenticateNICAuaAuthIRISRD2");
        //RDIRIS//

        qDebug()<<"User_Id="<<ui->ldtRationCard->text().toAscii().data();

        fprintf(fp,"%s","<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        fprintf(fp,"%s","<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://www.uidai.gov.in/authentication/uid-auth-request/2.0\" xmlns:ns2=\"http://service.fetch.rationcard/\">");
        fprintf(fp,"%s","<SOAP-ENV:Body>");
        fprintf(fp,"%s","<ns2:getAuthenticateNICAuaAuthIRISRD2>");

        fprintf(fp,"<fps_seesion_Id>%s</fps_seesion_Id>",cSessionId);
        fprintf(fp,"<Shop_no>%s</Shop_no>",ui->ldtShpId->text().toAscii().data());
        fprintf(fp, "<User_Id>%s</User_Id>",cDealMembuff);
        fprintf(fp, "<uidNumber>%s</uidNumber>", cUID);
        fprintf(fp, "<udc>%s</udc>", cmachineID);
        fprintf(fp, "<authMode>%s</authMode>","B");

        fprintf(fp, "<%s>", "auth_packet");
        fprintf(fp,"<ns1:certificateIdentifier>%s</ns1:certificateIdentifier>",ttt->ci);
        fprintf(fp,"<ns1:dataType>%s</ns1:dataType>","X");
        fprintf(fp,"<ns1:dc>%s</ns1:dc>",ttt->dev_info.dc);
        fprintf(fp,"<ns1:dpId>%s</ns1:dpId>",ttt->dev_info.dpid);
        fprintf(fp,"<ns1:encHmac>%s</ns1:encHmac>",ttt->Hmac);
        fprintf(fp,"<ns1:mc>%s</ns1:mc>",ttt->dev_info.mc);
        fprintf(fp,"<ns1:mid>%s</ns1:mid>",ttt->dev_info.mi);
        fprintf(fp,"<ns1:rdId>%s</ns1:rdId>",ttt->dev_info.rdsid);
        fprintf(fp,"<ns1:rdVer>%s</ns1:rdVer>",ttt->dev_info.rdsver);
        fprintf(fp,"<ns1:secure_pid>%s</ns1:secure_pid>",ttt->pid);
        fprintf(fp,"<ns1:sessionKey>%s</ns1:sessionKey>",ttt->Skey);
        fprintf(fp,"%s","</auth_packet>");
        //fprintf(fp,"<password>%s</password>",QPassword.toAscii().data());
        fprintf(fp,"<scannerId>%s</scannerId>","");
        fprintf(fp,"<authType>%s</authType>","IRIS");
        fprintf(fp,"%s","</ns2:getAuthenticateNICAuaAuthIRISRD2>");
        fprintf(fp,"%s","</SOAP-ENV:Body>");
        fprintf(fp,"%s","</SOAP-ENV:Envelope>");

    }
    else if(iSelectIrisScanner == IRITECH)
    {
        strcpy(ServiceIris, "getAuthenticateNICAuaAuthIRISRD2");
        //RDIRIS-IRITECH//
        qDebug()<<"User_Id="<<ui->ldtRationCard->text().toAscii().data();

        fprintf(fp,"%s","<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        fprintf(fp,"%s","<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:ns1=\"http://www.uidai.gov.in/authentication/uid-auth-request/2.0\" xmlns:ns2=\"http://service.fetch.rationcard/\">");
        fprintf(fp,"%s","<SOAP-ENV:Body>");
        fprintf(fp,"%s","<ns2:getAuthenticateNICAuaAuthIRISRD2>");

        fprintf(fp,"<fps_seesion_Id>%s</fps_seesion_Id>",cSessionId);
        fprintf(fp,"<Shop_no>%s</Shop_no>",ui->ldtShpId->text().toAscii().data());
        fprintf(fp, "<User_Id>%s</User_Id>",cDealMembuff);
        fprintf(fp, "<uidNumber>%s</uidNumber>", cUID);
        fprintf(fp, "<udc>%s</udc>", cmachineID);
        fprintf(fp, "<authMode>%s</authMode>","B");

        fprintf(fp, "<%s>", "auth_packet");

        fprintf(fp,"<ns1:certificateIdentifier>%s</ns1:certificateIdentifier>",iriTech_pid_Info.ci);
        fprintf(fp,"<ns1:dataType>%s</ns1:dataType>","X");

        fprintf(fp,"<ns1:dc>%s</ns1:dc>",iriTech_pid_Info.iriTech_dev_data.dc);

        fprintf(fp,"<ns1:dpId>%s</ns1:dpId>",iriTech_pid_Info.iriTech_dev_data.dpId);
        fprintf(fp,"<ns1:encHmac>%s</ns1:encHmac>",iriTech_pid_Info.Hmac);
        fprintf(fp,"<ns1:mc>%s</ns1:mc>",iriTech_pid_Info.iriTech_dev_data.mc);
        fprintf(fp,"<ns1:mid>%s</ns1:mid>",iriTech_pid_Info.iriTech_dev_data.mi);
        fprintf(fp,"<ns1:rdId>%s</ns1:rdId>",iriTech_pid_Info.iriTech_dev_data.rdsId);
        fprintf(fp,"<ns1:rdVer>%s</ns1:rdVer>",iriTech_pid_Info.iriTech_dev_data.rdsVer);
        fprintf(fp,"<ns1:secure_pid>%s</ns1:secure_pid>",iriTech_pid_Info.pid);
        fprintf(fp,"<ns1:sessionKey>%s</ns1:sessionKey>",iriTech_pid_Info.Skey);

        fprintf(fp,"%s","</auth_packet>");
        //fprintf(fp,"<password>%s</password>",QPassword.toAscii().data());
        fprintf(fp,"<scannerId>%s</scannerId>","");
        fprintf(fp,"<authType>%s</authType>","IRIS");
        fprintf(fp,"%s","</ns2:getAuthenticateNICAuaAuthIRISRD2>");
        fprintf(fp,"%s","</SOAP-ENV:Body>");
        fprintf(fp,"%s","</SOAP-ENV:Envelope>");

    }
    fclose(fp);

    char cUrlName[256];
    memset(cUrlName, 0, sizeof(cUrlName));
    QSqlQuery query;
    query.prepare("SELECT URL FROM Settings");
    if(!query.exec())
        qDebug() << "In SendTxnToServer" << query.lastError();
    else
    {
        while(query.next())
        {
            memset(cUrlName, 0, sizeof(cUrlName));
            strcpy(cUrlName, query.value(0).toString().toAscii().data());
        }
    }

    char cmdCurl[1024];
    memset(cmdCurl, 0, sizeof(cmdCurl));
    sprintf(cmdCurl, "curl -H 'SOAPACTION: \"http://tempuri.org/%s\"' -X POST -H 'Content-type: text/xml' -d @%s %s > %s -v -m %d", ServiceIris, "/mnt/jffs2/IrisReq.xml", cUrlName, "/mnt/jffs2/IrisResponse.xml", SERVER_TIME_OUT);
    qDebug()<<"\nCurl Cmd : "<<cmdCurl;

    int systemRet = system(cmdCurl);
    qDebug() << "systemRet = " << systemRet;

    if(SystemCurlResponse(systemRet, AUTH_SERVER) < 0)
    {
        dlgPopup2->close();
        dlgPopup2->deleteLater();

        return -1;
    }

    dlgPopup2->close();
    dlgPopup2->deleteLater();

    qDebug() << "**********IrisResponse.xml*********";
    system("cat /mnt/jffs2/IrisResponse.xml");
    qDebug() << "***********************************************";

    if(ParsingFirstCondition((char *)"/mnt/jffs2/IrisResponse.xml") < 0)
        return -1;

    if(Parse_Iris_Response("/mnt/jffs2/IrisResponse.xml") < 0)
        return -1;

    return 0;
}

int Widget::Calling_Biomatiques_Iris(int iEyeSelect)
{
    qDebug() << "In Calling_Biomatiques_Iris()";

    static int i;

    t = BiomatiquesRDS_Discovery();

    if(t == NULL)
    {
        return -3;
    }
    if(strcmp(t->status,"NOTREADY") == 0)
    {

        return -2;
    }
    if(t!=NULL)
    {
        printf("\n\n\n**************************************************************");
        printf("\n Port          = %d",t->port);
        printf("\n Status        = %s",t->status);
        printf("\n info          = %s",t->info);
        printf("\n capture_path  = %s",t->capture_Path);
        printf("\n Info_path     = %s",t->info_path);
        printf("\n**************************************************************\n");

        if(i == 0)
        {

            tt=BiomatiquesRDS_DeviceInfo(t);
            if(tt = NULL)
            {
                return -3;

            }
            if(tt!=NULL)
            {
                printf("\n\n*************************************************************\n");
                printf("\n dpId   = %s \n",tt->dpid);
                printf("\n rdsid  = %s \n",tt->rdsid);
                printf("\n rdsver = %s \n",tt->rdsver);
                printf("\n dc     = %s \n",tt->dc);
                printf("\n mi     = %s \n",tt->mi);
                printf("\n mc     = %s \n",tt->mc);
                printf("\n\n*************************************************************\n");

            }
        }

        i++;

        pidopts p;
        sprintf(p.env,"%s","P");  // S - staging P - production PP-preproduction
        sprintf(p.wadh,"%s","");

        ttt=BiomatiquesRDS_Capture(t,p);

        if(ttt == NULL)
        {
            return -3;
        }
        if(ttt!=NULL)
        {

            printf("\n\n*************************************************************\n");
            printf("\n *******************Response************************************ ");
            printf("\n\n err_code = %s",ttt->err_code);
            printf("\n\n err_info = %s",ttt->err_info);

            if(strcmp(ttt->err_code,"0") != 0)
            {
                CMisc misc;
                misc.DisplayWideMsgBox(ttt->err_info);
                return -1;
            }
            printf("\n\n fCount   = %s",ttt->fCount);
            printf("\n\n fType    = %s",ttt->fType);
            printf("\n\n iType    = %s",ttt->iType);
            printf("\n\n iCount   = %s",ttt->iCount);
            printf("\n\n pType    = %s",ttt->pType);
            printf("\n\n pCount   = %s",ttt->pCount);
            printf("\n\n dpId     =%s",ttt->dev_info.dpid);
            printf("\n\n rdsid    =%s",ttt->dev_info.rdsid);
            printf("\n\n rdsVer   =%s",ttt->dev_info.rdsver);
            printf("\n\n dc       =%s",ttt->dev_info.dc);
            printf("\n\n mi       =%s",ttt->dev_info.mi);
            printf("\n\n mc       =%s",ttt->dev_info.mc);
            printf("\n\n CI       = %s",ttt->ci);
            printf("\n\n Skey     = %s",ttt->Skey);
            printf("\n\n Hmac     = %s",ttt->Hmac);
            printf("\n\n type     = %s",ttt->type);
            printf("\n\n PID     = %s",ttt->pid);
            printf("\n\n*************************************************************\n");
        }
    }
    return 0;
}

int Widget::Calling_Iritech_Iris(int iEyeSelect)
{
    int RetVal;

    qDebug() << "In Calling_Iritech_Iris()";

   memset(&iriTech_pid_Info,0,sizeof(iriTech_pData));
    memset(&eye_Capture1,0,sizeof(iriTech_eye_capture));
    memset(&iriTech_rdStatus1,0,sizeof(iriTech_rd_status));
   // memset(iriTech_pid_Infopid,0,15000);

    RetVal=Iritech_rd_service_discovery(&iriTech_rdStatus1);
    qDebug()<<"Iritech_rd_service_discovery"<<RetVal<<endl<<iriTech_rdStatus1.status<<endl;

    if(RetVal != 0)
    {
        return -2;
    }

    if(strcmp(iriTech_rdStatus1.status,"NOTREADY")==0)
        return -2;

    eye_Capture1.eye_Count=1;
    sprintf(eye_Capture1.pidVer,"%s","2.0");
    sprintf(eye_Capture1.env,"%s","P");
    eye_Capture1.Timeout = 15000;

    RetVal= Iritech_rd_capture(&iriTech_pid_Info,&eye_Capture1);
    qDebug()<<"RetVal="<<RetVal;

    if(atoi(iriTech_pid_Info.err_code) !=0)
    {
        qDebug()<<"Error="<<iriTech_pid_Info.err_info;

    }
    if(RetVal == -1 || RetVal == 0)
    {
        if(iriTech_pid_Info.pid!= NULL)
        {
            /*
            printf("\nerror code = %s\n",iriTech_pid_Info->err_code);
            printf("\nerror info = %s\n",iriTech_pid_Info->err_info);
            printf("\nnmPoints = %s\n",iriTech_pid_Info->nmPoints);
            printf("\nqScore = %s\n",iriTech_pid_Info->qScore);
            printf("\nci = %s\n",iriTech_pid_Info->ci);
            printf("\nSkey = %s\n",iriTech_pid_Info->Skey);
            printf("\nHmac = %s\n",iriTech_pid_Info->Hmac);
            printf("\npid type = %s\n",iriTech_pid_Info->type);
            printf("\npid = %s-----",iriTech_pid_Info->pid);

            printf("\ndc = %s\n",iriTech_pid_Info->iriTech_dev_data.dc);
            printf("\nmi = %s\n",iriTech_pid_Info->iriTech_dev_data.mi);
            printf("\nmc = %s\n",iriTech_pid_Info->iriTech_dev_data.mc);
            printf("\ndpId = %s\n",iriTech_pid_Info->iriTech_dev_data.dpId);
            printf("rdsId = %s\n",iriTech_pid_Info->iriTech_dev_data.rdsId);
            printf("\nrdsVer = %s\n",iriTech_pid_Info->iriTech_dev_data.rdsVer);
            printf("\n serialnumber = %s\n",iriTech_pid_Info->serialnumber);
            */
            printf("\n#***********debug********************#\n");

        }
    }

    return 0;
}

discoveryiris * BiomatiquesRDS_Discovery()
{
    discoveryiris * ret=NULL;

    int port=11100;
    for(port=11100;port<=11120;port++)
    {
        int res=chekPort(port);

        if(res==0 )
        {

            temp.port=port;

            ret=&temp;
            break;
        }
    }
    return ret;
}

dev_infoiris * BiomatiquesRDS_DeviceInfo(discoveryiris * rd)
{
    dev_infoiris *t=NULL;
    char cmd[200];
    sprintf(cmd,"curl -X DEVICEINFO http://127.0.0.1:%d%s",rd->port,rd->info_path);
    printf("\n DeviceInfo command %s \n",cmd);
    FILE *FP=popen(cmd,"r");
    if(FP!=NULL)
    {

        char path[100];
        char CurlResponse[2000];
        memset(CurlResponse,'\0',2000);
        while (fgets(path, sizeof(path)-1, FP) != NULL)
        {
            strcat(CurlResponse,path);
        }

        xmlDoc *doc = NULL;
        xmlNode *root_element = NULL;
        doc= xmlParseDoc((xmlChar *)CurlResponse);
        if(doc!=NULL)
        {


            root_element = xmlDocGetRootElement(doc);
            if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"DeviceInfo")==0)
            {
                xmlChar *uri;
                uri = xmlGetProp(root_element,(xmlChar *) "dpId");
                if(uri!=NULL)
                {
                    sprintf(temp_dev_info.dpid,"%s",uri);
                }
                uri = xmlGetProp(root_element, (xmlChar *)"rdsId");
                if(uri!=NULL)
                {
                    sprintf(temp_dev_info.rdsid,"%s",uri);
                }
                uri = xmlGetProp(root_element,(xmlChar *) "rdsVer");
                if(uri!=NULL)
                {
                    sprintf(temp_dev_info.rdsver,"%s",uri);
                }
                uri = xmlGetProp(root_element,(xmlChar *) "dc");
                if(uri!=NULL)
                {
                    sprintf(temp_dev_info.dc,"%s",uri);
                }
                uri = xmlGetProp(root_element, (xmlChar *)"mi");
                if(uri!=NULL)
                {
                    sprintf(temp_dev_info.mi,"%s",uri);
                }
                uri = xmlGetProp(root_element,(xmlChar *) "mc");
                if(uri!=NULL)
                {
                    sprintf(temp_dev_info.mc,"%s",uri);
                }
                t=0;
                xmlFree(uri);
                sprintf(temp_dev_info.raw_deviceInfo,"%s",CurlResponse);
                t=&temp_dev_info;
            }
            xmlFreeDoc(doc);


        }

    }
    return t;

}

captureiris * BiomatiquesRDS_Capture(discoveryiris * rd,pidopts opts)
{
    captureiris *cap=NULL;

    char PidOptions[500];
    sprintf(PidOptions,"<PidOptions ver=\"1.0\"> <Opts env=\"%s\" fCount=\"0\" fType=\"0\" iCount=\"1\" iType=\"0\" pCount=\"0\" pType=\"0\" format=\"0\"   pidVer=\"2.0\" timeout=\"10000\" otp=\"\" wadh=\"%s\" posh=\"\"/> <Demo></Demo><CustOpts></CustOpts></PidOptions>",opts.env,opts.wadh);
    char cmd[1000];
    sprintf(cmd,"curl -X CAPTURE -d '%s' http://127.0.0.1:%d%s",PidOptions,rd->port,rd->capture_Path);
    printf("\n Capture command %s \n",cmd);
    FILE *FP=popen(cmd,"r");
    if(FP!=NULL)
    {

        char path[100];
        char CurlResponse[10000];
        memset(CurlResponse,'\0',10000);
        while (fgets(path, sizeof(path)-1, FP) != NULL)
        {
            strcat(CurlResponse,path);
        }

        xmlDoc *doc = NULL;
        xmlNode *root_element = NULL;
        doc= xmlParseDoc((xmlChar *)CurlResponse);
        if(doc!=NULL)
        {
            root_element = xmlDocGetRootElement(doc);
            if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"PidData")==0)
            {
                root_element = root_element->xmlChildrenNode;
                while (root_element != NULL) {
                    xmlChar *uri;
                    if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"Resp")==0)
                    {
                        uri = xmlGetProp(root_element,(xmlChar *) "errCode");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.err_code,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "errInfo");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.err_info,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "pType");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.pType,"%s",uri);
                        }
                        uri = xmlGetProp(root_element, (xmlChar *)"qScore");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.qScore,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "fCount");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.fCount,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "fType");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.fType,"%s",uri);
                        }
                        uri = xmlGetProp(root_element, (xmlChar *)"iCount");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.iCount,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "iType");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.iType,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "pCount");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.pCount,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "pType");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.pType,"%s",uri);
                        }
                    }
                    else if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"DeviceInfo")==0)
                    {

                        uri = xmlGetProp(root_element, (xmlChar *)"dpId");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.dev_info.dpid,"%s",uri);
                        }
                        uri = xmlGetProp(root_element, (xmlChar *)"rdsId");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.dev_info.rdsid,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "rdsVer");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.dev_info.rdsver,"%s",uri);
                        }
                        uri = xmlGetProp(root_element, (xmlChar *)"dc");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.dev_info.dc,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "mi");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.dev_info.mi,"%s",uri);
                        }
                        uri = xmlGetProp(root_element,(xmlChar *) "mc");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.dev_info.mc,"%s",uri);
                        }
                    }
                    else if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"Skey")==0)
                    {
                        uri = xmlGetProp(root_element, (xmlChar *)"ci");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.ci,"%s",uri);
                        }
                        sprintf(temp_capture.Skey,"%s",xmlNodeGetContent(root_element));

                    }
                    else if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"Hmac")==0)
                    {
                        sprintf(temp_capture.Hmac,"%s",xmlNodeGetContent(root_element));

                    }
                    else if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"Data")==0)
                    {
                        uri = xmlGetProp(root_element,(xmlChar *) "type");
                        if(uri!=NULL)
                        {
                            sprintf(temp_capture.type,"%s",uri);
                        }
                        sprintf(temp_capture.pid,"%s",xmlNodeGetContent(root_element));
                    }

                    root_element = root_element->next;

                }

                sprintf(temp_capture.raw_capture,"%s",CurlResponse);
                cap=&temp_capture;
            }
        }
    }
    return cap;
}

int chekPort(int port)
{
    int retval=-1;
    char cmd[50];
    sprintf(cmd,"curl -X RDSERVICE http://127.0.0.1:%d",port);
    FILE * FP=popen(cmd,"r");
    if(FP!=NULL)
    {
        char path[100];
        char CurlResponse[1000];
        memset(CurlResponse,'\0',1000);
        while (fgets(path, sizeof(path)-1, FP) != NULL)
        {
            strcat(CurlResponse,path);
        }
        printf("\n%s",CurlResponse);

        QString str(CurlResponse);
        if(str.contains("Biomatiques Iris (Model: EPI-1000)"))
        {
            xmlDoc *doc = NULL;
            xmlNode *root_element = NULL;
            doc= xmlParseDoc((xmlChar *)CurlResponse);
            if(doc!=NULL)
            {
                //   printf("\n Response = %s",CurlResponse);
                root_element = xmlDocGetRootElement(doc);

                if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"RDService")==0)
                {
                    xmlChar *uri;
                    uri = xmlGetProp(root_element,(xmlChar *) "status");
                    if(uri!=NULL)
                    {
                        sprintf(temp.status,"%s",uri);
                    }
                    uri = xmlGetProp(root_element, (xmlChar *)"info");
                    if(uri!=NULL)
                    {
                        sprintf(temp.info,"%s",uri);
                    }
                    root_element = root_element->xmlChildrenNode;
                    while (root_element != NULL)
                    {
                        if(xmlStrcasecmp(root_element->name,(xmlChar *)(const char *)"Interface")==0)
                        {
                            uri = xmlGetProp(root_element, (xmlChar *)"id");
                            if(uri!=NULL && xmlStrcasecmp(uri,(xmlChar *)"CAPTURE")==0)
                            {
                                uri = xmlGetProp(root_element,(xmlChar *) "path");
                                sprintf(temp.capture_Path,"%s",uri);

                            }
                            else if(uri!=NULL && xmlStrcasecmp(uri,(xmlChar *)"DEVICEINFO")==0)
                            {
                                uri = xmlGetProp(root_element, (xmlChar *)"path");
                                sprintf(temp.info_path,"%s",uri);
                            }
                        }
                        root_element = root_element->next;
                    }
                    sprintf(temp.raw_discovery,"%s",CurlResponse);
                    xmlFree(uri);
                    retval=0;
                }
            }
        }
    }
    return retval;
}


