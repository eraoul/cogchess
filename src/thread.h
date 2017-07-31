//
//  thread.h
//  CogChess
//
//  Created by Eric Nichols on 1/12/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//

#ifndef thread_h
#define thread_h

#include <stdio.h>
#include <thread>

#include "board.h"
#include "types.h"

class Thread {
  public:
    Thread(){};
    virtual void go() = 0;
};

class MakeMoveThread : Thread {
  public:
    MakeMoveThread(){};
    MakeMoveThread(Board& board);
    void go();

  private:
    Board board_;
    Move choose_move();

    // void choose_move(std::promise<Move>&& p);
};

#endif /* thread_h */
