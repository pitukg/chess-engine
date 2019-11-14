// header file for move generating functions


#ifndef MOVE_GENERATION_H

#define MOVE_GENERATION_H


#include "moves.h"
#include "stack.h"

Move *generate_castling_moves(Move *moveList, Color color, const Board *board);
Move *generate_nonpawn_moves(Move *moveList, Color color, const Board *board);
Move *generate_pawn_moves(Move *moveList, Color color, const Board *board);
Move *generate_pseudolegal_moves(Move *moveList, Color color, const Board* board);

void make_move(Board *board, Move move, Stack *stack);
void unmake_move(Board *board, Move move, Stack *stack); // color is whose move is being unmade


#endif


