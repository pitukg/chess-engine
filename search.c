#include "search.h"
#include "moves.h"
#include "move_generation.h"


Score alphaBeta(Score alpha, Score beta, int depthLeft, Board *board, Stack *stack) {

    if (depthLeft == 0) {
        return evaluate(board);
    }

    Move begin[10000], *end;

    end = generate_pseudolegal_moves(begin, board->meta.sideToMove, board);

Score testEVAL = evaluate(board);

    for (Move *move = begin; move != end; ++move) {

        make_move(board, *move, stack);

    // MAYBE NOT NECESSARY TO CHECK FOR KING IN CHECK
    if (attacks_to_king(board, !(board->meta.sideToMove)) == 0ll) {

        Score current = -alphaBeta(-beta, -alpha, depthLeft - 1, board, stack);

        if (current >= beta) {
            unmake_move(board, *move, stack);
            return beta;
        }

        if (current > alpha) {
            alpha = current;
        }
    }

        unmake_move(board, *move, stack);
    
    }
    return alpha;

}


