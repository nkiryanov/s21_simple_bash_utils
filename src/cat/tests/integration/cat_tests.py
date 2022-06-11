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

S21_TMP = INTEGRATION_TESTS_DIR / "s21_grep_tmp.txt"
STD_TMP = INTEGRATION_TESTS_DIR / "std_grep_tmp.txt"

test_files = [file for file in os.listdir(TESTS_DATA)]
flags = (
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

for file in test_files:
    print(f"Тест файла: {file}")
    for flag_1 in flags:
        print(f"Тестов завершено {failed + success}")
        for flag_2 in reversed(flags):
            file = TESTS_DATA / file
            cat_args = f"{flag_1} {flag_2}"
            s21_cat_cmd = f"{S21_CAT} {cat_args} {file}"
            std_cat_cmd = f"{STD_CAT} {cat_args} {file}"

            os.system(f"{s21_cat_cmd} > {S21_TMP}")
            os.system(f"{std_cat_cmd} > {STD_TMP}")

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
                print(f"Разные результаты для {s21_cat_cmd}")


os.system(f"rm {S21_TMP} {STD_TMP}")

print(f"Успешно тестов: {success}")
print(f"Провалено тестов: {failed}")
