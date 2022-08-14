/************************************************
 * NAME    : zboot.c
************************************************/
//#include <stdlib.h>
//#include <string.h>
#include "def.h"
#include "nand.h"
#include "244x_lib.h"
#include "s3c2440.h"

#include "parameters.h"

static inline void MMU_EnableICache(void)
{

	asm (
		"mrc p15,0,r0,c1,c0,0\n"
		"orr r0,r0,#(1<<12)\n"
		"mcr p15,0,r0,c1,c0,0\n"
	);
}

static inline void MMU_EnableDCache(void)
{
	asm (
		"mrc p15,0,r0,c1,c0,0\n"
		"orr r0,r0,#(1<<2)\n"
		"mcr p15,0,r0,c1,c0,0\n"
	);
}

static void CallLinux(void)
{
	struct param_struct {
		union {
			struct {
				unsigned long page_size;	/*  0 */
				unsigned long nr_pages;	/*  4 */
				unsigned long ramdisk_size;	/*  8 */
				unsigned long flags;	/* 12 */
				unsigned long rootdev;	/* 16 */
				unsigned long video_num_cols;	/* 20 */
				unsigned long video_num_rows;	/* 24 */
				unsigned long video_x;	/* 28 */
				unsigned long video_y;	/* 32 */
				unsigned long memc_control_reg;	/* 36 */
				unsigned char sounddefault;	/* 40 */
				unsigned char adfsdrives;	/* 41 */
				unsigned char bytes_per_char_h;	/* 42 */
				unsigned char bytes_per_char_v;	/* 43 */
				unsigned long pages_in_bank[4];	/* 44 */
				unsigned long pages_in_vram;	/* 60 */
				unsigned long initrd_start;	/* 64 */
				unsigned long initrd_size;	/* 68 */
				unsigned long rd_start;	/* 72 */
				unsigned long system_rev;	/* 76 */
				unsigned long system_serial_low;	/* 80 */
				unsigned long system_serial_high;	/* 84 */
				unsigned long mem_fclk_21285;	/* 88 */
			} s;
			char unused[256];
		} u1;
		union {
			char paths[8][128];
			struct {
				unsigned long magic;
				char n[1024 - sizeof(unsigned long)];
			} s;
		} u2;
		char commandline[1024];
	};

	struct param_struct *p = (struct param_struct *)0x30000100;
	memset(p, 0, sizeof(*p));
	memcpy(p->commandline, g_linux_cmd_line, sizeof(g_linux_cmd_line));
	p->u1.s.page_size = 4 * 1024;
	p->u1.s.nr_pages = 64 * 1024 * 1024 / (4 * 1024);

	{
		unsigned int *pp = (unsigned int *)(0x30008024);
		if (pp[0] == 0x016f2818) {  // Magic number of zImage
			//Uart_SendString("\n\rOk\n\r");
		} else {
			Uart_SendString("\n\rWrong Linux Kernel\n\r");
			for (;;) ;
		}

	}
 	asm (
		"mov	r5, %2\n"
		"mov	r0, %0\n"
		"mov	r1, %1\n"
		"mov	ip, #0\n"
		"mov	pc, r5\n"
		"nop\n" "nop\n":	/* no outpus */
		:"r"(0), "r"(1999), "r"(g_os_ram_start)
	);
}

void ReadImageFromNand(void)
{
	unsigned int Length;
	U8 *RAM;
	unsigned BlockNum;
	unsigned pos;

	Length = g_os_length;
	Length = (Length + BLOCK_SIZE - 1) >> (BYTE_SECTOR_SHIFT + SECTOR_BLOCK_SHIFT) << (BYTE_SECTOR_SHIFT + SECTOR_BLOCK_SHIFT); // align to Block Size

	BlockNum = g_os_start >> (BYTE_SECTOR_SHIFT + SECTOR_BLOCK_SHIFT);
	RAM = (U8 *) g_os_ram_start;
	for (pos = 0; pos < Length; pos += BLOCK_SIZE) {
		unsigned int i;
		// skip badblock
		for (;;) {
			if (NandIsGoodBlock
			    (BlockNum <<
			     (BYTE_SECTOR_SHIFT + SECTOR_BLOCK_SHIFT))) {
				break;
			}
			BlockNum++;	//try next
		}
		for (i = 0; i < BLOCK_SIZE; i += SECTOR_SIZE) {
			int ret =
			    NandReadOneSector(RAM,
					      (BlockNum <<
					       (BYTE_SECTOR_SHIFT +
						SECTOR_BLOCK_SHIFT)) + i);
			RAM += SECTOR_SIZE;
			ret = 0;

		}

		BlockNum++;
	}

	CallLinux();
}

static inline void GetParameters(void)
{
	U32 Buf[2048];
	g_os_type = OS_LINUX;
	g_os_start = 0x60000;
	g_os_length = 0x500000;

	g_os_ram_start = 0x30008000;

	// vivi LINUX CMD LINE
	NandReadOneSector((U8 *)Buf, 0x48000);
	if (Buf[0] == 0x49564956 && Buf[1] == 0x4C444D43) {
		memcpy(g_linux_cmd_line, (char *)&(Buf[2]), sizeof g_linux_cmd_line);
	}
}

void Main(void)
{
	MMU_EnableICache();
	MMU_EnableDCache();

	Port_Init();
	NandInit();

	if (g_page_type == PAGE_UNKNOWN) {
		Uart_SendString("\r\nunsupport NAND\r\n");
		for(;;);
	}

	GetParameters();

	Uart_SendString("load Image of Linux...\n\r");
	ReadImageFromNand();
}
