// header file for basic types used in representing the bitboards in the chess position


#ifndef BOARD_H

#define BOARD_H

// TYPES 

// type for 64 bit bitboards
typedef long long U64;


// type for colors
enum Color {
    WHITE, BLACK
};
typedef enum Color Color;


// type for pieces
enum PieceType {
    wPawn,
    bPawn,
    wKnight,
    bKnight,
    wBishop,
    bBishop,
    wRook,
    bRook,
    wQueen,
    bQueen,
    wKing,
    bKing,
    none
};
typedef enum PieceType PieceType;



// type for square codes
enum SquareCode {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};
typedef enum SquareCode SquareCode;



// type for storing the score of positions in CENTIPAWNS i.e. 1 pawn = 100
typedef short Score;

// struct with bitboards for each piece type
struct Board {
    // a bitboard for each type eg. pieceBB[bBishop]
    U64 pieceBB[13];
    // for checking occupation
    U64 occupied;
    // occupaion for white and black pieces e.g. piecesByColor[WHITE]
    U64 piecesByColor[2];
    // contains the code of the piece on each square, if occupied, *none* otherwise
    PieceType pieceCode[64]; // eg. pieceCode[b4] = wPawn;
    // keeps the square of each king redundantly
    SquareCode kingSquare[2];    
    // structure for storing aspects of current board
    struct Meta {
        // a bitboard for en passant legality
        U64 enPassantTargets;
        // TODO: 32 bit integer, retrieve info by using masks
            /*
            // bitmasks for meta data
            const unsigned int sideToMoveMask = (1 << 31);
            const unsigned int whiteCastlingRightsMask = (1 << 30);
            const unsigned int blackCastlingRightsMask = (1 << 29);
            */
        Color sideToMove; // 1 bit
        unsigned int kingsideCastlingRight[2], queensideCastlingRight[2]; // 4*1 bit
        U64 castlingSquaresAttacked; // squares that need checking for castling legality
        unsigned int halfmoveClock;
        unsigned int fullmoveCounter; // need not be stored in stack -> pull out of `meta` in the future
    } meta;
    // evaluation score of current position
    Score score;

};
typedef struct Board Board;


// type for directions
enum Dir {
    N, NE, E, SE, S, SW, W, NW
};
typedef enum Dir Dir;


// functions for manipulating bitboards

// returns the bit at square-th position // only print uses this
short int bb_get_bit(U64 * bitboard, SquareCode square);

// sets the given square to 1
void bb_set_true(U64 * bitboard, SquareCode square);

void bb_set_false(U64 * bitboard, SquareCode square);

void add_piece(Board * board, PieceType pt, SquareCode square);

void remove_piece(Board * board, PieceType pt, SquareCode square);

// move a bitboard by one unit
static const U64 notAfile = ~0x0101010101010101;
static const U64 notHfile = ~0x8080808080808080;
U64 bb_move_N(U64 b);
U64 bb_move_NE(U64 b);
U64 bb_move_E(U64 b);
U64 bb_move_SE(U64 b);
U64 bb_move_S(U64 b);
U64 bb_move_SW(U64 b);
U64 bb_move_W(U64 b);
U64 bb_move_NW(U64 b);


// bitscan functions for bitboard serialization
SquareCode bitscan_forward(U64 table);

SquareCode bitscan_reverse(U64 table);


// function for parsing a chess position of the Forsyth-Edwards Notation
Board parse_FEN(char *FEN);



#endif









