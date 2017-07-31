//
//  perft.cpp
//  CogChess
//
//  Created by Eric Nichols on 1/7/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//

#include <ctime>
#include <iostream>
#include <string>

#include "bitboard.h"
#include "board.h"

using std::cout;
using std::string;

// const string ENGINE_VERSION = "0.0.1";


int main(int argc, const char* argv[]) {
    // insert code here...
    cout << "\nCogChess *perft* " << ENGINE_VERSION << "\n";
    cout << "by Eric Nichols (epnichols@gmail.com)\n";
    cout << "Copyright © 2015-2016 Eric Nichols. All rights reserved.\n";
    cout << "--------------------------------------------------------\n\n";

    Board board;

    // Perft pos 1. start. CORRECT to ply 6
    // board.set_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");


    // Perft pos 2. kiwipete. CORRECT
    // board.set_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");


    // Perft pos 3. CORRECT
    // board.set_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");

    // 1  14        0   0
    // 2  191       0   0
    // 3  2812      2   0
    // 4  43238     123 0
    // 5  674624    1165   0
    // 6  11030083  33325  0
    // 7  178633661 294874 0


    // Perft pos 4.  CORRECT through depth 6
    // board.set_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

    // Perft pos 4b (reversed)  CORRECT through depth 5

    // board.set_fen("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");

    // Correct: nodes/ep/castles/promotions
    // Perft 1: 6 / 0 / 0 / 0
    // Perft 2: 264 / 0 / 6 / 48
    // Perft 3: 9467 / 4 / 0 / 120
    // Perft 4: 422333 (*got 424005) / 0 / 7795 / 60032 (*got 61704)


    // Perft pos 5. CORRECT through depth 5.
    // board.set_fen("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    // http://www.talkchess.com/forum/viewtopic.php?topic_view=threads&p=450621&t=42463&sid=230a04eae90aec35937fd0e16dc06505

    // Depth
    // Nodes
    // 1 // 44
    // 2 // 1,486
    // 3 // 62,379
    // 4 // 2,103,487
    // 5 // 89,941,194

    // Perft pos 6. CORRECT through ply 5.
    // board.set_fen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

    // Depth
    // Nodes
    // 0 // 1
    // 1 // 46
    // 2 // 2,079
    // 3 // 89,890
    // 4 // 3,894,594
    // 5 // 164,075,551
    // 6 // 6,923,051,137
    // 7 // 287,188,994,746
    // 8 // 11,923,589,843,526
    // 9 // 490,154,852,788,714


    // dad vs computer a2a3
    // board.set_fen("r3kb1r/2pq4/ppnpNn2/1Q2p3/7p/2N3B1/PPP1PPPP/1K1R1B1R b kq - 0 15");

    // Hangs program in alpha-beta.
    board.set_fen("3rr3/ppp4p/2n2n1k/2q5/6b1/1P4P1/P4PKP/1q4NR w - - 2 21");

    board.print();
    // board.print_misc_bbs();
    // board.print_piece_bbs();

    cout << "        # pos / #ep / # castles / # promotions\n";
    for (int i = 1; i <= 5; i++) {
        clock_t begin = clock();
        int64 num_en_passant = 0;
        int64 num_castles = 0;
        int64 num_promotions = 0;
        // int64 num_pos = board.perft_divide(i, num_en_passant, num_castles, num_promotions);
        int64 num_pos = board.perft(i, num_en_passant, num_castles, num_promotions);
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        double nps = num_pos / elapsed_secs;
        cout << "Perft " << i << ": " << num_pos << " / " << num_en_passant << " / " << num_castles << " / "
             << num_promotions << "\tTime: " << elapsed_secs << " sec (" << nps / 1000 << " kNps)\n";
    }

    printf("\n\nExiting CogChess.\n");

    return 0;
}
