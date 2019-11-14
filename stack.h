// header file for the stack data structure used for storing a copy of the state of the board whenever making a move


#ifndef STACK_H

#define STACK_H


#include "board.h"
//#include "moves.c"


/*
    
    Stack data structure for storing metadata of game state before making moves
    ie. {
        captured piece
        en passant possibilities
        castling rights
        halfmove clock
    }

*/


struct StackObject {
    PieceType capturedPiece;
    // board structure's Meta struct
    struct Meta meta;
};
typedef struct StackObject StackObject;


struct Stack {
    StackObject stackArray[512]; // maximum depth of search
    unsigned short size;
};


typedef struct Stack Stack;

void push(StackObject stObj, Stack *stack);
StackObject pop(Stack *stack);



#endif


