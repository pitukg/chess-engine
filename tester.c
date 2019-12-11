#include "debugger_functions.c"
#include "search.h"
#include "transposition.h"
#include <stdlib.h>
#include <string.h>

// driver
int main() {

//    RUN PERFT:
    perft_check();

    transpos_init();

    eval_testing();

    transpos_init();

    Board board = parse_FEN("3r2b1/1n2p1kp/3p1pp1/1N1p4/2P5/3PPP2/4KQPP/8 b - - 1 1");
    Stack stack;

//    EVALUATE A POSITION WITH A SIMPLE ALPHA BETA SEARCH IN DEPTH 6
    Move bestMove;
    PrincipalVariation pv;
    pv.depth = 0;
    memset(pv.root, 0, 512);
    Score score = iterate(&board, &stack, &pv);
    printf("score = %.2f\n", (double)score/100.);
    for (int i = 0; i < 10; i++) {
        print_move(pv.root[i], board.pieceCode[get_from(pv.root[i])]);
    }

    return 0;
}



