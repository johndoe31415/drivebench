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
#include <assert.h>
#include "throughput.h"

struct throughput_t *throughput_init(unsigned int buckets, unsigned int runs) {
	struct throughput_t *result = calloc(sizeof(*result) + (sizeof(*result->values) * buckets * runs), 1);
	if (!result) {
		return NULL;
	}
	result->buckets = buckets;
	result->runs = runs;
	return result;
}

void throughput_set(struct throughput_t *throughput, unsigned int bucket, unsigned int run, double value) {
	unsigned int index = (run * throughput->buckets) + bucket;
	throughput->values[index] = value;
}

double throughput_get(const struct throughput_t *throughput, unsigned int bucket, unsigned int run) {
	unsigned int index = (run * throughput->buckets) + bucket;
	return throughput->values[index];
}

void throughput_dump(const struct throughput_t *throughput) {
	for (unsigned int bucket = 0; bucket < throughput->buckets; bucket++) {
		printf("%d: ", bucket);
		for (unsigned int run = 0; run < throughput->runs; run++) {
			printf("%.1f ", throughput_get(throughput, bucket, run) / 1024. / 1024.);
		}
		printf("\n");
	}
}

void throughput_free(struct throughput_t *throughput) {
	if (!throughput) {
		return;
	}
	free(throughput);
}
