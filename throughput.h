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

#ifndef __THROUGHPUT_H__
#define __THROUGHPUT_H__

struct throughput_t {
	unsigned int buckets;
	unsigned int runs;
	double values[];
};

/*************** AUTO GENERATED SECTION FOLLOWS ***************/
struct throughput_t *throughput_init(unsigned int buckets, unsigned int runs);
void throughput_set(struct throughput_t *throughput, unsigned int bucket, unsigned int run, double value);
double throughput_get(const struct throughput_t *throughput, unsigned int bucket, unsigned int run);
void throughput_dump(const struct throughput_t *throughput);
void throughput_free(struct throughput_t *throughput);
/***************  AUTO GENERATED SECTION ENDS   ***************/

#endif
