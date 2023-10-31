// [ ===== Created using duckify.huhn.me @ dev aadd7808 ===== ] //
// Duckify is a free and Open-Source web-app for converting Ducky Script into Arduino code.
// Please consider supporting us on ko-fi.com/spacehuhn <3
// Learn more about us at spacehuhn.com

// Platform: Digispark
// Keyboard Layout: DE

#include "DigiKeyboard.h"

// https://www.youtube.com/watch?v=dQw4w9WgXcQ
const uint8_t key_arr_0[] PROGMEM = {0,11, 0,23, 0,23, 0,19, 0,22, 2,55, 2,36, 2,36, 0,26, 0,26, 0,26, 0,55, 0,29, 0,18, 0,24, 0,23, 0,24, 0,5, 0,8, 0,55, 0,6, 0,18, 0,16, 2,36, 0,26, 0,4, 0,23, 0,6, 0,11, 2,45, 0,25, 2,39, 0,7, 2,20, 0,26, 0,33, 0,26, 0,38, 2,26, 0,10, 2,27, 0,6, 2,20};

void duckyString(const uint8_t* keys, size_t len) {  
    for(size_t i=0; i<len; i+=2) {
        DigiKeyboard.sendKeyStroke(pgm_read_byte_near(keys + i+1), pgm_read_byte_near(keys + i));
    }
}

void setup() {
    pinMode(1, OUTPUT); // Enable LED
    digitalWrite(1, LOW); // Turn LED off
    DigiKeyboard.sendKeyStroke(0); // Tell computer no key is pressed

    DigiKeyboard.delay(2000); // DELAY 2000
    DigiKeyboard.sendKeyStroke(21, 8); // GUI r
    DigiKeyboard.delay(200); // DELAY 200
    duckyString(key_arr_0, sizeof(key_arr_0)); // STRING https://www.youtube.com/watch?v=dQw4w9W...
    DigiKeyboard.sendKeyStroke(40, 0); // ENTER
    DigiKeyboard.delay(5000); // DELAY 5000
    DigiKeyboard.sendKeyStroke(44, 0); // SPACE
    DigiKeyboard.delay(1000); // DELAY 1000
    for(size_t i=0; i<10; ++i) {
    DigiKeyboard.delay(200); // DELAY 200
    DigiKeyboard.sendKeyStroke(82, 0); // UPARROW
    }
    DigiKeyboard.delay(30000); // DELAY 30000
    DigiKeyboard.sendKeyStroke(7, 8); // GUI d
    DigiKeyboard.delay(200); // DELAY 200
    DigiKeyboard.sendKeyStroke(61, 4); // ALT F4
    DigiKeyboard.delay(200); // DELAY 200
    DigiKeyboard.sendKeyStroke(82, 0); // UP
    DigiKeyboard.sendKeyStroke(82, 0); // UP
    DigiKeyboard.sendKeyStroke(40, 0); // ENTER
}

void loop() {}

// Created using duckify.huhn.me @ dev aadd7808