#ifndef SRC_CAT_S21_CAT_INTERNAL_H_
#define SRC_CAT_S21_CAT_INTERNAL_H_

#include <stdio.h>

enum CAT_FLAGS_BIT {
    NUM_NON_BLANK = 01,
    NUM_ALL = 02,
    PRINT_NON_PRINTS = 04,
    PRINT_END_LINES = 010,
    PRINT_TABS = 020,
    SQUEEZE_BLANK = 040
};

// Cat internals

int _set_options(int argc, char *argv[], int *flags, int *next);
int _turn_off_incompatible_flags(int flags);
void _update_cache(int c, char cache[2]);
void _process_and_output(FILE *file_in, FILE *file_out, int flags);

#endif  // SRC_CAT_S21_CAT_INTERNAL_H_
