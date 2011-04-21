#include <stdio.h>
#include <stdlib.h>
#include "file_utils.h"

size_t getFileSize( FILE* fp ) {
	/* Returns the total size of the file */
	size_t ret;
	fseek( fp, 0L, SEEK_END );
	ret = ftell ( fp );
	fseek( fp, 0L, SEEK_SET );
	return ret;
}

FILE* my_fopen( const char* name, const char* mode ) {
	/* Opens a file with error checking */
	FILE* fp;
	fp = fopen(name,mode);
	if ( fp == NULL ) {
		printf("Can not open file %s.\n",name);
		exit(1);
	}
	return fp;
}
