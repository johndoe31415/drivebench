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

#ifndef __PGMOPTS_H__
#define __PGMOPTS_H__

#include <stdint.h>
#include <stdbool.h>

struct pgmopts_t {
	const char *device;
	const char *seed;
	unsigned int sequential_chunk_size;
	unsigned int sequential_samples;
	unsigned int sequential_iterations;
	unsigned int thread_count;
	unsigned int read_counts_total;
	unsigned int read_4k_buckets;
	bool run_sequential;
	bool run_single_threaded_4k;
	bool run_multi_threaded_4k;
	const char *json_output_filename;
	unsigned int verbose;
};

extern const struct pgmopts_t *pgmopts;

/*************** AUTO GENERATED SECTION FOLLOWS ***************/
void pgmopts_parse(int argc, char **argv);
/***************  AUTO GENERATED SECTION ENDS   ***************/

#endif
