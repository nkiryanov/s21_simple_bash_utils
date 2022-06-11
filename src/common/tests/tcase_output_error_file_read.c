#include <check.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "../s21_common.h"

static char buffer[100];

void setup(void) {
    memset(buffer, '\0', 100);
    freopen("/dev/null", "a", stderr);
    setbuf(stderr, buffer);
}

void teardown(void) {
    freopen("/dev/tty", "a", stderr);
}

START_TEST(test_output_error_format) {
    char expected_error[100] = "";
    strcat(expected_error, "s21_cat: ./filename: ");
    strcat(expected_error, strerror(ENOENT));
    strcat(expected_error, "\n");

    output_error_file_read("./s21_cat", "./filename", ENOENT);

    ck_assert_str_eq(expected_error, buffer);
}
END_TEST

TCase *tcase_output_error_file_read(void) {
    TCase *tc;

    tc = tcase_create("Util output_error_file_read");
    tcase_add_checked_fixture(tc, setup, teardown);
    tcase_add_test(tc, test_output_error_format);

    return tc;
}
