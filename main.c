//
//  main.c
//  PentaGo
//
//  Created by Anton Wiehe on 24.06.16.
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "bot.h"

State newEmptyBoard(){
    State new;
    int i,j;
    for(i=0;i<6;i++){
        for(j=0;j<6;j++){
            new.field[i][j] = 0;
        }
    }
    new.player = 1;
    new.moveNumber = 0;
    return new;
}

int printBoard (State board) {
    int i,j,x;
    for(x=0;x<17;x++){
        printf("#");
    }
    printf("\n");
    for(i=5;i>=0;i--){
        printf("# ");
        for(j=0;j<6;j++) {
            printf("%d", board.field[i][j]);
            if( j == 2) {
                printf(" | ");
            } else {
                printf(" ");
            }
        }
        printf("#");
        if( i == 3) {
            printf("\n# ");
            for(x=0;x<13;x++){
                printf("-");
            }
            printf(" #\n");
        } else {
            printf("\n");
        }
    }
    for(x=0;x<17;x++){
        printf("#");
    }
    printf("\n");
    if( board.player == 1) {
        printf("Black to play.\n");
    } else if( board.player == -1) {
        printf("White to play.\n");
    } else {
        return 1;
    }
    return 0;
}

State setStone(State currentGame, int x, int y) {
    int stone;
    if( currentGame.player == 1) {
        stone = 1;
    } else{
        stone = 2;
    }
    currentGame.field[x][y] = stone;
    currentGame.player = currentGame.player * -1;
    currentGame.moveNumber++;
    return currentGame;
}

State applyRotation(State board, int x, int y, int dir){
    int midX = 1;
    int midY = 1;
    midX = midX + x * 3;
    midY = midY + y * 3;
    int temp = board.field[midX+1][midY+1];
    
    if( dir == 0 ) {
        //corners:
        board.field[midX+1][midY+1] = board.field[midX+1][midY-1];
        
        board.field[midX+1][midY-1] = board.field[midX-1][midY-1];
        
        board.field[midX-1][midY-1] = board.field[midX-1][midY+1];
        
        board.field[midX-1][midY+1] = temp;
        //sides:
        temp = board.field[midX+1][midY];
        
        board.field[midX+1][midY] = board.field[midX][midY-1];
        
        board.field[midX][midY-1] = board.field[midX-1][midY];
        
        board.field[midX-1][midY] = board.field[midX][midY+1];
        
        board.field[midX][midY+1] = temp;
    } else {
        //corners:
        board.field[midX+1][midY+1] = board.field[midX-1][midY+1];
        
        board.field[midX-1][midY+1] = board.field[midX-1][midY-1];
        
        board.field[midX-1][midY-1] = board.field[midX+1][midY-1];
        
        board.field[midX+1][midY-1] = temp;
        //sides:
        temp = board.field[midX+1][midY];
        
        board.field[midX+1][midY] = board.field[midX][midY+1];
        
        board.field[midX][midY+1] = board.field[midX-1][midY];
        
        board.field[midX-1][midY] = board.field[midX][midY-1];
        
        board.field[midX][midY-1] = temp;
    }
    return board;
}

State readBoard(){
    int x = 0;
    int y = 0;
    char c;
    State new;
    new.moveNumber = 0;
    new.player = 0;
    c = getchar();
    while( y != 6 ) {
        c = getchar();
        if( c == '\n'){
            if( x == 6 && y < 5 ) {
                y++;
                x = 0;
            } else {
                break;
            }
        } else if( c == '0' || c == '1' || c == '2') {
            if( x < 6) {
                if( c== '1' || c == '2'){
                    new.moveNumber++;
                }
                new.field[x][y] = (int)c - '0';
                x++;
            } else {
                break;
            }
        } else{
            break;
        }
        
    }
    if( x == 6 && y == 5 ){
        c = getchar();
        switch( c ) {
            case 'B':
                new.player = 1;
                break;
            case 'W':
                new.player = -1;
                break;
            case 'b':
                new.player = 1;
                break;
            case 'w':
                new.player = -1;
                break;
            default:
                new.player = 0;
        }
    }
    printf("Game loaded successfully!\n");
    return new;
}
                   
void makeMove(State* currentGame){
    int x, y;
    int rotX, rotY;
    int rotDir; // (1) for Clockwise, (2) for Counter-Clockwise.
    char move[7];
    

    do{
        printf("Enter your move in this format: x(1-6) y(1-6) rotation:x (1/2) rotation:y(1/2) rotation dir: clockwise(1)/counter-clockwise (2)\n");
        scanf("%s",move);
        printf("move: %s\n",move);
        x = (int) move[0] -  '1';
        y = (int) move[1] -  '1';
        rotX = (int) move[2] - '1';
        rotY = (int) move[3] - '1';
        rotDir = (int) move[4] - '1';
    } while(x > 5 || y > 5 || x < -1 || y < -1 || rotDir > 1 || rotDir < -1  || rotX > 1 || rotY > 1 || rotX < -1 || rotY < -1 || currentGame->field[x][y] );
    
    if( x != -1 && y != -1){
        *currentGame = setStone(*currentGame,x,y);
    }
    if( rotDir != -1){
        *currentGame = applyRotation(*currentGame,rotX,rotY,rotDir);
    }
    
    strcpy(currentGame->lastMoves[currentGame->moveNumber], move);
}

void takeBackMove(State* game){
    int x, y;
    int rotX, rotY;
    int rotDir;
    if(game->moveNumber == 0){
        return;
    }
    char move[7];
    strcpy(move, game->lastMoves[game->moveNumber - 1]);
    game->moveNumber = game->moveNumber - 1;
    x = (int) move[0] -  '1';
    y = (int) move[1] -  '1';
    rotX = (int) move[2] - '1';
    rotY = (int) move[3] - '1';
    rotDir = (int) move[4] - '1';
    if(rotDir == 1){
        rotDir = 0;
    }else{
        rotDir = 1;
    }
    *game = applyRotation(*game,rotX,rotY,rotDir);
    game->field[x][y] = 0;
    game->player = game->player * -1;
}
                   
                   
void playGame(){
    char c;
    int winner = 2;
    int decision = 0; // (0) for undecided, (1) for empty game, (2) for load game.
    State currentGame;
    
    printf("Welcome. ");
    do{
        printf("Do you want to start a (N)ew empty game, do you want to (R)ead a game or do you want to (E)xit the program?\n");
        scanf("%c",&c);
        if( c == 'N' ||  c == 'n'){
            currentGame = newEmptyBoard();
            decision = 1;
        } else if( c == 'R' ||  c == 'r'){
            currentGame = readBoard();
            if( currentGame.player != 0) {
                decision = 2;
            } else{
                printf("Error reading board!\n");
            }
        } else if( c == 'E' ||  c == 'e'){
            return;
        }
    } while( decision == 0);
    winner = 2;
    printBoard(currentGame);
    while(winner == 2){
        if(currentGame.moveNumber == 36){
            break;
        }
        printf("Movenumber of current board: %d\n",currentGame.moveNumber);
        printf("Make a (M)ove, let the (B)ot make a move,e(V)aluate the position, (T)ake back the last move or (E)xit the program.\n");
        do{
            scanf("%c",&c);
            if( c == 'M' || c == 'm'){
                makeMove(&currentGame);
            } else if( c == 'T' || c == 't'){
                takeBackMove(&currentGame);
            } else if( c == 'B' || c == 'b'){
                botMove(&currentGame);
            } else if( c == 'E' ||  c == 'e'){
                return;
            } else if( c == 'V' ||  c == 'v'){
                printf("Value of current board: %d\n",evaluateBoard(currentGame,1));
            }

        } while( c != 'M' && c != 'm' && c != 'T' && c != 't' && c != 'B' && c != 'b' && c != 'E' &&  c != 'e' );
        printBoard(currentGame);
        winner = evaluateBoard(currentGame,0);
    }
    if( winner == 1) {
        printf("Black(1) Wins!\n");
    } else if( winner == -1) {
        printf("White(2) Wins!\n");
    } else {
        printf("It is a tie! \n");
    }
                       
}

int main(int argc, char** argv){
    playGame();
    return 0;
}
