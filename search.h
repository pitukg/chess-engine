// header file for the search-related components

#ifndef SEARCH_H

#define SEARCH_H


#include "evaluation.h"
#include "stack.h"
#include "moves.h"


Move best_move(Board *board, Stack *stack, int depth);


Score alphaBeta(Score alpha, Score beta, int depthLeft, Board *board, Stack *stack);


Score quiesce(Score alpha, Score beta, Board *board, Stack *stack);



#endif


