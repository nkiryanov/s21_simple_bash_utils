#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

char *get_basename(char *filename) {
    // Return pointer to file basename.
    // Takes relative or absolute filename and move pointer to basename start.
    char *basename = filename;

    if (basename != NULL) {
        basename = strrchr(filename, '/');
        basename = (basename != NULL) ? (basename + 1) : filename;
    }

    return basename;
}

int fopen_or_error(char *filename, FILE **file_ptr) {
    // Try to open a file with a give filename. If any error occurs return err.
    struct stat buffer;
    int status;
    int error_num = ENOENT;
    *file_ptr = NULL;

    status = stat(filename, &buffer);

    // Change the error for known errors.
    if (status == 0 && S_ISDIR(buffer.st_mode)) error_num = EISDIR;

    if (status == 0 && S_ISREG(buffer.st_mode)) {
        *file_ptr = fopen(filename, "r");
        if (*file_ptr != NULL) error_num = 0;
    }

    return error_num;
}

void output_error_file_read(char *program_name, char *filename, int err_num) {
    // Output formatted error of reading file.
    // The format:
    //     `program_basename: filename: Error message`
    char *basename, *description;

    basename = get_basename(program_name);
    description = strerror(err_num);

    fprintf(stderr, "%s: %s: %s\n", basename, filename, description);
}
