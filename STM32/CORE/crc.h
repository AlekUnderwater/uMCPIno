// (C) Aleksandr Dikarev, 2015

#ifndef _CRC_H_
#define _CRC_H_

unsigned char CRC8_Update(unsigned char prev, unsigned char next);
unsigned char CRC8_Get(unsigned char* buffer, int sIdx, int cnt);

#endif
