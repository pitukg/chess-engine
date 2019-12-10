#include "search.h"
#include "moves.h"
#include "move_generation.h"

Move best_move(Board *board, Stack *stack, int depth) {

    Score alpha = -30000;
    Move begin[10000], *end;
    Move bestMove = 0;

    end = generate_pseudolegal_moves(begin, board->meta.sideToMove, board);

    for (Move *move = begin; move != end; ++move) {

        make_move(board, *move, stack);

//    if (attacks_to_king(board, !(board->meta.sideToMove)) == 0ll) {

        Score current = -alphaBeta(-30000, -alpha, depth, board, stack);

        if (current > alpha) {
            alpha = current;
            bestMove = *move;
        }
//    }
    
        unmake_move(board, *move, stack);
    
    }
    return bestMove;


}


Score alphaBeta(Score alpha, Score beta, int depthLeft, Board *board, Stack *stack) {

    if (depthLeft == 0) {
        return quiesce(alpha, beta, board, stack);
    }

    Move begin[10000], *end;

    end = generate_pseudolegal_moves(begin, board->meta.sideToMove, board);

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




