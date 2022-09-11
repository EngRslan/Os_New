#ifndef BITS_H
#define BITS_H
#define BITREAD(value,bit)(((value) >> (bit)) & 0x1)
#define BITSET(value,bit)((value) |= (1UL << (bit)))
#define BITCLEAR(value,bit)((value) &= ~(1UL << (bit)))
#define SWITCH_ENDIAN16(nb)((uint16_t)((nb>>8)|(nb<<8)))
#define SWITCH_ENDIAN32(nb)((uint32_t)(((nb>>24)&0xFF) | ((nb<<8)&0xff0000) | ((nb>>8)&0xFF00) | (nb<<24)&0xFF000000))
#define SWITCH_BITS(nb,b)((uint8_t)((nb>>b)|(nb<<b)))
#endif