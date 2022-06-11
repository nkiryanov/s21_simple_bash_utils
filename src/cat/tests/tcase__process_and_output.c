#include <check.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../src/s21_cat_internal.h"

static FILE *tmp_file_in = NULL;
static FILE *tmp_file_out = NULL;
static char out_buffer[1000];

static void setup(void) {
    // Change stdout stream to tmp file.
    memset(out_buffer, '\0', 1000);
    tmp_file_in = tmpfile();
    tmp_file_out = tmpfile();
    if (tmp_file_in == NULL || tmp_file_out == NULL) {
        perror("Can't create temporary files for tests");
        exit(1);
    }
}

static void teardown(void) {
    if (tmp_file_in) fclose(tmp_file_in);
    if (tmp_file_out) fclose(tmp_file_out);
}

static void read_entire_file_to_buffer(FILE *file, char *buffer_old) {
    // Copy entire file to a string. The string have has enough space.
    int c;
    char *buffer = buffer_old;
    fseek(file, 0, SEEK_SET);

    while ((c = getc(file)) != EOF) {
        *buffer++ = (char)c;
    }
    *buffer = '\0';
    fseek(file, 0, SEEK_SET);
}

static void write_line_to_a_file(char *line, FILE *file) {
    // Write string to the beginning of a file.
    fseek(file, 0, SEEK_SET);
    fprintf(file, "%s", line);
    fseek(file, 0, SEEK_SET);
}

START_TEST(non_opts) {
    // Non empty file without options.
    int flags = 0;
    char *file_content = "line_1\nline_2\nline_3\n\n";
    write_line_to_a_file(file_content, tmp_file_in);

    _process_and_output(tmp_file_in, tmp_file_out, flags);
    read_entire_file_to_buffer(tmp_file_out, out_buffer);

    ck_assert_str_eq(file_content, out_buffer);
}
END_TEST

START_TEST(num_non_blank) {
    // Non empty file, numerate not empty blank.
    int flags = NUM_NON_BLANK;
    char *file_content = "line_1\nline2\n \n\nline3\n";
    char *expected_output =
        "     1\tline_1\n     2\tline2\n     3\t \n\n     4\tline3\n";
    write_line_to_a_file(file_content, tmp_file_in);

    _process_and_output(tmp_file_in, tmp_file_out, flags);
    read_entire_file_to_buffer(tmp_file_out, out_buffer);

    ck_assert_str_eq(expected_output, out_buffer);
}
END_TEST

START_TEST(num_all) {
    // Numerate all lines.
    int flags = NUM_ALL;
    char *file_content = "line_1\nline2\n \n\nline3\n";
    char *expected_output =
        "     1\tline_1\n     2\tline2\n     3\t \n     4\t\n     5\tline3\n";
    write_line_to_a_file(file_content, tmp_file_in);

    _process_and_output(tmp_file_in, tmp_file_out, flags);
    read_entire_file_to_buffer(tmp_file_out, out_buffer);

    ck_assert_str_eq(expected_output, out_buffer);
}
END_TEST

START_TEST(squeeze_lines) {
    // Lines in the middle and in the end has to be squeezed.
    int flags = NUM_ALL | SQUEEZE_BLANK;
    char *file_content = "line_1\n\n\nline2\n\n";
    char *expected_output =
        "     1\tline_1\n     2\t\n     3\tline2\n     4\t\n";
    write_line_to_a_file(file_content, tmp_file_in);

    _process_and_output(tmp_file_in, tmp_file_out, flags);
    read_entire_file_to_buffer(tmp_file_out, out_buffer);

    ck_assert_str_eq(expected_output, out_buffer);
}
END_TEST

START_TEST(squeeze_lines_five_empty_lines_only) {
    // The file contains only 5 empty lines
    int flags = SQUEEZE_BLANK;
    char *file_content = "\n\n\n\n\n";
    char *expected_output = "\n";
    write_line_to_a_file(file_content, tmp_file_in);

    _process_and_output(tmp_file_in, tmp_file_out, flags);
    read_entire_file_to_buffer(tmp_file_out, out_buffer);

    ck_assert_str_eq(expected_output, out_buffer);
}
END_TEST

START_TEST(print_non_print_chars) {
    // The file contains only non print chars
    int flags = PRINT_NON_PRINTS | PRINT_TABS;
    char file_content[] = {1, 2, 3, '\n', '\0'};
    char *expected_output = "^A^B^C\n";
    write_line_to_a_file(file_content, tmp_file_in);

    _process_and_output(tmp_file_in, tmp_file_out, flags);
    read_entire_file_to_buffer(tmp_file_out, out_buffer);

    ck_assert_str_eq(expected_output, out_buffer);
}
END_TEST

START_TEST(print_end_lines) {
    // End line should be printed as $
    int flags = PRINT_END_LINES;
    char *file_content = "\n";
    char *expected_output = "$\n";
    write_line_to_a_file(file_content, tmp_file_in);

    _process_and_output(tmp_file_in, tmp_file_out, flags);
    read_entire_file_to_buffer(tmp_file_out, out_buffer);

    ck_assert_str_eq(expected_output, out_buffer);
}
END_TEST

START_TEST(print_tabs) {
    // Tabs should be printed as `^I`
    int flags = PRINT_TABS;
    char *file_content = "\t\n\t\t";
    char *expected_output = "^I\n^I^I";
    write_line_to_a_file(file_content, tmp_file_in);

    _process_and_output(tmp_file_in, tmp_file_out, flags);
    read_entire_file_to_buffer(tmp_file_out, out_buffer);

    ck_assert_str_eq(expected_output, out_buffer);
}
END_TEST

TCase *tcase__process_and_output(void) {
    TCase *tc;

    tc = tcase_create("Utility process and output file");
    tcase_add_checked_fixture(tc, setup, teardown);

    tcase_add_test(tc, non_opts);
    tcase_add_test(tc, num_non_blank);
    tcase_add_test(tc, num_all);
    tcase_add_test(tc, squeeze_lines);
    tcase_add_test(tc, squeeze_lines_five_empty_lines_only);
    tcase_add_test(tc, print_non_print_chars);
    tcase_add_test(tc, print_end_lines);
    tcase_add_test(tc, print_tabs);

    return tc;
}
