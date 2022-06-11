# Simple integration test for s21_grep
# It requires s21_grep to be compiled and located at src/grep
import os
import subprocess
from pathlib import Path

CAT_ROOT_DIR = Path(__file__).resolve(strict=True).parent.parent.parent
INTEGRATION_TESTS_DIR = CAT_ROOT_DIR / "tests" / "integration"
TESTS_DATA = INTEGRATION_TESTS_DIR / "test_data"

S21_CAT = CAT_ROOT_DIR / "s21_cat"
STD_CAT = "cat"
LEAK_LOG = "leaks_log.log"

S21_TMP = INTEGRATION_TESTS_DIR / "s21_grep_tmp.txt"
STD_TMP = INTEGRATION_TESTS_DIR / "std_grep_tmp.txt"


def check_leaks_and_errors(s21_cmd, leaks, errors):
    # Run the self implemented function with valgring and look for leaks/error
    no_leaks_pattern = "All heap blocks were freed -- no leaks are possible"
    no_errors_pattern = "ERROR SUMMARY: 0 errors"

    with open(LEAK_LOG, "w+") as log:
        subprocess.run(
            [
                "valgrind",
                "--leak-check=summary",
                S21_CAT,
                ">",
                "/dev/null",
            ],
            stderr=log,
        )

    is_leaks = subprocess.run(
        ["egrep", no_leaks_pattern, LEAK_LOG],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    if is_leaks.returncode:
        print(f"Утечки при выполнении {s21_cmd}")
        leaks += is_leaks.returncode

    is_errors = subprocess.run(
        ["egrep", no_errors_pattern, LEAK_LOG],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    if is_errors.returncode:
        print(f"Ошибки при выполнении {s21_cmd}")
        leaks += is_leaks.returncode

    return leaks, errors


def main():
    test_files = [file for file in os.listdir(TESTS_DATA)]
    flags = (
        "",
        "-b",
        "-e",
        "-n",
        "-s",
        "-t",
        "-v",
        "-E",
        "-T",
        "--number-nonblank",
        "--number",
        "--squeeze-blank",
    )

    failed = 0
    success = 0
    leaks = 0
    errors = 0

    for file in test_files:
        print(f"Тест файла: {file}")
        for flag_1 in flags:
            print(f"Тестов завершено {failed + success}")
            for flag_2 in reversed(flags):
                file = TESTS_DATA / file
                cat_args = f"{flag_1} {flag_2}"
                s21_cmd = f"{S21_CAT} {cat_args} {file}"
                std_cmd = f"{STD_CAT} {cat_args} {file}"

                os.system(f"{s21_cmd} > {S21_TMP}")
                os.system(f"{std_cmd} > {STD_TMP}")

                file_diff = subprocess.run(
                    [
                        "diff",
                        "-q",
                        S21_TMP,
                        STD_TMP,
                    ]
                )

                if file_diff.returncode == 0:
                    success += 1
                else:
                    failed += 1
                    print(f"Разные результаты для {s21_cmd}")

                leaks, errors = check_leaks_and_errors(s21_cmd, leaks, errors)

    os.system(f"rm {S21_TMP} {STD_TMP} {LEAK_LOG}")

    print(f"Успешно тестов: {success}")
    print(f"Провалено тестов: {failed}")
    print(f"Утечек найдено: {leaks}")
    print(f"Ошибок найдено: {errors}")

if (__name__) == "__main__":
    main()