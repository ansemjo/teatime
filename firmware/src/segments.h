// Mapping of LCD segments in teatime PCB with GoodDisplay GDC04212.
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#pragma once

#include <stdint.h>

/**
 * Schematic of the wiring between Rohm BU9796AMUV pins and
 * GoodDisplay GDC04212 display segment lines on teatime PCB.
 * 
 *  ┏━━━━━━━━━━━━━━┓               ┏━━━━━━━━━━━━┓
 *  ┃              ┃               ┃            ┃
 *  ┃  BU9796AMUV  ┃               ┃  GDC04212  ┃
 *  ┃              ┃               ┃            ┃
 *  ┃    SEG0   04╶╂───────────────╂╴12  4abcd  ┃
 *  ┃    SEG1   05╶╂───────────────╂╴11  4fgep  ┃
 *  ┃    SEG2   06╶╂───────────────╂╴10  3abcd  ┃
 *  ┃    SEG3   07╶╂───────────────╂╴09  3fgep  ┃
 *  ┃    SEG4   08╶╂───────────────╂╴08  2abcd  ┃
 *  ┃    SEG5   09╶╂───────────────╂╴07  2fgep  ┃
 *  ┃    SEG6   10╶╂───────────────╂╴06  1abcd  ┃
 *  ┃    SEG7   11╶╂───────────────╂╴05  1fgep  ┃
 *  ┃    SEG8   12╶╂─X             ┃            ┃
 *  ┃    SEG9   13╶╂─X    ╭────────╂╴04  COM4   ┃
 *  ┃    SEG10  14╶╂─X    │╭───────╂╴03  COM3   ┃
 *  ┃    SEG11  15╶╂─X    ││╭──────╂╴02  COM2   ┃
 *  ┃              ┃      │││╭─────╂╴01  COM1   ┃
 *  ┃    COM0   16╶╂──────╯│││     ┃            ┃
 *  ┃    COM1   17╶╂───────╯││     ┗━━━━━━━━━━━━┛
 *  ┃    COM2   18╶╂────────╯│
 *  ┃    COM3   19╶╂─────────╯
 *  ┃              ┃
 *  ┗━━━━━━━━━━━━━━┛
 * 
 * Which leads to the following segment mapping
 * within each digit:
 * 
 *          ┌───┐         ┏━━━━━━┳━━━┯━━━┓
 *        ┌─┼─a─┼─┐       ┃Nibble┃ H │ L ┃
 *        │ │   │ │       ┣━━━━━━╋━━━┿━━━┫
 *        │f│   │b│       ┃ COM1 ┃ d │ p ┃
 *        └─┼───┼─┘       ┃ COM2 ┃ c │ e ┃
 *        ┌─┼─g─┼─┐       ┃ COM3 ┃ b │ g ┃
 *        │ │   │ │       ┃ COM4 ┃ a │ f ┃
 *        │e│   │c│       ┣━━━━━━┻━━━┷━━━┫
 *        └─┼───┼─┘ ┌┐    ┃ > 0bdcbapegf ┃
 *          └─d─┘   └p    ┗━━━━━━━━━━━━━━┛
 * 
 * The first DDRAM byte corresponds to the fourth (rightmost)
 * digit. The dots are mapped (from left to right) to the
 * following bytes: dp1 -> 4, dp2 -> 3, col -> 2, dp3 -> 1.
 * 
 **/

// individual segments
#define Ad 0b10000000 // d, bottom segment
#define Ac 0b01000000 // c, bottom right segment
#define Ab 0b00100000 // b, top right segment
#define Aa 0b00010000 // a, top segment
#define Ap 0b00001000 // p, decimal point segment
#define Ae 0b00000100 // e, bottom left segment
#define Ag 0b00000010 // g, center segment
#define Af 0b00000001 // f, top left segment

// numbers
#define CHAR_0 Aa|Ab|Ac|Ad|Ae|Af      // the number 0
#define CHAR_1 Ab|Ac                  // the number 1
#define CHAR_2 Aa|Ab|Ag|Ae|Ad         // the number 2
#define CHAR_3 Aa|Ab|Ac|Ad|Ag         // the number 3
#define CHAR_4 Af|Ag|Ab|Ac            // the number 4
#define CHAR_5 Aa|Af|Ag|Ac|Ad         // the number 5
#define CHAR_6 Aa|Af|Ae|Ad|Ac|Ag      // the number 6
#define CHAR_7 Aa|Ab|Ac               // the number 7
#define CHAR_8 Aa|Ab|Ac|Ad|Ae|Af|Ag   // the number 8
#define CHAR_9 Aa|Ab|Ac|Ad|Ag|Af      // the number 9

uint8_t NUMBERS[10] = { CHAR_0, CHAR_1, CHAR_2, CHAR_3,
  CHAR_4, CHAR_5, CHAR_6, CHAR_7, CHAR_8, CHAR_9 };

// symbols
#define CHAR_POINT    Ap        // a point '.' (bytes 1, 3, 4)
#define CHAR_COLON    Ap        // a colon ':' (byte 2)
#define CHAR_MINUS    Ag        // a minus '-'
#define CHAR_UNDER    Ad        // an underscore '_'
#define CHAR_GREATER  Ac|Ad     // a greater than '>'
#define CHAR_LESS     Ad|Ae     // a less than '<'
#define CHAR_EQUAL    Ad|Ag     // an equals sign '='
#define CHAR_IDENT    Aa|Ad|Ag  // an identical sign '≡'
#define CHAR_SPACE    0         // an empty space ' '

// alphabet
#define CHAR_A  Aa|Ab|Ac|Ae|Af|Ag
#define CHAR_a  CHAR_A
#define CHAR_b  Ac|Ad|Ae|Af|Ag
#define CHAR_B  CHAR_b
#define CHAR_C  Aa|Ad|Ae|Af
#define CHAR_c  Ad|Ae|Ag
#define CHAR_d  Ab|Ac|Ad|Ae|Ag
#define CHAR_D  CHAR_d
#define CHAR_E  Aa|Ad|Ae|Af|Ag
#define CHAR_e  CHAR_E
#define CHAR_F  Aa|Ae|Af|Ag
#define CHAR_f  CHAR_F
#define CHAR_G  CHAR_6
#define CHAR_g  CHAR_9
#define CHAR_H  Ab|Ac|Ae|Af|Ag
#define CHAR_h  Ac|Ae|Af|Ag
#define CHAR_I  Ae|Af
#define CHAR_i  Ae
#define CHAR_J  Ab|Ac|Ad
#define CHAR_j  CHAR_J
#define CHAR_k  Aa|Ac|Ae|Af|Ag
#define CHAR_K  CHAR_k
#define CHAR_L  Ad|Ae|Af
#define CHAR_l  CHAR_I
#define CHAR_Ml Aa|Ab|Ae|Af
#define CHAR_Mr Aa|Ab|Ac|Af
#define CHAR_ml CHAR_Ml
#define CHAR_mr CHAR_Mr
#define CHAR_n  Ac|Ae|Ag
#define CHAR_N  CHAR_n
#define CHAR_O  CHAR_0
#define CHAR_o  Ac|Ad|Ae|Ag
#define CHAR_P  Aa|Ab|Ae|Af|Ag
#define CHAR_p  CHAR_P
#define CHAR_q  Aa|Ab|Ac|Af|Ag
#define CHAR_Q  CHAR_q
#define CHAR_r  Ae|Ag
#define CHAR_R  CHAR_r
#define CHAR_S  Aa|Ac|Ad|Af|Ag
#define CHAR_s  CHAR_S
#define CHAR_t  Ad|Ae|Af|Ag
#define CHAR_T  CHAR_t
#define CHAR_U  Ab|Ac|Ad|Ae|Af
#define CHAR_u  Ac|Ad|Ae
#define CHAR_V  CHAR_U
#define CHAR_v  CHAR_u
#define CHAR_Wl Ac|Ad|Ae|Af
#define CHAR_Wr Ab|Ac|Ad|Ae
#define CHAR_wl CHAR_Wl
#define CHAR_wr CHAR_Wr
#define CHAR_X  CHAR_H
#define CHAR_x  CHAR_H
#define CHAR_y  Ab|Ac|Ad|Af|Ag
#define CHAR_Y  CHAR_y
#define CHAR_Z  CHAR_2
#define CHAR_z  CHAR_2