#ifndef _XMATCHPRO_H_
#define _XMATCHPRO_H_


#include "dictionary.h"
#include "diffXmatch.h"
#include "js_list.h"

#define HEADER_LENGTH		1

#define COMPLETE_MATCHED	0
#define UNMATCHED			15

#define MATCHED_HEADER		1
#define UNMATCHED_HEADER	0

#define NUM_BYTES_PER_TOKEN 4
#define MATCHE_TYPE_LENGTH	NUM_BYTES_PER_TOKEN
#define NUM_BITS_PER_TOKEN  NUM_BYTES_PER_TOKEN*8


struct xmatchContext
{
	//input, output pointer
	unsigned int* input;
	unsigned int* output;
	//size of input
	int inputSize;
	//read, write counter	
	int readCnt;
	int writeCnt;
	//linked list based dictionary
	DICTIONARY* dictionary;
	//buffer integer and number of valid bits
	unsigned int bitBuffer;
	int	numValidBit;
	//consecutive same data count
	int repetitiveCnt;
};


int xmatchCompress(unsigned char* input, int size, unsigned char* output);
int xmatchDecompress(unsigned char* input, int size, unsigned char* output);
void createXmatchContext(	struct xmatchContext* compContext, 
							unsigned char* input, 
							int size, 
							unsigned char* output);
void destroyXmatchContext( struct xmatchContext* xmatchContext);
int numMatch(int matchType);
int compare(unsigned int valueA, unsigned int valueB);
int updateDictionary(	unsigned int input, 
						int* index, 
						struct xmatchContext* compContext);
unsigned int restData(int matchType, unsigned int newEntry);
int xmatchEncode(struct xmatchContext* compContext);
void writeToOutput(	int numValidBitInData, 
					unsigned int encodedData, 
					struct xmatchContext* compContext);

int xmatchDecode(struct xmatchContext* decompContext, int orgSize);
unsigned int readInput(int numBitsToRead, struct xmatchContext* decompContext);
unsigned int getOrgData(int matchType, unsigned int refValue, unsigned int uniqueValue);
void rebuildDictionary(unsigned int orgData, int index, int matchType, DICTIONARY* dictionary);
void printfBinary(unsigned int input);

#endif
