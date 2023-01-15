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

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <linux/hdreg.h>
#include "diskinfo.h"

#define MINOF(x, y)		(((x) < (y)) ? (x) : (y))

static void rstrip(char *string) {
	int len = strlen(string);
	while (len && ((string[len - 1] == '\n') || (string[len - 1] == '\r') || (string[len - 1] == '\t') || (string[len - 1] == ' '))) {
		string[--len] = 0;
	}
}

static void read_sysfs_node(const char *filename, char *data, unsigned int max_size) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		return;
	}
	fread(data, 1, max_size - 1, f);
	fclose(f);

	data[max_size - 1] = 0;
	rstrip(data);
}

void get_diskinfo(const char *devname, struct diskinfo_t *diskinfo) {
	memset(diskinfo, 0, sizeof(*diskinfo));

	struct statx statbuf;
	if (statx(0, devname, 0, STATX_ALL, &statbuf) == 0) {
		if ((statbuf.stx_mode & S_IFMT) == S_IFBLK) {
			/* Block device, extract major/minor */
			unsigned int devmajor = statbuf.stx_rdev_major;
			unsigned int devminor = statbuf.stx_rdev_minor;

			char sysfs_filename[256];
			snprintf(sysfs_filename, sizeof(sysfs_filename), "/sys/dev/block/%u:%u/device/vendor", devmajor, devminor);
			read_sysfs_node(sysfs_filename, diskinfo->vendor, sizeof(diskinfo->vendor));

			snprintf(sysfs_filename, sizeof(sysfs_filename), "/sys/dev/block/%u:%u/device/model", devmajor, devminor);
			read_sysfs_node(sysfs_filename, diskinfo->model, sizeof(diskinfo->model));

			snprintf(sysfs_filename, sizeof(sysfs_filename), "/sys/dev/block/%u:%u/device/serial", devmajor, devminor);
			read_sysfs_node(sysfs_filename, diskinfo->serial, sizeof(diskinfo->serial));
		}
	}

	if (strlen(diskinfo->serial) == 0) {
		/* Attempt via ATA command */
		int fd = open(devname, O_RDONLY);
		if (fd == -1) {
			fprintf(stderr, "failed to open(2) %s: %s\n", devname, strerror(errno));
			exit(EXIT_FAILURE);
		}

		uint8_t identity[512];
		if (ioctl(fd, HDIO_GET_IDENTITY, identity) == 0) {
			struct hd_driveid *id = (struct hd_driveid*)identity;
			strncpy(diskinfo->serial, (char*)id->serial_no, MINOF(MAX_SERIAL_SIZE, 20));
			rstrip(diskinfo->serial);
		}
		close(fd);
	}

}
