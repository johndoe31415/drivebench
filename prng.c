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
#include <stdint.h>
#include <string.h>
#include "md5.h"
#include "prng.h"

static void prng_advance(struct prng_state_t *state) {
  state->state ^= state->state << 13;
  state->state ^= state->state >> 7;
  state->state ^= state->state << 17;
}

uint8_t prng_uint8(struct prng_state_t *state) {
	prng_advance(state);
	return state->state & 0xff;
}

uint64_t prng_uint(struct prng_state_t *state, uint64_t maxval) {
	prng_advance(state);
	return state->state % maxval;
}

void prng_init(struct prng_state_t *state, const char *seed, uint32_t diversification) {
	unsigned int len = strlen(seed);

	struct md5ctx_t ctx;
	md5_init(&ctx);
	uint8_t hash_buffer[4 + len];
	*((uint32_t*)hash_buffer) = diversification;
	memcpy(hash_buffer + 4, seed, len);
	md5_update(&ctx, hash_buffer, sizeof(hash_buffer));

	md5_finalize(&ctx);

	memcpy(&state->state, ctx.digest, sizeof(uint64_t));
}
