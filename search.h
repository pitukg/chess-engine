// header file for the search-related components

#ifndef SEARCH_H

#define SEARCH_H


#include "evaluation.h"
#include "stack.h"
#include "moves.h"

struct PrincipalVariation {
    Move root[512];
    int depth; // filled up by depth+1 elements in fact
    // access PV move at depth i by .root[depth-i] for (0 <= i <= depth)
};
typedef struct PrincipalVariation PrincipalVariation;


Score iterate(Board *board, Stack *stack, PrincipalVariation *pv);


Score alphaBeta(Score alpha, Score beta, int depthLeft, Board *board, Stack *stack, PrincipalVariation *pv);


Score quiesce(Score alpha, Score beta, Board *board, Stack *stack);



#endif


