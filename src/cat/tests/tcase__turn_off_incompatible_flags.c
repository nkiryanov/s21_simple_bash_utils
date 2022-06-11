#include <check.h>

#include "../s21_cat_internal.h"

START_TEST(test_num_non_blank_takes_precedence) {
    int flags = 0;
    flags |= NUM_NON_BLANK | NUM_ALL;
    flags = _turn_off_incompatible_flags(flags);

    ck_assert_int_eq(flags, NUM_NON_BLANK);
}
END_TEST

TCase *tcase__turn_off_incompatible_flags(void) {
    TCase *tc;

    tc = tcase_create("Internal util _turn_off_incompatible_flags");
    tcase_add_test(tc, test_num_non_blank_takes_precedence);

    return tc;
}
