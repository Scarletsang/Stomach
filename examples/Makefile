CC:=clang
CFLAGS:= -pedantic -Wall -Wextra -Werror
LDFLAGS= -pedantic

ifdef DEBUG
	CFLAGS += -g
	LDFLAGS += -g
endif

ifdef	FSANITIZE
	CFLAGS += -g -fsnaitize=address
	LDFLAGS += -g -fsanitize=address
endif

LIBSTOMACH:=../stomach.a
LDFLAGS += -I ..

math_expr: $(LIBSTOMACH) math_expr.c
	@$(CC) $(CFLAGS) $(LIBSTOMACH) math_expr.c $(LDFLAGS) -o math_expr.out && echo "math_expr.out Compilation successful"

$(LIBSTOMACH):
	@$(MAKE) $(if $(DEBUG),DEBUG=1,) $(if $(FSANITIZE),FSANITIZE=1,) -C ..

.PHONY: $(LIBSTOMACH)
