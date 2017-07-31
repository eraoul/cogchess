//
//  bitboard.cpp
//  CogChess
//
//  Created by Eric Nichols on 1/3/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//

//#include <exception>
#include <iostream>
#include <stdio.h>

#include "bitboard.h"
#include "consts.h"
#include "types.h"

using std::cout;

// Accessing a square of the bitboard
BitBoard bitboard::get(BitBoard b, Square square) {
    return (b & (BB(1) << square));
}

/// bb_str returns an ASCII representation of a bitboard suitable
/// to be printed to standard output. Useful for debugging.

const std::string bitboard::bb_str(BitBoard b) {

    std::string s = "+---+---+---+---+---+---+---+---+\n";

    for (Rank r = RANK_8; r >= RANK_1; r--) {
        for (File f = FILE_A; f <= FILE_H; f++)
            s += b & (BB(1) << make_square(f, r)) ? "| X " : "|   ";

        s += "|\n+---+---+---+---+---+---+---+---+\n";
    }

    return s;
}

void bitboard::print_bb(BitBoard b) {
    printf("%s\n", bb_str(b).c_str());
}


// Class to precompute bitboard data for move generation.
Precomputed::Precomputed() {
    cout << "\nPrecomputing legal move tables...\n";

    // Pawn moves, per color.
    for (Square sq = SQ_A1; sq <= SQ_H8; sq++) {
        BitBoard bb_sq = BB(1) << sq;
        BitBoard b_white = 0;
        BitBoard b_black = 0;

        // Generate all single step forward moves.
        b_white |= bitboard::shiftNorthOne(bb_sq);
        b_black |= bitboard::shiftSouthOne(bb_sq);

        pawn_moves[WHITE][sq] = b_white;
        pawn_moves[BLACK][sq] = b_black;

        // Now generate all diagonal captures.
        b_white = 0;
        b_black = 0;
        b_white |= (bb_sq & ~FILE_A_ALL) << UP_LEFT;  // Move up+left
        b_white |= (bb_sq & ~FILE_H_ALL) << UP_RIGHT; // Move up+right
        b_black |= (bb_sq & ~FILE_A_ALL) >> UP_RIGHT; // Move down+left
        b_black |= (bb_sq & ~FILE_H_ALL) >> UP_LEFT;  // Move down+right

        pawn_attacks[WHITE][sq] = b_white;
        pawn_attacks[BLACK][sq] = b_black;
    }

    // King moves/attacks, from each square.
    for (Square sq = SQ_A1; sq <= SQ_H8; sq++) {
        BitBoard bb_sq = BB(1) << sq;
        BitBoard b = 0;
        b |= (bb_sq & ~FILE_A_ALL) >> SIDEWAYS; // Move left
        b |= (bb_sq & ~FILE_H_ALL) << SIDEWAYS; // Move right
        b |= (bb_sq & ~RANK_1_ALL) >> UPDOWN;   // Move down
        b |= (bb_sq & ~RANK_8_ALL) << UPDOWN;   // Move up

        b |= (bb_sq & ~(FILE_A_ALL | RANK_8_ALL)) << UP_LEFT;  // Move up+left
        b |= (bb_sq & ~(FILE_H_ALL | RANK_8_ALL)) << UP_RIGHT; // Move up+right
        b |= (bb_sq & ~(FILE_A_ALL | RANK_1_ALL)) >> UP_RIGHT; // Move down+left
        b |= (bb_sq & ~(FILE_H_ALL | RANK_1_ALL)) >> UP_LEFT;  // Move down+right

        king_moves[sq] = b;
    }

    // Knight moves/attacks, from each square.
    for (Square sq = SQ_A1; sq <= SQ_H8; sq++) {
        BitBoard bb_sq = BB(1) << sq;
        BitBoard b = 0;
        b |= (bb_sq & ~(FILE_A_ALL | FILE_B_ALL | RANK_8_ALL)) << NWW; // Up 1, left 2.
        b |= (bb_sq & ~(FILE_A_ALL | RANK_7_ALL | RANK_8_ALL)) << NNW; // Up 2, left 1.
        b |= (bb_sq & ~(FILE_H_ALL | RANK_7_ALL | RANK_8_ALL)) << NNE; // Up 2, right 1.
        b |= (bb_sq & ~(FILE_G_ALL | FILE_H_ALL | RANK_8_ALL)) << NEE; // Up 1, right 2.

        b |= (bb_sq & ~(FILE_G_ALL | FILE_H_ALL | RANK_1_ALL)) >> NWW; // Down 1, right 2.
        b |= (bb_sq & ~(FILE_H_ALL | RANK_2_ALL | RANK_1_ALL)) >> NNW; // Down 2, right 1.
        b |= (bb_sq & ~(FILE_A_ALL | RANK_2_ALL | RANK_1_ALL)) >> NNE; // Down 2, left 1.
        b |= (bb_sq & ~(FILE_A_ALL | FILE_B_ALL | RANK_1_ALL)) >> NEE; // Down 1, left 2.

        knight_moves[sq] = b;
    }

    // Ray attacks, in all 8 directions, from each square.
    // https: // chessprogramming.wikispaces.com/On+an+empty+Board

    // Easy directions first: North & South.

    // North.
    BitBoard ray = 0x0101010101010100; // A1, pointing north
    for (Square sq = SQ_A1; sq <= SQ_H8; sq++, ray <<= 1)
        ray_attacks[sq][DIR_N] = ray;

    // South.
    ray = 0x0080808080808080; // H8, pointing south
    for (Square sq = SQ_H8; sq >= SQ_A1; sq--, ray >>= 1)
        ray_attacks[sq][DIR_S] = ray;

    // Trickier directions: must avoid wraparound and board sides.

    // East.
    ray = 0xfe; // A1, pointing east
    for (File f = FILE_A; f <= FILE_H; f++, ray = bitboard::shiftEastOne(ray)) {
        BitBoard e = ray;
        for (Rank r = RANK_1; r <= RANK_8; r++, e = bitboard::shiftNorthOne(e)) // Shift the ray north one square.
            ray_attacks[make_square(f, r)][DIR_E] = e;
    }
    // West.
    ray = 0x7f; // H1, pointing west
    for (File f = FILE_H; f >= FILE_A; f--, ray = bitboard::shiftWestOne(ray)) {
        BitBoard w = ray;
        for (Rank r = RANK_1; r <= RANK_8; r++, w = bitboard::shiftNorthOne(w)) // Shift the ray north one square.
            ray_attacks[make_square(f, r)][DIR_W] = w;
    }
    // Northeast.
    ray = 0x8040201008040200; // A1, pointing northeast
    for (File f = FILE_A; f <= FILE_H; f++, ray = bitboard::shiftEastOne(ray)) {
        BitBoard ne = ray;
        for (Rank r = RANK_1; r <= RANK_8; r++, ne = bitboard::shiftNorthOne(ne)) // Shift the ray north one square.
            ray_attacks[make_square(f, r)][DIR_NE] = ne;
    }
    // Northwest.
    ray = 0x102040810204000; // H1, pointing northwest
    for (File f = FILE_H; f >= FILE_A; f--, ray = bitboard::shiftWestOne(ray)) {
        BitBoard nw = ray;
        for (Rank r = RANK_1; r <= RANK_8; r++, nw = bitboard::shiftNorthOne(nw)) // Shift the ray north one square.
            ray_attacks[make_square(f, r)][DIR_NW] = nw;
    }
    // Southeast.
    ray = 0x2040810204080; // A8, pointing southeast
    for (File f = FILE_A; f <= FILE_H; f++, ray = bitboard::shiftEastOne(ray)) {
        BitBoard se = ray;
        for (Rank r = RANK_8; r >= RANK_1; r--, se = bitboard::shiftSouthOne(se)) // Shift the ray south one square.
            ray_attacks[make_square(f, r)][DIR_SE] = se;
    }
    // Southwest.
    ray = 0x40201008040201; // H8, pointing southwest
    for (File f = FILE_H; f >= FILE_A; f--, ray = bitboard::shiftWestOne(ray)) {
        BitBoard sw = ray;
        for (Rank r = RANK_8; r >= RANK_1; r--, sw = bitboard::shiftSouthOne(sw)) // Shift the ray south one square.
            ray_attacks[make_square(f, r)][DIR_SW] = sw;
    }
}

// Construct a precomputed data object.
Precomputed precomputed;
