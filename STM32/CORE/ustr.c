// (C) Aleksandr Dikarev, 2015-2019

#include <ustr.h>
#include <ff.h>

void StrB_WriterInit(unsigned char* buffer, unsigned int* srcIdx, unsigned int size)
{
	*srcIdx = 0;
	ff_fill_u8(buffer, 0, size);
}

void StrB_WriteByte(unsigned char* buffer, unsigned int* srcIdx, unsigned char c)
{
	buffer[*srcIdx] = c;
	(*srcIdx)++;
}


