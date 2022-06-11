DEBUG =					NO
COVERAGE =				NO
ASAN =					NO
SUFFIX =
POSSIBLE_SUFFIXES =		_debug _coverage _asan

ifeq ($(DEBUG), YES)
	SUFFIX = 			_debug
	CFLAGS += 			-g
endif

ifeq ($(ASAN), YES)
	CFLAGS += 			-fsanitize=address -g
	LDFLAGS += 			-fsanitize=address
	LDLIBS += 			-static-libasan
	SUFFIX =			_asan
endif

CC := 					gcc
CFLAGS += 				-Wall -Werror -Wextra
LDFLAGS +=				-Wall -Werror -Wextra
LDLIBS +=

SRC_DIR :=				.
OBJ_DIR :=				$(SRC_DIR)/build
T_SRC_DIR :=			$(SRC_DIR)/tests
T_OBJ_DIR :=			$(OBJ_DIR)/tests

SRC :=					$(wildcard $(SRC_DIR)/*.c)
OBJ_ALL :=				$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%$(SUFFIX).o, $(SRC))
OBJ_MAIN :=				$(filter %_main$(SUFFIX).o, $(OBJ_ALL))
T_SRC :=				$(wildcard $(T_SRC_DIR)/*.c)
OBJ :=					$(filter-out $(OBJ_MAIN), $(OBJ_ALL))
T_OBJ :=				$(patsubst $(T_SRC_DIR)/%.c, $(T_OBJ_DIR)/%$(SUFFIX).o, $(T_SRC))


# Set this variables in sub makefile if need
# LIB_BASENAME :=
# EXEC_BASENAME :=
# TARGET_TYPE :=
# DEPENDENCIES :=

LIB := $(patsubst %, lib%$(SUFFIX).a, $(LIB_BASENAME))
EXEC := $(addsuffix $(SUFFIX), $(EXEC_BASENAME))

ifeq ($(TARGET_TYPE), lib)
	TARGET := $(LIB)
else ifeq ($(TARGET_TYPE), exec)
	TARGET := $(EXEC)
endif

T_EXEC_BASENAME := tests_runner
T_EXEC := $(addsuffix $(SUFFIX), $(T_EXEC_BASENAME))

POSSIBLE_LIBS = 		$(patsubst %, lib$(LIB_BASENAME)%.a, $(POSSIBLE_SUFFIXES))
POSSIBLE_EXECS = 		$(addprefix $(EXEC_BASENAME), $(POSSIBLE_SUFFIXES))
POSSIBLE_T_EXECS = 		$(addprefix $(T_EXEC_BASENAME), $(POSSIBLE_SUFFIXES))

.PHONY: all clean echo tests cpplint dependencies
.SILENT: all

all: $(TARGET)

dependencies:
	$(DEPENDENCIES)

tests: $(T_EXEC)
	@./$^

echo:
	@echo "\t" DEBUG=$(DEBUG)
	@echo "\t" COVERAGE=$(COVERAGE)
	@echo "\t" CC=$(CC)
	@echo "\t" CFLAGS=$(CFLAGS)
	@echo "\t" LDFLAGS=$(LDFLAGS)
	@echo "\t" SRC_DIR=$(SRC_DIR)
	@echo "\t" OBJ_DIR=$(OBJ_DIR)
	@echo "\t" SRC: $(SRC)
	@echo "\t" OBJ_ALL: $(OBJ_ALL)
	@echo "\t" OBJ_MAIN: $(OBJ_MAIN)
	@echo "\t" OBJ: $(OBJ)
	@echo "\t" LIB: $(LIB)
	@echo "\t" EXEC: $(EXEC)
	@echo "\t" TARGET: $(TARGET)
	@echo "\t" POSSIBLE_LIBS: $(POSSIBLE_LIBS)
	@echo "\t" POSSIBLE_EXECS: $(POSSIBLE_EXECS)
	@echo "\t" POSSIBLE_T_EXECS: $(POSSIBLE_T_EXECS)
	@echo "\t" DEPENDENCIES: $(DEPENDENCIES)

$(EXEC): dependencies | $(OBJ) $(OBJ_MAIN)
	$(CC) $| $(LDFLAGS) -o $@ $(LDLIBS)

$(LIB): $(OBJ)
	$(AR) -rcs $@ $^

$(T_OBJ_DIR)/%$(SUFFIX).o: $(T_SRC_DIR)/%.c | $(T_OBJ_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJ_DIR)/%$(SUFFIX).o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJ_DIR) $(T_OBJ_DIR):
	mkdir -p $@

$(T_EXEC): CFLAGS += $(shell pkg-config --cflags-only-I check)
$(T_EXEC): LDLIBS += $(shell pkg-config --libs check)
$(T_EXEC): $(OBJ) $(T_OBJ) | $(OBJ_DIR) $(T_OBJ_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -rf $(OBJ_DIR) $(LIB) $(EXEC) $(T_EXEC) \
	$(POSSIBLE_LIBS) $(POSSIBLE_EXECS) $(POSSIBLE_T_EXECS)

cpplint:
	cp ../materials/linters/CPPLINT.cfg .
	python3 ../materials/linters/cpplint.py --extension=c \
	./*.c ./*/*.c ./*/*/*.c ./*.h ./*/*.h ./*/*/*.h
	rm CPPLINT.cfg
