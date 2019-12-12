// (C) Aleksandr Dikarev, 2015-2019

#ifndef _USTR_H_
#define _USTR_H_

void StrB_WriterInit(unsigned char* buffer, unsigned int* srcIdx, unsigned int size);
void StrB_WriteByte(unsigned char* buffer, unsigned int* srcIdx, unsigned char c);

#endif
