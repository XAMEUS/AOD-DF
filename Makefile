CC = gcc
CFLAGS = -Werror -std=c99

all: clean frechet

frechet:
	${CC} ${CFLAGS} bin/frechet.c -o bin/frechet

clean:
	$(RM) bin/frechet
