#ifndef JSONRESPONSE_H
#define JSONRESPONSE_H
#include "json.h"
#include "cstdlib"
#include "misc.h"
#include "database.h"
class JsonResponse
{
public:
    JsonResponse();
    ~JsonResponse();

    typedef struct _Auto_App_Download_Resp{
        char *fpsurl;
        char* vesionnum ;
        char *checksum ;
        char* respcode ;
        char *respMsg;
        char *fpsid;
    } Auto_App_Download_Resp;


    int ParseAppDownloadResp(char* FIleName , Auto_App_Download_Resp *);
    int ParseACKAppUpdateRespfile(char *, char *, char *);
    //Stock Recieve add by vivek 28062017
    int parseStockDetailsResponseJson(char *);
    int ParsingFirstConditionJson(char *);



    CMisc objMisc;

private:
    //QtJson::JsonObject GetJsonObjectFromString(QString Str);
    //QtJson::JsonArray GetJsonArrayFromJsonObject(QtJson::JsonObject JObj,QString Str);
};

#endif // JSONRESPONSE_H
