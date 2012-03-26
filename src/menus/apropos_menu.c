/* vifm
 * Copyright (C) 2001 Ken Steen.
 * Copyright (C) 2011 xaizek.
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

#include <string.h> /* strdup() strchr() strlen() */

#include "../running.h"
#include "../status.h"
#include "menus.h"

#include "apropos_menu.h"

void
show_apropos_menu(FileView *view, char *args)
{
	char buf[256];
	int were_errors;

	static menu_info m;
	m.top = 0;
	m.current = 1;
	m.len = 0;
	m.pos = 0;
	m.hor_pos = 0;
	m.win_rows = getmaxy(menu_win);
	m.type = APROPOS;
	m.matching_entries = 0;
	m.matches = NULL;
	m.match_dir = NONE;
	m.regexp = NULL;
	m.title = NULL;
	m.args = strdup(args);
	m.items = NULL;
	m.data = NULL;

	m.title = (char *)malloc((strlen(args) + 12) * sizeof(char));
	snprintf(m.title, strlen(args) + 11,  " Apropos %s ",  args);
	snprintf(buf, sizeof(buf), "apropos %s", args);

	were_errors = capture_output_to_menu(view, buf, &m);
	if(!were_errors && m.len < 1)
		show_error_msgf("Nothing Appropriate", "No matches for \'%s\'", m.title);
}

void
execute_apropos_cb(menu_info *m)
{
	char *line;
	char *man_page;
	char *free_this;
	char *num_str;
	char command[256];

	free_this = man_page = line = strdup(m->items[m->pos]);
	if(free_this == NULL)
	{
		(void)show_error_msg("Memory Error", "Unable to allocate enough memory");
		return;
	}

	if((num_str = strchr(line, '(')))
	{
		int z = 0;

		num_str++;
		while(num_str[z] != ')')
		{
			z++;
			if(z > 40)
			{
				free(free_this);
				return;
			}
		}

		num_str[z] = '\0';
		line = strchr(line, ' ');
		line[0] = '\0';

		snprintf(command, sizeof(command), "man %s %s", num_str, man_page);

		curr_stats.auto_redraws = 1;
		shellout(command, 0, 1);
		curr_stats.auto_redraws = 0;
	}
	free(free_this);
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 : */
