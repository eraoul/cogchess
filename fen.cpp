#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "bitboard.h"
#include "board.h"
#include "pystring.h"
#include "types.h"


using std::cout;
using std::endl;
using std::string;
using std::vector;

// Initial chess position:
// FEN: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

// Reset the board to the given FEN state.
int Board::set_fen(string aFEN) {

    BoardState board_state;
    Bitboards& bbs = board_state.bbs;

    unsigned int i, j;
    Square sq;
    char letter;
    int aRank, aFile;
    vector<string> strList;
    pystring::split(aFEN, strList, " ");

    /*for (const auto& s : strList) {
        cout << "strList: " << s << endl;
    }*/

    // Empty the board quares
    for (sq = SQ_A1; sq <= SQ_NONE; sq++)
        bbs.squares[sq] = NO_PIECE;
    // read the board - translate each loop idx into a square
    j = 1;
    i = 0;
    while ((j <= 64) && (i <= strList[0].length())) {
        letter = strList[0].at(i);
        i++;
        aFile = 1 + ((j - 1) % 8);
        aRank = 8 - ((j - 1) / 8);
        sq = (Square)(((aRank - 1) * 8) + (aFile - 1));
        // clang-format off
        switch(letter) {
            case 'p' : bbs.squares[sq] = B_PAWN; break;
            case 'r' : bbs.squares[sq] = B_ROOK; break;
            case 'n' : bbs.squares[sq] = B_KNIGHT; break;
            case 'b' : bbs.squares[sq] = B_BISHOP; break;
            case 'q' : bbs.squares[sq] = B_QUEEN; break;
            case 'k' : bbs.squares[sq] = B_KING; break;
            case 'P' : bbs.squares[sq] = W_PAWN; break;
            case 'R' : bbs.squares[sq] = W_ROOK; break;
            case 'N' : bbs.squares[sq] = W_KNIGHT; break;
            case 'B' : bbs.squares[sq] = W_BISHOP; break;
            case 'Q' : bbs.squares[sq] = W_QUEEN; break;
            case 'K' : bbs.squares[sq] = W_KING; break;
            case '/' : j--; break;
            case '1' : break;
            case '2' : j++; break;
            case '3' : j+=2; break;
            case '4' : j+=3; break;
            case '5' : j+=4; break;
            case '6' : j+=5; break;
            case '7' : j+=6; break;
            case '8' : j+=7; break;
            default: return -1;
        }
        // clang-format on
        j++;
    }

    // So here the variable i marks the current position in the FEN string. The
    // variable j walks through the board in the direction the FEN squares occur
    // (A8 .. H1). Next we set the turn which is stored in the 2nd substring

    // set the turn; default = White
    side_to_move = WHITE;

    // cout << "strList[1]: " << strList[1] << endl;

    if (strList.size() >= 2) {
        if (strList[1].c_str()[0] == 'w')
            side_to_move = WHITE;
        else if (strList[1].c_str()[0] == 'b')
            side_to_move = BLACK;
        else
            return -1;
    }

    // Next the casteling rights. For efficience we store all casteling rights
    // into a single integer where different bits indicate a certain right is
    // there (bit is 1) or right is missing (bit is 0). For bit manipulating we
    // rely on small inline functions that do just set the correct bit to 1.

    // set boardstate to initial 0
    int castle_rights = NO_CASTLING;
    // Initialize all castle possibilities
    if (strList.size() >= 3) {
        if (strList[2].find('K') != string::npos)
            castle_rights |= WHITE_OO;
        if (strList[2].find('Q') != string::npos)
            castle_rights |= WHITE_OOO;
        if (strList[2].find('k') != string::npos)
            castle_rights |= BLACK_OO;
        if (strList[2].find('q') != string::npos)
            castle_rights |= BLACK_OOO;
    }

    // The next sub string stands for a possible en passant square. The square is
    // initialized to the square "SQ_NONE".
    // read en passent and save it into "sq" Default := None
    sq = SQ_NONE;
    if ((strList.size() >= 4) && (strList[3].length() >= 2)) {
        if ((strList[3].at(0) >= 'a') && (strList[3].at(0) <= 'h') && ((strList[3].at(1) == '3') || (strList[3].at(1) == '6'))) {
            aFile = strList[3].at(0) - 96; // ASCII 'a' = 97
            aRank = strList[3].at(1) - 48; // ASCII '1' = 49
            sq = (Square)((aRank - 1) * 8 + aFile - 1);
        } else
            return -1;
    }
    if (sq == 0)
        sq = SQ_NONE;

    // Finally we retrieve the turn number

    // we start at ply 0 by default
    current_ply = 0;
    search_start_ply = 0;
    if (strList.size() >= 6) {
        current_ply = 2 * (atoi(strList[5].c_str()) - 1) + 1;
        if (current_ply < 0)
            current_ply = 0; // avoid possible underflow
        if (side_to_move == BLACK)
            current_ply++;
    }
    search_start_ply = current_ply;
    board_states[current_ply] = board_state; // copy the board state to the correct ply.

    initialize_board_state(castle_rights, sq);
    initialize_bitboards();
    return 0;
}

// Please notice the final call to initializeBitBoards(). This call synchronizes
// the piece BitBoards with the contents of "squares" that we just set. It is
// very importatnt that the square centric representation and the BitBoard
// representation is always in sync.

/*************************************************
initialize the bitboards with the contents of
bbs.squares
**************************************************/
void Board::initialize_bitboards() {
    Bitboards& bbs = board_states[current_ply].bbs;

    Square sq;
    memset(&bbs.pcs, 0, sizeof(bbs.pcs));
    // initialize the piece bit boards
    for (sq = SQ_A1; sq <= SQ_H8; sq++) {
        // bbs.pcs[bbs.squares[sq]] |= BB_SQUARES[sq];
        bbs.pcs[bbs.squares[sq]] |= BB(1) << sq;
    }
    // calculate the utility bitboards
    bbs.occupied_per_color[WHITE] = bbs.pcs[W_PAWN] | bbs.pcs[W_KNIGHT] | bbs.pcs[W_BISHOP] | bbs.pcs[W_ROOK] |
                                    bbs.pcs[W_QUEEN] | bbs.pcs[W_KING];
    bbs.occupied_per_color[BLACK] = bbs.pcs[B_PAWN] | bbs.pcs[B_KNIGHT] | bbs.pcs[B_BISHOP] | bbs.pcs[B_ROOK] |
                                    bbs.pcs[B_QUEEN] | bbs.pcs[B_KING];
    bbs.occupied = bbs.occupied_per_color[WHITE] | bbs.occupied_per_color[BLACK];
    bbs.empty = ~bbs.occupied;
}