#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED
#include "stddef.h"
struct buffer {
      void   *data;
      size_t  size;

};

struct RGBImage {
      unsigned char   *data; // RGB888 <=> RGB24
      size_t          width;
      size_t          height;
      size_t          size; // width * height * 3
};

struct BIOMTQS_ISO
{
unsigned char   *data;
 size_t          length;
};


#endif // DATA_H_INCLUDED
