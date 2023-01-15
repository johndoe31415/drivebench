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
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/random.h>
#include <assert.h>
#include <pthread.h>
#include "pgmopts.h"
#include "prng.h"
#include "semaphore.h"
#include "drivebench.h"
#include "seektime.h"
#include "throughput.h"
#include "jsonwriter.h"

#define PRNG_CONSTANT_SEQUENTIAL		0x0
#define PRNG_CONSTANT_4K_READS			0x100000

struct result_sequential_t {
	struct throughput_t *throughput;
};

struct result_4k_reads_t {
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
	struct benchmark_results_t benchmark_results;
};

struct thread_ctx_t {
	struct drivebench_t *bench;
	int fd;
	struct prng_state_t prng;
	unsigned int read_counts;
	struct seektime_t *seektimes;
};

static double get_time(void) {
	struct timeval tm;
	gettimeofday(&tm, NULL);
	return tm.tv_sec + (tm.tv_usec * 1e-6);
}

/* Alignment must be a power of two */
static void *pointer_align(void *memory, const uint64_t alignment) {
	void *aligned_memory = (void*)(((uint64_t)memory + alignment) & ~(alignment - 1));
	return aligned_memory;
}

static void benchmark_sequential(struct drivebench_t *bench) {
	const uint64_t alignment = READ_ALIGNMENT_BYTES;	/* Must be power of two */
	void *memory = calloc(1, CHUNK_SIZE_BYTES + alignment);
	if (!memory) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	bench->benchmark_results.sequential.throughput = throughput_init(pgmopts->sequential_samples, pgmopts->sequential_iterations);
	if (!bench->benchmark_results.sequential.throughput) {
		perror("throughput_init");
		free(memory);
		exit(EXIT_FAILURE);
	}

	void *aligned_memory = pointer_align(memory, alignment);
	if (pgmopts->verbose >= 2) {
		fprintf(stderr, "Alocated %d bytes of memory at %p, aligned to %" PRIu64 " bytes -> %p\n", CHUNK_SIZE_BYTES, memory, alignment, aligned_memory);
	}

	struct prng_state_t prng;
	prng_init(&prng, bench->prng_seed, PRNG_CONSTANT_SEQUENTIAL);

	uint64_t step = (bench->disk_size - pgmopts->sequential_chunk_size * CHUNK_SIZE_BYTES) / pgmopts->sequential_samples;
	for (unsigned int iteration = 0; iteration < pgmopts->sequential_iterations; iteration++) {
		if (pgmopts->verbose >= 1) {
			printf("Started sequential read iteration %u of %u, %u MiB blocks at %u samples\n", iteration + 1, pgmopts->sequential_iterations, pgmopts->sequential_chunk_size, pgmopts->sequential_samples);
		}
		double read_time_sum = 0;
		for (unsigned int i = 0; i < pgmopts->sequential_samples; i++) {
			uint64_t offset = step * i;
			/* randomize access */
			offset += prng_uint(&prng, step);
			offset = offset / alignment * alignment;
			if (lseek(bench->fds[0], offset, SEEK_SET) == (off_t)-1) {
				perror("lseek");
				exit(EXIT_FAILURE);
			}

			const double t0 = get_time();
			for (unsigned int j = 0; j < pgmopts->sequential_chunk_size; j++) {
				ssize_t read_result = read(bench->fds[0], aligned_memory, CHUNK_SIZE_BYTES);
				if (read_result != CHUNK_SIZE_BYTES) {
					if (errno == 0) {
						fprintf(stderr, "read fd %d: read %ld bytes, but expected %d at offset %" PRIu64 " (disk size %" PRIu64 ")\n", bench->fds[0], read_result, CHUNK_SIZE_BYTES, offset + (j * CHUNK_SIZE_BYTES), bench->disk_size);
					} else {
						fprintf(stderr, "read fd %d: %s. read %ld bytes, but expected %d at offset %" PRIu64 " (disk size %" PRIu64 ")\n", bench->fds[0], strerror(errno), read_result, CHUNK_SIZE_BYTES, offset + (j * CHUNK_SIZE_BYTES), bench->disk_size);
					}
					exit(EXIT_FAILURE);
				}
			}
			const double t1 = get_time();
			const double tdiff = t1 - t0;
			read_time_sum += tdiff;
			const double bytes_per_sec = pgmopts->sequential_chunk_size * CHUNK_SIZE_BYTES / tdiff;
			throughput_set(bench->benchmark_results.sequential.throughput, i, iteration, bytes_per_sec);
			if (pgmopts->verbose >= 2) {
				printf("Sequential read of %.1f MiB at %.1f%% / %.1f GiB (%#" PRIx64 "): %.1f MiB/sec\n", CHUNK_SIZE_BYTES * pgmopts->sequential_chunk_size / 1024. / 1024., (double)i / (pgmopts->sequential_samples - 1) * 100.0, offset / 1024. / 1024. / 1024., offset, bytes_per_sec / 1024. / 1024.);
			}
		}
		const double total_bytes_per_sec = (uint64_t)pgmopts->sequential_samples * pgmopts->sequential_chunk_size * CHUNK_SIZE_BYTES / read_time_sum;
		printf("Sequential read iteration %u of %u: read %.1f MiB in %.1f sec: %.1f MiB/sec average\n", iteration + 1, pgmopts->sequential_iterations, pgmopts->sequential_samples * CHUNK_SIZE_BYTES * pgmopts->sequential_chunk_size / 1024. / 1024., read_time_sum, total_bytes_per_sec / 1024. / 1024.);
	}
	if (pgmopts->verbose >= 4) {
		throughput_dump(bench->benchmark_results.sequential.throughput);
	}

	free(memory);
}

static void* benchmark_4k_read_thread(void *vctx) {
	struct thread_ctx_t *ctx = (struct thread_ctx_t *)vctx;
	uint8_t buffer[4096 + READ_ALIGNMENT_BYTES];
	void *aligned_buffer = pointer_align(buffer, READ_ALIGNMENT_BYTES);

	uint64_t max_value = (ctx->bench->disk_size - 4096) / READ_ALIGNMENT_BYTES;
	uint64_t last_offset = 0;
	for (unsigned int i = 0; i < ctx->read_counts; i++) {
		uint64_t offset = prng_uint(&ctx->prng, max_value) * READ_ALIGNMENT_BYTES;
		double t0 = get_time();
		if (lseek(ctx->fd, offset, SEEK_SET) == (off_t)-1) {
			perror("lseek");
			exit(EXIT_FAILURE);
		}
		if (read(ctx->fd, aligned_buffer, 4096) != 4096) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		double t1 = get_time();
		seektime_add(ctx->seektimes, last_offset, offset, t1 - t0);
		last_offset = offset;
	}

	semaphore_V(&ctx->bench->threads_finished, 1);
	return NULL;
}

static void benchmark_4k_reads(struct drivebench_t *bench, unsigned int thread_count) {
	const unsigned int read_count_per_thread = pgmopts->read_counts_total / thread_count;
	assert(thread_count <= MAX_NUMBER_THREADS);
	semaphore_init(&bench->threads_finished, 0);

	const uint64_t seeks_total = thread_count * read_count_per_thread;
	if (pgmopts->verbose >= 1) {
		printf("Started 4k block read test with random access pattern using %u concurrent threads. %u reads per thread, %" PRIu64 " seeks total.\n", thread_count, read_count_per_thread, seeks_total);
	}

	/* Initialize data first, don't count that into timing */
	struct thread_ctx_t threads[thread_count];
	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
	for (unsigned int i = 0; i < thread_count; i++) {
		threads[i].read_counts = read_count_per_thread;
		threads[i].bench = bench;
		threads[i].fd = bench->fds[i];
		prng_init(&threads[i].prng, bench->prng_seed, PRNG_CONSTANT_4K_READS | i);
		threads[i].seektimes = seektime_init(pgmopts->read_4k_buckets, bench->disk_size);
		if (!threads[i].seektimes) {
			fprintf(stderr, "fatal: failed to allocate thread seektimes: %s", strerror(errno));
			return;
		}
	}

	double t0 = get_time();
	/* Then start all threads */
	for (unsigned int i = 0; i < thread_count; i++) {
		pthread_t thread_id;
		pthread_create(&thread_id, &attrs, benchmark_4k_read_thread, threads + i);
	}

	/* Wait until all threads finished */
	semaphore_P(&bench->threads_finished, thread_count);
	semaphore_free(&bench->threads_finished);

	pthread_attr_destroy(&attrs);

	/* Merge results and free thread-specific data */
	struct seektime_t *merged_result = seektime_init(pgmopts->read_4k_buckets, bench->disk_size);
	if (!merged_result) {
		fprintf(stderr, "fatal: failed to allocate result seektimes: %s", strerror(errno));
		return;
	}
	if (thread_count == 1) {
		bench->benchmark_results.reads_4k_single_threaded.seektimes = merged_result;
	} else {
		bench->benchmark_results.reads_4k_multi_threaded.seektimes = merged_result;
	}
	for (unsigned int i = 0; i < thread_count; i++) {
		seektime_merge(merged_result, threads[i].seektimes);
		seektime_free(threads[i].seektimes);
	}

	if (pgmopts->verbose >= 4) {
		seektime_dump(merged_result);
	}

	double t1 = get_time();
	double tdiff = t1 - t0;
	uint64_t bytes_read = seeks_total * 4096;
	printf("4k reads using %u threads completed in %.2f secs; total read %.0f MiB at %.1f MiB/sec. Average seek time %.2f ms\n", thread_count, tdiff, bytes_read / 1024. / 1024., bytes_read / 1024. / 1024. / tdiff, tdiff * 1000 / seeks_total);
}

int main(int argc, char **argv) {
	pgmopts_parse(argc, argv);

	struct drivebench_t bench = { 0 };
	char random_seed[32];
	if (pgmopts->seed) {
		bench.prng_seed = pgmopts->seed;
	} else {
		uint8_t rndval[8];
		if (getrandom(rndval, sizeof(rndval), 0) != sizeof(rndval)) {
			perror("getrandom");
			exit(EXIT_FAILURE);
		}
		random_seed[0] = 0;
		char *s = random_seed;
		for (unsigned int i = 0; i < sizeof(rndval); i++) {
			s += snprintf(s, sizeof(random_seed) - (s - random_seed), "%02x", rndval[i]);
		}
		bench.prng_seed = random_seed;
	}

	for (unsigned int i = 0; i < MAX_NUMBER_THREADS; i++) {
		bench.fds[i] = open(pgmopts->device, O_RDONLY | O_DIRECT);
		if (bench.fds[i] == -1) {
			fprintf(stderr, "open(%s) index %d: %s\n", pgmopts->device, i, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	bench.disk_size = lseek(bench.fds[0], 0, SEEK_END);
	if ((off_t)bench.disk_size == (off_t)-1) {
		perror("lseek");
		exit(EXIT_FAILURE);
	}

	if (pgmopts->verbose >= 2) {
		printf("Disk size of %s: %" PRIu64 " bytes\n", pgmopts->device, bench.disk_size);
	}

	if (pgmopts->run_sequential) {
		benchmark_sequential(&bench);
	}
	if (pgmopts->run_single_threaded_4k) {
		benchmark_4k_reads(&bench, 1);
	}
	if (pgmopts->run_multi_threaded_4k) {
		benchmark_4k_reads(&bench, pgmopts->thread_count);
	}

	for (unsigned int i = 0; i < MAX_NUMBER_THREADS; i++) {
		close(bench.fds[i]);
	}
	throughput_free(bench.benchmark_results.sequential.throughput);
	seektime_free(bench.benchmark_results.reads_4k_single_threaded.seektimes);
	seektime_free(bench.benchmark_results.reads_4k_multi_threaded.seektimes);
	return 0;
}
