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
#include <stdlib.h>
#include "argparse.h"
#include "pgmopts.h"

static struct pgmopts_t pgmopts_rw = {
	.sequential_chunk_size = ARGPARSE_DEFAULT_SEQUENTIAL_CHUNK_SIZE,
	.sequential_samples = ARGPARSE_DEFAULT_SEQUENTIAL_SAMPLES,
	.thread_count = ARGPARSE_DEFAULT_THREAD_COUNT,
	.read_counts_total = ARGPARSE_DEFAULT_READ_COUNTS_TOTAL,
	.read_4k_buckets = ARGPARSE_DEFAULT_READ_4K_BUCKETS,
	.run_sequential = true,
	.run_single_threaded_4k = true,
	.run_multi_threaded_4k = true,
	.verbose = ARGPARSE_DEFAULT_VERBOSE,
};
const struct pgmopts_t *pgmopts = &pgmopts_rw;

static bool argument_callback(enum argparse_option_t option, const char *value, argparse_errmsg_callback_t errmsg_callback) {
	switch (option) {
		case ARG_DEVICE:
			pgmopts_rw.device = value;
			break;

		case ARG_SEQUENTIAL_CHUNK_SIZE:
			pgmopts_rw.sequential_chunk_size = atoi(value);
			break;

		case ARG_SEQUENTIAL_SAMPLES:
			pgmopts_rw.sequential_samples = atoi(value);
			break;

		case ARG_THREAD_COUNT:
			pgmopts_rw.thread_count = atoi(value);
			break;

		case ARG_READ_COUNTS_TOTAL:
			pgmopts_rw.read_counts_total = atoi(value);
			break;

		case ARG_READ_4K_BUCKETS:
			pgmopts_rw.read_4k_buckets = atoi(value);
			break;

		case ARG_SEED:
			pgmopts_rw.seed = value;
			break;

		case ARG_NO_SEQUENTIAL:
			pgmopts_rw.run_sequential = false;
			break;

		case ARG_NO_SINGLE_THREADED_4K:
			pgmopts_rw.run_single_threaded_4k = false;
			break;

		case ARG_NO_MULTI_THREADED_4K:
			pgmopts_rw.run_multi_threaded_4k = false;
			break;

		case ARG_JSON_OUTPUT:
			pgmopts_rw.json_output_filename = value;
			break;

		case ARG_VERBOSE:
			pgmopts_rw.verbose++;
			break;
	}
	return true;
}

void pgmopts_parse(int argc, char **argv) {
	argparse_parse_or_quit(argc, argv, argument_callback, NULL);

}
