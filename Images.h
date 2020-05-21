const uint8_t activeSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00011000,
    B00100100,
    B01000010,
    B01000010,
    B00100100,
    B00011000
};

const uint8_t inactiveSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000
};

const uint8_t satelite[] PROGMEM = {
  B00000000,
  B11100000,
  B11100100,
  B01111000,
  B00111000,
  B00111100,
  B01001110,
  B00001110
};

uint8_t locW = 7, locH = 8;
const uint8_t loc[] PROGMEM = {
  B0011100,
  B0111110,
  B0110110,
  B0111110,
  B0011100,
  B0011100,
  B0001000,
  B0001000
};

const uint8_t size_micro[] PROGMEM = {
  B0000000,
  B0000000,
  B0000000,
  B0000000,
  B0000000,
  B0000000,
  B0001000,
  B0000000
};

const uint8_t size_small[] PROGMEM = {
  B0000000,
  B0000000,
  B0000000,
  B0000000,
  B0000000,
  B0011000,
  B0011000,
  B0000000
};

const uint8_t size_regular[] PROGMEM = {
  B0000000,
  B0000000,
  B0000000,
  B0000000,
  B0111100,
  B0111100,
  B0111100,
  B0000000
};

const uint8_t size_large[] PROGMEM = {
  B0000000,
  B0000000,
  B1111111,
  B1111111,
  B1111111,
  B1111111,
  B1111111,
  B0000000
};

const uint8_t size_other[] PROGMEM = {
  B1111111,
  B1000001,
  B1001001,
  B1010101,
  B1000101,
  B1001001,
  B1001001,
  B1111111
};

const uint8_t *sizes[] = {size_micro, size_small, size_regular, size_large, size_other};
