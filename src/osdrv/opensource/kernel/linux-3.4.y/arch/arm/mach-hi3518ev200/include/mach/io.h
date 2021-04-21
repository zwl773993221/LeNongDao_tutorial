#ifndef __ASM_ARM_ARCH_IO_H
#define __ASM_ARM_ARCH_IO_H

#define IO_SPACE_LIMIT 0xffffffff

#define __io(a)		__typesafe_io(a)

/*
 * phy: 0x20000000 ~ 0x20700000
 * vir: 0xFE100000 ~ 0xFE800000
 */
#define HI3518EV200_IOCH2_PHYS	0x20000000
#define IO_OFFSET_HIGH		0xDE100000
#define HI3518EV200_IOCH2_VIRT (HI3518EV200_IOCH2_PHYS + IO_OFFSET_HIGH)
#define HI3518EV200_IOCH2_SIZE 0x700000

/* phy: 0x10000000 ~ 0x100E0000
 * vir: 0xFE000000 ~ 0xFE0E0000
 */
#define HI3518EV200_IOCH1_PHYS	0x10000000
#define IO_OFFSET_LOW		0xEE000000
#define HI3518EV200_IOCH1_VIRT (HI3518EV200_IOCH1_PHYS + IO_OFFSET_LOW)
#define HI3518EV200_IOCH1_SIZE 0xE0000

#define IO_ADDRESS(x) ((x) >= HI3518EV200_IOCH2_PHYS ? (x) + IO_OFFSET_HIGH \
						: (x) + IO_OFFSET_LOW)

#define __io_address(n)		((void __iomem __force *)IO_ADDRESS(n))

#endif
