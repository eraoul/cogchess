//
//  consts.hpp
//  CogChess
//
//  Created by Eric Nichols on 1/3/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//

#ifndef consts_h
#define consts_h

#include <stdio.h>

#include "types.h"

const int CUR_SEARCH_DEPTH = 7;

const string ENGINE_NAME = "CogChess";
const string ENGINE_VERSION = "0.0.1";
const string ENGINE_AUTHOR = "Eric P. Nichols (epnichols@gmail.com)";

extern const int64 POLYGLOT_RANDOM_ARRAY[];

// Only positive moves expressed here. For negative moves, use right-shift with these.
const int SIDEWAYS = 1;
const int UPDOWN = 8;
// const int UPDOWN_2 = 16;
const int UP = 8;
const int RIGHT = 1;
const int UP_LEFT = 7;
const int UP_RIGHT = 9;

// Positive knight moves
const int NWW = 6;
const int NNW = 15;
const int NNE = 17;
const int NEE = 10;


// int DOWN_RIGHT = -7;
// int DOWN = -8;
// int LEFT = -1;
// int DOWN_LEFT = -9;

const BitBoard FILE_A_ALL = 0x101010101010101;
const BitBoard FILE_B_ALL = 0x202020202020202;
const BitBoard FILE_C_ALL = 0x404040404040404;
const BitBoard FILE_D_ALL = 0x808080808080808;
const BitBoard FILE_E_ALL = 0x1010101010101010;
const BitBoard FILE_F_ALL = 0x2020202020202020;
const BitBoard FILE_G_ALL = 0x4040404040404040;
const BitBoard FILE_H_ALL = 0x8080808080808080;
const BitBoard RANK_1_ALL = 0xff;
const BitBoard RANK_2_ALL = 0xff00;
const BitBoard RANK_3_ALL = 0xff0000;
const BitBoard RANK_4_ALL = 0xff000000;
const BitBoard RANK_5_ALL = 0xff00000000;
const BitBoard RANK_6_ALL = 0xff0000000000;
const BitBoard RANK_7_ALL = 0xff000000000000;
const BitBoard RANK_8_ALL = 0xff00000000000000;

const BitBoard CASTLE_KINGSIDE_MASK_W = 0x60;
const BitBoard CASTLE_QUEENSIDE_MASK_W = 0x0e;
const BitBoard CASTLE_KINGSIDE_MASK_B = 0x6000000000000000;
const BitBoard CASTLE_QUEENSIDE_MASK_B = 0xe00000000000000;

const BitBoard BLOCKING_PAWNS_W_O_O = 0xf800;             // Pawns on D,E,F,G,H2
const BitBoard BLOCKING_PAWNS_W_O_O_O = 0x3e00;           // Pawns on B,C,D,E,F2
const BitBoard BLOCKING_PAWNS_B_O_O = 0xf8000000000000;   // Pawns on D,E,F,G,H7
const BitBoard BLOCKING_PAWNS_B_O_O_O = 0x3e000000000000; // Pawns on B,C,D,E,F7


#endif /* consts_h */
