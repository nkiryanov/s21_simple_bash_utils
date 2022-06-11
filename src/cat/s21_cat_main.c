#include <s21_common.h>
#include <stdio.h>

#include "s21_cat_internal.h"

int main(int argc, char *argv[]) {
    int main_code = 0;
    int flags = 0;
    int non_opts_index;

    main_code = _set_options(argc, argv, &flags, &non_opts_index);

    if (main_code == 0 && non_opts_index < argc) {
        // Proceed output for files

        flags = _turn_off_incompatible_flags(flags);

        while (non_opts_index++ < argc) {
            FILE *file = NULL;
            int file_read_code;
            char *filename = argv[non_opts_index - 1];

            file_read_code = fopen_or_error(filename, &file);

            if (file_read_code != 0) {
                output_error_file_read(argv[0], filename, file_read_code);
                main_code = 1;
            }

            if (file_read_code == 0) {
                _process_and_output(file, stdout, flags);
                fclose(file);
            }
        }
    } else if (main_code == 0 && non_opts_index == argc) {
        // Proceed output for stdin
        _process_and_output(stdin, stdout, flags);
    }

    return main_code;
}
