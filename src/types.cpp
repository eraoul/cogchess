//
//  types.cpp
//  CogChess
//
//  Created by Eric Nichols on 1/3/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//


#include <iostream>
#include <stdio.h>
#include <string>

#include "board.h"
#include "types.h"


using std::cout;
using std::string;

char piece_char(PieceType pt) {
    if (pt == NO_PIECE_TYPE)
        return ' ';

    switch (pt) {
        case PAWN:
            return 'P';
        case KNIGHT:
            return 'N';
        case BISHOP:
            return 'B';
        case ROOK:
            return 'R';
        case QUEEN:
            return 'Q';
        case KING:
            return 'K';
        default:
            return ' ';
    }
}

char piece_char(Piece piece) {
    PieceType pt = type_of(piece);
    // clang-format off
    if (pt == NO_PIECE_TYPE)  return ' ';
    
    if (color_of(piece) == WHITE) {
        switch (pt) {
            case PAWN:          return 'P';
            case KNIGHT:        return 'N';
            case BISHOP:        return 'B';
            case ROOK:          return 'R';
            case QUEEN:         return 'Q';
            case KING:          return 'K';
            case NO_PIECE_TYPE: return ' ';
            default:            return '?';
        }
    } else {
        switch (pt) {
            case PAWN:          return 'p';
            case KNIGHT:        return 'n';
            case BISHOP:        return 'b';
            case ROOK:          return 'r';
            case QUEEN:         return 'q';
            case KING:          return 'k';
            case NO_PIECE_TYPE: return ' ';
            default:            return '?';
        }
        // clang-format on
    }
}

string move_str(Move move) {
    if (move.is_castle) {
        if (file_of(move.to) == FILE_G)
            return "O-O";
        else
            return "O-O-O";
    }
    string s = square_str(move.from);
    s += '-';
    s += square_str(move.to);

    if (move.is_en_passant)
        s += " e.p.";
    if (move.promote_to != NO_PIECE_TYPE) {
        s += " (";
        s += piece_char(move.promote_to);
        s += ")";
    }

    return s;
}

string move_str_uci(Move move) {
    string s = square_str(move.from);
    s += square_str(move.to);
    if (move.promote_to != NO_PIECE_TYPE) {
        s += piece_char(move.promote_to);
    }
    return s;
}

string square_str(Square sq) {
    if (sq == SQ_NONE)
        return "None";

    string s = "";
    s += 'a' + file_of(sq);
    s += '1' + rank_of(sq);
    return s;
}

Move::Move(const string& move_str, Board& board) {
    assert(move_str.length() >= 4);

    char f1 = move_str[0];
    char r1 = move_str[1];
    char f2 = move_str[2];
    char r2 = move_str[3];
    from = Square((f1 - 'a') + (r1 - '1') * 8);
    to = Square((f2 - 'a') + (r2 - '1') * 8);
    if (move_str.length() == 5) {
        char promote = move_str[4];
        switch (promote) {
            case 'q':
                promote_to = QUEEN;
                break;
            case 'r':
                promote_to = ROOK;
                break;
            case 'n':
                promote_to = KNIGHT;
                break;
            case 'b':
                promote_to = BISHOP;
                break;
            default:
                assert(false);
        }
    }
    board.generate_pseudolegal_moves();
    auto& moves = board.GetPseudolegalMoves();
    int i = 0;
    while (!(moves[i].is_no_move())) {
        const Move& move = moves[i++];
        if (move.from == from && move.to == to) {
            is_castle = move.is_castle;
            is_en_passant = move.is_en_passant;
            return;
        }
    }
    cout << "illegal move: " << move_str << "\n";
    assert(false);
}