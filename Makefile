
CC = xc8-cc
CHIP = 12f675
PG = ardpicprog
DIST = ./out
OSCCAL = 30 
CCARGS = -ansi -mc90lib -O2 -fasmfile -fshort-float 

# Use to prevent to override osccal
#-mno-osccal

all: main

clean: 
	rm $(DIST)/*

main: main.c max7219.c
	$(CC) $(CCARGS) -moscval=$(OSCCAL) -mcpu=$(CHIP) -o $(DIST)/main.hex main.c

program: main
	$(PG) --quiet --pic-serial-port /dev/ttyUSB0 --erase --burn --force-calibration --input-hexfile $(DIST)/main.hex

p:
	$(PG) --quiet --pic-serial-port /dev/ttyUSB0 --erase --burn --force-calibration --input-hexfile OUTPUT.hex 

read:
	$(PG) --quiet --pic-serial-port /dev/ttyUSB0 --ihx8m --output-hexfile OUTPUT 


.PHONY: program clean
