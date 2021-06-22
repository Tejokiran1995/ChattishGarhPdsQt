#ifndef EPI1000_H_INCLUDED
#define EPI1000_H_INCLUDED

/** Small C++ wrapper around V4L example code to access the webcam
**/

#include <string>
#include <memory> // unique_ptr
#include "IrisCapture.h"

#include <iostream>       // std::cout
#include "data.h"
#include <stdio.h>






class EPI1000 {


public:
    EPI1000();
    ~EPI1000();
    static void *startthread(void * obj);
    static void *startProcess(void * obj);
    bool startauto();


public:

    bool init_mmap();
    bool open_device();
    bool close_device();
    bool start_capturing();
    bool stop_capturing();
    int read_frame();
    bool init_device();
    bool uninit_device();
    bool startPreview();
    void frame(int i);
    bool getscale(int * pixel,int w,int h, int w2, int h2);
    std::string device;
    int fd;
    RGBImage rgb_frame;
    RGBImage rgb_frame_process;

    bool next;
    bool nextOP;
    struct buffer    *buffers;
    unsigned int     n_buffers;

    size_t xres, yres;
    size_t stride;
    bool force_format ;
    IrisCapture *mobj;
    pthread_t preview;
    pthread_t process;
    int *a;

    int *pixel;
    int *range;

    int *Repixel;
    int *Rerange;

    int ct;
    unsigned char *pdate;
    bool isStartPreview;
    bool isStartAutocapture;
    bool isInitialize;
    long tstart,tend,tCapMillisecond;
    int kind;
    int position;
    bool isStopPreview;





};








#endif // EPI1000_H_INCLUDED
