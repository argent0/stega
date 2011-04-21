#include <stdio.h>
#include <png.h>
#include <stdlib.h>
#include <string.h>

#include "png_util.h"

void usage( const char* prgName ) {
	printf("Usage %s <PNG file>\n", prgName);
}

int main ( int argc, char** argv) {

	FILE* fp;

	size_t width = 0;
	size_t height = 0;
	size_t depth = 0;
	size_t channels = 0;

	size_t r; /* counter for rows */

	PNGImage* image;

	if ( argc < 2 ) {
		usage( argv[0] );
		exit(1);
	}	

	image = readPNGfile(argv[1]);
	 
	if ( image == NULL ) {
		exit(1);
	}

	/* Retrive basic image information */
	width = png_get_image_width( image->png, image->info );
	height = png_get_image_height( image->png, image->info );
	depth = png_get_bit_depth( image->png, image->info );
	channels = png_get_channels( image->png, image->info );

	printf("Image Info:\nWidth :%d\nHeight :%d\nBit depth:%d\n", width, height, depth );
	printf("Channels :%u\n", channels);


	printf("Creating info file\n"); /*-------------------------------------------*/

	fp = fopen("info","wb");

	if (!fp) {
		printf("Could not create info file\n");
		png_destroy_read_struct( &image->png, &image->info, &image->end);
		exit(1);
	}

	fwrite(image->info, sizeof(png_info), 1,fp );

	fclose(fp);

	printf("Creating end file\n"); /*-------------------------------------------*/

	fp = fopen("end","wb");

	if (!fp) {
		printf("Could not create end file\n");
		png_destroy_read_struct( &image->png, &image->info, &image->end);
		exit(1);
	}

	fwrite(image->end, sizeof(png_info), 1,fp );

	fclose(fp);

	printf("Creating raw file\n"); /*-------------------------------------------*/
	fp = fopen("raw","wb");

	if (!fp) {
		printf("Could not create raw file\n");
		png_destroy_read_struct( &image->png, &image->info, &image->end);
		exit(1);
	}

	for ( r=0; r < height; r = r+1 )
		fwrite(image->row_pointers[r], sizeof(png_byte), width, fp );

	fclose(fp);

	printf("Releasing memory\n");	
	png_destroy_read_struct( &image->png, &image->info, &image->end);

	return 0;
}
