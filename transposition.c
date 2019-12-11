#include "transposition.h"
#include <stdlib.h>

// TODO: for now implement the `always update` design

#define HASH_SIZE ( 32 ) // in bits
#define HASH_SIZE_MASK ( ( U64 )0X00000000FFFFFFFF )


static U64 U64_rand();


static TransposTableEntry *transpositionTable;


// function definitions
void transpos_init() {

    // fill up zobrist structure with pseudo-random numbers
    for (SquareCode square = 0; square < 64; square++) {
        zobristKeys.enPassantSquare[square] = U64_rand();
        for (PieceType pt = 0; pt < 13; pt++) {
            zobristKeys.pieceAtSquare[pt][square] = U64_rand();
        }
    }
    zobristKeys.flipSideToMove = U64_rand();
    zobristKeys.flipKingsideCastlingRight[WHITE] = U64_rand();
    zobristKeys.flipKingsideCastlingRight[BLACK] = U64_rand();
    zobristKeys.flipQueensideCastlingRight[WHITE] = U64_rand();
    zobristKeys.flipQueensideCastlingRight[BLACK] = U64_rand();

    // initialise transposition table
    transpositionTable =
        ( TransposTableEntry * )malloc( ((unsigned long)1 << HASH_SIZE) * sizeof(TransposTableEntry) );

}


EvalType transpos_lookup(U64 hash, int depth, Score *scoreBuffer, Move *bestMoveBuffer) {

    TransposTableEntry *entry = transpositionTable + (hash & HASH_SIZE_MASK);

    if (entry->hash != hash || entry->depth < depth) {
        return NO_ENTRY;
    }

    *scoreBuffer = entry->eval;
    *bestMoveBuffer = entry->bestMove;

    return entry->type;
}



void transpos_save(U64 hash, int depth, EvalType type, Score score, Move bestMove) {

    TransposTableEntry *entry = transpositionTable + (hash & HASH_SIZE_MASK);

    entry->hash = hash;
    entry->depth = depth;
    entry->type = type;
    entry->eval = score;
    entry->bestMove = bestMove;

}



// this function is taken from Sungorus chess engine
static U64 U64_rand() {
  static U64 next = 1;

  next = next * 1103515245 + 12345;
  return next;
}

