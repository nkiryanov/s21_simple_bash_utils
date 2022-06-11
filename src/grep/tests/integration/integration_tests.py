# Simple integration test for s21_grep
# It requires s21_grep to be compiled and located at src/grep
import os
import subprocess
from pathlib import Path

GREP_ROOT_DIR = Path(__file__).resolve(strict=True).parent.parent.parent
INTEGRATION_TESTS_DIR = GREP_ROOT_DIR / "tests" / "integration"
TESTS_DATA = INTEGRATION_TESTS_DIR / "test_data"
FILE_PATTERNS = INTEGRATION_TESTS_DIR / "patterns.txt"

S21_GREP = GREP_ROOT_DIR / "s21_grep"
STD_GREP = "grep"

S21_TMP = INTEGRATION_TESTS_DIR / "s21_grep_tmp.txt"
STD_TMP = INTEGRATION_TESTS_DIR / "std_grep_tmp.txt"
LEAK_LOG = "leak_log.log"


def compare_with_system_implementation(s21_cmd, std_cmd, success, failed):
    # Compare the result of the self written implementations with system one

    os.system(f"{s21_cmd} > {S21_TMP}")
    os.system(f"{std_cmd} > {STD_TMP}")

    diff = subprocess.run(
        [
            "diff",
            "-q",
            S21_TMP,
            STD_TMP,
        ]
    )

    if diff.returncode == 0:
        success += 1
    else:
        failed += 1
        print(f"Разные результаты для {s21_cmd}")

    return success, failed


def check_leaks_and_errors(s21_cmd, leaks, errors):
    # Run the self implemented function with valgring and look for leaks/error
    no_leaks_pattern = "All heap blocks were freed -- no leaks are possible"
    no_errors_pattern = "ERROR SUMMARY: 0 errors"

    with open(LEAK_LOG, "w+") as log:
        subprocess.run(
            [
                "valgrind",
                "--leak-check=summary",
                S21_GREP,
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
    failed = 0
    success = 0
    leaks = 0
    errors = 0

    test_files = [file for file in os.listdir(TESTS_DATA)]
    flags = ("", "-v", "-c", "-l", "-n", "-h", "-s", "-o", "-i")
    patterns = (".", "b", "include", "[0-9]", "-e .", f"-f {FILE_PATTERNS}")

    for file in test_files:
        print(f"Тест файла: {file}")
        for flag_1 in flags:
            print(f"Тестов завершено {failed + success}")
            for flag_2 in reversed(flags):
                for pattern in patterns:
                    file = TESTS_DATA / file
                    grep_args = f"{flag_1} {flag_2} {pattern}"
                    s21_cmd = f"{S21_GREP} {grep_args} {file}"
                    std_cmd = f"{STD_GREP} {grep_args} {file}"

                    success, failed = compare_with_system_implementation(
                        s21_cmd, std_cmd, success, failed
                    )
                    leaks, errors = check_leaks_and_errors(
                        s21_cmd, leaks, errors
                    )

    os.system(f"rm {S21_TMP} {STD_TMP} {LEAK_LOG}")

    print(f"Успешно тестов: {success}")
    print(f"Провалено тестов: {failed}")
    print(f"Утечек найдено: {leaks}")
    print(f"Ошибок найдено: {errors}")


if (__name__) == "__main__":
    main()
