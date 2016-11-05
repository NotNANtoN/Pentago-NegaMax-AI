//
//  main.h
//  
//
//  Created by Manuela Timme on 25.06.16.
//
//

#ifndef _main_h
#define _main_h

typedef struct Stone{
    int x;
    int y;
    int player;
}Stone;

typedef struct State{
    int field[6][6];
    int player; //Black(1) or White (-1)
    int moveNumber;
    char lastMoves[36][6];
    int value;
    int marker;
}State;

State newEmptyBoard();
int printBoard (State board);
State setStone(State currentGame, int x, int y) ;
State applyRotation(State board, int x, int y, int dir);
State readBoard();
void readMove(char move[5]);
void makeMove(State* currentGame);
void takeBackMove(State* game);
void playGame();

#endif
