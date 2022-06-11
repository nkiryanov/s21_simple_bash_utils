# school 21 Simple Bash Utils 

Basicl implemetation of cat and grep bash utilities. \
The implementation is written on C. \
In case of different behavior on mac or lunux the linux behavior was choosen.

The library `regex.h` is used to parse regular expression.

The code parcialy covered with unit tests.
The integration tests is written as python script (look for tests/integration dirrectory)

## cat

`s21_cat [OPTION] [FILE]...`

### cat Options

| No. | Options | Description |
| ------ | ------ | ------ |
| 1 | -b (GNU: --number-nonblank) | numbers only non-empty lines |
| 2 | -e implies -v (GNU only: -E the same, but without implying -v) | but also display end-of-line characters as $  |
| 3 | -n (GNU: --number) | number all output lines |
| 4 | -s (GNU: --squeeze-blank) | squeeze multiple adjacent blank lines |
| 5 | -t implies -v (GNU: -T the same, but without implying -v) | but also display tabs as ^I  |


## s21_grep

`grep [options] template [file_name]`

### grep Options

| No. | Options | Description |
| ------ | ------ | ------ |
| 1 | -e | pattern |
| 2 | -i | Ignore uppercase vs. lowercase.  |
| 3 | -v | Invert match. |
| 4 | -c | Output count of matching lines only. |
| 5 | -l | Output matching files only.  |
| 6 | -n | Precede each matching line with a line number. |
| 7 | -h | Output matching lines without preceding them by file names. |
| 8 | -s | Suppress error messages about nonexistent or unreadable files. |
| 9 | -f file | Take regexes from a file. |
| 10 | -o | Output the matched parts of a matching line. |

## How to build

1. Change dirrectory to src
   ```bash
      cd src
   ```
2. Build executables
   ```
      make all
   ```

## How to test

1. Parcial unit tests
   - Run from src dirrectory
   ```
      make tests
   ```
2. Integration tests (should be run in Docker or ubuntu linux)
   - Start docker image
   ```bash
      docker-compose up -d
   ```
   - Login to docker image
   ```bash
      docker-compose exec tests fish
   ```
   - Change directory to utililiry subdirectory. e.g. grep
   ```bash
      cd src/grep/tests/integration
   ```
   - Run python script
   ```bash
      python3 integration_tests.py
   ```
