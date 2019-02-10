#include "xmatchPro.h"

#define CHUNKSIZE 4096

int diffcompress(unsigned char* ref, unsigned char* src, unsigned char* dest);
int diffdecompress(unsigned char* ref, unsigned char* src, unsigned char* dest, int inputSize);
