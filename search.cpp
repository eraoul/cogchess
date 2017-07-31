//
//  search.cpp
//  CogChess
//
//  Created by Eric Nichols on 1/12/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//

#include <iostream>

#include "bitboard.h"
#include "board.h"
#include "consts.h"
#include "search.h"
#include "types.h"

using std::cout;
using std::endl;

Move Board::choose_move() {
    search_start_ply = current_ply;
    side_to_move_at_search_root = side_to_move;
    num_nodes_visited_ = 0;
    Move move;
    Score score = negamax(CUR_SEARCH_DEPTH, -99999, 99999, &move); // TODO: +/- inf
    cout << "info score " << score << endl;
    return move;
}

bool Board::is_in_check(const Square& king_square) {
    make_move(NULL_MOVE);
    generate_pseudolegal_moves();
    auto& moves = pseudolegal_moves_[current_ply - search_start_ply];
    const bool in_check = can_capture_king(moves, king_square);
    undo_move();
    return in_check;
}

Score Board::negamax(int depth, Score alpha, Score beta, Move* best_move) {

    // Test for illegal incoming move.
    Move& prev_move = board_states[current_ply].prev_move;
    Color other_color = (side_to_move == WHITE) ? BLACK : WHITE;
    generate_pseudolegal_moves();
    auto& moves = pseudolegal_moves_[current_ply - search_start_ply];

    // Check for king capture.
    const Square& other_king_square = get_king_square(other_color, board_states[current_ply].bbs);
    if (can_capture_king(moves, other_king_square)) {
        *best_move = ILLEGAL_MOVE;
        return 0;
    }

    if (prev_move.is_castle) {
        // Test for an illegal castle. Note: if WHITE just castled, side_to_move is already BLACK,
        // so the "enemy" pawns are the color of the current side_to_move.
        const BitBoard& enemy_pawns = board_states[current_ply].bbs.pcs[make_piece(side_to_move, PAWN)];
        if (castled_through_check(prev_move, moves, enemy_pawns)) {
            *best_move = ILLEGAL_MOVE;

            return 0;
        }
    }

    num_nodes_visited_++;

    if (depth < 1) {
        return quiesce(alpha, beta, best_move);
    }

    Move argmax = NO_MOVE;
    // score_type = SCORE_TYPE_ALPHA;

    int i = 0;
    bool found_legal = false;
    // bool found_king_capture = false;
    while (!(moves[i].is_no_move())) {
        const Move& move = moves[i++];
        // if (!found_king_capture && move.to == king_square)
        //    found_king_capture = true;
        make_move(move);
        Move new_best_move;
        Score val = -negamax(depth - 1, -beta, -alpha, &new_best_move);
        undo_move();
        if (new_best_move == ILLEGAL_MOVE)
            continue;
        if (!found_legal) {
            argmax = move;
            found_legal = true;
        }

        // Beta cutoff.
        if (val >= beta) {
            // record_hash(full_hash, hash_key, depth, SCORE_TYPE_BETA, beta, move);
            *best_move = move;
            return beta;
        }

        // Alpha update.
        if (val > alpha) {
            // score_type = SCORE_TYPE_EXACT;
            argmax = move;
            alpha = val;
        }
        if (depth == CUR_SEARCH_DEPTH) {
            cout << "multipv 1 depth " << depth << " score cp " << alpha << " pv " << move_str_uci(argmax) << endl;
        }
    }
    if (found_legal) {
        *best_move = argmax;
        return alpha;
    }

    // No legal moves (checkmate or stalemate)
    // Check to see if king is in check.
    const Square king_square = get_king_square(side_to_move, board_states[current_ply].bbs);
    if (is_in_check(king_square))
        return -9999 - depth; // TODO: all this checkmate stuff: fix.

    // Stalemate.
    return 0;
}

Score Board::quiesce(Score alpha, Score beta, Move* best_move) {
    // TODO
    return eval();
}

Score eval_material(const Bitboards& bbs, Color color) {
    const int pawns = bitboard::popcount(bbs.pcs[make_piece(color, PAWN)]);
    const int knights = bitboard::popcount(bbs.pcs[make_piece(color, KNIGHT)]);
    const int bishops = bitboard::popcount(bbs.pcs[make_piece(color, BISHOP)]);
    const int rooks = bitboard::popcount(bbs.pcs[make_piece(color, ROOK)]);
    const int queens = bitboard::popcount(bbs.pcs[make_piece(color, QUEEN)]);

    return pawns * 100 + knights * 300 + bishops * 320 + rooks * 500 + queens * 910;
}

Score eval_position(const Bitboards& bbs, Color color) {
    // Test: move to g-file!
    return 500 * bitboard::popcount(bbs.occupied_per_color[color] & FILE_G_ALL);
    // return 0;
}


// Returns eval score (in centipawns) from white's point of view.
Score Board::eval() {
    const BoardState& board_state = board_states[current_ply];
    const Bitboards& bbs = board_state.bbs;
    const Score score_material = eval_material(bbs, WHITE) - eval_material(bbs, BLACK);
    const Score score_position = eval_position(bbs, WHITE) - eval_position(bbs, BLACK);
    const Score score = score_material + score_position;
    return (side_to_move == WHITE) ? score : -score;
}
