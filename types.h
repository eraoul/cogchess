//  types.h
//  CogChess
//
//  Created by Eric Nichols (epnichols@gmail.com) on 1/3/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//
//  Inspired by Stockfish:
//  https://github.com/official-stockfish/Stockfish/blob/master/src/types.h

#ifndef TYPES_H
#define TYPES_H

#include <cassert>
#include <string>

using std::string;

// BitBoard = unsigned int64
// typedef uint64_t BitBoard;
// typedef uint64_t int64;
typedef unsigned long long BitBoard;
typedef unsigned long long int64;

typedef int Score;

// Macro to define a bitboard const.
#define BB(x) x##ULL
#define BB_FULL BB(0xFFFFFFFFFFFFFFFF)
#define BB_EMPTY BB(0)
#define BB_H8 BB(0x8000000000000000)
#define BB_A1 BB(0x0000000000000001)


///////////////////////////////////////

// Square Identifiers
enum Square {
    // clang-format off
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_NONE
    // clang-format on
};

// Piece Identifiers
enum PieceType {
    // clang-format off
    NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    ALL_PIECES = 0
    // clang-format on
};

enum Piece {
    // clang-format off
    NO_PIECE,
    W_PAWN = 1, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN = 9, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING
    // clang-format on
};

class Board;

struct Move {
    Square from;
    Square to;
    PieceType promote_to;
    bool is_castle;
    bool is_en_passant;

    Move(){};

    Move(Square from, Square to, PieceType promote_to = NO_PIECE_TYPE, bool is_castle = false, bool is_en_passant = false)
    : from(from), to(to), promote_to(promote_to), is_castle(is_castle), is_en_passant(is_en_passant){};

    // Move from UCI string. Requires current board state to parse castle and e.p. moves.
    Move(const string& move_str, Board& board);

    inline bool is_no_move() const {
        return from == SQ_A1 && to == SQ_A1;
    }

    bool operator==(const Move& rhs) {
        return rhs.from == this->from && rhs.to == this->to && rhs.promote_to == this->promote_to;
    }
};


const Move NO_MOVE = Move(SQ_A1, SQ_A1);   // end of move list
const Move NULL_MOVE = Move(SQ_H8, SQ_H8); // no-op, no-nothing move
const Move ILLEGAL_MOVE = Move(SQ_E1, SQ_NONE);
const Move KING_CAPTURE = Move(SQ_D8, SQ_NONE);

enum File { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, NO_FILE };

enum Rank { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 };

// Ordered this way because the first 4 directions here are positive on a bitboard; while the last 4 are negative.
enum Direction { DIR_NW, DIR_N, DIR_NE, DIR_E, DIR_SE, DIR_S, DIR_SW, DIR_W, DIR_END };

enum CastlingRight {
    NO_CASTLING,
    WHITE_OO,
    WHITE_OOO = WHITE_OO << 1,
    BLACK_OO = WHITE_OO << 2,
    BLACK_OOO = WHITE_OO << 3,
    ANY_CASTLING = WHITE_OO | WHITE_OOO | BLACK_OO | BLACK_OOO
};

const int NOT_WHITE_OO = ANY_CASTLING ^ WHITE_OO;
const int NOT_WHITE_OOO = ANY_CASTLING ^ WHITE_OOO;
const int NOT_BLACK_OO = ANY_CASTLING ^ BLACK_OO;
const int NOT_BLACK_OOO = ANY_CASTLING ^ BLACK_OOO;

// Piece/player colors
enum Color { WHITE, BLACK, NO_COLOR };

// Square and piece inlines and macros from Stockfish:
// https://github.com/official-stockfish/Stockfish/blob/master/src/types.h)
inline Square make_square(File f, Rank r) {
    return Square((r << 3) | f);
}

inline Piece make_piece(Color c, PieceType pt) {
    return Piece((c << 3) | pt);
}

inline PieceType type_of(Piece pc) {
    return PieceType(pc & 7);
}

inline Color color_of(Piece pc) {
    assert(pc != NO_PIECE);
    return Color(pc >> 3);
}

inline bool is_ok(Square s) {
    return s >= SQ_A1 && s <= SQ_H8;
}

inline File file_of(Square s) {
    return File(s & 7);
}

inline Rank rank_of(Square s) {
    return Rank(s >> 3);
}

inline Square relative_square(Color c, Square s) {
    return Square(s ^ (c * 56));
}

inline Rank relative_rank(Color c, Rank r) {
    return Rank(r ^ (c * 7));
}

inline Rank relative_rank(Color c, Square s) {
    return relative_rank(c, rank_of(s));
}

inline bool opposite_colors(Square s1, Square s2) {
    int s = int(s1) ^ int(s2);
    return ((s >> 3) ^ s) & 1;
}

// clang-format off
#define ENABLE_BASE_OPERATORS_ON(T)                             \
inline T operator+(T d1, T d2) { return T(int(d1) + int(d2)); } \
inline T operator-(T d1, T d2) { return T(int(d1) - int(d2)); } \
inline T operator*(int i, T d) { return T(i * int(d)); }        \
inline T operator*(T d, int i) { return T(int(d) * i); }        \
inline T operator-(T d) { return T(-int(d)); }                  \
inline T& operator+=(T& d1, T d2) { return d1 = d1 + d2; }      \
inline T& operator-=(T& d1, T d2) { return d1 = d1 - d2; }      \
inline T& operator*=(T& d, int i) { return d = T(int(d) * i); }

#define ENABLE_FULL_OPERATORS_ON(T)                             \
ENABLE_BASE_OPERATORS_ON(T)                                     \
inline T& operator++(T& d) { return d = T(int(d) + 1); }        \
inline T& operator--(T& d) { return d = T(int(d) - 1); }        \
inline T operator++(T& d, int)  {T old = d; d = T(int(d) + 1); return old; } \
inline T operator--(T& d, int)  {T old = d; d = T(int(d) - 1); return old; } \
inline T operator/(T d, int i) { return T(int(d) / i); }        \
inline int operator/(T d1, T d2) { return int(d1) / int(d2); }  \
inline T& operator/=(T& d, int i) { return d = T(int(d) / i); }
// clang-format off

ENABLE_FULL_OPERATORS_ON(PieceType)
ENABLE_FULL_OPERATORS_ON(Piece)
ENABLE_FULL_OPERATORS_ON(Color)
ENABLE_FULL_OPERATORS_ON(Square)
ENABLE_FULL_OPERATORS_ON(File)
ENABLE_FULL_OPERATORS_ON(Rank)
ENABLE_FULL_OPERATORS_ON(Direction)


#undef ENABLE_FULL_OPERATORS_ON
#undef ENABLE_BASE_OPERATORS_ON

char piece_char(Piece piece);
string move_str(Move move);
string move_str_uci(Move move);
string square_str(Square sq);

#endif  // # TYPES_H