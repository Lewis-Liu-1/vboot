#ifndef __NAND_H
#define __NAND_H

void NandInit(void);
int NandIsGoodBlock(U32 addr);
int NandReadOneSector(U8 * buffer, U32 addr);

void Uart_SendString(char *);

#define PAGE_UNKNOWN 0
#define PAGE512      1
#define PAGE2048     2

#define BYTE_SECTOR_SHIFT   (g_page_type == PAGE512 ? 9 : 11)
#define SECTOR_BLOCK_SHIFT  (g_page_type == PAGE512 ? 5 : 6)

#define SECTOR_SIZE (1 << BYTE_SECTOR_SHIFT)
#define BLOCK_SIZE  (SECTOR_SIZE << SECTOR_BLOCK_SHIFT)

extern int g_page_type;

#endif /*__NAND_H*/
