#include <stdio.h>
#include <stdlib.h>

#include "stega.h"
#include "file_utils.h"

void usage( const char* prgName ) {
	printf("Usage %s <file to hide> <file where to hide> <resulting output file>\n", prgName);
}
	
int main ( int argc, char** argv) {
	FILE* inputFp;
	size_t inputSize;

	FILE* hideoutFp;
	size_t hideoutSize;

	FILE* outputFp;

	if ( argc < 4 ) {
		usage( argv[0] );
		exit(1);
	}	

	inputFp = my_fopen(argv[1],"rb");

	/* I try to stablish the input file size */
	inputSize = getFileSize( inputFp );

	hideoutFp = my_fopen(argv[2],"rb");
	hideoutSize = getFileSize( hideoutFp );

	printf("Input file size = %lu\n", (unsigned long) inputSize );
	printf("Hideout file size = %lu\n", (unsigned long) hideoutSize );

	outputFp = my_fopen(argv[3],"w");

	/* Small sanity check */
	if ( inputSize > hideoutSize ) {
		printf("You are trying to hide %lu bytes into", (unsigned long) inputSize );
		printf(" %lu bytes. Good luck!\n", (unsigned long) hideoutSize );
		exit(1);
	}

	RiggTheFile( outputFp, inputFp, inputSize, hideoutFp, hideoutSize, 1 ); 
	
	fclose( hideoutFp );
	fclose(inputFp);
	fclose( outputFp );

	return 0;
}
