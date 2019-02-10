#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"

DIC_ENTRY* createEntry(unsigned int value)
{
	return createNode(value);
}
DICTIONARY* createDictionary(void)
{
	DICTIONARY* newDictionary = (DICTIONARY*) malloc(sizeof(DICTIONARY));
	initList(newDictionary);			
	return newDictionary;
}
void destroyDictionary(DICTIONARY* dictionary)
{
	DIC_ENTRY* curEntry;
	while(numEntry(dictionary) != 0)
	{
		curEntry = getEntry(-1, dictionary);
		deleteNode(curEntry, dictionary);
		free(curEntry);
	}
	free(dictionary);
}

int numEntry(DICTIONARY* dictionary)
{
	return dictionary->numNode;
}

unsigned int getValueOfEntry(DIC_ENTRY* entry)
{
	return entry->value;
}

DIC_ENTRY* getNextEntry(DIC_ENTRY* entry)
{
	return entry->nextNode;
}

DIC_ENTRY* getPrevEntry(DIC_ENTRY* entry)
{
	return entry->prevNode;
}

DIC_ENTRY* getEntry(int index, DICTIONARY* dictionary)
{
	return getNode(index, dictionary);
}

int moveFront(DIC_ENTRY* entry, DICTIONARY* dictionary)
{
	deleteNode(entry, dictionary);	
	return insertNode(0, entry, dictionary);
}

int insertEntry(int index, DIC_ENTRY* entry, DICTIONARY* dictionary)
{
	DIC_ENTRY* curNode;
	if(insertNode(index, entry, dictionary) == -1)
	{
		printf("dictionary ERROR: insertion failed\n");
		return -1;
	}
	if(numEntry(dictionary) > DICTIONARY_SIZE)
	{
		curNode = getNode(-1, dictionary);
		deleteNode(curNode, dictionary);
		free(curNode);
	}
	return 0;
}
