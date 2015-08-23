/* vifm
 * Copyright (C) 2015 xaizek.
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

#include "bmarks.h"

#include <stddef.h> /* NULL size_t */
#include <stdlib.h> /* free() realloc() */
#include <string.h> /* strdup() strcmp() */

#include "utils/str.h"

/* Single bookmark representation. */
typedef struct
{
	char *path; /* Path to the directory. */
	char *tags; /* Comma-seperated list of tags. */
}
bmark_t;

static int add_bmark(const char path[], const char tags[]);

/* Array of the bookmarks. */
static bmark_t *bmarks;
/* Current number of bookmarks. */
static size_t bmark_count;

int
bmarks_set(const char path[], const char tags[])
{
	size_t i;
	/* Try to update tags of an existing bookmark. */
	for(i = 0U; i < bmark_count; ++i)
	{
		if(strcmp(path, bmarks[i].path) == 0)
		{
			return replace_string(&bmarks[i].tags, tags);
		}
	}

	return add_bmark(path, tags);
}

/* Adds new bookmark.  Returns zero on success and non-zero otherwise. */
static int
add_bmark(const char path[], const char tags[])
{
	bmark_t *bm;

	void *p = realloc(bmarks, sizeof(*bmarks)*(bmark_count + 1));
	if(p == NULL)
	{
		return 1;
	}
	bmarks = p;

	bm = &bmarks[bmark_count];
	bm->path = strdup(path);
	bm->tags = strdup(tags);
	if(bm->path == NULL || bm->tags == NULL)
	{
		free(bm->path);
		free(bm->tags);
		return 1;
	}

	++bmark_count;
	return 0;
}

void
bmarks_list(bmarks_find_cb cb, void *arg)
{
	size_t i;
	for(i = 0U; i < bmark_count; ++i)
	{
		if(bmarks[i].tags[0] != '\0')
		{
			cb(bmarks[i].path, bmarks[i].tags, arg);
		}
	}
}

void
bmarks_clear(void)
{
	size_t i;
	for(i = 0U; i < bmark_count; ++i)
	{
		free(bmarks[i].path);
		free(bmarks[i].tags);
	}
	free(bmarks);

	bmarks = NULL;
	bmark_count = 0U;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 filetype=c : */
