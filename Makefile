NAME:=Stomach.a
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

SRCS:= Stomach.c
OBJS:= $(SRCS:.c=.o)

all: $(OBJS)
	@ar rcs $(NAME) $(OBJS) && echo "$(NAME) Compilation successful"

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean fclean re
