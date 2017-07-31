//
//  uci.h
//  CogChess
//
//  Created by Eric Nichols on 1/11/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//

#ifndef uci_h
#define uci_h

#include <stdio.h>

#include "board.h"
#include "types.h"

class UciInterface {

  public:
    UciInterface();
    void main_loop();

  private:
    Board board;
};

#endif /* uci_h */
