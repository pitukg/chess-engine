#include "search.h"
#include "moves.h"
#include "move_generation.h"
#include "transposition.h"
#include <stdlib.h>


Score alphaBeta(Score alpha, Score beta, int depthLeft, Board *board, Stack *stack, Move *bestMoveBuffer) {
    // bestMoveBuffer must be a valid pointer
    *bestMoveBuffer = 0;// note that 0 is not a legal move, so if we are left with 0 no changes were made

    if (depthLeft == 0) {
        return quiesce(alpha, beta, board, stack);
    }

    Move begin[10000], *end;
    Move buffer;

    end = generate_pseudolegal_moves(begin, board->meta.sideToMove, board);


    // check for position in transposition table
    Score lookupScore;

    EvalType lookupType = transpos_lookup(board->hash, depthLeft, &lookupScore, bestMoveBuffer);

    switch (lookupType) {

    case EXACT:
        // TODO: check if pseudolegal move to avoid hash key collisions
        return lookupScore;

    case ALPHA:
        if (alpha >= lookupScore /*>= real score*/) return alpha;
        break;

    case BETA:
        if (beta <= lookupScore /*<= real score*/) return beta;
        break;

    case NO_ENTRY:;
        // calculate

    }



    for (Move *move = begin; move != end; ++move) {

        make_move(board, *move, stack);

    // MAYBE NOT NECESSARY TO CHECK FOR KING IN CHECK
    if (attacks_to_king(board, !(board->meta.sideToMove)) == 0ll) {

        Score current = -alphaBeta(-beta, -alpha, depthLeft - 1, board, stack, &buffer);

        if (current >= beta) {
            unmake_move(board, *move, stack);
            // beta node [lower bound]
            transpos_save(board->hash, depthLeft, BETA, beta, 0);            
            return beta;
        }

        if (current > alpha) {
            alpha = current;
            *bestMoveBuffer = *move;
            // if this happens at least once, exact node
            // if not, alpha node [upper bound]
            // this is signalled by bestMove = 0
        }
    }

        unmake_move(board, *move, stack);

    }

    // if ([replacement_logic]) :

    if (*bestMoveBuffer == 0) {
        // alpha node
        transpos_save(board->hash, depthLeft, ALPHA, alpha, 0);
    }
    else {
        // exact node
        transpos_save(board->hash, depthLeft, EXACT, alpha, *bestMoveBuffer);
    }


    return alpha;

}




Score quiesce(Score alpha, Score beta, Board *board, Stack *stack) {

    Score standPat = evaluate(board);
    if (standPat >= beta) {
        return beta;
    }
    if (alpha < standPat) {
        alpha = standPat;
    }

    Move begin[10000], *end;

    end = generate_pseudolegal_moves(begin, board->meta.sideToMove, board);

    for (Move *move = begin; move != end; ++move) {


        // TODO: include checks and check evasions
        if (!(*move & captureFlag)) {
            continue;
        }

        // don't analyse move with SEE < 0
        if (capture_static_evaluation(get_from(*move), get_to(*move), board, stack) < 0) {
            continue;
        }


        make_move(board, *move, stack);

    // MAYBE NOT NECESSARY TO CHECK FOR KING IN CHECK
    if (attacks_to_king(board, !(board->meta.sideToMove)) == 0ll) {

        Score current = -quiesce(-beta, -alpha, board, stack);

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




