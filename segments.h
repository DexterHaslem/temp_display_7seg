#ifndef SEGMENTS_H
#define	SEGMENTS_H

#include <stdint.h>

/* PORTC bits mapped to segs
  dp G F E D C B A
 */
const uint8_t seg_f = 0b01110001;
const uint8_t seg_h = 0b01110110;
const uint8_t seg_dp = 0b10000000;

const uint8_t seg_nums[] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01100111,
};


#endif

