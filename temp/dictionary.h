#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include "js_list.h"

#define DICTIONARY_SIZE		255
#define LOG_DICTIONARY_SIZE 8

#define DICTIONARY	struct jsList
#define DIC_ENTRY	struct nodeJSList

DIC_ENTRY* createEntry(unsigned int value);
DICTIONARY* createDictionary(void);
void destroyDictionary(DICTIONARY* dictionary);
int numEntry(DICTIONARY* dictionary);
unsigned int getValueOfEntry(DIC_ENTRY* entry);
DIC_ENTRY* getNextEntry(DIC_ENTRY* entry);
DIC_ENTRY* getPrevEntry(DIC_ENTRY* entry);
DIC_ENTRY* getEntry(int index, DICTIONARY* dictionary);
int moveFront(DIC_ENTRY* entry, DICTIONARY* dictionary);
int insertEntry(int index, DIC_ENTRY* entry, DICTIONARY* dictionary);


#endif
