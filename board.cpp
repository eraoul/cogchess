//
//  bitboard.cpp
//  CogChess
//
//  Created by Eric Nichols on 1/3/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//

#include <iostream>
#include <stdio.h>

#include "bitboard.h"
#include "board.h"
#include "consts.h"
#include "types.h"

using std::cout;

Board::Board() {
    set_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void Board::print_piece_bbs() {
    Bitboards& bbs = board_states[current_ply].bbs;
    for (Color c = WHITE; c <= BLACK; c++) {
        for (PieceType pt = PAWN; pt <= KING; pt++) {
            BitBoard bb = bbs.pcs[make_piece(c, pt)];
            printf("\nBitboard for %c: %llu\n", piece_char(make_piece(c, pt)), bb);
            printf("%s", bitboard::bb_str(bb).c_str());
        }
    }
}

void Board::print_misc_bbs() {
    Bitboards& bbs = board_states[current_ply].bbs;
    printf("\nOccupied squares: %llu\n%s", bbs.occupied, bitboard::bb_str(bbs.occupied).c_str());
    printf("\nEmpty squares: %llu\n%s", bbs.empty, bitboard::bb_str(bbs.empty).c_str());
    printf("\nOccupied squares White: %llu\n%s", bbs.occupied_per_color[WHITE],
           bitboard::bb_str(bbs.occupied_per_color[WHITE]).c_str());
    printf("\nOccupied squares Black: %llu\n%s", bbs.occupied_per_color[BLACK],
           bitboard::bb_str(bbs.occupied_per_color[BLACK]).c_str());
}

void Board::print() {
    Bitboards& bbs = board_states[current_ply].bbs;

    std::string s = "+---+---+---+---+---+---+---+---+\n";

    for (Rank r = RANK_8; r >= RANK_1; r--) {
        for (File f = FILE_A; f <= FILE_H; f++) {
            s += "| ";
            s += piece_char(bbs.squares[make_square(f, r)]);
            s += " ";
        }
        s += "|\n+---+---+---+---+---+---+---+---+\n";
    }
    s += "                          ";
    s += std::to_string(current_ply / 2);
    s += " ";
    if (side_to_move == WHITE)
        s += "(W)\n";
    else
        s += "(B)\n";

    printf("%s", s.c_str());
}

void Board::initialize_board_state(int castling_rights, Square en_passant_square) {
    BoardState& board_state = board_states[current_ply];

    board_state.castling_rights = castling_rights;
    board_state.en_passant_square = en_passant_square;
    board_state.prev_move = Move(SQ_NONE, SQ_NONE);
}

inline int64 get_zorbist(const Bitboards& bbs, Color color, Square sq) {
    int piece_index = (type_of(bbs.squares[sq]) - PAWN) * 2 + // Subtract off PAWN offset.
                      ((color == BLACK) ? 0 : 1);             // * 2 for black piece indices.
    return POLYGLOT_RANDOM_ARRAY[(piece_index << 6) + (rank_of(sq) << 3) + file_of(sq)];
}

inline int64 get_zorbist(Color color, Square sq, PieceType pt) {
    int piece_index = (pt - PAWN) * 2 +           // Subtract off PAWN offset.
                      ((color == BLACK) ? 0 : 1); // * 2 for black piece indices.
    return POLYGLOT_RANDOM_ARRAY[(piece_index << 6) + (rank_of(sq) << 3) + file_of(sq)];
}

int64 Board::compute_full_zorbist_hash() {
    BoardState& board_state = board_states[current_ply];
    Bitboards& bbs = board_state.bbs;

    int64 hash = 0;

    // Black pieces.
    BitBoard squares = bbs.occupied_per_color[BLACK];
    while (squares != 0) {
        Square sq = bitboard::pop_lsb(&squares);
        hash ^= get_zorbist(bbs, BLACK, sq);
    }

    // White pieces.
    squares = bbs.occupied_per_color[WHITE];
    while (squares != 0) {
        Square sq = bitboard::pop_lsb(&squares);
        hash ^= get_zorbist(bbs, WHITE, sq);
    }

    // side-to-move.
    if (side_to_move == BLACK)
        hash ^= POLYGLOT_RANDOM_ARRAY[768];

    // castling rights.
    const int& castling_rights = board_state.castling_rights;
    if (castling_rights & WHITE_OO)
        hash ^= POLYGLOT_RANDOM_ARRAY[769];
    if (castling_rights & WHITE_OOO)
        hash ^= POLYGLOT_RANDOM_ARRAY[770];
    if (castling_rights & BLACK_OO)
        hash ^= POLYGLOT_RANDOM_ARRAY[771];
    if (castling_rights & BLACK_OOO)
        hash ^= POLYGLOT_RANDOM_ARRAY[772];

    // en-passant file.
    Square en_passant = board_state.en_passant_square;
    if (en_passant != SQ_NONE)
        hash ^= POLYGLOT_RANDOM_ARRAY[773 + file_of(en_passant)];

    return hash;
}


/*inline int64 Board::add_zorbist_hash(int64 incremental_zorbist_hash) {
    return board_state.hash ^ incremental_zorbist_hash;
}*/

void Board::make_move(Move move) {
    current_ply++;

    board_states[current_ply] = board_states[current_ply - 1]; // Copy the old state to the new state.

    BoardState& board_state = board_states[current_ply];
    Bitboards& bbs = board_state.bbs;
    /*
        if (bitboard::popcount(board_states[current_ply].bbs.pcs[make_piece(WHITE, KING)]) != 1 ||
            bitboard::popcount(board_states[current_ply].bbs.pcs[make_piece(BLACK, KING)]) != 1) {
            cout << "\n\nBoard is broken\n";
            print();
            print_piece_bbs();
            print_misc_bbs();

            cout << "popcount K: " << bitboard::popcount(board_states[current_ply].bbs.pcs[make_piece(WHITE, KING)]) <<
       "\n";
            cout << "popcount k: " << bitboard::popcount(board_states[current_ply].bbs.pcs[make_piece(BLACK, KING)]) <<
       "\n";
            assert(false);
        }
        // Sanity check the board squares.
        for (Square i = SQ_A1; i <= SQ_H8; i++) {
            auto val = bbs.squares[i];
            if (val < NO_PIECE || val > B_KING || (val > W_KING && val < B_PAWN)) {
                cout << "\n\nBoard is broken at square index " << i << " with value=" << val << "\n";
                print();
                print_piece_bbs();
                print_misc_bbs();

                cout << "popcount k: " << bitboard::popcount(board_states[current_ply].bbs.pcs[make_piece(WHITE, KING)])
       << "\n";

                assert(false);
            }
        }
    */
    board_state.prev_move = move;

    int64& hash = board_state.hash;

    const Color& color = side_to_move;
    const Color other_color = (side_to_move == WHITE) ? BLACK : WHITE;

    if (move == NULL_MOVE) {
        side_to_move = other_color;
        hash ^= POLYGLOT_RANDOM_ARRAY[768];
        return;
    }


    //////////////////////////
    // Remaining code updates state of the copy made above.
    //////////////////////////

    Piece piece = bbs.squares[move.from];        // This is the piece we're moving.
    Piece captured_piece = bbs.squares[move.to]; // This might be NO_PIECE.
    Square capture_square = move.to;             // Reset later for en_passant.

    if (move.is_castle) {
        // Move the rook. Handle king later.
        const Rank rank = rank_of(move.to);
        Square sq_rook_start, sq_rook_end;

        if (file_of(move.to) == FILE_G) { // Kingside.
            sq_rook_start = make_square(FILE_H, rank);
            sq_rook_end = make_square(FILE_F, rank);
        } else { // Queenside
            sq_rook_start = make_square(FILE_A, rank);
            sq_rook_end = make_square(FILE_D, rank);
        }
        const BitBoard rook_mask = bitboard::sq_to_bb(sq_rook_start) | bitboard::sq_to_bb(sq_rook_end);
        const Piece rook = make_piece(color, ROOK);

        bbs.squares[sq_rook_start] = NO_PIECE;
        bbs.squares[sq_rook_end] = rook;
        bbs.pcs[rook] ^= rook_mask;
        bbs.occupied_per_color[color] ^= rook_mask;
        hash ^= get_zorbist(color, sq_rook_start, ROOK);
        hash ^= get_zorbist(color, sq_rook_end, ROOK);

        // Update castle rights.
        if (color == WHITE) {
            // update hash.
            if (board_state.castling_rights & WHITE_OO)
                hash ^= POLYGLOT_RANDOM_ARRAY[769];
            if (board_state.castling_rights & WHITE_OOO)
                hash ^= POLYGLOT_RANDOM_ARRAY[770];

            board_state.castling_rights &= BLACK_OO | BLACK_OOO;
        } else {
            // update hash.
            if (board_state.castling_rights & BLACK_OO)
                hash ^= POLYGLOT_RANDOM_ARRAY[771];
            if (board_state.castling_rights & BLACK_OOO)
                hash ^= POLYGLOT_RANDOM_ARRAY[772];

            board_state.castling_rights &= WHITE_OO | WHITE_OOO;
        }
    } else {
        // Update castling rights for rook and king non-castle moves.
        if (piece == W_ROOK) {
            if (move.from == SQ_H1 && (board_state.castling_rights & WHITE_OO)) {
                board_state.castling_rights &= NOT_WHITE_OO;
                // update hash.
                hash ^= POLYGLOT_RANDOM_ARRAY[769];
            }
            if (move.from == SQ_A1 && (board_state.castling_rights & WHITE_OOO)) {
                board_state.castling_rights &= NOT_WHITE_OOO;
                hash ^= POLYGLOT_RANDOM_ARRAY[770];
            }
        } else if (piece == B_ROOK) {
            if (move.from == SQ_H8 && (board_state.castling_rights & BLACK_OO)) {
                board_state.castling_rights &= NOT_BLACK_OO;
                // update hash.
                hash ^= POLYGLOT_RANDOM_ARRAY[771];
            }
            if (move.from == SQ_A8 && (board_state.castling_rights & BLACK_OOO)) {
                board_state.castling_rights &= NOT_BLACK_OOO;
                hash ^= POLYGLOT_RANDOM_ARRAY[772];
            }
        } else if (piece == W_KING) {
            if (board_state.castling_rights & WHITE_OO) {
                board_state.castling_rights &= NOT_WHITE_OO;
                // update hash.
                hash ^= POLYGLOT_RANDOM_ARRAY[769];
            }
            if (board_state.castling_rights & WHITE_OOO) {
                board_state.castling_rights &= NOT_WHITE_OOO;
                hash ^= POLYGLOT_RANDOM_ARRAY[770];
            }
        } else if (piece == B_KING) {
            if (board_state.castling_rights & BLACK_OO) {
                board_state.castling_rights &= NOT_BLACK_OO;
                // update hash.
                hash ^= POLYGLOT_RANDOM_ARRAY[771];
            }
            if (board_state.castling_rights & BLACK_OOO) {
                board_state.castling_rights &= NOT_BLACK_OOO;
                hash ^= POLYGLOT_RANDOM_ARRAY[772];
            }
        }
    }


    // Remove the piece.
    bbs.squares[move.from] = NO_PIECE;
    // The piece is added/removed from bitboards at the end of this function.

    if (move.promote_to == NO_PIECE_TYPE) {
        // Not a promotion. Add the piece back to its new square.
        bbs.squares[move.to] = piece;
    } else {
        // Promotion. Make a new piece for the new square.
        Piece promotion_piece = make_piece(color, move.promote_to);
        bbs.squares[move.to] = promotion_piece;
        bbs.pcs[promotion_piece] ^= bitboard::sq_to_bb(move.to);
        bbs.occupied_per_color[color] ^= bitboard::sq_to_bb(move.to);
        hash ^= get_zorbist(color, move.to, move.promote_to);
    }

    if (move.is_en_passant) {
        // Remove the en_passant pawn.
        // Record captured piece type for later logic.
        if (color == WHITE) {
            capture_square = (Square)(move.to - UPDOWN);
            bbs.squares[capture_square] = NO_PIECE;
            captured_piece = make_piece(BLACK, PAWN);
        } else {
            capture_square = (Square)(move.to + UPDOWN);
            bbs.squares[capture_square] = NO_PIECE;
            captured_piece = make_piece(WHITE, PAWN);
        }
    }

    // Clear en passant square. (But it might be set again later in this function.)
    if (board_state.en_passant_square != SQ_NONE) {
        hash ^= POLYGLOT_RANDOM_ARRAY[773 + file_of(board_state.en_passant_square)];
        board_state.en_passant_square = SQ_NONE;
    }

    // Captures, 50 move clock.
    if (captured_piece == NO_PIECE) {
        board_state.ply_since_move_or_capture++;
    } else {
        // This is a capture.
        board_state.ply_since_move_or_capture = 0;

        // Update bitboard for captured piece type.
        bbs.pcs[captured_piece] ^= bitboard::sq_to_bb(capture_square);
        bbs.occupied_per_color[other_color] ^= bitboard::sq_to_bb(capture_square);
        hash ^= get_zorbist(other_color, capture_square, type_of(captured_piece));

        // Update castle rights if a rook was captured.
        if (move.to == SQ_H1 && (board_state.castling_rights & WHITE_OO)) {
            board_state.castling_rights &= NOT_WHITE_OO;
            hash ^= POLYGLOT_RANDOM_ARRAY[769];
        }
        if (move.to == SQ_A1 && (board_state.castling_rights & WHITE_OOO)) {
            board_state.castling_rights &= NOT_WHITE_OOO;
            hash ^= POLYGLOT_RANDOM_ARRAY[770];
        }
        if (move.to == SQ_H8 && (board_state.castling_rights & BLACK_OO)) {
            board_state.castling_rights &= NOT_BLACK_OO;
            hash ^= POLYGLOT_RANDOM_ARRAY[771];
        }
        if (move.to == SQ_A8 && (board_state.castling_rights & BLACK_OOO)) {
            board_state.castling_rights &= NOT_BLACK_OOO;
            hash ^= POLYGLOT_RANDOM_ARRAY[772];
        }
    }


    // Pawn moves, 50 move clock (has to come after capture test above to avoid
    // setting the clock to 1 for a pawn non-capture move.)
    if (type_of(piece) == PAWN) {
        // Reset 50 move clock.
        board_state.ply_since_move_or_capture = 0;

        // Update en_passant square for a pawn double-move.
        if (rank_of(move.to) - rank_of(move.from) == 2) {
            // White pawn advanced two.
            board_state.en_passant_square = make_square(file_of(move.to), RANK_3);
            hash ^= POLYGLOT_RANDOM_ARRAY[773 + file_of(move.to)];
        } else if (rank_of(move.from) - rank_of(move.to) == 2) {
            // Black pawn advanced two.
            board_state.en_passant_square = make_square(file_of(move.to), RANK_6);
            hash ^= POLYGLOT_RANDOM_ARRAY[773 + file_of(move.to)];
        }
    }

    // Update bitboards for moved piece (unless it's a promotion)
    BitBoard moved_piece_mask;
    if (move.promote_to == NO_PIECE_TYPE) {
        // Not promtion. Normal move. Clear original square, fill new square.
        moved_piece_mask = bitboard::sq_to_bb(move.from) | bitboard::sq_to_bb(move.to);
        hash ^= get_zorbist(color, move.from, type_of(piece));
        hash ^= get_zorbist(color, move.to, type_of(piece));
    } else {
        // Promotion. Clear the original square.
        moved_piece_mask = bitboard::sq_to_bb(move.from);
        hash ^= get_zorbist(color, move.from, type_of(piece));
    }
    bbs.pcs[piece] ^= moved_piece_mask;
    bbs.occupied_per_color[color] ^= moved_piece_mask;

    // Sanity check: make sure white and black occupancy is distinct.
    assert(!(bbs.occupied_per_color[WHITE] & bbs.occupied_per_color[BLACK]));

    // Update occupied/empty bitboards.
    bbs.occupied = bbs.occupied_per_color[WHITE] | bbs.occupied_per_color[BLACK];
    bbs.empty = ~bbs.occupied;

    // Finally update side to move.
    side_to_move = other_color;
    hash ^= POLYGLOT_RANDOM_ARRAY[768];
}


// Undo the most recent move.
//
// Updates bitboards as necessary.
//
// The BoardState is restored automatically since it's stored in the array of states
// at the previous ply.
void Board::undo_move() {
    current_ply--;

    // Update color.
    side_to_move = (side_to_move == WHITE) ? BLACK : WHITE;

    //////////////////////////

    // TODO: implement move/unmove for bitboard updates.
    // Currently we use copy/make with a previous state stack instead.

    // Update bitboads.
}

bool Board::can_capture_king(const Move (&moves)[MAX_LEGAL_MOVES], const Square& king_square) {
    int i = 0;
    while (!(moves[i].is_no_move())) {
        if (moves[i].to == king_square)
            return true;
        i++;
    }

    return false;
}

bool Board::castled_through_check(const Move& prev_move, const Move (&moves)[MAX_LEGAL_MOVES], const BitBoard& enemy_pawns) {

    int i = 0;
    const Square& king_sq = prev_move.from;
    const Square& king_to_sq = prev_move.to;
    Square travel_sq;
    BitBoard enemy_blocking_pawn_squares;

    // Look up the travel square given the to_square.
    // TODO: could also look this up in an array if the conditionals are slow.
    if (king_to_sq == SQ_G1) {
        travel_sq = SQ_F1;
        enemy_blocking_pawn_squares = BLOCKING_PAWNS_W_O_O;
    } else if (king_to_sq == SQ_C1) {
        travel_sq = SQ_D1;
        enemy_blocking_pawn_squares = BLOCKING_PAWNS_W_O_O_O;
    } else if (king_to_sq == SQ_G8) {
        travel_sq = SQ_F8;
        enemy_blocking_pawn_squares = BLOCKING_PAWNS_B_O_O;
    } else if (king_to_sq == SQ_C8) {
        travel_sq = SQ_D8;
        enemy_blocking_pawn_squares = BLOCKING_PAWNS_B_O_O_O;
    } else
        assert(false);

    // Check for enemy pawns controlling the king, travel-through, or destination square.
    // This is necessary because pawns do not capture the same way in which they move.
    // Checking the destination square isn't necessary since generated moves will make a capture
    // there, but it costs no more time to check for those in advance.
    if (enemy_pawns & enemy_blocking_pawn_squares)
        return true;

    // Finally, check for enemy moves to the travel-through square (for castling through check)
    // or the original king square (for castling while in check).
    while (!(moves[i].is_no_move())) {
        if (moves[i].to == travel_sq || moves[i].to == king_sq)
            return true;
        i++;
    }

    return false;
}


int64 Board::perft_divide(int depth, int64& num_en_passant, int64& num_castles, int64& num_promotions) {
    generate_pseudolegal_moves();
    auto& moves = pseudolegal_moves_[current_ply - search_start_ply];

    int64 result = 0;
    int i = 0;
    while (!(moves[i].is_no_move())) {
        make_move(moves[i]);
        int64 this_result = perft(depth - 1, num_en_passant, num_castles, num_promotions);
        if (this_result)
            cout << move_str(moves[i]) << ": " << this_result << "\n";
        result += this_result;
        undo_move();
        i++;
    }

    return result;
}

int64 Board::perft(int depth, int64& num_en_passant, int64& num_castles, int64& num_promotions) {
    Move& prev_move = board_states[current_ply].prev_move;

    Color other_color = (side_to_move == WHITE) ? BLACK : WHITE;
    generate_pseudolegal_moves();
    auto& moves = pseudolegal_moves_[current_ply - search_start_ply];
    // Check for incoming illegal move. If position is illegal, stop search.
    if (can_capture_king(moves, get_king_square(other_color, board_states[current_ply].bbs)))
        return 0;

    if (prev_move.is_castle) {
        // Test for an illegal castle. Note: if WHITE just castled, side_to_move is already BLACK,
        // so the "enemy" pawns are the color of the current side_to_move.
        const BitBoard& enemy_pawns = board_states[current_ply].bbs.pcs[make_piece(side_to_move, PAWN)];
        if (castled_through_check(prev_move, moves, enemy_pawns))
            return 0;
    }

    // Base case: reached a leaf.
    if (depth < 1) {
        // Count move types.
        if (prev_move.is_en_passant)
            num_en_passant++;
        else if (prev_move.is_castle) {
            num_castles++;
        } else if (prev_move.promote_to != NO_PIECE_TYPE)
            num_promotions++;
        return 1;
    }

    // Make all moves and compute perft recursively.
    int64 result = 0;
    int i = 0;
    while (!(moves[i].is_no_move())) {
        make_move(moves[i++]);
        result += perft(depth - 1, num_en_passant, num_castles, num_promotions);
        undo_move();
    }

    return result;
}

Move (&Board::GetPseudolegalMoves())[MAX_LEGAL_MOVES] {
    return pseudolegal_moves_[current_ply - search_start_ply];
}

void Board::print_moves() {
    auto& moves = pseudolegal_moves_[current_ply - search_start_ply];
    int i = 0;
    while (!(moves[i].is_no_move()))
        cout << move_str(moves[i++]) << "\n";
}