#ifndef IRISCAPTURE_H_INCLUDED
#define IRISCAPTURE_H_INCLUDED
#include "data.h"
class IrisCapture
{
public:
    virtual void OnFrameRecieve(RGBImage * i)=0;
//    virtual void OnCaptureCompleted(ISO * i)=0;
    virtual void OnCaptureCompleted(BIOMTQS_ISO * i)=0;
    virtual void OnCaptureCompletedImage(RGBImage * im,int kind,int * Points)=0;

};



#endif // IRISCAPTURE_H_INCLUDED
