// header file for the search-related components

#ifndef SEARCH_H

#define SEARCH_H


#include "evaluation.h"
#include "stack.h"
#include "moves.h"


Score alphaBeta(Score alpha, Score beta, int depthLeft, Board *board, Stack *stack, Move *bestMoveBuffer);


Score quiesce(Score alpha, Score beta, Board *board, Stack *stack);



#endif


