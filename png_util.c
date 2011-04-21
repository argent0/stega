#include "png_util.h"
#include <stdlib.h>

PNGImage* readPNGfile( const char* filename ) {
	/* Reads the PNG image with *filename*, and does all the error checking. */
	PNGImage* ret=NULL;
	FILE* imgfp = NULL;
	png_byte header[8];
	size_t height;

	ret = (PNGImage*) malloc ( sizeof(PNGImage) );

	imgfp = fopen(filename,"rb");
	
	if ( imgfp == NULL ) {
		printf("Error opening File\n");
		free ( ret );
		ret = NULL;
	}
	else	{
		if ( fread( header,sizeof(png_byte),8,imgfp) < 8 ) {
			printf("Error reading header\n");
			free ( ret );
			ret =NULL;
		}
		else {
			if ( png_sig_cmp ( header,0,8) != 0 ) {
				printf("PNG header not found.\n");
				free ( ret );
				ret = NULL;
			}
			else {
				ret->png = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
				if ( ret->png == NULL ) {
					printf("Error creating read struct\n");
					free(ret);
					ret = NULL;
				}
				else {
					ret->info = png_create_info_struct( ret->png );
				
					if ( ret->info == NULL ) {
						png_destroy_read_struct(&ret->png , (png_infopp)NULL, (png_infopp)NULL);
						printf("Error creating PNG info.\n");
						free ( ret );
						ret = NULL;
					}
					else {
						ret->end = png_create_info_struct( ret->png );

						if ( ret->end == NULL ) {
							png_destroy_read_struct(&ret->png , &ret->info, (png_infopp)NULL);
							printf("Error creating PNG *end* info.\n");
							free(ret);
							ret = NULL;
						}
						else {
							if ( setjmp(png_jmpbuf(ret->png)) != 0 ) {
								printf("PNG exeption.\n");
								exit(1);
							}
							else {
								/* set up the input code */
								png_init_io( ret->png, imgfp );

								/* tell libpng that i already readed 8 bytes for the signature */
								png_set_sig_bytes( ret->png, 8 );

								png_read_png(ret->png,ret->info,PNG_TRANSFORM_IDENTITY,NULL);

								height = png_get_image_height( ret->png, ret->info );

								ret->row_pointers = (png_bytep*) malloc(sizeof(png_bytep)*height);

								ret->row_pointers = png_get_rows(ret->png, ret->info);
							}
						}
					}
				}
			}
		}
	}

	if (imgfp != NULL) fclose(imgfp);
	return ret;
	
}
