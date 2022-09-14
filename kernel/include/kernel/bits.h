#ifndef BITS_H
#define BITS_H
#define BITREAD(value,bit)(((value) >> (bit)) & 0x1)
#define BITSET(value,bit)((value) |= (1UL << (bit)))
#define BITCLEAR(value,bit)((value) &= ~(1UL << (bit)))
#define SWITCH_ENDIAN16(nb)((uint16_t)(__builtin_bswap16(nb)))
#define SWITCH_ENDIAN32(nb)((uint32_t)(__builtin_bswap32(nb)))
#define SWITCH_BITS(nb,b)((uint8_t)((nb>>b)|(nb<<b)))
#endif