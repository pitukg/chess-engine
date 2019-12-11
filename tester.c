#include "debugger_functions.c"
#include "search.h"
#include "transposition.h"

// driver
int main() {

//    RUN PERFT:
//    perft_check();

//    eval_testing();

    transpos_init();

    Board board = parse_FEN("3r2b1/1n2p1kp/3p1pp1/1N1p4/2P5/3PPP2/4KQPP/8 b - - 1 1");
    Stack stack;

//    EVALUATE A POSITION WITH A SIMPLE ALPHA BETA SEARCH IN DEPTH 6
    Move bestMove;
    printf("position score: %.2f (relative to side to move)\n", (double)alphaBeta(-30000, +30000, 6, &board, &stack, &bestMove) / 100.);
    print_move(bestMove, board.pieceCode[get_from(bestMove)]);

    return 0;
}



