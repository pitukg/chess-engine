#include "search.h"
#include "moves.h"
#include "move_generation.h"
#include "transposition.h"

#define NUM_MOVES_MAX (10000)



static Move *sort_move_list(Move *moveListBegin, Move *moveListEnd, Move *sortedListBegin,
                                        int depthLeft, PrincipalVariation *pv, Board *board, Stack *stack);


Score iterate(Board *board, Stack *stack, PrincipalVariation *pv) {
    // TODO: ponder, time mgmt

    Score lastScore = 0;

    for (int depth = 0; depth < 5; depth++) {
        Score itScore = alphaBeta(-30000, +30000, depth, board, stack, pv);
        lastScore = itScore;
        pv->depth++;
    }

    return lastScore;
}



Score alphaBeta(Score alpha, Score beta, int depthLeft, Board *board, Stack *stack, PrincipalVariation *pv) {

    if (depthLeft == 0) {
        return quiesce(alpha, beta, board, stack);
    }


    Move begin[NUM_MOVES_MAX], *end;
    Move sortedListBegin[NUM_MOVES_MAX], *sortedListEnd;
    Move bestMove = 0;



    // check for position in transposition table
    Score lookupScore;

    EvalType lookupType = transpos_lookup(board->hash, depthLeft, &lookupScore, &bestMove);

    // if bounds are enough return them
    if (lookupType == ALPHA && alpha >= lookupScore) {
        return alpha;
    }
    if (lookupType == BETA && beta <= lookupScore) {
        return beta;
    }



    end = generate_pseudolegal_moves(begin, board->meta.sideToMove, board);

    if (lookupType == EXACT) {
        // check if pseudolegal move to avoid hash key collisions
        for (Move *move = begin; move != end; ++move) {
            if (*move == bestMove) {
                if (lookupScore < alpha) {
                    return alpha;
                }
                if (lookupScore >= beta) {
                    return beta;
                }
                return lookupScore;
            }
        }
    }

    bestMove = 0; // 0 is invalid move, if it stays 0 no changes were made

    // MOVE-ORDERING
    sortedListEnd = sort_move_list(begin, end, sortedListBegin, depthLeft, pv, board, stack);


    short isPVsearch = 1; // boolean to signal principal variation

    for (Move *move = sortedListBegin; move != sortedListEnd; ++move) {

        make_move(board, *move, stack);

    // MAYBE NOT NECESSARY TO CHECK FOR KING IN CHECK
    if (attacks_to_king(board, !(board->meta.sideToMove)) == 0ll) {

        Score current;

        if (isPVsearch) {
            // do full search
            current = -alphaBeta(-beta, -alpha, depthLeft - 1, board, stack, pv);
            isPVsearch = 0;
        }
        else {
            // do zero window search
            current = -alphaBeta(-alpha-1, -alpha, depthLeft - 1, board, stack, pv);
            
            if (current > alpha) {
                // if failed high, do full re-search
                current = -alphaBeta(-beta, -alpha, depthLeft - 1, board, stack, pv);
            }
        }
        

        if (current >= beta) {
            unmake_move(board, *move, stack);
            // beta node [lower bound]
            transpos_save(board->hash, depthLeft, BETA, beta, 0);            
            return beta;
        }

        if (current > alpha) {
            alpha = current;
            bestMove = *move;
            // store the new PV move
            pv->root[pv->depth - depthLeft] = bestMove;
        }
    }

        unmake_move(board, *move, stack);

    }

    // if ([replacement_logic]) :

    if (bestMove == 0) {
        // alpha node
        transpos_save(board->hash, depthLeft, ALPHA, alpha, 0);
    }
    else {
        // exact node
        transpos_save(board->hash, depthLeft, EXACT, alpha, bestMove);
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

    Move begin[NUM_MOVES_MAX], *end;

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




static Move *sort_move_list(Move *moveListBegin, Move *moveListEnd, Move *sortedListBegin,
                                        int depthLeft, PrincipalVariation *pv, Board *board, Stack *stack) {

    Move *it = sortedListBegin;
    short visited[NUM_MOVES_MAX] = {0};

    // I. principal variation

    Move pvMove = pv->root[pv->depth - depthLeft];
    Score scoreBuffer;
    Move moveBuffer;

    for (Move *move = moveListBegin; move != moveListEnd; ++move) {
        if (*move == pvMove) {
            *it++ = *move;
            visited[(int)(move - moveListBegin)] = 1;
        }
    }

    // II. hash moves
    for (Move *move = moveListBegin; move != moveListEnd; ++move) {

        if (visited[(int)(move - moveListBegin)]) {
            continue;
        }

        make_move(board, *move, stack);

        if (transpos_lookup(board->hash, depthLeft, &scoreBuffer, &moveBuffer) != NO_ENTRY) {
            *it++ = *move;
            visited[(int)(move - moveListBegin)] = 1;
        }

        unmake_move(board, *move, stack);

    }

    // III. captures with SEE > 0

    for (Move *move = moveListBegin; move != moveListEnd; ++move) {

        if (visited[(int)(move - moveListBegin)]) {
            continue;
        }

        if ((*move & captureFlag) &&
                    capture_static_evaluation(get_from(*move), get_to(*move), board, stack) >= 0) {
            *it++ = *move;
            visited[(int)(move - moveListBegin)] = 1;
        }

    }

    // IV. captures with SEE = 0

    for (Move *move = moveListBegin; move != moveListEnd; ++move) {

        if (visited[(int)(move - moveListBegin)]) {
            continue;
        }

        if ((*move & captureFlag) &&
                    capture_static_evaluation(get_from(*move), get_to(*move), board, stack) == 0) {
            *it++ = *move;
            visited[(int)(move - moveListBegin)] = 1;
        }

    }

    // V. other moves

    for (Move *move = moveListBegin; move != moveListEnd; ++move) {

        if (visited[(int)(move - moveListBegin)]) {
            continue;
        }

        *it++ = *move;
        visited[(int)(move - moveListBegin)] = 1;
    }


    return it;

}


