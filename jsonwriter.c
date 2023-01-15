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
#include <json-c/json.h>
#include "jsonwriter.h"

static json_object *json_get_meta(void) {
	json_object *meta = json_object_new_object();
	json_object_object_add(meta, "timestamp", json_object_new_string("offline"));
	json_object_object_add(meta, "hostname", json_object_new_string("offline"));
	json_object_object_add(meta, "device", json_object_new_string("offline"));
	json_object_object_add(meta, "model", json_object_new_string("offline"));
	json_object_object_add(meta, "serial", json_object_new_string("offline"));
	return meta;
}

bool write_json_results(const char *filename) {
	json_object *root = json_object_new_object();
	if (!root) {
		return false;
	}

	json_object_object_add(root, "id", json_get_meta());


	json_object_to_file(filename, root);
	json_object_put(root);


   return true;
}
