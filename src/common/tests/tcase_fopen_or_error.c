#include <check.h>
#include <errno.h>
#include <stddef.h>

#include "../src/s21_common.h"

FILE *file;
int result_code;

START_TEST(not_existed_file) {
    // Try to open not existed file. The error should occur.
    result_code = fopen_or_error("not_exitsted_file_name_ever!!.txtc", &file);

    ck_assert_int_eq(result_code, ENOENT);
    ck_assert_ptr_null(file);
}
END_TEST

START_TEST(directory) {
    // Try to open directory. The error should occur.
    result_code = fopen_or_error("./", &file);

    ck_assert_int_eq(result_code, EISDIR);
    ck_assert_ptr_null(file);
}

START_TEST(existed_file) {
    // Try to the file with the test itself.
    result_code = fopen_or_error("./tests/tcase_fopen_or_error.c", &file);

    ck_assert_int_eq(result_code, 0);
    ck_assert_ptr_nonnull(file);
}

TCase *tcase_fopen_or_error(void) {
    TCase *tc;

    tc = tcase_create("Util fopen_or_error");
    tcase_add_test(tc, not_existed_file);
    tcase_add_test(tc, directory);
    tcase_add_test(tc, existed_file);

    return tc;
}
