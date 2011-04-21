#ifndef H_PNGUTIL
#define H_PNGUTIL

#include <png.h>

typedef struct {
	png_structp png;
	png_infop info;
	png_infop end;
	png_bytep *row_pointers;
} PNGImage;

PNGImage* readPNGfile( const char* filename );

#endif
