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

#ifndef __DRIVEBENCH_H__
#define __DRIVEBENCH_H__

#include <stdint.h>
#include "throughput.h"
#include "seektime.h"
#include "diskinfo.h"
#include "semaphore.h"

#define MAX_NUMBER_THREADS		64
#define CHUNK_SIZE_BYTES		(1024 * 1024)
#define READ_ALIGNMENT_BYTES	4096

struct result_sequential_t {
	unsigned int sample_size_mib;
	struct throughput_t *throughput;
};

struct result_4k_reads_t {
	unsigned int thread_count;
	unsigned int read_count_per_thread;
	struct seektime_t *seektimes;
};

struct benchmark_results_t {
	struct result_sequential_t sequential;
	struct result_4k_reads_t reads_4k_single_threaded;
	struct result_4k_reads_t reads_4k_multi_threaded;
};

struct drivebench_t {
	int fds[MAX_NUMBER_THREADS];
	uint64_t disk_size;
	struct semaphore_t threads_finished;
	const char *prng_seed;
	struct diskinfo_t diskinfo;
	struct benchmark_results_t benchmark_results;
};

/*************** AUTO GENERATED SECTION FOLLOWS ***************/
/***************  AUTO GENERATED SECTION ENDS   ***************/

#endif
