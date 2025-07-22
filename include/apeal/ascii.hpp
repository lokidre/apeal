/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

// TODO: convert to enum class Ascii

namespace apl {

const int ASCII_NUL   = 0x00 ;
const int ASCII_SOH   = 0x01 ;  // ^A
const int ASCII_STX   = 0x02 ;  // ^B
const int ASCII_ETX   = 0x03 ;
const int ASCII_EOT   = 0x04 ;
const int ASCII_ENQ   = 0x05 ;
const int ASCII_ACK   = 0x06 ;  // ^F
const int ASCII_BEL   = 0x07 ;
const int ASCII_BS    = 0x08 ;
const int ASCII_HT    = 0x09 ;
const int ASCII_LF    = 0x0A ;
const int ASCII_VT    = 0x0B ;
const int ASCII_FF    = 0x0C ;
const int ASCII_CR    = 0x0D ;
const int ASCII_SO    = 0x0E ;
const int ASCII_SI    = 0x0F ;

const int ASCII_DLE   = 0x10 ;  // 16    ^P
const int ASCII_DC1   = 0x11 ;
const int ASCII_XON   = 0x11 ;  // 17  ^Q
const int ASCII_DC2   = 0x12 ;
const int ASCII_DC3   = 0x13 ;
const int ASCII_XOFF  = 0x13 ;  // 19    ^S
const int ASCII_DC4   = 0x14 ;
const int ASCII_NAK   = 0x15 ;  // ^U
const int ASCII_SYN   = 0x16 ;  // 22  ^V
const int ASCII_ETB   = 0x17 ;
const int ASCII_CAN   = 0x18 ;  // 24  ^X
const int ASCII_EM    = 0x19 ;
const int ASCII_SUB   = 0x1A ;
const int ASCII_ESC   = 0x1B ;
const int ASCII_FS    = 0x1C ;
const int ASCII_GS    = 0x1D ;
const int ASCII_RS    = 0x1E ;
const int ASCII_US    = 0x1F ;

const int ASCII_SP    = 0x20 ;

const int ASCII_DEL   = 0x7F ;

}
