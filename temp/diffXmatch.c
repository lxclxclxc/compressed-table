#ifndef _DIFFXMATCH_C
#define _DIFFXMATCH_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "diffXmatch.h"

int diffcompress(unsigned char* ref, unsigned char* src, unsigned char* dest)
{
 	int i, retValue;
	unsigned char temp[CHUNKSIZE * 2];

	for(i = 0 ; i < CHUNKSIZE ; i++)
	{
		temp[i] = (ref[i] ^ src[i]);
	}

	retValue = xmatchCompress(temp, CHUNKSIZE, dest);
	
	return retValue;
}

int diffdecompress(unsigned char* ref, unsigned char* src, unsigned char* dest, int inputSize)
{
	int i, retValue;	
	unsigned char temp[CHUNKSIZE];

	retValue = xmatchDecompress(src, inputSize, temp);

	for(i = 0 ; i <  CHUNKSIZE ; i++)
	{
		dest[i] = (ref[i] ^ temp[i]);
	}

	return retValue;
}

#endif
