
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

// shutdown mode (datasheet table 3)
#define MAX7219_OFF             (0x0)
#define MAX7219_ON              (0x1)


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


void display(enum position pos, unsigned int number, unsigned char dp) {
    unsigned short i;
    unsigned int digitvalue;
    unsigned char offset = pos * 4;

    for (i = 0; i < 4; i++){
        digitvalue = number % 10;
        if ((digitvalue == 0) && (number == 0) && (i > 0)) {
            digitvalue |= 0b00001111;
        }
        else if (dp == i) {
            digitvalue |= 0b10000000;
        }
        set_register(MAX7219_DIGIT_REG(offset + i), digitvalue);
        number /= 10;
    }
}


void displayfloat(enum position pos, float v) {
    v *= 10;
    display(pos, (unsigned int)v, (unsigned char)(v > 10));
}

void max7219_init() {
    MAX7219_LOAD = 0;
    MAX7219_CLOCK = 0;
    MAX7219_DATA = 0;

    // disable test mode. datasheet table 10
    set_register(MAX7219_DISPLAYTEST_REG, MAX7219_OFF);
    // set medium intensity. datasheet table 7
    set_register(MAX7219_INTENSITY_REG, 0x2);
    // turn on display. datasheet table 3
    set_register(MAX7219_SHUTDOWN_REG, MAX7219_ON);
    // drive 8 digits. datasheet table 8
    set_register(MAX7219_SCANLIMIT_REG, 7);
    // decode mode for all positions. datasheet table 4
    set_register(MAX7219_DECODE_REG, 0b11111111);
}
