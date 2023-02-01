OUT_DIR = $(HOME)/cmpt433/public/myApps
LOG_LEVEL_FILE=$(OUT_DIR)/logLevel.txt

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -std=gnu11 -D _POSIX_C_SOURCE=200809L -Og -Wall -Werror -Wshadow
OFLAGS = $(CFLAGS) -c

OBJS = main.o utils.o log.o adc.o i2c.o gpio.o digit_display.o

all: light_sampler

light_sampler: $(OBJS)
	$(CC_C) $(CFLAGS) $(OBJS) -o $(OUT_DIR)/light_sampler

# Makes new log level file if it doesn't exist.
	./makeLogLevelFile.sh $(LOG_LEVEL_FILE)

main.o: main.c return_val.h adc.o utils.o gpio.o i2c.o log.o digit_display.o
	$(CC_C) $(OFLAGS) main.c

utils.o: utils.c utils.h int_typedefs.h return_val.h log.o
	$(CC_C) $(OFLAGS) utils.c

log.o: log.c log.h return_val.h
	$(CC_C) $(OFLAGS) log.c

adc.o: adc.c adc.h int_typedefs.h utils.o log.o
	$(CC_C) $(OFLAGS) adc.c

i2c.o: i2c.c i2c.h
	$(CC_C) $(OFLAGS) i2c.c

gpio.o: gpio.c gpio.h return_val.h int_typedefs.h utils.o log.o
	$(CC_C) $(OFLAGS) gpio.c

digit_display.o: digit_display.c digit_display.h i2c.o utils.o
	$(CC_C) $(OFLAGS) digit_display.c


clean:
	rm -f $(OUT_DIR)/light_sampler
	rm -f *.o *.s *.out

clean-log:
	rm -f $(LOG_LEVEL_FILE)