// (C) Aleksandr Dikarev, 2013-2014

#include <ff.h>

void ff_fill_u8(unsigned char* dst, unsigned char value, unsigned int size)
{
	unsigned int blkCnt = size >> 2u;
	while (blkCnt > 0u)
	{
		*dst++ = value;
		*dst++ = value;
		*dst++ = value;
		*dst++ = value;

		blkCnt--;
	}

	blkCnt = size % 0x4u;

	while(blkCnt > 0u)
	{
		*dst++ = value;
		blkCnt--;
	}
}

void ff_copy_u8(unsigned char* src, unsigned char* dst, unsigned int size)
{
	unsigned int blkCnt = size >> 2u;
	while (blkCnt > 0u)
	{
		*dst++ = (*src++);
		*dst++ = (*src++);
		*dst++ = (*src++);
		*dst++ = (*src++);

		blkCnt--;
	}

	blkCnt = size % 0x4u;

	while(blkCnt > 0u)
	{
		*dst++ = (*src++);
		blkCnt--;
	}
}
