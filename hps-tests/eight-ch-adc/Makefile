
# Makefile based on Terasic Makefile for my_first_hps-fpga template project

TARGET = eight_ch_adc_test_new

CFLAGS = -static -g -Wall -I /home/debian/hwlib/include 
LDFLAGS = -g -Wall -ljson
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
