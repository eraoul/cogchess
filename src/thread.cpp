//
//  thread.cpp
//  CogChess
//
//  Created by Eric Nichols on 1/12/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//

#include <future>
#include <iostream>
#include <thread>

#include "board.h"
#include "thread.h"
#include "types.h"

// using std::thread;

using std::cout;
using std::endl;

MakeMoveThread::MakeMoveThread(Board& board) {
    board_ = board;
}

void MakeMoveThread::go() {
    // thread t(choose_move());
    // t.join();


    auto ret = std::async(std::launch::async, &MakeMoveThread::choose_move, this);
    Move move = ret.get();


    // Move move = board_.choose_move();


    // int value = 50; // TODO

    cout << "bestmove " << move_str_uci(move) << endl;
}

/*void MakeMoveThread::choose_move(std::promise<Move>&& p) {
    p.set_value(Move(SQ_E2, SQ_E4));
}*/

Move MakeMoveThread::choose_move() {
    return board_.choose_move();
}