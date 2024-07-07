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
	@$(CC) $(CFLAGS) $(LIBSTOMACH) math_expr.c $(LDFLAGS) -o math_expr && echo "Compilation successful"

$(LIBSTOMACH):
	$(MAKE) -C ..

