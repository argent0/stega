#ifndef H_STEGA
#define H_STEGA

#include <stdint.h>
#include <stdio.h>

#define BITS_PER_CHAR 8

void uint8ToBinary(const uint8_t c, char* bin ); 
uint8_t binaryToUint8(const char* bin);
void setbit( const char, size_t, char* );
int setbitOnBuffer( const char , size_t , uint8_t* , size_t , size_t );
int hideUint8_tOnBuffer(const uint8_t , uint8_t* , size_t , size_t );

void RiggTheFile(FILE* output,
		FILE* input, 
		const size_t inputLen,
		FILE* hideout, 
		const size_t hideoutLen, 
		const size_t hideoutItemSize);

uint8_t squeezeBuffer(const uint8_t *buffer, const size_t bufferLenght, const size_t bufferItemSize );

void recoverTheFile(FILE* recovered,
		const size_t inputLen,
		FILE* hideout, 
		const size_t hideoutLen, 
		const size_t hideoutItemSize);

#endif
