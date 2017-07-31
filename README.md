# cogchess
## An efficient bitboard-based move- and game-tree-generator for chess

Most of this is my original work, but some bits of code are taken from Stockfish as I was learning how to do things like call popcount from C++.

## Perft
A "perft" program is included to verify that move generation and tree search works correctly; this was invaluable for debugging the bitboard move generation code.

Build perft as follows:

clang++ -o perft -O3 -std=c++11 bitboard.cpp board.cpp consts.cpp fen.cpp genmove.cpp perft.cpp pystring.cpp types.cpp 

## Engine
A UCI chess engine is also included. Build rules will follow once it is complete; at present it has some bugs in the alpha-beta search.

Eventually the plan is to integrate the basic move generation logic here with a cognitively-inspired main engine, instead of using brute-force search. 
