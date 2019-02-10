#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xmatchPro.h"

int matchTypeToNumMatch[16] = {4, 3, 3, 2, 3, 2, 2, 0, 3, 2, 2, 0, 2, 0, 0, 0};
int validByte[16][4] = {
	{0, 0, 0, 0},
	{0, 0, 0, 1},
	{0, 0, 1, 0},
	{0, 0, 1, 1},
	{0, 1, 0, 0},
	{0, 1, 0, 1},
	{0, 1, 1, 0},
	{1, 1, 1, 1},
	{1, 0, 0, 0},
	{1, 0, 0, 1},
	{1, 0, 1, 0},
	{1, 1, 1, 1},
	{1, 1, 0, 0},
	{1, 1, 1, 1},
	{1, 1, 1, 1},
	{1, 1, 1, 1}};

int xmatchCompress(unsigned char* input, int size, unsigned char* output)
{
	int retValue;
	int* orgSize = (int*) output;

	struct xmatchContext* compContext = (struct xmatchContext*) malloc(sizeof(struct xmatchContext));


	//The first 4bytes is the original size of data.(number of integers)
	createXmatchContext(compContext, input, size, (output + 4));
	(*orgSize) = compContext->inputSize;
	
	retValue = xmatchEncode(compContext) + 4;

	destroyXmatchContext(compContext);

	return retValue;
}

int xmatchDecompress(unsigned char* input, int size, unsigned char* output)
{
	int retValue;
	int* orgSize = (int*) input;

	struct xmatchContext* decompContext = (struct xmatchContext*) malloc(sizeof(struct xmatchContext));

	//first 4bytes is the original size of compressed data.(number of integers)
	createXmatchContext(decompContext, input + 4, size - 4, output);

	retValue = xmatchDecode(decompContext, *orgSize);

	destroyXmatchContext(decompContext);

	return retValue;
}

//initialize compressor context
void createXmatchContext(	struct xmatchContext* compContext, 
							unsigned char* input, 
							int size, 
							unsigned char* output)
{
	compContext->input = (unsigned int*) input;
	compContext->output = (unsigned int*) output;
	
	compContext->inputSize = size / NUM_BYTES_PER_TOKEN;
	if((size % NUM_BYTES_PER_TOKEN) != 0)
	{
		(compContext->inputSize)++;
	}
	
	compContext->readCnt = 0;
	compContext->writeCnt = 0;

	compContext->dictionary = createDictionary();

	compContext->bitBuffer = 0;
	compContext->numValidBit = 0;

	compContext->repetitiveCnt = 0;
}

void destroyXmatchContext( struct xmatchContext* xmatchContext)
{
	destroyDictionary(xmatchContext->dictionary);
	free(xmatchContext);
}

//return number of matched byte from match type
int numMatch(int matchType)
{
	return matchTypeToNumMatch[matchType];
}

//return match type of two entries
int compare(unsigned int valueA, unsigned int valueB)
{
	int retValue = 0;
	int i;
	for(i = 3 ; i >= 0 ; i--)
	{
		retValue =  (retValue << 1) + (((unsigned char) (valueA >> (8 * i))) != ((unsigned char) (valueB >> (8 * i))));
	}
	
	return retValue;
}

//find matched entry with input value in the dictionary and update it
//return value = matching type
//store the index of updated entry in input "index" pointer
int updateDictionary(	unsigned int input, 
						int* index, 
						struct xmatchContext* compContext)
{
	int i, tempMatchType, maxMatch = 0, maxMatchType = UNMATCHED;
	int* repetitiveCnt = (&compContext->repetitiveCnt);

	DICTIONARY* dictionary = compContext->dictionary;
	DIC_ENTRY* curEntry;

	//search all entries in dictionary
	for(i = 0, curEntry = getEntry(0, dictionary); i < numEntry(dictionary) ; i++, curEntry = getNextEntry(curEntry))
	{
		tempMatchType = compare(getValueOfEntry(curEntry), input);
		if(numMatch(tempMatchType) > maxMatch)
		{
			(*index) = i;
			maxMatchType = tempMatchType;
			maxMatch = numMatch(tempMatchType);

			//found completely matched entry: escape the loop.
			if(maxMatchType == COMPLETE_MATCHED)
			{
				break;
			}
		}
	}

	//complete match
	if(maxMatchType == COMPLETE_MATCHED)
	{
		//Complete match occured repetitively.
		if((*index) == 0)
		{
			//maxium repetitive match
			if((*repetitiveCnt) != DICTIONARY_SIZE)
			{
				//increase repetitive reference count.
				(*repetitiveCnt)++;
				//To refer the matche is repetitive, make index out of range.
				(*index) = DICTIONARY_SIZE;
				
			}
		}
		else
		{
			moveFront(curEntry, dictionary);
		}
	}
	//not complete match
	else
	{
		//unmatched: The reference index is 0.
		if(maxMatchType == UNMATCHED)
		{
			(*index) = 0;
		}
		//insert the new entry at the first place in the dictionary
		insertEntry(0, createEntry(input), dictionary);
	}

	return maxMatchType;
}

unsigned int restData(int matchType, unsigned int newEntry)
{
	int i;
	unsigned int retValue = 0;
	unsigned char* newEntryChar = (unsigned char*) &newEntry;

	for(i = 0 ; i < NUM_BYTES_PER_TOKEN ; i++)
	{
		//check validity of new entry
		if(validByte[matchType][i] == 1)
		{
			retValue = (retValue << 8) | newEntryChar[NUM_BYTES_PER_TOKEN - (i + 1)];
		}
	}
	return retValue;
}

int xmatchEncode(struct xmatchContext* compContext)
{
	int index, matchType, inputSize = compContext->inputSize;
	int* readCnt = &(compContext->readCnt);
	int* repetitiveCnt = (&compContext->repetitiveCnt);
	unsigned int* input = compContext->input;
	unsigned int newEntry;
	
	while(*readCnt < inputSize)
	{
		newEntry = input[(*readCnt)++];

		matchType = updateDictionary(newEntry, &index, compContext);
		//If the matche is repetitive: need incoding
		if(index != DICTIONARY_SIZE)
		{
			//There is data to write that repetitively matched.
			if((*repetitiveCnt) != 0)
			{
				//store header
				writeToOutput(HEADER_LENGTH, MATCHED_HEADER, compContext);
				//store the index of referenced entry in dictionary: size of dictionary
				writeToOutput(LOG_DICTIONARY_SIZE, DICTIONARY_SIZE, compContext);
				//store the repetitive count
				writeToOutput(LOG_DICTIONARY_SIZE, (*repetitiveCnt), compContext);
			}

			(*repetitiveCnt) = 0;
			
			//not matched
			if(matchType == UNMATCHED)
			{
				//store header
				writeToOutput(HEADER_LENGTH, UNMATCHED_HEADER, compContext);
				//store data
				writeToOutput(NUM_BITS_PER_TOKEN, newEntry, compContext);
			}
			//matched
			else
			{
				//store header
				writeToOutput(HEADER_LENGTH, MATCHED_HEADER, compContext);
				//store the index of referenced entry in dictionary
				writeToOutput(LOG_DICTIONARY_SIZE, index, compContext);
				//store the match type
				writeToOutput(MATCHE_TYPE_LENGTH, matchType, compContext);
				//write rest of data
				if(matchType != COMPLETE_MATCHED)
				{
					writeToOutput(8*(NUM_BYTES_PER_TOKEN - numMatch(matchType)), restData(matchType, newEntry), compContext);
				}
			}
		}
	}
	//compression is end with repetitive reference.
	if((*repetitiveCnt) != 0)
	{
		//store header
		writeToOutput(HEADER_LENGTH, MATCHED_HEADER, compContext);
		//store the index of referenced entry in dictionary: size of dictionary
		writeToOutput(LOG_DICTIONARY_SIZE, DICTIONARY_SIZE, compContext);
		//store the repetitive count
		writeToOutput(LOG_DICTIONARY_SIZE, (*repetitiveCnt), compContext);
	}

	//If there is remain data in the bit buffer: write it.
	if(compContext->numValidBit != 0)
	{
		compContext->output[(compContext->writeCnt)++] = (compContext->bitBuffer << (NUM_BITS_PER_TOKEN - compContext->numValidBit));
	}

	//return compressed size
	return NUM_BYTES_PER_TOKEN*(compContext->writeCnt);
}

//write encoded data to the output buffer through bit buffer
void writeToOutput(	int numValidBitInData, 
					unsigned int encodedData, 
					struct xmatchContext* compContext)
{
	unsigned int* output = compContext->output;
	unsigned int* bitBuffer = &(compContext->bitBuffer);

	int* writeCnt = &(compContext->writeCnt);
	int* numValidBitInBuffer = &(compContext->numValidBit);

	int remainBits;// = NUM_BITS_PER_TOKEN - (*numValidBitInBuffer);

	if((*numValidBitInBuffer) == NUM_BITS_PER_TOKEN)
	{
		output[(*writeCnt)++] = (*bitBuffer);
		(*bitBuffer) = 0;
		(*numValidBitInBuffer) = 0;
	}

	remainBits = NUM_BITS_PER_TOKEN - (*numValidBitInBuffer);

	//There is not enough space for the new data in the bit-buffer.
	if(numValidBitInData > remainBits)
	{
		//Store the bit-buffer appending part of the new data
		output[(*writeCnt)++] = ((*bitBuffer) << remainBits) |
		   						(encodedData >> (numValidBitInData - remainBits));
		//updata the bit-buffer from the remain part of the new data
		(*bitBuffer) = encodedData;
		(*numValidBitInBuffer) = (numValidBitInData - remainBits);
	}
	//There is enough space for the new data in the bit-buffer.
	else
	{
		//Size of encoded data is maxium value and bit-buffer is empty:
		//Store the encoded data directly without any update of bit-buffer.
		if(numValidBitInData == NUM_BITS_PER_TOKEN)
		{
			output[(*writeCnt)++] = encodedData;
		}
		//Just update in bitBuffer.
		else
		{
			(*bitBuffer) = (((*bitBuffer) << numValidBitInData) | encodedData);	
			(*numValidBitInBuffer) += numValidBitInData;
		}
	}
}

int xmatchDecode(struct xmatchContext* decompContext, int orgSize)
{
	unsigned int *output = decompContext->output;
	int* writeCnt = &(decompContext->writeCnt);
	DICTIONARY* dictionary = decompContext->dictionary;
	
	int matchType, isMatch, index, repetitiveCnt = 0;

	unsigned int decodedData;

	while((*writeCnt) < orgSize)
	{
		isMatch = readInput(HEADER_LENGTH, decompContext);
		if(isMatch == UNMATCHED_HEADER)
		{
			matchType = UNMATCHED;	
			decodedData = readInput(NUM_BITS_PER_TOKEN, decompContext);
		}
		else
		{
			index = (int) readInput(LOG_DICTIONARY_SIZE, decompContext); 
			if(index == DICTIONARY_SIZE)
			{
				repetitiveCnt = (int) readInput(LOG_DICTIONARY_SIZE, decompContext);
				decodedData = getValueOfEntry(getEntry(0, dictionary));
			}
			else
			{
				matchType = (int) readInput(MATCHE_TYPE_LENGTH, decompContext);	
				if(matchType != COMPLETE_MATCHED)
				{
					decodedData = readInput(8*(NUM_BYTES_PER_TOKEN - numMatch(matchType)), decompContext);
				}
				decodedData = getOrgData(matchType, getValueOfEntry(getEntry(index, dictionary)), decodedData);
			}
		}

		if(repetitiveCnt != 0)
		{
			while(repetitiveCnt != 0)
			{
				repetitiveCnt--;
				output[(*writeCnt)++] = decodedData;
			}
		}
		else
		{
			output[(*writeCnt)++] = decodedData;
			rebuildDictionary(decodedData, index, matchType, dictionary);
		}
	}
	return (*writeCnt)*NUM_BYTES_PER_TOKEN;
}

unsigned int readInput(int numBitsToRead, struct xmatchContext* decompContext)
{
	unsigned int* input = decompContext->input;	
	unsigned int* bitBuffer = &(decompContext->bitBuffer);
	int* readCnt = &(decompContext->readCnt);
	int* numValidBit = &(decompContext->numValidBit);
	int retValue;

	#define MASK(n) (0xFFFFFFFF>>(32-n))

	if(numBitsToRead == 0)
	{
		return -1;
	}

	if((*numValidBit) == 0)
	{
		(*bitBuffer) = input[(*readCnt)++];
		(*numValidBit) = 32;
	}

	if(numBitsToRead == (*numValidBit))
	{
		retValue = (MASK(numBitsToRead) & (*bitBuffer));
		(*bitBuffer) = input[(*readCnt)++];
		(*numValidBit) = 32;
	}
	else if(numBitsToRead < (*numValidBit))
	{
		retValue = (MASK(numBitsToRead)) & ((*bitBuffer) >> ((*numValidBit) - numBitsToRead));
		(*numValidBit) -= numBitsToRead;
	}
	else
	{
		retValue = (MASK(NUM_BITS_PER_TOKEN - (*numValidBit)) & (*bitBuffer));
		retValue <<= (numBitsToRead - (*numValidBit));

		(*bitBuffer) = input[(*readCnt)++];
		(*numValidBit) = NUM_BITS_PER_TOKEN - (numBitsToRead - (*numValidBit));

		retValue |= ((*bitBuffer) >> (*numValidBit));
	}

	//printf("read:%d, refCnt:%d\n", retValue, *readCnt);
	return retValue;
}

unsigned int getOrgData(int matchType, unsigned int refValue, unsigned int uniqueValue)
{
	int i;
	unsigned int retValue;
	unsigned char* refValueChar = (unsigned char*) &refValue;
	unsigned char nextChar = (numMatch(matchType) == 2) ? (unsigned char) (uniqueValue >> 8) : (unsigned char) uniqueValue;

	if(matchType == COMPLETE_MATCHED)
	{
		retValue = refValue;
	}
	else
	{
		for(i = 0 ; i < NUM_BYTES_PER_TOKEN ; i++)
		{
			retValue <<= 8;
			if(validByte[matchType][i] == 1)
			{
				retValue |= nextChar;
				nextChar = (unsigned char) uniqueValue;
			}
			else
			{
				retValue |= refValueChar[NUM_BYTES_PER_TOKEN - 1 - i];
			}
		}
	}

	return retValue;
}

void rebuildDictionary(unsigned int orgData, int index, int matchType, DICTIONARY* dictionary)
{
	DIC_ENTRY* curEntry = getEntry(index, dictionary);
	if(matchType == COMPLETE_MATCHED)
	{
		if(index != 0)
		{
			moveFront(curEntry, dictionary);
		}
	}
	else
	{
		insertEntry(0, createEntry(orgData), dictionary);
	}
}

void printfBinary(unsigned int input)
{
	int i;
	for(i = 0 ; i <NUM_BITS_PER_TOKEN ; i++)
	{
		printf("%d", ((input << i) >> (NUM_BITS_PER_TOKEN - 1)));
	}
}
