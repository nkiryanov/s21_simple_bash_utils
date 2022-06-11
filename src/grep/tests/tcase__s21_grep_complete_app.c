#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static FILE *std_grep_tmp = NULL;
static FILE *s21_grep_tmp = NULL;
static int saved_stdout;

static char s21_grep_buffer[1000];
static char std_grep_buffer[1000];

static void setup(void) {
    // Create tmp files and redirect save default stdout descriptor.
    saved_stdout = dup(STDOUT_FILENO);

    std_grep_tmp = tmpfile();
    s21_grep_tmp = tmpfile();

    if (std_grep_tmp == NULL || s21_grep_tmp == NULL) {
        perror("Cant create tmp files");
        exit(1);
    }
}

static void teardown(void) {
    // Set back stdout and close temp files and descriptors.
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);

    if (std_grep_tmp) fclose(std_grep_tmp);
    if (s21_grep_tmp) fclose(s21_grep_tmp);
}

static void redirect_stdout_to_file(FILE *file) {
    // Redirect STDOUT to a file
    dup2(fileno(file), STDOUT_FILENO);
}

static void read_entire_file_to_buffer(FILE *file, char *buffer) {
    int c;
    fseek(file, 0, SEEK_SET);
    while ((c = getc(file)) != EOF) {
        *buffer++ = (char)c;
    }
    *buffer = '\0';
    fseek(file, 0, SEEK_SET);
}

START_TEST(s21_grep_basic) {
    // Run std grep and s21_grep with same flag and same data.
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep t tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep t tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST

START_TEST(s21_grep_empty_pattern) {
    // Check results if empty pattern is passed.
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep \"\" tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep \"\" tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST

START_TEST(s21_grep_dot_pattern) {
    // Check results if empty pattern is passed.
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep . tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep . tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST

START_TEST(s21_grep_two_e_flags) {
    // Run std grep and s21_grep with -e flag two patterns.
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep -e a -e b tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep -e a -e b tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST

START_TEST(s21_grep_e_pipe_used) {
    // Combine two patterns in one.
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep -e \"a|b\" tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep -E -e \"a|b\" tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST

START_TEST(s21_grep_two_f_flags) {
    // Run std grep and s21_grep with -e flag two patterns.
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep -f tests/ptrn.txt -f tests/ptrn.txt tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep -E -f tests/ptrn.txt -f tests/ptrn.txt tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST

START_TEST(s21_grep_n_flag) {
    // Check `n` (count) flag.
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep -n . tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep -n . tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST

START_TEST(s21_grep_i_flag) {
    // Check `i` (ignore case) flag.
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep -i L tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep -i L tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST

START_TEST(s21_grep_two_files_search) {
    // Check that if two files passed then result prefixed with filenames
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep l tests/data.txt tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep l tests/data.txt tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST

START_TEST(s21_grep_h_flag) {
    // Check `h` flag. Should hide filenames even for two files
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep -h l tests/data.txt tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep -h l tests/data.txt tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST

START_TEST(s21_grep_hn_flag_combinations) {
    // Check `h` (hide) and `n` (count lines) flag.
    // Lines has to be with counts but without files prefixes.
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep -nh l tests/data.txt tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep -nh l tests/data.txt tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST

START_TEST(s21_grep_c_flag) {
    // Check `c` (matching amount only) flag.
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep -c l tests/data.txt tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep -c l tests/data.txt tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST

START_TEST(s21_grep_v_flag) {
    // Check `v` (invert match).
    redirect_stdout_to_file(s21_grep_tmp);
    system("./s21_grep -v l tests/data.txt tests/data.txt");
    read_entire_file_to_buffer(s21_grep_tmp, s21_grep_buffer);

    redirect_stdout_to_file(std_grep_tmp);
    system("grep -v l tests/data.txt tests/data.txt");
    read_entire_file_to_buffer(std_grep_tmp, std_grep_buffer);

    ck_assert_str_eq(s21_grep_buffer, std_grep_buffer);
}
END_TEST


TCase *tcase_s21_grep_complete_app(void) {
    TCase *tc;

    tc = tcase_create("s21_grep complete app");
    tcase_add_checked_fixture(tc, setup, teardown);
    tcase_add_test(tc, s21_grep_basic);
    tcase_add_test(tc, s21_grep_empty_pattern);
    tcase_add_test(tc, s21_grep_dot_pattern);
    tcase_add_test(tc, s21_grep_two_e_flags);
    tcase_add_test(tc, s21_grep_e_pipe_used);
    tcase_add_test(tc, s21_grep_two_f_flags);
    tcase_add_test(tc, s21_grep_n_flag);
    tcase_add_test(tc, s21_grep_i_flag);
    tcase_add_test(tc, s21_grep_two_files_search);
    tcase_add_test(tc, s21_grep_h_flag);
    tcase_add_test(tc, s21_grep_hn_flag_combinations);
    tcase_add_test(tc, s21_grep_c_flag);
    tcase_add_test(tc, s21_grep_v_flag);

    return tc;
}
