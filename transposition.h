#include "board.h"
#include "moves.h"

#ifndef TRANSPOSITION_H

#define TRANSPOSITION_H


// type of evaluation for entry
enum EvalType {
    NO_ENTRY,
    EXACT,
    ALPHA,
    BETA,
};
typedef enum EvalType EvalType;


// keys for zobrist hash generation
struct ZobristKeys {
    // usage: `pieceAtSquare[piece][square]`
    U64 pieceAtSquare[13][64]; // 12 piece would be enough if necessary
    U64 flipSideToMove;
    U64 flipKingsideCastlingRight[2];
    U64 flipQueensideCastlingRight[2];
    U64 enPassantSquare[64]; // serialise `board.enPassantTargets` and hash into `enPassantSquare[square]`
} zobristKeys;


// entry in transposition table
struct TransposTableEntry {
    // full hash of position
    U64 hash;
    // depth of previous search
    int depth;
    // type of previous evaluation
    EvalType type; 
    // evaluation
    Score eval;
    // best move found -- 0 if not exact
    Move bestMove;
};
typedef struct TransposTableEntry TransposTableEntry;



void transpos_init();

void transpos_save(U64 hash, int depth, EvalType type, Score score, Move bestMove);

EvalType transpos_lookup(U64 hash, int depth, Score *scoreBuffer, Move *bestMoveBuffer);


#endif