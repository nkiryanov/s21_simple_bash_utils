#ifndef SRC_GREP_S21_GREP_INTERNAL_H_
#define SRC_GREP_S21_GREP_INTERNAL_H_

#include <regex.h>
#include <stdio.h>

#define PATTERN_MAX_SIZE 1024
#define MAX_MATCHES 100

enum GREP_FLAGS {
    GREP_OUT_DEFAULT = 01,
    GREP_OUT_AMOUNT_ONLY = 02,
    GREP_OUT_FILENAMES_ONLY = 04,
    GREP_OUT_STRING_MATCH = 010,
    GREP_IGNORE_CASE = 020,
    GREP_INVERT_MATCH = 040,
    GREP_COUNT_LINES = 0100,
    GREP_PATTER_WAS_READ = 0200,
    GREP_SUPPRESS_FILENAMES = 0400,
    GREP_SUPPRESS_ERRORS = 01000,
    GREP_OUTPUT_FILENAMES = 02000,
};

typedef struct grep_opts {
    int flags;
    char patterns[PATTERN_MAX_SIZE];
} grep_opts;

#define GREP_EMPTY_OPTS \
    { 0, "" }

void _util_strip_newline_char(char *line, int size);
void _util_opts_patterns_append(grep_opts *opts, char *pattern_to_add);
int _util_set_compatible_flags(int flags, int num_files);

int _get_reg_from_file(grep_opts *opts, char *program_name, char *filename);

void _output_file_read_error(char *app_name, char *filename, int err_code,
                          grep_opts *opts);
void _output_match(int flags, char *filename, int line_count, char *line,
                int start, int finish);
void _output_newline_if_required(char last_char);

int _grep_opts_set(int argc, char *argv[], grep_opts *opts, int *next);
int _grep_re_prepare_and_compile(grep_opts *opts, regex_t *preg);

void _match_out_general(grep_opts *opts, regex_t *preg, char *file_in_name,
                        FILE *file_in);
void _match_out_string_match(grep_opts *opts, regex_t *preg, char *filename,
                             FILE *file_in);

#endif  // SRC_GREP_S21_GREP_INTERNAL_H_
