#include "s21_grep_internal.h"

#include <getopt.h>
#include <regex.h>
#include <s21_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void _util_strip_newline_char(char *line, int size) {
    // Strip newline character from line
    char *end_char = line + size - 1;
    if (*end_char == '\n') *end_char = '\0';
}

void _util_opts_patterns_append(grep_opts *opts, char *pattern_to_add) {
    // Add pattern to patterns string. If pattern is not empty add it with |
    size_t patterns_size;
    size_t pattern_to_add_size;

    // If the string is empty replace it with all match pattern
    pattern_to_add = (strlen(pattern_to_add)) ? pattern_to_add : ".*";

    pattern_to_add_size = strlen(pattern_to_add);
    patterns_size = strlen(opts->patterns);

    // Append pattern only if it have enough space
    if (patterns_size + pattern_to_add_size + 3 < PATTERN_MAX_SIZE) {
        if (opts->flags & GREP_PATTER_WAS_READ) {
            sprintf((opts->patterns) + patterns_size, "|(%s)", pattern_to_add);
        } else {
            sprintf((opts->patterns) + patterns_size, "(%s)", pattern_to_add);
            opts->flags |= GREP_PATTER_WAS_READ;
        }
    }
}

int _util_set_compatible_flags(int flags, int num_files) {
    // If output type `filenames only` then turn off `amount only` and ``
    if (flags & GREP_OUT_FILENAMES_ONLY) {
        flags &= ~(GREP_OUT_AMOUNT_ONLY);
        flags &= ~(GREP_OUT_STRING_MATCH);
        flags &= ~(GREP_OUTPUT_FILENAMES);
        flags &= ~(GREP_COUNT_LINES);
        // If output type amount only than turn off match (-o) flag
    } else if (flags & GREP_OUT_AMOUNT_ONLY) {
        flags &= ~(GREP_OUT_STRING_MATCH);
        flags &= ~(GREP_COUNT_LINES);
        // Set default output flag if other is set off
    } else if (!(flags & GREP_OUT_STRING_MATCH)) {
        flags |= GREP_OUT_DEFAULT;
    }

    // Turn off output for flags `output match` (-o) and `invert match` (-v)
    if (flags & GREP_OUT_STRING_MATCH && flags & GREP_INVERT_MATCH) {
        flags &= ~(GREP_OUT_AMOUNT_ONLY | GREP_OUT_FILENAMES_ONLY |
                   GREP_OUT_STRING_MATCH | GREP_OUT_DEFAULT);
    }

    // If more than one file has to be read and SUPPRESS_FILENAMES is off then
    // turn on `OUTPUT_FILENAMES`
    if (num_files > 1 && (flags & GREP_SUPPRESS_FILENAMES) == 0) {
        flags |= GREP_OUTPUT_FILENAMES;
    }

    return flags;
}

int _get_reg_from_file(grep_opts *opts, char *program_name, char *filename) {
    // Get regex from file. Patterns are read line by line.
    // If file could be found or open return error.
    int read_code;
    FILE *file = NULL;
    char *line = NULL;
    size_t line_volume = 0;
    ssize_t size;

    read_code = fopen_or_error(filename, &file);
    if (read_code != 0) {
        output_error_file_read(program_name, filename, read_code);
    } else {
        while ((size = getline(&line, &line_volume, file)) != -1) {
            _util_strip_newline_char(line, size);
            _util_opts_patterns_append(opts, line);
        }
    }

    if (line != NULL) free(line);
    if (file != NULL) fclose(file);

    return read_code;
}

void _output_file_read_error(char *app_name, char *filename, int err_code,
                             grep_opts *opts) {
    // Wrapper for output_error_file_read().
    // Support suppress files read errors.
    if ((opts->flags & GREP_SUPPRESS_ERRORS) == 0) {
        output_error_file_read(app_name, filename, err_code);
    }
}

void _output_newline_if_required(char last_char) {
    // Output newline character if line doesn't have it
    if (last_char != '\n') printf("\n");
}

void _output_match(int flags, char *filename, int line_count, char *line,
                   int start, int finish) {
    // Output line with filename or lines count prefix according to flags.
    // The start and finish index is set then it out chars from start to
    // finish. If start and finish values is 0 than out whole line
    if (flags & GREP_OUTPUT_FILENAMES) printf("%s:", filename);
    if (flags & GREP_OUT_FILENAMES_ONLY) printf("%s\n", filename);
    if (flags & GREP_COUNT_LINES) printf("%d:", line_count);
    if (start < finish && finish != 0) {
        printf("%.*s\n", (finish - start), (line + start));
    } else {
        printf("%s", line);
    }
}

int _grep_opts_set(int argc, char *argv[], grep_opts *opts, int *next) {
    // Read short and long options and set corresponding flags.
    // Returns the number of correctly proceeded arguments.
    int result_code = 0;
    int ch = 0;
    char *short_opts = "e:ivclnhsf:o";
    struct option long_opts[] = {{NULL, 0, NULL, 0}};

    while ((ch = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1 &&
           result_code == 0) {
        switch (ch) {
            case 'e':
                _util_opts_patterns_append(opts, optarg);
                break;
            case 'f':
                result_code = _get_reg_from_file(opts, argv[0], optarg);
                break;
            case 'i':
                opts->flags |= GREP_IGNORE_CASE;
                break;
            case 'l':
                opts->flags |= GREP_OUT_FILENAMES_ONLY;
                break;
            case 'v':
                opts->flags |= GREP_INVERT_MATCH;
                break;
            case 'c':
                opts->flags |= GREP_OUT_AMOUNT_ONLY;
                break;
            case 'n':
                opts->flags |= GREP_COUNT_LINES;
                break;
            case 'h':
                opts->flags |= GREP_SUPPRESS_FILENAMES;
                break;
            case 's':
                opts->flags |= GREP_SUPPRESS_ERRORS;
                break;
            case 'o':
                opts->flags |= GREP_OUT_STRING_MATCH;
                break;
            case '?':
            default:
                break;
        }
    }

    // Get pattern from string if it still empty and there is something to get.
    if (!(opts->flags & GREP_PATTER_WAS_READ) && optind < argc) {
        _util_opts_patterns_append(opts, argv[optind++]);
    }

    *next = optind;
    return result_code;
}

int _grep_re_prepare_and_compile(grep_opts *opts, regex_t *preg) {
    int re_compile_code = 0;
    int cflags = REG_EXTENDED | REG_NEWLINE;
    if (opts->flags & GREP_IGNORE_CASE) cflags |= REG_ICASE;
    if (opts->flags & GREP_OUT_AMOUNT_ONLY) cflags |= REG_NOSUB;

    re_compile_code = regcomp(preg, opts->patterns, cflags);
    return re_compile_code;
}

void _match_out_general(grep_opts *opts, regex_t *preg, char *filename,
                        FILE *file_in) {
    // Proccess all type of output except substring matching
    if (!(opts->flags & GREP_OUT_STRING_MATCH)) {
        char last_char = '\n';
        char *line = NULL;
        size_t volume = 0;
        ssize_t line_size;
        int lines_count = 0;
        int match_count = 0;
        int is_line_matched = 0;

        while ((line_size = getline(&line, &volume, file_in)) != -1) {
            int reg_match;

            lines_count++;
            reg_match = regexec(preg, line, 0, NULL, 0);
            is_line_matched =
                ((reg_match == 0 && (opts->flags & GREP_INVERT_MATCH) == 0) ||
                 ((reg_match != 0 && opts->flags & GREP_INVERT_MATCH)))
                    ? 1
                    : 0;

            if (is_line_matched) match_count++;
            if (is_line_matched && opts->flags & GREP_OUT_DEFAULT) {
                _output_match(opts->flags, filename, lines_count, line, 0, 0);
                last_char = line[line_size - 1];
            }
        }

        _output_newline_if_required(last_char);

        if (opts->flags & GREP_OUT_AMOUNT_ONLY) {
            char amount_buffer[100] = "";
            sprintf(amount_buffer, "%d\n", match_count);
            _output_match(opts->flags, filename, 0, amount_buffer, 0, 0);
        }

        if (match_count && opts->flags & GREP_OUT_FILENAMES_ONLY) {
            _output_match(opts->flags, filename, 0, "", 0, 0);
        }

        if (line != NULL) free(line);
    }
}

void _match_out_string_match(grep_opts *opts, regex_t *preg, char *filename,
                             FILE *file_in) {
    // Process -o flag is it set
    if (opts->flags & GREP_OUT_STRING_MATCH) {
        regmatch_t match[1];
        char *line = NULL;
        size_t volume = 0;
        int count = 0;

        while (getline(&line, &volume, file_in) != -1) {
            char *p = line;
            count++;
            while (regexec(preg, p, 1, match, 0) == 0) {
                int start = match[0].rm_so;
                int finish = match[0].rm_eo;
                _output_match(opts->flags, filename, count, p, start, finish);
                p = p + finish;
            }
        }
        if (line != NULL) free(line);
    }
}
