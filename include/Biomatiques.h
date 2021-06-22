#ifndef BIOMATIQUES_H_INCLUDED
#define BIOMATIQUES_H_INCLUDED
#include "IrisCapture.h"
#include "EPI1000.h"
#include <exception>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
class Biomatiques
{


    private:
    bool isStartPreview;
    bool isStartAutocapture;
    long tstart,tend,tCapMillisecond;
    int kind;
    int position;
    EPI1000  eobj;

public:
    Biomatiques();
    ~Biomatiques();
    bool InitializeDevice();
    bool FinalizeDevice();
    bool IsDeviceConnected();
    bool StartPreview(IrisCapture * obj);
    bool StopPreview();
    bool StartAutoCapture(int kind, int position ,bool stoppreview,long CaptureTimeOut);
    bool StartAutoCapture(int kind, int position ,bool stoppreview);
    bool StopAutoCapture();
    bool incheckDevice();


};

#endif // BIOMATIQUES_H_INCLUDED
