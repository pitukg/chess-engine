#include "board.h"
#include "evaluation.h"


// returns the bit at square-th position // only print uses this
short int bb_get_bit(U64 * bitboard, SquareCode square) {
    return ((*bitboard & (1ll << square)) != 0);
}

// sets the bit at square-th position to 1
inline void bb_set_true(U64 * bitboard, SquareCode square) {
    *bitboard |= (1ll << square);
}
// sets the bit at square-th position to 0
inline void bb_set_false(U64 * bitboard, SquareCode square) {
    *bitboard &= ~(1ll << square);
}


// function for adding a piece to the board
void add_piece(Board * board, PieceType pt, SquareCode square) {
    Color col = (pt == wPawn || pt == wKing || pt == wQueen || pt == wRook || pt == wBishop || pt == wKnight) ? WHITE : BLACK; // TODO: refactor
    board -> pieceCode[ square ] = pt;
    bb_set_true( &(board -> pieceBB[ pt ]), square );
    bb_set_true( &(board -> occupied), square );
    bb_set_true( &(board -> piecesByColor[col]), square );
    // update board score
    board->score += pieceScore[ pt ];
    board->score += pieceSquareTable[ pt ][ square ];
    if (pt == wKing) board -> kingSquare[WHITE] = square;
    else if (pt == bKing) board -> kingSquare[BLACK] = square;
}

// function for removing a piece from the board
void remove_piece(Board * board, PieceType pt, SquareCode square) {
    Color col = (pt == wPawn || pt == wKing || pt == wQueen || pt == wRook || pt == wBishop || pt == wKnight) ? WHITE : BLACK; // TODO: refactor
    board -> pieceCode[ square ] = none;
    bb_set_false( &(board -> pieceBB[ pt ]), square );
    bb_set_false( &(board -> occupied), square );
    bb_set_false( &(board -> piecesByColor[col]), square );
    // update board score
    board->score -= pieceScore[ pt ];
    board->score -= pieceSquareTable[ pt ][ square ];
}


inline U64 bb_move_N(U64 b) {return (b << 8);}
inline U64 bb_move_NE(U64 b) {return ((b & notHfile) << 9);}
inline U64 bb_move_E(U64 b) {return ((b & notHfile) << 1);}
inline U64 bb_move_SE(U64 b) {return ((b & notHfile) >> 7);}
inline U64 bb_move_S(U64 b) {return (b >> 8);}
inline U64 bb_move_SW(U64 b) {return ((b & notAfile) >> 9);}
inline U64 bb_move_W(U64 b) {return ((b & notAfile) >> 1);}
inline U64 bb_move_NW(U64 b) {return ((b & notAfile) << 7);}




SquareCode bitscan_forward(U64 table) {//make sure table != 0

    U64 LSB = table & ( - table );
    int idx = 0;
    U64 bit = 1ll;
    while (!(LSB & bit)) {
        ++idx;
        bit <<= 1;
    }
    return idx;
}


// returns the square code of the 1 in the most significant position
SquareCode bitscan_reverse(U64 table) {//make sure table != 0

    U64 MSB = table;
    for (int i = 1; i < 64; i *= 2)
        MSB |= (MSB >> i);//(unsigned)MSB volt ?? de igy jo talan
    MSB++;
    int idx = 0;
    U64 bit = 2ll;
    while (!(MSB & bit)) {
        ++idx;
        bit <<= 1;
    }
    return idx;
}



void init_board(Board *board) {
//    board->meta.kingsideCastlingRight[WHITE] = board->meta.queensideCastlingRight[WHITE] = 
  //      board->meta.kingsideCastlingRight[BLACK] = board->meta.queensideCastlingRight[BLACK] = 1;
    board->occupied = board->piecesByColor[WHITE] = board->piecesByColor[BLACK] = 0ll;
    for (SquareCode s = a1; s <= h8; ++s) board->pieceCode[s] = none;
    for (PieceType pt = wPawn; pt <= none; ++pt) board->pieceBB[pt] = 0ll;
    // ...
    board->meta.kingsideCastlingRight[WHITE] = board->meta.kingsideCastlingRight[BLACK]
        = board->meta.queensideCastlingRight[WHITE] = board->meta.queensideCastlingRight[BLACK] = 0;
    board->meta.halfmoveClock = 0;
    board->meta.fullmoveCounter = 1;
    board->score = 0;
}

int is_digit(char c) {
    return ('0' <= c && c <= '9');
}
int digit_to_int(char c) {
    return (c - '0');
}


Board parse_FEN(char *FEN) {

    Board board;
    init_board(&board);

    int it = 0;

    // parse piece placement
    for (int firstSquareInRank = a8; firstSquareInRank >= 0; firstSquareInRank -= 8, ++it) {
        SquareCode square = firstSquareInRank;
        for (; FEN[it] != '/' && FEN[it] != ' '; ++square, ++it) {

            if (is_digit(FEN[it])) {
                square += digit_to_int(FEN[it])-1;
            }
            else {
                PieceType pt;
                switch (FEN[it]) {
                    case 'P':  pt = wPawn; break;
                    case 'p':  pt = bPawn; break;
                    case 'N':  pt = wKnight; break;
                    case 'n':  pt = bKnight; break;
                    case 'B':  pt = wBishop; break;
                    case 'b':  pt = bBishop; break;
                    case 'R':  pt = wRook; break;
                    case 'r':  pt = bRook; break;
                    case 'Q':  pt = wQueen; break;
                    case 'q':  pt = bQueen; break;
                    case 'K':  pt = wKing; break;
                    case 'k':  pt = bKing; break;
                }

                add_piece(&board, pt, square);
            }

        }
        // if (square % 8) return FALSE_FEN;
    }

    // parse side to move
    board.meta.sideToMove = (FEN[it++] == 'w') ? WHITE : BLACK;

    ++it;
    // parse castling rights
    for (; FEN[it] != ' '; ++it) {
        switch (FEN[it]) {
            case 'K': board.meta.kingsideCastlingRight[WHITE] = 1; break;
            case 'k': board.meta.kingsideCastlingRight[BLACK] = 1; break;
            case 'Q': board.meta.queensideCastlingRight[WHITE] = 1; break;
            case 'q': board.meta.queensideCastlingRight[BLACK] = 1; break;
        }
    }
    ++it;

    // parse en passant target squares
    if (FEN[it] == '-') {
        board.meta.enPassantTargets = 0ll;
        ++it;
    } else {
        char fileLetter = FEN[it++];
        char rankDigit = FEN[it++];
        int file7 = fileLetter - 'a';
        int rank7 = digit_to_int(rankDigit) - 1;
        SquareCode epSquare = rank7 * 8 + file7;
        board.meta.enPassantTargets = (1ll << epSquare);
    }
    ++it;

    // parse halfmove clock
    char firstDigit = FEN[it++];
    if (FEN[it] == ' ') {
        board.meta.halfmoveClock = digit_to_int(firstDigit);
    }
    else {
        char secondDigit = FEN[it++];
        board.meta.halfmoveClock = 10 * digit_to_int(firstDigit) + digit_to_int(secondDigit);
    }
    ++it;

    // parse fullmove counter
    int val = 0;
    for (; FEN[it] != '\0' && FEN[it] != ' '; ++it) {
        val = 10*val + digit_to_int(FEN[it]);
    }
    board.meta.fullmoveCounter = val;

    return board;

}





