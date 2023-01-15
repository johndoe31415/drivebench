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
#include "seektime.h"

#define INDEXOF(seektime_ptr, x, y)		((((seektime_ptr)->buckets) * (y)) + (x))

struct seektime_t *seektime_init(unsigned int buckets, uint64_t maxval) {
	assert(buckets != 0);
	assert(maxval != 0);
	struct seektime_t *result = calloc(sizeof(*result), 1);
	if (!result) {
		return NULL;
	}

	result->buckets = buckets;
	result->max_value = maxval;
	result->values = calloc(sizeof(*result->values) * buckets * buckets, 1);
	if (!result->values) {
		seektime_free(result);
		return NULL;
	}
	return result;
}

void seektime_merge(struct seektime_t *dest, const struct seektime_t *src) {
	assert(dest->buckets == src->buckets);
	for (unsigned int i = 0; i < (src->buckets * src->buckets); i++) {
		dest->values[i].count += src->values[i].count;
		dest->values[i].sum += src->values[i].sum;
	}
}

void seektime_add(struct seektime_t *seektime, uint64_t x, uint64_t y, double tm) {
	unsigned int xbucket = (uint64_t)seektime->buckets * x / seektime->max_value;
	unsigned int ybucket = (uint64_t)seektime->buckets * y / seektime->max_value;
	if (xbucket >= seektime->buckets) {
		xbucket = seektime->buckets - 1;
	}
	if (ybucket >= seektime->buckets) {
		ybucket = seektime->buckets - 1;
	}
	unsigned int index = INDEXOF(seektime, xbucket, ybucket);
	seektime->values[index].count += 1;
	seektime->values[index].sum += tm;
}

void seektime_dump(const struct seektime_t *seektime) {
	for (unsigned int y = 0; y < seektime->buckets; y++) {
		for (unsigned int x = 0; x < seektime->buckets; x++) {
			unsigned int index = INDEXOF(seektime, x, y);
			if (seektime->values[index].count) {
				printf("%.2f ", 1000 * seektime->values[index].sum / seektime->values[index].count);
			} else {
				printf("-    ");
			}
		}
		printf("\n");
	}
}

const struct seektime_value_t *seektime_get(const struct seektime_t *seektime, unsigned int x, unsigned int y) {
	unsigned int index = INDEXOF(seektime, x, y);
	return &seektime->values[index];
}

void seektime_free(struct seektime_t *seektime) {
	if (!seektime) {
		return;
	}
	if (seektime->values) {
		free(seektime->values);
	}
	free(seektime);
}
