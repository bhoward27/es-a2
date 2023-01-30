OUT_DIR = $(HOME)/cmpt433/public/myApps
LOG_LEVEL_FILE=$(OUT_DIR)/logLevel.txt

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -std=gnu11 -D _POSIX_C_SOURCE=200809L -Og -Wall -Werror -Wshadow
OFLAGS = $(CFLAGS) -c

OBJS = main.o utils.o log.o adc.o

all: light_sampler

light_sampler: $(OBJS)
	$(CC_C) $(CFLAGS) $(OBJS) -o $(OUT_DIR)/light_sampler

# Makes new log level file if it doesn't exist.
	./makeLogLevelFile.sh $(LOG_LEVEL_FILE)

main.o: main.c adc.o utils.o
	$(CC_C) $(OFLAGS) main.c

utils.o: utils.c utils.h int_typedefs.h return_val.h log.o
	$(CC_C) $(OFLAGS) utils.c

log.o: log.c log.h return_val.h
	$(CC_C) $(OFLAGS) log.c

adc.o: adc.c adc.h int_typedefs.h utils.o log.o
	$(CC_C) $(OFLAGS) adc.c

clean:
	rm -f $(OUT_DIR)/light_sampler
	rm -f *.o *.s *.out

clean-log:
	rm -f $(LOG_LEVEL_FILE)