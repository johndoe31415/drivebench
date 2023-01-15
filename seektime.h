#ifndef __SEEKTIME_H__
#define __SEEKTIME_H__

#include <stdint.h>

struct seektime_value_t {
	unsigned int count;
	double sum;
};

struct seektime_t {
	unsigned int buckets;
	uint64_t max_value;
	struct seektime_value_t *values;
};

/*************** AUTO GENERATED SECTION FOLLOWS ***************/
struct seektime_t *seektime_init(unsigned int buckets, uint64_t maxval);
void seektime_merge(struct seektime_t *dest, const struct seektime_t *src);
void seektime_add(struct seektime_t *seektime, uint64_t x, uint64_t y, double tm);
void seektime_dump(const struct seektime_t *seektime);
const struct seektime_value_t *seektime_get(const struct seektime_t *seektime, unsigned int x, unsigned int y);
void seektime_free(struct seektime_t *seektime);
/***************  AUTO GENERATED SECTION ENDS   ***************/

#endif
