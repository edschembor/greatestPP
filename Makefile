# CFLAGS += -Wall -Werror -Wextra -pedantic
# CFLAGS += -Wmissing-prototypes
# CFLAGS += -Wstrict-prototypes
# CFLAGS += -Wmissing-declarations

all: example

example: example.cpp greatestpp.h example-suite.o
	${CC} -o $@ example.c example-suite.o ${CFLAGS} ${LDFLAGS}

clean:
	rm -f example *.o *.core

example-suite.o: example-suite.c
