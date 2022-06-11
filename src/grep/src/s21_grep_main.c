#include <regex.h>
#include <s21_common.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_grep_internal.h"

int main(int argc, char *argv[]) {
    grep_opts opts = GREP_EMPTY_OPTS;
    regex_t preg;
    int file_index;
    int main_exit_code;

    main_exit_code = _grep_opts_set(argc, argv, &opts, &file_index);
    opts.flags = _util_set_compatible_flags(opts.flags, argc - file_index);

    if (main_exit_code == 0) {
        main_exit_code = _grep_re_prepare_and_compile(&opts, &preg);
    }

    if (main_exit_code == 0) {
        while (file_index++ < argc) {
            FILE *file = NULL;
            char *filename = argv[file_index - 1];
            int read_code = fopen_or_error(filename, &file);

            if (read_code != 0) {
                _output_file_read_error(argv[0], filename, read_code, &opts);
                main_exit_code = 1;
            }

            if (read_code == 0) {
                _match_out_general(&opts, &preg, filename, file);
                _match_out_string_match(&opts, &preg, filename, file);
                fclose(file);
            }
        }
    }

    regfree(&preg);
    return main_exit_code;
}
