#include <stic.h>

#include <stdlib.h> /* remove() */

#include "../../src/compat/os.h"
#include "../../src/utils/fswatch.h"

static int using_inotify(void);

TEST(watch_is_created_for_existing_directory)
{
	fswatch_t *watch;

	assert_non_null(watch = fswatch_create(SANDBOX_PATH));
	fswatch_free(watch);
}

TEST(two_watches_for_the_same_directory)
{
	fswatch_t *watch1, *watch2;

	assert_non_null(watch1 = fswatch_create(SANDBOX_PATH));
	assert_non_null(watch2 = fswatch_create(SANDBOX_PATH));
	fswatch_free(watch2);
	fswatch_free(watch1);
}

TEST(events_are_accumulated, IF(using_inotify))
{
	fswatch_t *watch;
	int error;

	assert_non_null(watch = fswatch_create(SANDBOX_PATH));

	os_mkdir(SANDBOX_PATH "/testdir", 0700);
	remove(SANDBOX_PATH "/testdir");
	assert_true(fswatch_changed(watch, &error));
	assert_false(error);
	assert_false(fswatch_changed(watch, &error));
	assert_false(error);

	fswatch_free(watch);
}

TEST(started_as_not_changed)
{
	fswatch_t *watch;
	int error;

	assert_non_null(watch = fswatch_create(SANDBOX_PATH));

	assert_false(fswatch_changed(watch, &error));
	assert_false(error);

	fswatch_free(watch);
}

TEST(handles_several_events_in_a_row, IF(using_inotify))
{
	fswatch_t *watch;
	int error;

	assert_non_null(watch = fswatch_create(SANDBOX_PATH));

	os_mkdir(SANDBOX_PATH "/testdir", 0700);
	assert_true(fswatch_changed(watch, &error));
	assert_false(error);

	remove(SANDBOX_PATH "/testdir");
	assert_true(fswatch_changed(watch, &error));
	assert_false(error);

	fswatch_free(watch);
}

TEST(to_many_events_causes_banning_of_same_events, IF(using_inotify))
{
	fswatch_t *watch;
	int error;
	int i;

	assert_non_null(watch = fswatch_create(SANDBOX_PATH));

	os_mkdir(SANDBOX_PATH "/testdir", 0700);

	for(i = 0; i < 100; ++i)
	{
		os_chmod(SANDBOX_PATH "/testdir", 0777);
		os_chmod(SANDBOX_PATH "/testdir", 0000);
		(void)fswatch_changed(watch, &error);
	}

	os_chmod(SANDBOX_PATH "/testdir", 0777);
	assert_false(fswatch_changed(watch, &error));
	assert_false(error);

	assert_success(remove(SANDBOX_PATH "/testdir"));
	assert_true(fswatch_changed(watch, &error));
	assert_false(error);

	fswatch_free(watch);
}

TEST(file_recreation_removes_ban, IF(using_inotify))
{
	fswatch_t *watch;
	int error;
	int i;

	assert_non_null(watch = fswatch_create(SANDBOX_PATH));

	os_mkdir(SANDBOX_PATH "/testdir", 0700);

	for(i = 0; i < 100; ++i)
	{
		os_chmod(SANDBOX_PATH "/testdir", 0777);
		os_chmod(SANDBOX_PATH "/testdir", 0000);
		(void)fswatch_changed(watch, &error);
	}

	assert_success(remove(SANDBOX_PATH "/testdir"));
	assert_true(fswatch_changed(watch, &error));
	assert_false(error);

	os_mkdir(SANDBOX_PATH "/testdir", 0700);
	assert_true(fswatch_changed(watch, &error));
	assert_false(error);

	os_chmod(SANDBOX_PATH "/testdir", 0777);
	assert_true(fswatch_changed(watch, &error));
	assert_false(error);

	fswatch_free(watch);

	assert_success(remove(SANDBOX_PATH "/testdir"));
}

static int
using_inotify(void)
{
#ifdef HAVE_INOTIFY
	return 1;
#else
	return 0;
#endif
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 filetype=c : */
