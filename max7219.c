
//
// Use one MAX7219 to control 8-digit seven-segment display
// the display module: http://www.icshop.com.tw/product_info.php/products_id/20686
//
// MAX7219 datasheet: https://datasheets.maximintegrated.com/en/ds/MAX7219-MAX7221.pdf
// 

// the MAX7219 address map (datasheet table 2)
#define MAX7219_DECODE_REG      (0x09)
#define MAX7219_INTENSITY_REG   (0x0A)
#define MAX7219_SCANLIMIT_REG   (0x0B)
#define MAX7219_SHUTDOWN_REG    (0X0C)
#define MAX7219_DISPLAYTEST_REG (0x0F)
#define MAX7219_DIGIT_REG(pos)  ((pos) + 1)


#define MAX7219_DATA   GP0
#define MAX7219_LOAD   GP5
#define MAX7219_CLOCK  GP1


enum position {
    right,
    left
};


void shiftout (unsigned char data) {
	int i;
	for (i = 0; i < 8; i++) {
        MAX7219_DATA = !!(data & (1 << (7 - i)));
        MAX7219_CLOCK = 1;
        MAX7219_CLOCK = 0;
	}
}


// update the register value of MAX7219
void set_register(unsigned char address, unsigned char value) {
    MAX7219_LOAD = 0;
    shiftout(address);
    shiftout(value);
    MAX7219_LOAD = 1;
}

#define DONE    0b01
#define NEG     0b10


void display(enum position pos, int number, unsigned char dp) {
    unsigned char i;
    unsigned char digitvalue;
    unsigned char offset = pos * 4;
    unsigned char stat = 0b00;
    if (number < 0) {
        stat |= NEG;
        number = -number;
    }

    for (i = 0; i < 4; i++){
        digitvalue = (unsigned char)(number % 10);
        if ((stat & DONE) == DONE) {
            if ((stat & NEG) == NEG) {
                digitvalue |= 0b00001010;
                stat &= DONE;
            }
            else {
                digitvalue |= 0b00001111;
            }
        }
        else if ((number < 10) && (i > 0)) {
            stat |= DONE;
        }

        if (dp == i) {
            digitvalue |= 0b10000000;
        }

        set_register((unsigned char)MAX7219_DIGIT_REG(offset + i), digitvalue);
        number /= 10;
    }
}


void displayfloat(enum position pos, float v) {
    v *= 10;
    display(pos, (int)v, 1);
}

void max7219_init() {
    MAX7219_LOAD = 0;
    MAX7219_CLOCK = 0;
    MAX7219_DATA = 0;

    // disable test mode. datasheet table 10
    set_register(MAX7219_DISPLAYTEST_REG, 0);
    // set medium intensity. datasheet table 7
    set_register(MAX7219_INTENSITY_REG, 2);
    // turn on display. datasheet table 3
    set_register(MAX7219_SHUTDOWN_REG, 1);
    // drive 8 digits. datasheet table 8
    set_register(MAX7219_SCANLIMIT_REG, 7);
    // decode mode for all positions. datasheet table 4
    set_register(MAX7219_DECODE_REG, 0b11111111);
}
