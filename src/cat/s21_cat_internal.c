#include "s21_cat_internal.h"

#include <errno.h>
#include <getopt.h>
#include <s21_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int _set_options(int argc, char *argv[], int *flags, int *next) {
    // Read short and long options and set corresponding flags.
    // Write to `next` the index of the next argv argument.
    // Return non zero result (1) code if something went wrong. The execution
    // must stop with first error.

    int ch = 0;
    int result_code = 0;
    char *short_opts = "bevEnstT";
    struct option long_opts[] = {{"number-nonblank", no_argument, NULL, 'b'},
                                 {"number", no_argument, NULL, 'n'},
                                 {"squeeze-blank", no_argument, NULL, 's'},
                                 {NULL, 0, NULL, 0}};

    while ((ch = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1 &&
           result_code == 0) {
        switch (ch) {
            case 'b':
                *flags |= NUM_NON_BLANK;
                break;
            case 'e':
                *flags |= PRINT_NON_PRINTS | PRINT_END_LINES;
                break;
            case 'E':
                *flags |= PRINT_END_LINES;
                break;
            case 'n':
                *flags |= NUM_ALL;
                break;
            case 'v':
                *flags |= PRINT_NON_PRINTS;
                break;
            case 's':
                *flags |= SQUEEZE_BLANK;
                break;
            case 't':
                *flags |= PRINT_NON_PRINTS | PRINT_TABS;
                break;
            case 'T':
                *flags |= PRINT_TABS;
                break;
            case '?':
            default:
                result_code = 1;
                break;
        }
    }

    *next = optind;
    return result_code;
}

int _turn_off_incompatible_flags(int flags) {
    // Num all lines and num only not empty are incompatible.
    // Turn off NUM_ALL if NUM_NON_BLANK is on.
    if (flags & NUM_NON_BLANK && flags & NUM_ALL) flags &= ~NUM_ALL;
    return flags;
}

void _update_cache(int c, char cache[2]) {
    cache[0] = cache[1];
    cache[1] = (char)c;
}

void _process_and_output(FILE *file_in, FILE *file_out, int flags) {
    int count = 0;
    int c;
    char cache[2] = {'\0', '\n'};

    while ((c = fgetc(file_in)) != EOF) {
        if (flags & SQUEEZE_BLANK && c == '\n' && cache[0] == '\n' &&
            cache[1] == '\n') {
            _update_cache(c, cache);
            continue;
        }

        if (cache[1] == '\n') {
            if (flags & NUM_ALL || (flags & NUM_NON_BLANK && c != '\n')) {
                fprintf(file_out, "%6d\t", ++count);
            }
        }

        if (flags & PRINT_NON_PRINTS) {
            if (c >= 128) {
                fprintf(file_out, "M-");
                c -= 128;
            }
            if (c < ' ' && c != '\n' && c != '\t') {
                c += '@';
                putc('^', file_out);
            } else if (c == 127) {
                c -= '@';
                putc('^', file_out);
            }
        }

        if (flags & PRINT_END_LINES && c == '\n') putc('$', file_out);
        if (flags & PRINT_TABS && c == '\t') putc('^', file_out), c = 'I';

        putc(c, file_out);
        _update_cache(c, cache);
    }
}
