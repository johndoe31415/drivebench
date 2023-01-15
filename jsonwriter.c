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
#include <time.h>
#include <json-c/json.h>
#include "pgmopts.h"
#include "drivebench.h"
#include "jsonwriter.h"

static void now_utc(char datetime[32]) {
	time_t t = time(NULL);
	struct tm *tm = gmtime(&t);
	strftime(datetime, 31, "%Y-%m-%dT%H:%M:%SZ", tm);
}

static json_object *json_get_meta(const struct drivebench_t *results) {
	char timestamp[32];
	now_utc(timestamp);

	json_object *meta = json_object_new_object();
	json_object_object_add(meta, "timestamp", json_object_new_string(timestamp));
	json_object_object_add(meta, "device", json_object_new_string(pgmopts->device));

	json_object *devinfo = json_object_new_object();
	json_object_object_add(meta, "devinfo", devinfo);
	json_object_object_add(devinfo, "disksize", json_object_new_int64(results->disk_size));
	if (results->diskinfo.vendor[0]) {
		json_object_object_add(devinfo, "vendor", json_object_new_string(results->diskinfo.vendor));
	}
	if (results->diskinfo.model[0]) {
		json_object_object_add(devinfo, "model", json_object_new_string(results->diskinfo.model));
	}
	if (results->diskinfo.serial[0]) {
		json_object_object_add(devinfo, "serial", json_object_new_string(results->diskinfo.serial));
	}
	return meta;
}

static json_object *json_get_sequential_test_results(const struct drivebench_t *results) {
	json_object *test_results = json_object_new_object();
	json_object_object_add(test_results, "test_type", json_object_new_string("sequential"));
	json_object_object_add(test_results, "sample_size_mib", json_object_new_int(results->benchmark_results.sequential.sample_size_mib));

	json_object *data = json_object_new_array();
	json_object_object_add(test_results, "data", data);
	for (unsigned int sample = 0; sample < results->benchmark_results.sequential.throughput->buckets; sample++) {
		json_object *inner = json_object_new_array();
		json_object_array_add(data, inner);
		for (unsigned int run = 0; run < results->benchmark_results.sequential.throughput->runs; run++) {
			json_object_array_add(inner, json_object_new_double(throughput_get(results->benchmark_results.sequential.throughput, sample, run)));
		}
	}

	return test_results;
}

static json_object *json_get_4k_read_test_results(const struct drivebench_t *results, const struct result_4k_reads_t *results_4k) {
	json_object *test_results = json_object_new_object();
	json_object_object_add(test_results, "test_type", json_object_new_string("random_4k_reads"));
	json_object_object_add(test_results, "thread_count", json_object_new_int(results_4k->thread_count));
	json_object_object_add(test_results, "read_count_per_thread", json_object_new_int(results_4k->read_count_per_thread));
	json_object *data = json_object_new_array();
	json_object_object_add(test_results, "data", data);
	for (unsigned int x = 0; x < results_4k->seektimes->buckets; x++) {
		json_object *inner = json_object_new_array();
		json_object_array_add(data, inner);
		for (unsigned int y = 0; y < results_4k->seektimes->buckets; y++) {
			const struct seektime_value_t *value = seektime_get(results_4k->seektimes, x, y);

			json_object *data_point = json_object_new_object();
			json_object_array_add(inner, data_point);
			json_object_object_add(data_point, "cnt", json_object_new_int(value->count));
			json_object_object_add(data_point, "sum", json_object_new_double(value->sum));

		}
	}

	return test_results;
}

static json_object *json_get_test_results(const struct drivebench_t *results) {
	json_object *test_results = json_object_new_object();
	if (pgmopts->run_sequential) {
		json_object_object_add(test_results, "sequential", json_get_sequential_test_results(results));
	}
	if (pgmopts->run_single_threaded_4k) {
		json_object_object_add(test_results, "single_threaded_4k_reads", json_get_4k_read_test_results(results, &results->benchmark_results.reads_4k_single_threaded));
	}
	if (pgmopts->run_multi_threaded_4k) {
		json_object_object_add(test_results, "multi_threaded_4k_reads", json_get_4k_read_test_results(results, &results->benchmark_results.reads_4k_multi_threaded));
	}
	return test_results;
}

bool write_json_results(const char *filename, const struct drivebench_t *results) {
	json_object *root = json_object_new_object();
	if (!root) {
		return false;
	}

	json_object_object_add(root, "metadata", json_get_meta(results));
	json_object_object_add(root, "test_results", json_get_test_results(results));

	json_object_to_file(filename, root);
	json_object_put(root);


   return true;
}
