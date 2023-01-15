/*
	drivebench - Block device performance measurement tool
	Copyright (C) 2022-2022 Johannes Bauer

	This file is part of drivebench.

	drivebench is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; this program is ONLY licensed under
	version 3 of the License, later versions are explicitly excluded.

	drivebench is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with drivebench; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	Johannes Bauer <JohannesBauer@gmx.de>
*/

#ifndef __DISKINFO_H__
#define __DISKINFO_H__

#define MAX_VENDOR_SIZE		128
#define MAX_MODEL_SIZE		128
#define MAX_SERIAL_SIZE		64

struct diskinfo_t {
	char vendor[MAX_VENDOR_SIZE];
	char model[MAX_SERIAL_SIZE];
	char serial[MAX_SERIAL_SIZE];
};

/*************** AUTO GENERATED SECTION FOLLOWS ***************/
void get_diskinfo(const char *devname, struct diskinfo_t *diskinfo);
/***************  AUTO GENERATED SECTION ENDS   ***************/

#endif
