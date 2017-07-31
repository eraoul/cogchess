//
//  bitboard.h
//  CogChess
//
//  Created by Eric Nichols on 1/3/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//

#ifndef bitboard_h
#define bitboard_h

#include <stdio.h>

#include "consts.h"
#include "types.h"

namespace bitboard {

// Retreive a square of the bitboard.
BitBoard get(BitBoard b, Square square);


// Make a bitboard with just 1 bit turned on, for the given square.
inline BitBoard sq_to_bb(Square sq) {
    return BB(1) << sq;
}

inline BitBoard sq_to_bb(File f, Rank r) {
    return BB(1) << make_square(f, r);
}

/// bb_str returns an ASCII representation of a bitboard suitable
/// to be printed to standard output. Useful for debugging.
const std::string bb_str(BitBoard b);

void print_bb(BitBoard b);

inline Square lsb(BitBoard b) { // Assembly code by Heinz van Saanen, from Stockfish
    BitBoard idx;
    __asm__("bsfq %1, %0" : "=r"(idx) : "rm"(b));
    return (Square)idx;
}

inline Square msb(BitBoard b) { // Assembly code by Heinz van Saanen, from Stockfish
    BitBoard idx;
    __asm__("bsrq %1, %0" : "=r"(idx) : "rm"(b));
    return (Square)idx;
}

inline int popcount(BitBoard b) {
    return __builtin_popcountll(b);
}

// From stockfish.
inline Square pop_lsb(BitBoard* b) {
    const Square s = lsb(*b);
    *b &= *b - 1;
    return s;
}

// Shift north, without wraparound.
inline BitBoard shiftNorthOne(BitBoard b) {
    return b << UPDOWN;
}
// inline BitBoard shiftNorthTwo(BitBoard b) {
//    return b << UPDOWN_2;
//}
// Shift south, without wraparound.
inline BitBoard shiftSouthOne(BitBoard b) {
    return b >> UPDOWN;
}
// inline BitBoard shiftSouthTwo(BitBoard b) {
//    return b >> UPDOWN_2;
//}
// Shift east, without wraparound.
inline BitBoard shiftEastOne(BitBoard b) {
    return (b & ~FILE_H_ALL) << SIDEWAYS;
}
// Shift west, without wraparound.
inline BitBoard shiftWestOne(BitBoard b) {
    return (b & ~FILE_A_ALL) >> SIDEWAYS;
}


} // namespace bitboard

class Precomputed {
  public:
    // First index for pawn arrays is color: WHITE or BLACK
    BitBoard pawn_moves[2][64];   // Only valid for indices 8-55 (A2-H7). Moves, not caputres.   1K
    BitBoard pawn_attacks[2][64]; // Only valid for indices 8-55. Captures only.                 1K

    BitBoard knight_moves[64]; // Moves/captures. 0.5K
    BitBoard king_moves[64];   // Moves/captures. 0.5K

    BitBoard ray_attacks[64][8]; // Moves/captures. 4K

    // Move tables: 7K so far.

    // Intel(R) Core(TM) i7-4650U CPU @ 1.70GHz
    // 64K L1 cache per core.
    // 256k L2 cache per core.
    // 2MB L3 cache per core

    Precomputed();
};

extern Precomputed precomputed;


#endif /* bitboard_h */
