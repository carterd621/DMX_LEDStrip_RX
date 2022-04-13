#define NUM_LEDS 50
#define SL 0 //defines starting LED
#define EL NUM_LEDS - 1 //defines ending LED
#define WAIT_TIME (-1.0 / 25.0 * NUM_LEDS) + 7

#define DATA_PIN 5
#define DBLED_PIN 13
#define ADRUP_PIN 2
#define ADRDN_PIN 8
#define MODE_PIN 3
#define RAND_PIN A3

#define AD_MEM 0x00 //address in EEPROM for use of address