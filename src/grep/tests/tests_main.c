#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "tcases.h"

Suite *suite_s21_grep(void) {
    Suite *s;

    s = suite_create("s21_grep");
    suite_add_tcase(s, tcase__grep_opts_set());
    suite_add_tcase(s, tcase_s21_grep_complete_app());
    return s;
}

int main(void) {
    int number_failed;
    SRunner *sr;

    sr = srunner_create(suite_s21_grep());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
