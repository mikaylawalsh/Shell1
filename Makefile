CFLAGS = -g3 -Wall -Wextra -Wconversion -Wcast-qual -Wcast-align -g
CFLAGS += -Winline -Wfloat-equal -Wnested-externs
CFLAGS += -pedantic -std=gnu99 -Werror

PROMPT = -DPROMPT

CC = gcc 

EXECS = 33sh 33noprompt

.PHONEY: all clean

all: $(EXECS)

33sh: sh.c
	$(CC) $(CFLAGS) $(PROMPT) $^ -o $@

33noprompt: sh.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(EXECS)

