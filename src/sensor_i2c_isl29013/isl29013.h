/*
 * ISL29013 ambient light sensor driver
 *
 * Copyright (C) 2013 iVeia, LLC
 * Author: Brian Silverman <bsilverman@iveia.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ISL29013_H__
#define __ISL29013_H__

#include <linux/types.h>

struct isl29013_platform_data {

	int poll_interval;

	int (*init)(void);
	void (*exit)(void);
	int (*power_on)(void);
	int (*power_off)(void);
};

#endif 