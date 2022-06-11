COVERAGE_FILE :=		coverage.txt
COVERAGE_REPORT_DIR :=	coverage


.PHONY: all tests clean cpplint echo\
		s21_common tests_s21_common \
		s21_cat tests_s21_cat \
		s21_grep tests_s21_grep \

all: s21_common s21_cat s21_grep

tests: tests_s21_common tests_s21_cat tests_s21_grep

cpplint:
	cp ../materials/linters/CPPLINT.cfg .
	python3 ../materials/linters/cpplint.py --extension=c \
	./*.c ./*/*.c ./*/*/*.c ./*.h ./*/*.h ./*/*/*.h
	rm CPPLINT.cfg

echo:
	@echo Echo varibles in submakefile:
	@echo
	@echo s21_cat:
	@$(MAKE) -C cat echo
	@echo
	@echo s21_common:
	@$(MAKE) -C common echo

s21_common:
	@$(MAKE) -C common all

tests_s21_common: s21_common
	@$(MAKE) -C common tests

s21_cat: s21_common
	@$(MAKE) -C cat all

tests_s21_cat: s21_cat
	@$(MAKE) -C cat tests

s21_grep: s21_common
	@$(MAKE) -C grep all

tests_s21_grep: s21_grep
	@$(MAKE) -C grep tests


gcov_report: SUFFIX :=		_coverage
gcov_report: LDFLAGS += 	-fprofile-arcs -ftest-coverage
gcov_report: CFLAGS += 		-fprofile-arcs -ftest-coverage
gcov_report: tests
	lcov --capture --directory . --output-file $(COVERAGE_FILE)
	genhtml $(COVERAGE_FILE) --output-directory $(COVERAGE_REPORT_DIR)

clean:
	$(MAKE) -C cat clean
	$(MAKE) -C grep clean
	$(MAKE) -C common clean
