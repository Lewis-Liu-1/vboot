

#undef LINUX_CMD_LINE
#define LINUX_CMD_LINE "noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0"

//#undef LINUX_CMD_LINE
//#define LINUX_CMD_LINE "console=ttySAC0 root=/dev/nfs nfsroot=192.168.1.99:/opt/FriendlyARM/mini2440/new_root_nfs ip=192.168.1.30:192.168.1.99:192.168.1.1:255.255.255.0:sbc2440.arm9.net:eth0:off"

#define OS_LINUX 0x02
#define OS_WINCE 0x04

#if 1
#define OS_TYPE		OS_LINUX
#define HAS_NAND_BIOS	0xFF
#define LOGO_POS	0xFFFF
#define OS_START	0x60000
#define OS_LENGTH	0x500000
#define OS_RAM_START	0x30008000

#else
#define OS_TYPE		OS_WINCE
#define HAS_NAND_BIOS	0xFF
#define LOGO_POS	0xFFFF
#define OS_START	(0xb00 * 512)
#define OS_LENGTH	(0x1E00000)
#define OS_RAM_START	0x30200000
#endif
#define ZBOOT_MAGIC	0x19710829
#if !defined(__ASSEMBLY__)
extern  struct zboot_first_sector {
	unsigned char  dont_care[0x20];
	unsigned int   magic;
	unsigned char  os_type;
	unsigned char  has_nand_bios;
	unsigned short logo_pos;
	unsigned int   os_start;
	unsigned int   os_length;
	unsigned int   os_ram_start;
	unsigned char  linux_cmd[512 - 0x34];
}  __attribute__((packed)) first_sector;

#define g_magic			(first_sector.magic)
#define g_os_type		(first_sector.os_type)
#define g_has_nand_bios		(first_sector.has_nand_bios)
#define g_logo_pos		(first_sector.logo_pos)
#define g_os_start		(first_sector.os_start)
#define g_os_length		(first_sector.os_length)
#define g_os_ram_start		(first_sector.os_ram_start)
#define g_linux_cmd_line	(first_sector.linux_cmd)

#endif
