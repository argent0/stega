#include <stdio.h>
#include <stdlib.h>

#include "stega.h"
#include "file_utils.h"
#include "debug.h"
			
void usage( const char* prgName ) {
	printf("Usage %s <file_where_information_is_hidden> <informatoin_lenght> <resulting output file>\n", prgName);
}
	
int main ( int argc, char** argv) {
	FILE* hideoutFp;
	size_t hideoutSize;
	size_t len;
	FILE* outputFp;

	if ( argc < 4 ) {
		usage( argv[0] );
		exit(1);
	}	

	hideoutFp = my_fopen(argv[1],"rb");
	hideoutSize = getFileSize( hideoutFp );

	WATCH_SIZE_T(hideoutSize);

	len = (size_t) strtoull(argv[2],NULL,10);

	WATCH_SIZE_T(len);

	if ( len == 0 ) {
		printf("No data to look for.\n");
		exit(1);
	}

	outputFp = my_fopen(argv[3],"wb");

	/* Small sanity check */
	if ( len > hideoutSize ) {
		printf("You are trying to recover %lu bytes from", (unsigned long) len );
		printf(" %lu bytes. Good luck!\n", (unsigned long) hideoutSize );
		exit(1);
	}

	recoverTheFile( outputFp, len, hideoutFp, hideoutSize, 1 );

	fclose(outputFp);
	fclose(hideoutFp);

	return 0;
}
