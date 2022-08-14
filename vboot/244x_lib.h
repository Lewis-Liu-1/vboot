//===================================================================
// File Name : 2442lib.h
// Function  : S3C2442 
// Date      : February 26, 2002
// Version   : 0.0
// History
//  0.0 :Feb.20.2002:SOP     : Programming start
//  0.01:Mar.29.2002:purnnamu: For POWEROFF_wake_up, the START... label is added
//===================================================================

#ifndef __2442lib_h__
#define __2442lib_h__
#define NO_USE_STANDARD_C_LIB

void *memset(void *dst, int src, unsigned int len);
void *memcpy(void *dst, const void *src, unsigned int len);

#define min(x1,x2) (((x1)<(x2))? (x1):(x2))
#define max(x1,x2) (((x1)>(x2))? (x1):(x2))

// 2442lib.c
void Port_Init(void);
void Uart_SendString(char *pt);

#endif				//__2442lib_h__
