#ifndef BITS_H
#define BITS_H
#define BITREAD(value,bit)(((value) >> (bit)) & 0x1)
#define BITSET(value,bit)((value) |= (1UL << (bit)))
#define BITCLEAR(value,bit)((value) &= ~(1UL << (bit)))
#endif