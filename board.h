//
//  bitboard.h
//  CogChess
//
//  Created by Eric Nichols on 1/3/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//

#ifndef board_h
#define board_h

#include <string>
#include <vector>

#include "bitboard.h"
#include "types.h"

using std::string;
using std::vector;

const int MAX_PLY = 1024;
const int MAX_SEARCH_DEPTH = 40;
const int MAX_LEGAL_MOVES = 256; // https://chessprogramming.wikispaces.com/Chess+Position

// A set of bitboards describing the current position.
struct Bitboards {
    // Holds the piece information per square
    Piece squares[SQ_H8 + 2]; // TODO: dummy entry for SQ_NONE. Used only in perft, for pre_move at ply 0.

    // Pieces: one bitboard per piece per color.
    BitBoard pcs[B_KING + 1];

    // Misc bitboards.
    BitBoard occupied;
    BitBoard empty;
    BitBoard occupied_per_color[BLACK + 1];
};

// Stores critical information about the board. Must be restored after an undo-move operation.
struct BoardState {
    int64 hash;                    // Zorbist hash of the position.
    int castling_rights;           // composed of flags from the CastlingRight enum
    Square en_passant_square;      // the square skipped by the opposing pawn vulnerable to en passant
    int ply_since_move_or_capture; // 0 if prev_move was a pawn move or a capture.
    int repetitions;               // set to 0 the first time a position occurs. set to 1 for a later duplicate
                                   // position. Once reptitions==2, draw may be claimed.
    Move prev_move;                // The most recent move made.

    Bitboards bbs;
};

// Represents the current position, using bitboards. Includes pseudo-legal move generation,
// move/undo move functionality, and setup from a FEN string.
//
// Actual (non-pseudo-) legal moves are not generated directly by this class. Rather, after making a
// move, the position must be checked for a pseudo-legal king capture. If such a capture exists, the
// move was not legal.
class Board {
  public:
    Color side_to_move;
    Color side_to_move_at_search_root;

    int current_ply;      // 0 in the initial game state. 1 after white moves.
    int search_start_ply; // update before a new search to track moves stack.
    Board();
    ~Board(){};

    int set_fen(string aFEN);
    void initialize_bitboards();

    // vector<Move> generate_pseudolegal_moves();
    void generate_pseudolegal_moves();
    Move (&GetPseudolegalMoves())[MAX_LEGAL_MOVES];
    int64 compute_full_zorbist_hash();

    void make_move(Move move);
    void undo_move();
    // bool can_capture_king(vector<Move> pseudolegal_moves);
    bool can_capture_king(const Move (&moves)[MAX_LEGAL_MOVES], const Square& king_square);
    bool castled_through_check(const Move& prev_move, const Move (&moves)[MAX_LEGAL_MOVES], const BitBoard& enemy_pawns);

    int64 perft(int depth, int64& num_en_passant, int64& num_castles, int64& num_promotions);
    int64 perft_divide(int depth, int64& num_en_passant, int64& num_castles, int64& num_promotions);

    void print();
    void print_piece_bbs();
    void print_misc_bbs();
    void print_moves();

    Move choose_move(); // TODO: set search_start_ply = current_ply at start!!
    Score negamax(int depth, Score alpha, Score beta, Move* best_move);
    Score eval();

    bool is_in_check(const Square& king_square);

    inline Square get_king_square(Color color, const Bitboards& bbs) {
        return bitboard::lsb(bbs.pcs[make_piece(color, KING)]);
    }

  private:
    Score quiesce(Score alpha, Score beta, Move* best_move);
    int num_nodes_visited_;
    Move pseudolegal_moves_[MAX_SEARCH_DEPTH][MAX_LEGAL_MOVES];

    BoardState board_states[MAX_PLY]; // Stack of states, indexed by ply.
                                      // Current state is at board_states[current_ply].

    void initialize_board_state(int castling_rights, Square en_passant_square);


    // int64 add_zorbist_hash(int64 incremental_zorbist_hash);

    // Move generation
    BitBoard getRayAttacksPositive(Square sq, Direction dir8);
    BitBoard getRayAttacksNegative(Square sq, Direction dir8);
};

#endif /* board_h */
