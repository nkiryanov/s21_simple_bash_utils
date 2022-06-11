#ifndef SRC_COMMON_S21_COMMON_H_
#define SRC_COMMON_S21_COMMON_H_

#include <stdio.h>

char *get_basename(char *argv_0);
int fopen_or_error(char *filename, FILE **file_ptr);
void output_error_file_read(char *program_name, char *filename, int err_num);

#endif  // SRC_COMMON_S21_COMMON_H_
