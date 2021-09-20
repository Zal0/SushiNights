#ifndef TRIG_H
#define TRIG_H

#include <gb/gb.h>

extern const UINT8 sin[];

#define SIN(A) (sin[0xFF & (A)] - 128)
#define COS(A) (sin[0xFF & ((A) + 64)] - 128)

#endif