#ifndef LETREIRO_H
#define LETREIRO_H

#include <TM1637Display.h>

void standbyMessage(TM1637Display &display, const char* message);

uint8_t encodeChar(char c);

void standbyMessage(TM1637Display &display, const char* message) {
  int length = strlen(message);
  
  for (int i = 0; i < length - 3; i++) {
    display.setSegments(&encodeChar(message[i]), 1, 0);
    display.setSegments(&encodeChar(message[i+1]), 1, 1);
    display.setSegments(&encodeChar(message[i+2]), 1, 2);
    display.setSegments(&encodeChar(message[i+3]), 1, 3);
    delay(500);
  }
}

uint8_t encodeChar(char c) {
  switch (c) {
    case '0': return 0x3F;
    case '1': return 0x06;
    case '2': return 0x5B;
    case '3': return 0x4F;
    case '4': return 0x66;
    case '5': return 0x6D;
    case '6': return 0x7D;
    case '7': return 0x07;
    case '8': return 0x7F;
    case '9': return 0x6F;
    case 'a': return 0x77;
    case 'b': return 0x7C; 
    case 'c': return 0x39; 
    case 'd': return 0x5E; 
    case 'e': return 0x79;
    case 'f': return 0x71;
    case 'g': return 0x3D;
    case 'h': return 0x76;
    case 'i': return 0x30;
    case 'j': return 0x1E;
    case 'k': return 0x76;
    case 'l': return 0x38;
    case 'm': return 0x15;
    case 'n': return 0x37;
    case 'o': return 0x3F;
    case 'p': return 0x73;
    case 'q': return 0x67;
    case 'r': return 0x50;
    case 's': return 0x6D;
    case 't': return 0x78;
    case 'u': return 0x3E;
    case 'v': return 0x1C;
    case 'w': return 0x2A;
    case 'x': return 0x76;
    case 'y': return 0x6E;
    case 'z': return 0x5B;
    default: return 0x00;
  }
}

#endif
