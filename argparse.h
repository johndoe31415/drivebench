/*
 *   This file was AUTO-GENERATED by pypgmopts.
 *
 *   https://github.com/johndoe31415/pypgmopts
 *
 *   Do not edit it by hand, your changes will be overwritten.
 *
 *   Generated at: 2023-01-15 17:13:08
 */

#ifndef __ARGPARSE_H__
#define __ARGPARSE_H__

#include <stdbool.h>

#define ARGPARSE_DEFAULT_SEQUENTIAL_CHUNK_SIZE		128
#define ARGPARSE_DEFAULT_SEQUENTIAL_SAMPLES		100
#define ARGPARSE_DEFAULT_SEQUENTIAL_ITERATIONS		3
#define ARGPARSE_DEFAULT_THREAD_COUNT		64
#define ARGPARSE_DEFAULT_READ_COUNTS_TOTAL		1280000
#define ARGPARSE_DEFAULT_READ_4K_BUCKETS		64
#define ARGPARSE_DEFAULT_VERBOSE		0

#define ARGPARSE_NO_OPTION		0
#define ARGPARSE_POSITIONAL_ARG	1

enum argparse_option_t {
	ARG_SEQUENTIAL_CHUNK_SIZE = 2,
	ARG_SEQUENTIAL_SAMPLES = 3,
	ARG_SEQUENTIAL_ITERATIONS = 4,
	ARG_THREAD_COUNT = 5,
	ARG_READ_COUNTS_TOTAL = 6,
	ARG_READ_4K_BUCKETS = 7,
	ARG_SEED = 8,
	ARG_NO_SEQUENTIAL = 9,
	ARG_NO_SINGLE_THREADED_4K = 10,
	ARG_NO_MULTI_THREADED_4K = 11,
	ARG_JSON_OUTPUT = 12,
	ARG_VERBOSE = 13,
	ARG_DEVICE = 14,
};

typedef void (*argparse_errmsg_callback_t)(const char *errmsg, ...);
typedef void (*argparse_errmsg_option_callback_t)(enum argparse_option_t error_option, const char *errmsg, ...);
typedef bool (*argparse_callback_t)(enum argparse_option_t option, const char *value, argparse_errmsg_callback_t errmsg_callback);
typedef bool (*argparse_plausibilization_callback_t)(argparse_errmsg_option_callback_t errmsg_callback);

bool argparse_parse(int argc, char **argv, argparse_callback_t argument_callback, argparse_plausibilization_callback_t plausibilization_callback);
void argparse_show_syntax(void);
void argparse_parse_or_quit(int argc, char **argv, argparse_callback_t argument_callback, argparse_plausibilization_callback_t plausibilization_callback);

#endif
