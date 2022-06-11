#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "tcases.h"

Suite *suite_s21_common(void) {
    Suite *s;

    s = suite_create("s21_common");
    suite_add_tcase(s, tcase_fopen_or_error());
    suite_add_tcase(s, tcase_get_basename());
    suite_add_tcase(s, tcase_output_error_file_read());
    return s;
}

int main(void) {
    int number_failed;
    SRunner *sr;

    sr = srunner_create(suite_s21_common());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
