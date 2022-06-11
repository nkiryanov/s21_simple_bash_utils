#include <check.h>

#include "../s21_grep_internal.h"
#define argc ((sizeof argv) / sizeof(char *))

static grep_opts opts;

void setup(void) {
    opts.flags = 0;
    memset(opts.patterns, '\0', PATTERN_MAX_SIZE);
}

void teardown(void) {}

START_TEST(two_pattern_match) {
    char *argv[] = {"pn", "-e", "[0-9]", "-e", "[a-b]"};
    int next = 0;

    _grep_opts_set(argc, argv, &opts, &next);

    ck_assert_str_eq("([0-9])|([a-b])", opts.patterns);
    ck_assert_int_eq(next, argc);
}
END_TEST

TCase *tcase__grep_opts_set(void) {
    TCase *tc;

    tc = tcase_create("Utility _grep_opts_set");
    tcase_add_checked_fixture(tc, setup, teardown);

    tcase_add_test(tc, two_pattern_match);

    return tc;
}
