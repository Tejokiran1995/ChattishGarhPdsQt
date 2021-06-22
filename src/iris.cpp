#include "iris.h"


cIris::cIris(QWidget *parent) :
    QWidget(parent)
{
}

int cIris::Iris_Initialisation()
{
//    int Status = iris_bio_init();
//    qDebug() <<  "bio init status  = " << Status;

//    if(Status != 0)
//        return -1;

    return 0;
}

int cIris::Scan_Iris_ISO(char *filename)
{
    char imgFmt, imgType;
    int sensor_timeout_period = 20;
    unsigned char *imageData = NULL;
    int imageSize = 0;

    imgType = IDDK_IISO_2011;
    //imgFmt = IDDK_IFORMAT_MONO_RAW;
    imgFmt = IDDK_IFORMAT_MONO_JPEG2000;

    imageData = (unsigned char*)malloc(100);
    int Status =0;// iris_bio_capture_image(imgFmt ,imgType, filename, sensor_timeout_period, imageData, &imageSize);

    qDebug() << "Status = " << Status;
    qDebug() << "image size = " << imageSize;
    if(Status != 0)
    {
        return -1;
    }

    for(int i = 0; i<imageSize; i++)
    {
        fprintf(stdout,"%02X ", imageData[i]);
    }
    fprintf(stdout,"\n");


    return 0;
}


void cIris::Iris_Uninitialised()
{
   // iris_bio_uninit();

    return;
}

