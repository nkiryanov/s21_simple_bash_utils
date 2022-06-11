#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "tcases.h"

Suite *suite_s21_cat(void) {
    Suite *s;

    s = suite_create("s21_cat");
    suite_add_tcase(s, tcase__process_and_output());
    suite_add_tcase(s, tcase__set_options());
    suite_add_tcase(s, tcase__turn_off_incompatible_flags());
    suite_add_tcase(s, tcase_s21_cat_complete_app());
    return s;
}

int main(void) {
    int number_failed;
    SRunner *sr;

    sr = srunner_create(suite_s21_cat());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
