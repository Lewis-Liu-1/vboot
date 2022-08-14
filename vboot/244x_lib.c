
#include "def.h"
#include "s3c2440.h"
#include "244x_lib.h"

#if 1
void *memset(void *dst, int src, unsigned int len)
{
	char *p = dst;
	while (len--)
		*p++ = src;
	return dst;
}
void *memcpy(void *dst, const void *src, unsigned int len)
{
	const char *s = src;
	      char *d = dst;
	while (len --) {
		*d++ = *s++;
	}
	return dst;
}
#endif

void Port_Init(void)
{
	GPACON = 0x7fffff;
	GPBCON = 0x044555;
	GPBUP = 0x7ff;		// The pull up function is disabled GPB[10:0]
	GPCCON = 0xaaaaaaaa;
	GPCUP = 0xffff;	// The pull up function is disabled GPC[15:0] 
	GPDCON = 0x00151544;
	GPDDAT = 0x0430;
	GPDUP = 0x877A;
	GPECON = 0xaa2aaaaa;
	GPEUP = 0xf7ff;	// GPE11 is NC
	GPFCON = 0x55aa;
	GPFUP = 0xff;		// The pull up function is disabled GPF[7:0]
	GPGCON = 1<<8;
	GPGDAT = 0;
	GPHCON = 0x16faaa;
	GPHUP = 0x7ff;		// The pull up function is disabled GPH[10:0]
	EXTINT0 = 0x22222222;	// EINT[7:0]
	EXTINT1 = 0x22222222;	// EINT[15:8]
	EXTINT2 = 0x22222222;	// EINT[23:16]
}

//=====================================================================
static inline void Uart_SendByte(int data)
{
	while (!(UTRSTAT0 & 0x2)) ;	//Wait until THR is empty.
	UTXH0 = data;
}

//====================================================================
void Uart_SendString(char *pt)
{
	while (*pt)
		Uart_SendByte(*pt++);
}
