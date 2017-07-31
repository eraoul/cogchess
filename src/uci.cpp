//
//  uci.cpp
//  CogChess
//
//  Created by Eric Nichols on 1/11/16.
//  Copyright © 2016 Eric Nichols. All rights reserved.
//

#include <iostream>
#include <sstream>

#include "consts.h"
#include "pystring.h"
#include "thread.h"
#include "uci.h"

using std::cout;
using std::cin;
using std::endl;
using pystring::split;

UciInterface::UciInterface() {
}

void UciInterface::main_loop() {

    MakeMoveThread make_move_thread;

    // cout << "Waiting for UCI commands.\n\n";

    while (true) {
        // cout << "% ";
        string command;
        std::getline(cin, command);

        // Parse command.
        vector<string> tokens;
        pystring::split(command, tokens);
        const string& cmd1 = tokens[0];

        if (tokens.size() < 1)
            continue;

        if (cmd1 == "quit")
            break;
        if (cmd1 == "uci") {
            cout << "id name " << ENGINE_NAME << " " << ENGINE_VERSION << endl;
            cout << "id author " << ENGINE_AUTHOR << endl;
            cout << "option name Ponder type check default false" << endl;
            cout << "option name Hash type spin default 1 min 1 max 128" << endl;
            cout << "option name Style type combo default Normal var Solid var Normal var Risky" << endl;
            cout << "uciok" << endl;
            continue;
        }

        if (cmd1 == "isready") {
            cout << "readyok" << endl;
            continue;
        }
        if (cmd1 == "print") {
            board.print();
            continue;
        }
        if (cmd1 == "position") {
            if (tokens.size() < 2) {
                cout << "Must have argument after 'position'\n";
                continue;
            } else {
                const string& cmd2 = tokens[1];
                if (cmd2 == "startpos") {
                    board = Board();
                    // handle optional "moves" argument in tokens[3]
                    if (tokens.size() > 3) {
                        if (tokens[2] == "moves") {
                            for (int i = 3; i < tokens.size(); i++) {
                                const string& move_str = tokens[i];
                                board.make_move(Move(move_str, board));
                            }
                        }
                    }
                } else if (cmd2 == "fen") {
                    cout << "Setting FEN position: \n";
                    if (tokens.size() < 8) {
                        cout << "must have a FEN string after 'position fen'\n";
                        continue;
                    }
                    std::stringstream fen;
                    fen << tokens[2] << " " << tokens[3] << " " << tokens[4] << " " << tokens[5] << " " << tokens[6]
                        << " " << tokens[7];
                    board = Board();
                    board.set_fen(fen.str());
                } else {
                    cout << "Error: unknown command " << cmd2 << "\n";
                    continue;
                }
                cout << "isready" << endl;
            }
        }
        if (cmd1 == "ucinewgame") {
            board = Board();
            continue;
        }

        if (cmd1 == "setoption") {
            // TODO.
            continue;
        }

        if (cmd1 == "go") {
            // TODO.

            cout << "info Starting engine " << ENGINE_NAME << endl;

            make_move_thread = MakeMoveThread(board);
            make_move_thread.go();
            continue;
        }
    }
}