#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static FILE *std_cat_tmp = NULL;
static FILE *s21_cat_tmp = NULL;
static int saved_stdout;

static char s21_cat_buffer[1000];
static char cat_buffer[1000];

static void setup(void) {
    // Create tmp files and redirect save default stdout descriptor.
    saved_stdout = dup(STDOUT_FILENO);

    std_cat_tmp = tmpfile();
    s21_cat_tmp = tmpfile();

    if (std_cat_tmp == NULL || s21_cat_tmp == NULL) {
        perror("Cant create tmp files");
        exit(1);
    }
}

static void teardown(void) {
    // Set back stdout and close temp files and descriptors.
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);

    if (std_cat_tmp) fclose(std_cat_tmp);
    if (s21_cat_tmp) fclose(s21_cat_tmp);
}

static void redirect_stdout_to_file(FILE *file) {
    // Redirect STDOUT to a file
    dup2(fileno(file), STDOUT_FILENO);
}

static void read_entire_file_to_buffer(FILE *file, char *buffer) {
    int c;
    fseek(file, 0, SEEK_SET);
    while ((c = getc(file)) != EOF) { *buffer++ = (char)c; }
    *buffer = '\0';
    fseek(file, 0, SEEK_SET);
}

START_TEST(s21_cat_no_opts) {
    // Run std cat and s21_cat with same flag and same data.
    redirect_stdout_to_file(s21_cat_tmp);
    system("./s21_cat tests/data.txt");
    read_entire_file_to_buffer(s21_cat_tmp, s21_cat_buffer);

    redirect_stdout_to_file(std_cat_tmp);
    system("cat tests/data.txt");
    read_entire_file_to_buffer(std_cat_tmp, cat_buffer);

    ck_assert_str_eq(s21_cat_buffer, cat_buffer);
}
END_TEST

START_TEST(s21_cat_num_non_blank) {
    // Run std cat and s21_cat with -b (num non blank) option.
    redirect_stdout_to_file(s21_cat_tmp);
    system("./s21_cat -b tests/data.txt");
    read_entire_file_to_buffer(s21_cat_tmp, s21_cat_buffer);

    redirect_stdout_to_file(std_cat_tmp);
    system("cat -b tests/data.txt");
    read_entire_file_to_buffer(std_cat_tmp, cat_buffer);

    ck_assert_str_eq(s21_cat_buffer, cat_buffer);
}
END_TEST

START_TEST(s21_cat_num_squeeze_blank) {
    // Run std cat and s21_cat with -s (squeeze blank) option.
    redirect_stdout_to_file(s21_cat_tmp);
    system("./s21_cat -s tests/data.txt");
    read_entire_file_to_buffer(s21_cat_tmp, s21_cat_buffer);

    redirect_stdout_to_file(std_cat_tmp);
    system("cat -s tests/data.txt");
    read_entire_file_to_buffer(std_cat_tmp, cat_buffer);

    ck_assert_str_eq(s21_cat_buffer, cat_buffer);
}
END_TEST

START_TEST(s21_cat_print_non_prints_and_ends) {
    // Run std cat and s21_cat with -e (print non prints and ends) option.
    redirect_stdout_to_file(s21_cat_tmp);
    system("./s21_cat -e tests/data.txt");
    read_entire_file_to_buffer(s21_cat_tmp, s21_cat_buffer);

    redirect_stdout_to_file(std_cat_tmp);
    system("cat -e tests/data.txt");
    read_entire_file_to_buffer(std_cat_tmp, cat_buffer);

    ck_assert_str_eq(s21_cat_buffer, cat_buffer);
}
END_TEST

START_TEST(s21_cat_print_non_prints_and_tabs) {
    // Run std cat and s21_cat with -t (print non prints and tabs) option.
    redirect_stdout_to_file(s21_cat_tmp);
    system("./s21_cat -t tests/data.txt");
    read_entire_file_to_buffer(s21_cat_tmp, s21_cat_buffer);

    redirect_stdout_to_file(std_cat_tmp);
    system("cat -t tests/data.txt");
    read_entire_file_to_buffer(std_cat_tmp, cat_buffer);

    ck_assert_str_eq(s21_cat_buffer, cat_buffer);
}
END_TEST

TCase *tcase_s21_cat_complete_app(void) {
    TCase *tc;

    tc = tcase_create("s21_cat complete app");
    tcase_add_checked_fixture(tc, setup, teardown);
    tcase_add_test(tc, s21_cat_no_opts);
    tcase_add_test(tc, s21_cat_num_non_blank);
    tcase_add_test(tc, s21_cat_num_squeeze_blank);
    tcase_add_test(tc, s21_cat_print_non_prints_and_ends);
    tcase_add_test(tc, s21_cat_print_non_prints_and_tabs);

    return tc;
}
