#include <stic.h>

#include <unistd.h> /* chdir() unlink() */

#include <stddef.h> /* NULL */
#include <stdlib.h> /* free() */
#include <string.h> /* strcpy() strdup() */

#include "../../src/utils/dynarray.h"
#include "../../src/utils/fs.h"
#include "../../src/utils/macros.h"
#include "../../src/utils/path.h"
#include "../../src/filelist.h"
#include "../../src/fileops.h"

static void free_view(FileView *view);
static int not_windows(void);

SETUP()
{
	assert_success(chdir(SANDBOX_PATH));

	/* lwin */
	strcpy(lwin.curr_dir, ".");

	lwin.list_rows = 1;
	lwin.list_pos = 0;
	lwin.dir_entry = dynarray_cextend(NULL,
			lwin.list_rows*sizeof(*lwin.dir_entry));
	lwin.dir_entry[0].name = strdup("file");
	lwin.dir_entry[0].origin = &lwin.curr_dir[0];

	/* rwin */
	strcpy(rwin.curr_dir, ".");

	rwin.list_rows = 0;
	rwin.filtered = 0;
	rwin.list_pos = 0;
	rwin.dir_entry = NULL;
	assert_int_equal(0, filter_init(&rwin.local_filter.filter, 0));

	curr_view = &lwin;
	other_view = &rwin;
}

TEARDOWN()
{
	free_view(&lwin);
	free_view(&rwin);

	assert_int_equal(0, chdir("../.."));

	filter_dispose(&rwin.local_filter.filter);
}

static void
free_view(FileView *view)
{
	int i;

	for(i = 0; i < view->list_rows; ++i)
	{
		free_dir_entry(view, &view->dir_entry[i]);
	}
	dynarray_free(view->dir_entry);
}

TEST(move_file)
{
	char new_fname[] = "new_name";
	char *list[] = { &new_fname[0] };

	FILE *const f = fopen(lwin.dir_entry[0].name, "w");
	fclose(f);

	assert_true(path_exists(lwin.dir_entry[0].name, DEREF));

	lwin.dir_entry[0].marked = 1;
	(void)cpmv_files(&lwin, list, ARRAY_LEN(list), CMLO_MOVE, 0);

	assert_false(path_exists(lwin.dir_entry[0].name, DEREF));
	assert_true(path_exists(new_fname, DEREF));

	(void)unlink(new_fname);
}

TEST(make_relative_link, IF(not_windows))
{
	char link_name[] = "link";
	char *list[] = { &link_name[0] };
	char link_value[PATH_MAX];

	strcpy(lwin.curr_dir, "/fake/absolute/path");

	lwin.dir_entry[0].marked = 1;
	(void)cpmv_files(&lwin, list, ARRAY_LEN(list), CMLO_LINK_REL, 0);

	assert_true(path_exists(link_name, NODEREF));

	assert_success(get_link_target(link_name, link_value, sizeof(link_value)));
	assert_false(is_path_absolute(link_value));

	(void)unlink(link_name);
}

TEST(make_absolute_link, IF(not_windows))
{
	char link_name[] = "link";
	char *list[] = { &link_name[0] };
	char link_value[PATH_MAX];

	strcpy(lwin.curr_dir, "/fake/absolute/path");

	lwin.dir_entry[0].marked = 1;
	(void)cpmv_files(&lwin, list, ARRAY_LEN(list), CMLO_LINK_ABS, 0);

	assert_true(path_exists(link_name, NODEREF));

	assert_success(get_link_target(link_name, link_value, sizeof(link_value)));
	assert_true(is_path_absolute(link_value));

	(void)unlink(link_name);
}

TEST(refuse_to_copy_or_move_to_source_files_with_the_same_name)
{
	assert_false(flist_custom_active(&rwin));

	flist_custom_start(&rwin, "test");
	flist_custom_add(&rwin, TEST_DATA_PATH "/existing-files/a");
	flist_custom_add(&rwin, TEST_DATA_PATH "/rename/a");
	assert_true(flist_custom_finish(&rwin, 0) == 0);

	curr_view = &rwin;
	other_view = &lwin;

	rwin.dir_entry[0].marked = 1;
	rwin.dir_entry[1].marked = 1;
	rwin.selected_files = 2;
	(void)cpmv_files(&rwin, NULL, 0, CMLO_COPY, 0);
	(void)cpmv_files(&rwin, NULL, 0, CMLO_COPY, 1);
	(void)cpmv_files(&rwin, NULL, 0, CMLO_MOVE, 0);
	(void)cpmv_files(&rwin, NULL, 0, CMLO_MOVE, 1);

	assert_false(path_exists("a", NODEREF));
}

static int
not_windows(void)
{
#ifdef _WIN32
	return 0;
#else
	return 1;
#endif
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 filetype=c : */
