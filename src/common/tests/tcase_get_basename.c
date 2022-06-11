#include <check.h>
#include <string.h>

#include "../src/s21_common.h"

START_TEST(test_filename_without_path) {
    char *filename = "filename_without_path.txt";
    char *basename = NULL;

    basename = get_basename(filename);

    ck_assert_ptr_eq(filename, basename);
}
END_TEST


START_TEST(test_filename_with_absolute_path) {
    char *filename = "/Users/folder/subfolder/filename_without_path.txt";
    char *basename = NULL;

    basename = get_basename(filename);

    ck_assert_str_eq("filename_without_path.txt", basename);
}
END_TEST

START_TEST(test_filename_with_relative_path) {
    char *filename = "./relative_path.txt";
    char *basename = NULL;

    basename = get_basename(filename);

    ck_assert_str_eq("relative_path.txt", basename);
}
END_TEST

TCase *tcase_get_basename(void) {
    TCase *tc;

    tc = tcase_create("Util get_basename");
    tcase_add_test(tc, test_filename_without_path);
    tcase_add_test(tc, test_filename_with_absolute_path);
    tcase_add_test(tc, test_filename_with_relative_path);

    return tc;
}
