#ifndef H_FILE_UTILS
#define H_FILE_UTILS
#include <stdio.h>

size_t getFileSize( FILE* fp );

FILE* my_fopen( const char* name, const char* mode );

#endif
