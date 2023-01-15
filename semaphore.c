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

#include <pthread.h>
#include <string.h>

#include "semaphore.h"

void semaphore_init(struct semaphore_t *data, unsigned int init_value) {
	memset(data, 0, sizeof(struct semaphore_t));
	data->value = init_value;
	pthread_mutex_init(&data->mutex, NULL);
	pthread_cond_init(&data->cond, NULL);
}

/* Decrement semaphore, block if necessary */
void semaphore_P(struct semaphore_t *data, unsigned int value) {
	pthread_mutex_lock(&data->mutex);
	while (data->value < value) {
		pthread_cond_wait(&data->cond, &data->mutex);
	}
	data->value -= value;
	pthread_mutex_unlock(&data->mutex);
}

/* Increment semaphore */
void semaphore_V(struct semaphore_t *data, unsigned int value) {
	pthread_mutex_lock(&data->mutex);
	data->value += value;
	pthread_cond_broadcast(&data->cond);
	pthread_mutex_unlock(&data->mutex);
}

void semaphore_free(struct semaphore_t *data) {
	pthread_mutex_destroy(&data->mutex);
	pthread_cond_destroy(&data->cond);
}

