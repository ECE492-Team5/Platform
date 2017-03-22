
# Makefile based on Terasic Makefile for my_first_hps-fpga template project
# Modified By: Satyen Akolkar
# March 22 2017

TARGET = eight_ch_adc_test

CFLAGS = -static -g -Wall
LDFLAGS = -g -Wall
CC = gcc
ARCH = arm

build: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) *.a *.o *~
