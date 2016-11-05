//
//  bot.h
//  
//
//  Created by Anton Wiehe on 25.06.16.
//
//

#ifndef _bot_h
#define _bot_h
#include "main.h"

typedef struct trieNode* trie;


typedef struct trieNode{
    int value;
    trie zero;
    trie one;
    trie two;
}trieNode;



typedef struct Line{
    int length;
    State moves[36];
}Line;

typedef struct Variation{
	int length;
	Line part[36];
}Variation;

trie newEmptyTrie();
int isInTrie(trie currentTrie,State* currentGame);
int addToTrie(trie currentTrie, State currentGame);
int areEqualBoards( State board1,  State board2);
int allPossibleRotations(State original, State possibleStates[9]);
int allPossibleMoves(State original, State possibleStates[36]);
int allPossibleStates(State original, State** allVariations);
void botMove(State* currentGame);
int evaluateBoard(State currentGame, int returnPoints);
int valueRow(int array[6], int len, int color, int* threats);
void createDiagonal(State currentGame, int x,int y,int dx,int dy, int array[6]);
//State negaMax(State currentGame, int depth, int alpha, int beta,trie evals,Line* pLine);
void quicksort(State list[],int m,int n);

#endif

