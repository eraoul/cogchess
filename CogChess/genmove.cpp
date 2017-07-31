//
//  genmove.cpp
//  CogChess
//
//  Created by Eric Nichols on 1/3/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//

#include <iostream>

#include "bitboard.h"
#include "board.h"
#include "genmove.h"

using std::cout;
using std::vector;


// Positive Rays
// https://chessprogramming.wikispaces.com/Classical+Approach
BitBoard Board::getRayAttacksPositive(Square sq, Direction dir8) {
    Bitboards& bbs = board_states[current_ply].bbs;

    BitBoard attacks = precomputed.ray_attacks[sq][dir8];
    BitBoard blocker = attacks & bbs.occupied;
    sq = bitboard::lsb(blocker | BB_H8);
    return attacks ^ precomputed.ray_attacks[sq][dir8];
}

// Negative Rays
BitBoard Board::getRayAttacksNegative(Square sq, Direction dir8) {
    Bitboards& bbs = board_states[current_ply].bbs;

    BitBoard attacks = precomputed.ray_attacks[sq][dir8];
    BitBoard blocker = attacks & bbs.occupied;
    sq = bitboard::msb(blocker | BB_A1);
    return attacks ^ precomputed.ray_attacks[sq][dir8];
}

void Board::generate_pseudolegal_moves() {

    BoardState& board_state = board_states[current_ply];
    Bitboards& bbs = board_state.bbs;

    auto& moves = pseudolegal_moves_[current_ply - search_start_ply];
    int move_num = 0;
    // moves.reserve(80);

    // printf("Generating moves\n");

    Square from_sq;

    // Knights.
    BitBoard from_squares = bbs.pcs[make_piece(side_to_move, KNIGHT)];
    while (from_squares) {
        from_sq = bitboard::pop_lsb(&from_squares);
        // printf("\nKnight at %d\n", from_sq);
        BitBoard to_squares = precomputed.knight_moves[from_sq];
        // bitboard::print_bb(to_squares);
        to_squares &= bbs.empty | bbs.occupied_per_color[!side_to_move];

        while (to_squares) {
            Square to_sq = bitboard::pop_lsb(&to_squares);
            moves[move_num++] = Move(from_sq, to_sq);
        }
    }

    // King.
    from_squares = bbs.pcs[make_piece(side_to_move, KING)];
    from_sq = bitboard::lsb(from_squares);
    // printf("\nKing at %d\n", from_sq);
    BitBoard to_squares = precomputed.king_moves[from_sq];
    // bitboard::print_bb(to_squares);
    to_squares &= bbs.empty | bbs.occupied_per_color[!side_to_move];
    while (to_squares) {
        Square to_sq = bitboard::pop_lsb(&to_squares);
        moves[move_num++] = Move(from_sq, to_sq);
    }

    // Castle.
    if (side_to_move == WHITE) {
        if (board_state.castling_rights & WHITE_OO) {
            if (!(CASTLE_KINGSIDE_MASK_W & bbs.occupied))
                moves[move_num++] = Move(SQ_E1, SQ_G1, NO_PIECE_TYPE, true);
        }
        if (board_state.castling_rights & WHITE_OOO)
            if (!(CASTLE_QUEENSIDE_MASK_W & bbs.occupied))
                moves[move_num++] = Move(SQ_E1, SQ_C1, NO_PIECE_TYPE, true);
    } else {
        if (board_state.castling_rights & BLACK_OO) {
            if (!(CASTLE_KINGSIDE_MASK_B & bbs.occupied))
                moves[move_num++] = Move(SQ_E8, SQ_G8, NO_PIECE_TYPE, true);
        }
        if (board_state.castling_rights & BLACK_OOO)
            if (!(CASTLE_QUEENSIDE_MASK_B & bbs.occupied))
                moves[move_num++] = Move(SQ_E8, SQ_C8, NO_PIECE_TYPE, true);
    }

    // Rooks.
    from_squares = bbs.pcs[make_piece(side_to_move, ROOK)];
    while (from_squares) {
        from_sq = bitboard::pop_lsb(&from_squares);
        // printf("\nRook at %d\n", from_sq);
        BitBoard to_squares = getRayAttacksPositive(from_sq, DIR_N) | getRayAttacksPositive(from_sq, DIR_E) |
                              getRayAttacksNegative(from_sq, DIR_S) | getRayAttacksNegative(from_sq, DIR_W);

        // bitboard::print_bb(to_squares);
        to_squares &= bbs.empty | bbs.occupied_per_color[!side_to_move];
        while (to_squares) {
            Square to_sq = bitboard::pop_lsb(&to_squares);
            moves[move_num++] = Move(from_sq, to_sq);
        }
    }

    // Bishops.
    from_squares = bbs.pcs[make_piece(side_to_move, BISHOP)];
    while (from_squares) {
        from_sq = bitboard::pop_lsb(&from_squares);
        // printf("\nBishop at %d\n", from_sq);
        BitBoard to_squares = getRayAttacksPositive(from_sq, DIR_NW) | getRayAttacksPositive(from_sq, DIR_NE) |
                              getRayAttacksNegative(from_sq, DIR_SE) | getRayAttacksNegative(from_sq, DIR_SW);

        // bitboard::print_bb(to_squares);
        to_squares &= bbs.empty | bbs.occupied_per_color[!side_to_move];
        while (to_squares) {
            Square to_sq = bitboard::pop_lsb(&to_squares);
            moves[move_num++] = Move(from_sq, to_sq);
        }
    }

    // Queens.
    from_squares = bbs.pcs[make_piece(side_to_move, QUEEN)];
    while (from_squares) {
        from_sq = bitboard::pop_lsb(&from_squares);
        // printf("\nQueen at %d\n", from_sq);
        BitBoard to_squares = getRayAttacksPositive(from_sq, DIR_NW) | getRayAttacksPositive(from_sq, DIR_N) |
                              getRayAttacksPositive(from_sq, DIR_NE) | getRayAttacksPositive(from_sq, DIR_E) |
                              getRayAttacksNegative(from_sq, DIR_SE) | getRayAttacksNegative(from_sq, DIR_S) |
                              getRayAttacksNegative(from_sq, DIR_SW) | getRayAttacksNegative(from_sq, DIR_W);

        // bitboard::print_bb(to_squares);
        to_squares &= bbs.empty | bbs.occupied_per_color[!side_to_move];
        while (to_squares) {
            Square to_sq = bitboard::pop_lsb(&to_squares);
            moves[move_num++] = Move(from_sq, to_sq);
        }
    }


    // Pawns.
    // Pretend an opponent pawn is at the en passant square.
    const Square en_passant_sq = board_state.en_passant_square;
    BitBoard capturable = bbs.occupied_per_color[!side_to_move];
    if (en_passant_sq != SQ_NONE)
        capturable |= BB(1) << en_passant_sq;
    const Rank promotion_rank = (side_to_move == WHITE) ? RANK_8 : RANK_1;
    const BitBoard rank_mask = (side_to_move == WHITE) ? RANK_3_ALL : RANK_6_ALL; // Used for double-pawn moves.

    from_squares = bbs.pcs[make_piece(side_to_move, PAWN)];
    while (from_squares) {
        from_sq = bitboard::pop_lsb(&from_squares);
        // Move forward one.
        BitBoard to_squares = precomputed.pawn_moves[side_to_move][from_sq] & bbs.empty;
        // Move forward two if possible.
        if (side_to_move == WHITE) // TODO: how to avoid this if statement? This test seems really clunky.
            to_squares |= ((to_squares & rank_mask) << UPDOWN) & bbs.empty;
        else
            to_squares |= ((to_squares & rank_mask) >> UPDOWN) & bbs.empty;

        // Captures.
        to_squares |= precomputed.pawn_attacks[side_to_move][from_sq] & capturable;

        while (to_squares) {
            Square to_sq = bitboard::pop_lsb(&to_squares);
            // Test for promotion.
            if (rank_of(to_sq) == promotion_rank) {
                moves[move_num++] = Move(from_sq, to_sq, QUEEN);
                moves[move_num++] = Move(from_sq, to_sq, ROOK);
                moves[move_num++] = Move(from_sq, to_sq, BISHOP);
                moves[move_num++] = Move(from_sq, to_sq, KNIGHT);
            } else if (to_sq == en_passant_sq)
                moves[move_num++] = Move(from_sq, to_sq, NO_PIECE_TYPE, false, true);
            else
                moves[move_num++] = Move(from_sq, to_sq);
        }
    }

    // Mark end of move list.
    moves[move_num] = NO_MOVE;

    // Print moves.
    /*for (auto move : moves)
        cout << move_str(move) << "\n";
    */

    // return moves;
}