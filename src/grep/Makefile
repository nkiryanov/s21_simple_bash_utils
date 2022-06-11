LIB_BASENAME :=		s21_grep
EXEC_BASENAME :=	s21_grep
TARGET_TYPE := 		exec
DEPENDENCIES :=		$(MAKE) -C ../common all

CFLAGS =			-I ../common/
LDFLAGS =			-L ../common/
LDLIBS = 			-ls21_common$(SUFFIX)

include ../submake.mk
