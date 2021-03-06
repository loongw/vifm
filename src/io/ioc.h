/* vifm
 * Copyright (C) 2013 xaizek.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef VIFM__IO__IOC_H__
#define VIFM__IO__IOC_H__

#include <sys/types.h> /* gid_t mode_t uid_t */

#include "ioe.h"
#include "ioeta.h"

/* ioc - I/O common - Input/Output common */

/* Conflict resolution strategy.  Defines what to do if destination path already
 * exists. */
typedef enum
{
	/* Abort operation if destination already exists. */
	IO_CRS_FAIL,

	/* Replace all existing items and remove excess ones. */
	IO_CRS_REPLACE_ALL,

	/* Overwrite files existing at destination. */
	IO_CRS_REPLACE_FILES,

	/* Appends the reset of data to files at destination (assumes previously
	 * terminated operation). */
	IO_CRS_APPEND_TO_FILES,
}
IoCrs;

/* Forward declaration for io_confirm. */
typedef struct io_args_t io_args_t;

/* Type for file overwrite confirmation requests.  Should return non-zero on
 * positive response and zero otherwise. */
typedef int (*io_confirm)(io_args_t *args, const char src[], const char dst[]);

typedef struct
{
	ioerr_cb errors_cb;  /* TODO: use this. */
	ioe_errlst_t errors;
}
io_result_t;

struct io_args_t
{
	union
	{
		const char *path;
		const char *src;
	}
	arg1;

	union
	{
		const char *dst;
		const char *target;

		/* Whether operation should consider parent elements of path. */
		int process_parents;
	}
	arg2;

	union
	{
		/* Conflict resolution strategy. */
		IoCrs crs;

		mode_t mode;
#ifndef _WIN32
		uid_t uid;
		gid_t gid;
#endif
	}
	arg3;

	/* Whether this operation should expect cancellation requests from the
	 * outside. */
	int cancellable;

	/* File overwrite confirmation callback.  Set to NULL to silently
	 * overwrite. */
	io_confirm confirm;

	/* Set to NULL to do not use estimates. */
	ioeta_estim_t *estim;

	/* Output of the operation after it finishes. */
	io_result_t result;
};

#endif /* VIFM__IO__IOC_H__ */

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 filetype=c : */
