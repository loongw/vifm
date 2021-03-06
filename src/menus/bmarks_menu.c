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

#include "bmarks_menu.h"

#include <string.h> /* strdup() */
#include <stdlib.h> /* free() */

#include "../ui/ui.h"
#include "../utils/path.h"
#include "../utils/str.h"
#include "../utils/string_array.h"
#include "../bmarks.h"
#include "../status.h"
#include "menus.h"

static int execute_bmarks_cb(FileView *view, menu_info *m);
static KHandlerResponse bmarks_khandler(menu_info *m, const wchar_t keys[]);
static void bmarks_cb(const char path[], const char tags[], time_t timestamp,
		void *arg);

int
show_bmarks_menu(FileView *view, const char tags[], int go_on_single_match)
{
	static menu_info m;
	init_menu_info(&m, strdup("Bookmarks"), strdup("No bookmarks found"));
	m.execute_handler = &execute_bmarks_cb;
	m.key_handler = &bmarks_khandler;

	if(is_null_or_empty(tags))
	{
		bmarks_list(&bmarks_cb, &m);
	}
	else
	{
		bmarks_find(tags, &bmarks_cb, &m);
	}

	if(go_on_single_match && m.len == 1)
	{
		(void)goto_selected_file(view, m.items[m.pos], 0);
		reset_popup_menu(&m);
		return curr_stats.save_msg;
	}

	return display_menu(&m, view);
}

/* Callback for listings of bookmarks. */
static void
bmarks_cb(const char path[], const char tags[], time_t timestamp, void *arg)
{
	menu_info *m = arg;
	char *line = format_str("%s: ", replace_home_part_strict(path));
	size_t len = strlen(line);

	char *dup = strdup(tags);
	char *tag = dup, *state = NULL;
	while((tag = split_and_get(tag, ',', &state)) != NULL)
	{
		strappendch(&line, &len, '[');
		strappend(&line, &len, tag);
		strappendch(&line, &len, ']');
	}
	free(dup);

	(void)add_to_string_array(&m->data, m->len, 1, path);
	m->len = put_into_string_array(&m->items, m->len, line);
}

/* Callback that is called when menu item is selected.  Should return non-zero
 * to stay in menu mode. */
static int
execute_bmarks_cb(FileView *view, menu_info *m)
{
	(void)goto_selected_file(view, m->data[m->pos], 0);
	return 0;
}

/* Menu-specific shortcut handler.  Returns code that specifies both taken
 * actions and what should be done next. */
static KHandlerResponse
bmarks_khandler(menu_info *m, const wchar_t keys[])
{
	if(wcscmp(keys, L"dd") == 0)
	{
		bmarks_remove(m->data[m->pos]);
		remove_current_item(m);
		return KHR_REFRESH_WINDOW;
	}
	else if(wcscmp(keys, L"gf") == 0)
	{
		(void)goto_selected_file(curr_view, m->data[m->pos], 0);
		return KHR_CLOSE_MENU;
	}
	else if(wcscmp(keys, L"e") == 0)
	{
		(void)goto_selected_file(curr_view, m->data[m->pos], 1);
		return KHR_REFRESH_WINDOW;
	}
	/* Can't reuse filelist_khandler() here as it works with m->items, not with
	 * m->data. */
	return KHR_UNHANDLED;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 filetype=c : */
