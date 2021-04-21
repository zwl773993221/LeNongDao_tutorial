/******************************************************************************
 *    Copyright (c) 2009-2013 by Hisilicon.
 *    All rights reserved.
 * ***
 *    Create by LiuHui, 2013-08-24
 *
******************************************************************************/
#ifndef ETHENVH
#define ETHENVH

#include <asm/types.h>

int getenv_phyaddr(int index, int defval);

enum eth_phyintf_t {
	ETH_PHY_MII   = 0,
	ETH_PHY_RMII  = 1,
	ETH_PHY_RGMII = 2,
	ETH_PHY_UNKNOWN = 3,
};
const char *ethphy_intfname(enum eth_phyintf_t phyintf);

enum eth_phyintf_t ethphy_intf(const char *intfname);

enum eth_phyintf_t getenv_phyintf(int index, enum eth_phyintf_t defval);

int getenv_phymdio(int index, int defval);

int getenv_phygpio(int index, u32 *gpio_base, u32 *gpio_bit);

#endif /* ETHENVH */
