//
//  main.cpp
//  CogChess
//
//  Created by Eric Nichols on 12/31/15.
//  Copyright © 2015-2016 Eric Nichols. All rights reserved.
//

#include <iostream>
#include <string>

#include "bitboard.h"
#include "board.h"
#include "uci.h"

using std::cout;
using std::string;


int main(int argc, const char* argv[]) {
    // std::cout.setf(std::ios_base::unitbuf);

    // insert code here...
    /*cout << "\nCogChess " << ENGINE_VERSION << "\n";
    cout << "by Eric Nichols (epnichols@gmail.com)\n";
    cout << "Copyright © 2015-2016 Eric Nichols. All rights reserved.\n";
    cout << "--------------------------------------------------------\n\n";
*/

    UciInterface uci;

    uci.main_loop();


    // Board board;

    // board.set_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");
    // board.set_fen("r3kb1r/2pq4/ppnpNn2/1Q2p3/7p/2N3B1/PPP1PPPP/1K1R1B1R b kq - 0 15");

    // board.set_fen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    // board.set_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");


    // board.print();
    // board.print_piece_bbs();
    // board.print_misc_bbs();

    //    BitBoard x = (BitBoard)(BB(65280)) >> (Square)1;
    //    printf("\n%llu", x);

    // printf("\nZorbist hash of initial position = %llu00\n", board.compute_full_zorbist_hash());


    // board.generate_pseudolegal_moves();

    /*
    board.make_move(Move(SQ_C3, SQ_B1));
    board.print();
    board.make_move(Move(SQ_C6, SQ_B4));
    board.print();
    board.make_move(Move(SQ_B1, SQ_D2));
    board.print();
    // board.make_move(Move(SQ_B4, SQ_C2));
    // board.print();
    board.print_misc_bbs();
    board.print_piece_bbs();
    printf("\nZorbist hash = %llu00\n", board.compute_full_zorbist_hash());
    cout << "Zorbist hash = 0x" << std::hex << board.compute_full_zorbist_hash();
    */

    // Print moves.
    // board.generate_pseudolegal_moves();
    // board.print_moves();
    // board.print_misc_bbs();

    printf("\n\nExiting CogChess.\n");

    return 0;
}
