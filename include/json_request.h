#ifndef JSONREQUEST_H
#define JSONREQUEST_H
#include <string>
#include <misc.h>
using std::string;
class JsonRequest
{

public:
    JsonRequest();
    ~JsonRequest();
    char* JSON_REQUEST_HAR();
    //char* Database_Download();
    char* PrepareAppDownloadRequestFile(char *,char *,char *,char *,char *);
    int PrepareACKAppUpdateReqfile(char *, char*, char*, char*, char*); //New Parameter added for version 01072017 by vivek

    CMisc objMisc;
private:
};

#endif // JSONREQUEST_H
