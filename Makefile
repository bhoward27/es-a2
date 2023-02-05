OUT_DIR = $(HOME)/cmpt433/public/myApps
LOG_LEVEL_FILE=$(OUT_DIR)/logLevel.txt

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
# Use -g, not -Og
# Just keep it at -g since will need to run valgrind during demo.
CFLAGS = -std=gnu11 -D _POSIX_C_SOURCE=200809L -pthread -g -Wall -Werror -Wshadow
OFLAGS = $(CFLAGS) -c

OBJS = main.o utils.o log.o adc.o i2c.o gpio.o digit_display.o adc_buffer.o light_meter.o potentiometer.o adc_stats.o console.o light_sampler.o shutdown.o periodTimer.o

all: light_sampler

light_sampler: $(OBJS)
	$(CC_C) $(CFLAGS) $(OBJS) -o $(OUT_DIR)/light_sampler

# Makes new log level file if it doesn't exist.
	./makeLogLevelFile.sh $(LOG_LEVEL_FILE)

main.o: main.c return_val.h adc.o utils.o gpio.o i2c.o log.o digit_display.o adc_buffer.o periodTimer.o console.o light_sampler.o shutdown.o
	$(CC_C) $(OFLAGS) main.c

utils.o: utils.c utils.h int_typedefs.h return_val.h log.o
	$(CC_C) $(OFLAGS) utils.c

log.o: log.c log.h return_val.h int_typedefs.h
	$(CC_C) $(OFLAGS) log.c

adc.o: adc.c adc.h int_typedefs.h utils.o log.o
	$(CC_C) $(OFLAGS) adc.c

i2c.o: i2c.c i2c.h int_typedefs.h utils.o log.o gpio.o
	$(CC_C) $(OFLAGS) i2c.c

gpio.o: gpio.c gpio.h return_val.h int_typedefs.h utils.o log.o
	$(CC_C) $(OFLAGS) gpio.c

digit_display.o: digit_display.c digit_display.h i2c.o utils.o log.o gpio.o
	$(CC_C) $(OFLAGS) digit_display.c

adc_buffer.o: adc_buffer.c adc_buffer.h adc.o log.o
	$(CC_C) $(OFLAGS) adc_buffer.c

light_meter.o: light_meter.c light_meter.h adc.o
	$(CC_C) $(OFLAGS) light_meter.c

potentiometer.o: potentiometer.c potentiometer.h adc.o
	$(CC_C) $(OFLAGS) potentiometer.c

adc_stats.o: adc_stats.c adc_stats.h adc.o
	$(CC_C) $(OFLAGS) adc_stats.c

console.o: console.c console.h shutdown.o utils.o log.o potentiometer.o adc_stats.o periodTimer.o adc_buffer.o
	$(CC_C) $(OFLAGS) console.c

light_sampler.o: light_sampler.c light_sampler.h light_meter.o shutdown.o log.o utils.o periodTimer.o adc_buffer.o
	$(CC_C) $(OFLAGS) light_sampler.c

shutdown.o: shutdown.c shutdown.h
	$(CC_C) $(OFLAGS) shutdown.c

periodTimer.o: periodTimer.c periodTimer.h
	$(CC_C) $(OFLAGS) periodTimer.c

clean:
	rm -f $(OUT_DIR)/light_sampler
	rm -f *.o *.s *.out

clean-log:
	rm -f $(LOG_LEVEL_FILE)
