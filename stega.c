#include <string.h>
#include <stdlib.h>
#include "stega.h"
#include "debug.h"

void uint8ToBinary(const uint8_t c, char* bin ) {
	/* Converts an uint8_t number to its binary representation and stores the
	 * value as a string
	 *
	 * c		:	The value to convert.
	 * bin	:	Where to store the resulting string.
	 */
	uint8_t old;
	uint8_t decimal = c;
	uint8_t remain;
	int k = 0;
	int n = 0;
	char out[BITS_PER_CHAR+1];
	memset(bin,'0',8);

	do {
		old = decimal;
		remain = decimal % 2;
		decimal = decimal / 2;

		out[k++] = remain + '0';
	} while ( decimal > 0 );

	n = BITS_PER_CHAR - k;

	while (k>=0)
		bin[n++] = out[--k];

	bin[BITS_PER_CHAR] = 0;

}

uint8_t binaryToUint8(const char* bin) {
	/* The inverse of uint8ToBinary
	 *
	 * bin	:	The string with the binary representation.
	 *
	 * *bin* is always 8 char long + 1 for null string termination 
	 */
	int i;
	uint8_t ret=0;
	for ( i = 0; i < BITS_PER_CHAR; i ++) {
		ret = ret << 1;
		if ( bin[i] == '1' ) 
			ret = ret + 1;
	}
	return ret;
}

void setbit( const char value, const size_t pos , char* dest ){
	/* Sets the bit at position *pos* in the binary number *dest* to value *value* 
	 *
	 * value	:	'0' or '1'
	 * pos	:	Bit position. LSB first.
	 * dest	:	The binary string where changes take place.
	 */
	dest[BITS_PER_CHAR-pos-1] = value;
}

int setbitOnBuffer( const char value, const size_t pos, uint8_t* dest, const size_t destLen, const size_t destItemSize ) {
	/* Buffer version of setbit
	 *	
	 * value	:	'0' or '1'
	 * pos	:	Bit position. LSB first.
	 * dest	:	The buffer where changes take place.
	 */
	char binstr[BITS_PER_CHAR+1];
	size_t arrayCoordinate = pos/(BITS_PER_CHAR*destItemSize);
	size_t bitCoordinate = pos%(BITS_PER_CHAR*destItemSize);
	size_t uint8_tCoordinate = bitCoordinate/(BITS_PER_CHAR);

	size_t finalCoordinate;

	bitCoordinate = bitCoordinate%(BITS_PER_CHAR);

	finalCoordinate = arrayCoordinate * destItemSize + uint8_tCoordinate;

	if ( arrayCoordinate > destLen ) {
		return -1; /* Index out of range */
	}

	uint8ToBinary(dest[finalCoordinate],binstr);

	setbit(value,bitCoordinate,binstr);

	dest[finalCoordinate] = binaryToUint8(binstr);

	return 0;
}

int hideUint8_tOnBuffer(const uint8_t data, uint8_t* buffer, const size_t bufferLen, const size_t bufferItemSize ) {
	/*	Hides one uint8_t *data* into the buffer *buffer*
	 *
	 *	data		:	The value to store(hide).
	 *	buffer	:	Where to hide.
	 *
	 */
	char binstr[BITS_PER_CHAR+1];
	size_t i;	/* conter */
	size_t separation = bufferLen * bufferItemSize;
	
	uint8ToBinary(data,binstr);

	for ( i=0; i<BITS_PER_CHAR; i=i+1 ) {
		setbitOnBuffer(binstr[i],separation*i,buffer,bufferLen, sizeof(uint8_t)); 
	}

	return 0;
}

void RiggTheFile(FILE* output,
		FILE* input, 
		const size_t inputLen,
		FILE* hideout, 
		const size_t hideoutLen, 
		const size_t hideoutItemSize) {
	/*	output				:	The file descriptor where the rigged data is written.
	 *	input					:	The file where the data is read from
	 *	inputLen				:	The size in uint8_t of the input
	 *	hideout				:	The file descriptor of the place where to hide the data.
	 *	hideoutLen			:	The size in uint8_t units of the hideout.
	 *	hideoutItemSize	:	The size in uint8_t units of one information element
	 *								in the hideout. This should be useful to ensure the
	 *								use of the LSBs.
	 *
	 * Condition:
	 *		hideoutLen >= inputLen
	 *
	 */

	/* First of all, calculate and effective hideoutLen, so that only the LSBs
	 * are used. We make sure that hideoutEffectiveLen is a multiple of
	 * BITS_PER_CHAR and as large as posible */

	size_t hideoutEffectiveLen = hideoutLen - ( hideoutLen % BITS_PER_CHAR );
	size_t hideoutPerSource;
	size_t finalOutputSize;

	size_t i;	/* conter */
	uint8_t inputBuffer;

	uint8_t *hideoutBuffer;

	if ( hideoutEffectiveLen == 0 ) {
		hideoutEffectiveLen = hideoutLen;
	}

	/* hideoutPerSource has no units. */
	hideoutPerSource = hideoutEffectiveLen / inputLen;

	hideoutBuffer = (uint8_t*) malloc( hideoutPerSource * sizeof( uint8_t) );

	for ( i=0; i<inputLen; i++ ) {
		/* read one uint8_t from the input file */
		fread( &inputBuffer, sizeof(uint8_t), 1, input );
		/* read the dest buffer */
		fread( hideoutBuffer, sizeof(uint8_t), hideoutPerSource, hideout );
		/* hide the date in the buffer */
		hideUint8_tOnBuffer( inputBuffer, hideoutBuffer, hideoutPerSource, sizeof(uint8_t));
		/* write the resulting buffer to the output file */
		fwrite( hideoutBuffer, sizeof(uint8_t), hideoutPerSource, output );
	}

	free( hideoutBuffer );
	finalOutputSize = ftell( output );
	if ( finalOutputSize < hideoutLen ) {
		hideoutBuffer = (uint8_t*) malloc( hideoutLen - finalOutputSize );
		fread(hideoutBuffer, sizeof(uint8_t), hideoutLen-finalOutputSize, hideout );
		fwrite(hideoutBuffer, sizeof(uint8_t), hideoutLen - finalOutputSize, output );
		free ( hideoutBuffer );
	}

}

uint8_t squeezeBuffer(const uint8_t *buffer, const size_t bufferLenght, const size_t bufferItemSize ) {
	/* Recover one uint8_t hiden in the *buffer* */
	size_t separation = bufferLenght * bufferItemSize;
	char binstr[BITS_PER_CHAR+1];
	char retstr[BITS_PER_CHAR+1];
	size_t i;
	size_t arrayCoordinate;
	size_t bitCoordinate;
	uint8_t ret;
	
	bzero(retstr,BITS_PER_CHAR+1);

	for ( i=0; i<BITS_PER_CHAR; i=i+1) {
		arrayCoordinate	= (separation*i)/BITS_PER_CHAR;
		bitCoordinate		= (separation*i)%BITS_PER_CHAR;
		uint8ToBinary(buffer[arrayCoordinate],binstr);
		strncpy(retstr+i,binstr+(BITS_PER_CHAR-bitCoordinate-1),1);
	}

	ret = binaryToUint8(retstr);

	return ret;
	
}

void recoverTheFile(FILE* recovered,
		const size_t inputLen,
		FILE* hideout, 
		const size_t hideoutLen, 
		const size_t hideoutItemSize) {
	/*	Recovers a file form within another file.
	 *
	 * recovered			:	the recovered file handler
	 * inputLen				:	how many bytes to recover
	 * hideout				:	the file where the information is heidden
	 * hideoutLen			:	the size in bytes of the hideout file
	 * hideoutItemSize	:	how many bytes per information item is there on the
	 *								hideout 
	 */

	uint8_t cache; /* This's where the recovered bytes will be held before being writen to the file */
	uint8_t *cacheBuffer; /* where the bytes containing *one* hidden byte are loaded before decodification */

	size_t hideoutEffectiveLen = hideoutLen - (hideoutLen % BITS_PER_CHAR );
	size_t hideoutPerSource;

	size_t i;

	if ( hideoutEffectiveLen == 0) hideoutEffectiveLen = hideoutLen;

	hideoutPerSource = hideoutEffectiveLen / inputLen;

	/* Alloc memory */
	cacheBuffer = (uint8_t*) malloc(hideoutPerSource);

	for ( i = 0; i < inputLen; i = i+1 ) {
		fread( cacheBuffer, hideoutItemSize, hideoutPerSource, hideout );
		cache = squeezeBuffer( cacheBuffer, hideoutPerSource, hideoutItemSize );
		fwrite( &cache, sizeof(uint8_t), 1, recovered );
	}

	/*Free memory*/
	free(cacheBuffer);
}
