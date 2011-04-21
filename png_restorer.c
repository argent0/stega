#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#include "png_util.h"

int main(int argc, char** argv) {
	FILE *outFp;
	FILE *fp;
	PNGImage *reference;
	size_t height;
	size_t width;

	if (argc < 3) {
		printf("Usage:\n%s <reference PNG> <raw file> <output file>\n",argv[0]);
		exit(1);
	}

	reference = readPNGfile(argv[1]);

	if (!reference) {
		printf("Could not read the reference PNG");
		exit(1);
	}

	/* Reading the raw file */

	fp = fopen(argv[2],"rb");
	if (!fp) {
		printf("Could not open raw file %s\n", argv[1] );
		exit(1);
	}

	height = png_get_image_height( reference->png, reference->info );
	width = png_get_image_width( reference->png, reference->info );

	size_t r;
	for ( r=0; r < height; r = r+1 )
		fread(reference->row_pointers[r],sizeof(png_byte),width,fp);

	fclose(fp);

	outFp = fopen(argv[3],"wb");
	if (!outFp) {
		printf("Could not create file %s.\n",argv[3]);
		exit(1);
	}
	
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr) {
		printf("Could not create write structure.\n");
		exit(1);
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		exit(1);
	}

	png_init_io(png_ptr, outFp);

	png_write_png(png_ptr,reference->info,PNG_TRANSFORM_IDENTITY,NULL);

	fclose(outFp);

	png_destroy_write_struct(&png_ptr, (png_infopp)NULL );
	
	png_destroy_read_struct( &reference->png, &reference->info, &reference->end);
	return 0;
}
