#ifndef PC
#define srand(seed){*((uint32*)(0xcc)) = (uint32)seed;}
#define rand()(((*((uint32*)(0xcc)) = *((uint32*)(0xcc)) * 214013L + 2531011L) >> 16) & 0x7fff)
#else
#define rand		random_1
#endif
