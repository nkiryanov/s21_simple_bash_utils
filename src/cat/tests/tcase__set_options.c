#include <check.h>
#include <stdlib.h>

#include "../src/s21_cat_internal.h"

START_TEST(test_one_option) {
    // Normal behaviour. Only one -n flag is used.
    char *argv[] = {"s21_cat", "-n"};
    int num_proceeded = 0;
    int flags = 0;

    _set_options(sizeof(argv) / sizeof(char *), argv, &flags, &num_proceeded);

    ck_assert_int_eq(flags, NUM_ALL);
    ck_assert_int_eq(num_proceeded, 2);
}
END_TEST

START_TEST(test_two_separate_options) {
    // Normal behaviour. Two options -n and -s separate.
    char *argv[] = {"s21_cat", "-n", "-s"};
    int num_proceeded = 0;
    int flags = 0;

    _set_options(sizeof(argv) / sizeof(char *), argv, &flags, &num_proceeded);

    ck_assert_int_eq(flags, NUM_ALL | SQUEEZE_BLANK);
    ck_assert_int_eq(num_proceeded, 3);
}
END_TEST

START_TEST(test_two_options) {
    // Normal behaviour. Two options -n and -s in one arg.
    char *argv[] = {"s21_cat", "-ns"};
    int num_proceeded = 0;
    int flags = 0;

    _set_options(sizeof(argv) / sizeof(char *), argv, &flags, &num_proceeded);

    ck_assert_int_eq(flags, NUM_ALL | SQUEEZE_BLANK);
    ck_assert_int_eq(num_proceeded, 2);
}
END_TEST

START_TEST(test_long_option) {
    // Normal behaviour. Two options -e and -s in one arg and one more long
    // option.
    char *argv[] = {"s21_cat", "-es", "--number-nonblank"};
    int num_proceeded = 0;
    int flags = 0;

    _set_options(sizeof(argv) / sizeof(char *), argv, &flags, &num_proceeded);

    ck_assert_int_eq(flags, SQUEEZE_BLANK | NUM_NON_BLANK | PRINT_END_LINES |
                                PRINT_NON_PRINTS);
    ck_assert_int_eq(num_proceeded, 3);
}
END_TEST

START_TEST(test_file_name_in_the_middle) {
    // Normal behaviour. Two options -e and -s in one arg and one more long
    // option. Arg values is separated by filename
    char *argv[] = {"s21_cat", "-es", "filename", "--number-nonblank"};
    int num_proceeded = 0;
    int flags = 0;

    _set_options(sizeof(argv) / sizeof(char *), argv, &flags, &num_proceeded);

    ck_assert_int_eq(flags, SQUEEZE_BLANK | NUM_NON_BLANK | PRINT_END_LINES |
                                PRINT_NON_PRINTS);
    ck_assert_int_eq(num_proceeded, 3);
}
END_TEST

START_TEST(test_all_options_on) {
    // Normal behaviour. All options is on.
    char *argv[] = {"s21_cat", "-nesEtTb", "--number-nonblank"};
    int num_proceeded = 0;
    int flags = 0;

    _set_options(sizeof(argv) / sizeof(char *), argv, &flags, &num_proceeded);

    ck_assert_int_eq(flags, SQUEEZE_BLANK | NUM_NON_BLANK | NUM_ALL |
                                PRINT_TABS | PRINT_END_LINES |
                                PRINT_NON_PRINTS);
    ck_assert_int_eq(num_proceeded, 3);
}
END_TEST

START_TEST(test_not_allowed_option) {
    // Normal behaviour. Only not allowed options.
    char *argv[] = {"s21_cat", "-A"};
    int num_proceeded = 0;
    int flags = 0;
    freopen("/dev/null", "a", stderr);

    _set_options(sizeof(argv) / sizeof(char *), argv, &flags, &num_proceeded);
    freopen("/dev/tty", "a", stderr);

    ck_assert_int_eq(flags, 0);
    ck_assert_int_eq(num_proceeded, 2);
}
END_TEST

TCase *tcase__set_options(void) {
    TCase *tc;

    tc = tcase_create("Internal util _set_options");
    tcase_add_test(tc, test_one_option);
    tcase_add_test(tc, test_two_separate_options);
    tcase_add_test(tc, test_two_options);
    tcase_add_test(tc, test_long_option);
    tcase_add_test(tc, test_file_name_in_the_middle);
    tcase_add_test(tc, test_all_options_on);
    tcase_add_test(tc, test_not_allowed_option);

    return tc;
}
