//
//  bot.c
//  
//
//  Created by Anton Wiehe on 25.06.16.
//
//

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "bot.h"
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#define INFINITY INT_MAX

#define MAXDEPTH 4
#define DEBUG 0
#define MAXNODES 1000000

// Counters:
int numbCuts = 0;
int numbEvals = 0;
int numbTries = 0;
int numbTrieNodes = 0;
int cnt = 0;
int sort = 0;


//extend: if one encounters a different stone, try to turn other board so that they match. Do this only once. If not possible, not equal.---- not really true
int areEqualBoards(State board1, State board2) {
    int i,j;
    int stone1, stone2;
    for(i=0;i<6;i++){
        for(j=0;j<6;j++){
            stone1 = board1.field[i][j];
            stone2 = board2.field[i][j];
            if( stone1 != stone2 ){
                return 0;
            }
        }
    }
    return 1;
}

trie newEmptyTrie(){
    numbTrieNodes++;
    trie new = malloc(sizeof(struct trieNode));
    assert(new != NULL);
    new->value = 0;
    new->zero = NULL;
    new->one = NULL;
    new->two = NULL;
    return new;
}

void freeTrie(trie current){
    if(current->zero != NULL){
        freeTrie(current->zero);
    }
    if(current->one != NULL){
        freeTrie(current->one);
    }
    if(current->two != NULL){
        freeTrie(current->two);
    }
    free(current);
}

void printTrie(trie current){
    if(current->zero != NULL){
        printf("0\n");
        printTrie(current->zero);
    }
    if(current->one != NULL){
        printf("1\n");
        printTrie(current->one);
    }
    if(current->two != NULL){
        printf("2\n");
        printTrie(current->two);
    }
}

int addToTrie(trie currentTrie, State currentGame){
    numbTries++;
    int i,j;
    trie root = currentTrie;
    trie new;
    for(i=0;i<6;i++){
        for(j=0;j<6;j++){
            switch(currentGame.field[i][j]){
                case 0:
                    if(currentTrie->zero == NULL){
                        new = newEmptyTrie();
                        currentTrie->zero = new;
                    }
                    currentTrie = currentTrie->zero;
                    
                    break;
                case 1:
                    if(currentTrie->one == NULL){
                        new = newEmptyTrie();
                        currentTrie->one = new;
                    }
                    currentTrie = currentTrie->one;
                    
                    break;
                case 2:
                    if(currentTrie->two == NULL){
                        new = newEmptyTrie();
                        currentTrie->two = new;
                    }
                    currentTrie = currentTrie->two;
            }
        }
    }
    currentTrie->value = currentGame.value;
    currentTrie = root;
    return 1;
}

int isInTrie(trie currentTrie,State* currentGame){
    int i,j;
    trieNode currentNode = *currentTrie;
    for(i=0;i<6;i++){
        for(j=0;j<6;j++){
            switch(currentGame->field[i][j]){
                case 0:
                    if(currentNode.zero == NULL){
                  
                        return 0;
                    } else{
                        currentNode = *(currentNode.zero);
                    }
                    break;
                case 1:
                    if(currentNode.one == NULL){
                        
                        return 0;
                    } else{
                        currentNode = *(currentNode.one);
                    }
                    break;
                case 2:
                    if(currentNode.two == NULL){
                   
                        return 0;
                    } else{
                        currentNode = *(currentNode.two);
                    }
            }
        }
    }
    currentGame->value = currentNode.value;

    return 1;
}


int allPossibleRotations(State original, State possibleStates[9]){
    int x,y,dir;
    int i;
    int cnt = 0;
    int duplicate;
    State temp;
    
    for(x = 0; x < 2; x++){
        for(y = 0; y < 2; y++){
            for(dir = 0; dir < 2; dir++){
                duplicate = 0;
                temp = applyRotation(original, x, y, dir);
                for(i=0;i<cnt;i++){
                    if( areEqualBoards(temp,possibleStates[i]) ){
                        duplicate = 1;
                        i = cnt;
                    }
                }
                if( duplicate == 0) {
                    possibleStates[cnt] = temp;
                    (cnt)++;
                }
            }
        }
    }
    return cnt;
}

//extend: compare if equal through rotation?
int allPossibleMoves(State original, State possibleStates[36]){
    int x,y;
    int cnt = 0;
    for(x = 0; x < 6; x++){
        for(y = 0; y < 6; y++){
            if( !original.field[x][y] ){
                possibleStates[cnt] = setStone(original,x,y);
                possibleStates[cnt].marker = 2;
                (cnt)++;
                
            }
        }
    }
    return cnt;
}

int allPossibleStates(State original, State** allVariations) {
    int i,j;
    int cnt = 0;
    State rotations[9];
    State variations[36];
    int numbVariations;
    int numbRotations;
    int size = 5;
    *allVariations = malloc(sizeof(State) * size);
    assert( *allVariations != NULL);

    numbRotations = allPossibleRotations(original,rotations);
    
    for(i=0;i<numbRotations;i++){
        numbVariations = allPossibleMoves(rotations[i],variations);
   
        for(j=0;j<numbVariations;j++){
            if( cnt >= size){
                size = size * 2;
                (*allVariations) = realloc(*allVariations,sizeof(State) * size);
                assert(*allVariations != NULL);
            }
            (*allVariations)[cnt] = variations[j];
            (cnt)++;
        }
    }
    
    *allVariations = realloc(*allVariations, sizeof(struct State) * (cnt));
    assert(*allVariations != NULL);
    return cnt;
}

int removeDuplicates(State* allVariations, int length, State** new) {
    int i,j;
    int cnt = 0;
    int duplicate;
    *new = malloc(sizeof(State) * length);
    assert( *new != NULL);
    
    
    for(i=0;i<length;i++){
        duplicate = 0;
        for(j=i+1;j<length;j++){
            if( areEqualBoards(allVariations[i],allVariations[j]) ){
                duplicate = 1;
                printf("Duplicate!\n");
                break;
            }
        }
        if( duplicate == 0){
            (*new)[cnt] = allVariations[i];
            cnt++;
        }
        
    }
    
    *new = realloc(*new, sizeof(State) * cnt);
    assert( new != NULL);
    return cnt;
}

int valueRow(int array[6], int len, int color, int* threats){
    int opponent;
    int i;
    int needStones = 5;
    int group = 0;
    int longestGroup = 0;
    if( color == 1) {
        opponent = 2;
    } else{
        opponent = 1;
    }
    
    if( len == 6){
        //Is it possible to win with this row?
        if( array[0] == opponent && array[5] == opponent){
            return -1;
        }
        
        for(i=1;i<5;i++){
            if(array[i] == opponent){
                return -1;
            }
        }
        
        for(i=0;i<6;i++){
            if(array[i] == color){
                needStones--;
                group++;
            } else if( array[i] == 0){
                if( group > longestGroup){
                    longestGroup = group;
                }
                group = 0;
            }
        }
    } else{
        for(i=0;i<5;i++){
            if(array[i] == opponent){
                return -1;
            } else if(array[i] == color){
                needStones--;
                group++;
            } else if( array[i] == 0){
                if( group > longestGroup){
                    longestGroup = group;
                }
                group = 0;
            }
        }

    }

    if( group > longestGroup){
        longestGroup = group;
    }
    if(longestGroup >= 5){
        return 200000000;
    
    //Extend threat detection
    } else if( (len == 6 && longestGroup == 4 && array[0] == 0 && array[5] == 0)) {
        return 100000;
    }else if(longestGroup == 4){
        (*threats)++;
    }
    return 5 + longestGroup * 5 - needStones;
}

void createDiagonal(State currentGame, int x,int y,int dx,int dy, int array[6]){
    array[0] = currentGame.field[x][y];
    array[1] = currentGame.field[x+1*dx][y+1*dy];
    array[2] = currentGame.field[x+2*dx][y+2*dy];
    array[3] = currentGame.field[x+3*dx][y+3*dy];
    array[4] = currentGame.field[x+4*dx][y+4*dy];
    array[5] = currentGame.field[x+5*dx][y+5*dy];
}

int evaluateBoard(State currentGame, int returnPoints){
    int points = 0;
    int i;
    int threatsB = 0;
    int threatsW = 0;
    int array[6];
    int pointsW = 0;
    int pointsB = 0;
//COLUMNS:
    for(i=0;i<6;i++){
        pointsB += valueRow(currentGame.field[i],6,1,&threatsB);
        pointsW += valueRow(currentGame.field[i],6,2,&threatsW);
    }
   
//ROWS:
    for(i=0;i<6;i++){
        createDiagonal(currentGame,0,i,1,0,array);
       // array = {currentGame.field[0][i],currentGame.field[1][i],currentGame.field[2][i],currentGame.field[3][i],currentGame.field[4][i],currentGame.field[5][i]};
        pointsB += valueRow(array,6,1,&threatsB);
        pointsW += valueRow(array,6,2,&threatsW);
    }
   
//DIAGONALS:
    //From down left to up right
    createDiagonal(currentGame,1,0,1,1,array);
    pointsB += valueRow(array,5,1,&threatsB);
    pointsW += valueRow(array,5,2,&threatsW);
   
    createDiagonal(currentGame,0,0,1,1,array);
    pointsB += valueRow(array,6,1,&threatsB);
    pointsW += valueRow(array,6,2,&threatsW);
   
    createDiagonal(currentGame,0,1,1,1,array);
    pointsB += valueRow(array,5,1,&threatsB);
    pointsW += valueRow(array,5,2,&threatsW);
   
    //From up left to down right
    createDiagonal(currentGame,1,5,1,-1,array);
    pointsB += valueRow(array,5,1,&threatsB);
    pointsW += valueRow(array,5,2,&threatsW);

    createDiagonal(currentGame,0,5,1,-1,array);
    pointsB += valueRow(array,6,1,&threatsB);
    pointsW += valueRow(array,6,2,&threatsW);
    

    createDiagonal(currentGame,0,4,1,-1,array);
    pointsB += valueRow(array,5,1,&threatsB);
    pointsW += valueRow(array,5,2,&threatsW);
    if(returnPoints){
        if(threatsB > 2){
            pointsB += 10000 * threatsB;
        }
        if(threatsW > 2){
            pointsW += 10000 * threatsW;
        }
        if( currentGame.player == 1){
            points = pointsB - pointsW;
        } else{
            points = pointsW - pointsB;
        }
       
        return points;
    }
    if( pointsB > 100000000 && pointsW > 100000000){
        return 0;
    } else if(pointsB > 100000000){
        return 1;
    } else if(pointsW > 100000000){
        return -1;
    } else {
        return 2;
    }
}

int compareBoards(State board1, State board2){
    if( board1.value < board2.value){
        return -1;
    }else if( board1.value > board2.value ){
        return 1;
    } else{
        return 0;
    }
}

void swap(State *x,State *y){
    State temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

int choose_pivot(int i,int j ){
    return((i+j) /2);
}

void quickSort(State list[],int m,int n){
    int key,i,j,k;
    if( m < n) {
        k = choose_pivot(m,n);
        swap(&list[m],&list[k]);
        key = list[m].value;
        i = m+1;
        j = n;
        while(i <= j) {
            while((i <= n) && (list[i].value <= key))
                i++;
            while((j >= m) && (list[j].value > key))
                j--;
            if( i < j)
                swap(&list[i],&list[j]);
        }
        /* swap two elements */
        swap(&list[m],&list[j]);
        
        /* recursively sort the lesser list */
        quickSort(list,m,j-1);
        quickSort(list,j+1,n);
    }
}


int isIndexOfEqualBoard(State state1, State* allStates, int numbAllStates){
	int i;
	for(i=0;i<numbAllStates;i++){
		if(areEqualBoards(state1,allStates[i])){
			return i;
		}
	}
	//printf("Error! Index of Equal Board not found!\n");
	return -1;
}

void sortStates(State currentGame, State* allStates,Line* pLine, int numbAllStates, Variation principal, int currentDepth){
    int i,j;
    State current;
    int cnt = 0;
    
	

    //Search the paths of previous iterations first:
    if(currentGame.marker == 0){
		sort++;
		for(i=principal.length - 1;i>=0;i--){
			if( principal.part[i].length >= currentDepth){
				current = principal.part[i].moves[currentDepth-1];
			} else{
				current.marker = -1;
			}
			if(current.marker == 2){
				j = isIndexOfEqualBoard(current,allStates,numbAllStates);
				if(j!=-1) {
					swap(allStates+cnt,allStates+j);
					allStates[cnt].marker = 0;
					cnt++;
				}
			}
			
		}
	}
    
    quickSort(allStates,0+cnt,numbAllStates-1);
}


State negaMax(State currentGame, int depth, int alpha, int beta, trie evals,Line* pLine, Variation principal, int currentDepth){
    Line currentLine;
    currentDepth++;
    currentLine.length = 0;
    cnt++;

    if(isInTrie(evals,&currentGame)){
        return currentGame;
    }
    
    int i,numbAllStates;
    State temp,best;
    State* allStates;
    best.value = alpha;
    best = currentGame;

    if( depth == 0 || currentGame.moveNumber == 36 ) {
        numbEvals++;
        currentGame.value = evaluateBoard(currentGame,1);
        if(numbTries < MAXNODES) addToTrie(evals,currentGame);
        pLine->length = 0;
        return currentGame;
    }
    
    numbAllStates = allPossibleStates(currentGame, &allStates);
    for( i = 0; i < numbAllStates; i++) {
        allStates[i].value = evaluateBoard(allStates[i],1);
    }
    
    sortStates(currentGame,allStates,pLine,numbAllStates,principal,currentDepth);
    //quickSort(allStates,0,numbAllStates-1);// remove
    for( i = 0; i < numbAllStates; i++ ) {
        if( depth == MAXDEPTH ){
            printf("Loading: %.2lf%% \t\t Alpha: %d Beta: %d\n", (double) 100* (i+1)/ numbAllStates,alpha, beta);
        }
        if( allStates[i].value < -160000000){
            allStates[i].value *= -1; // create a function for this line plus following three lines
            best = allStates[i];
            pLine->moves[0] = best;
            free(allStates);
            return best;
        } else if( allStates[i].value > 160000000){
            temp = allStates[i];
        } else{
            temp = negaMax(allStates[i], depth -1, -beta,-alpha,evals,&currentLine, principal, currentDepth);
        }
        temp.value *= -1;
       
        if( temp.value > alpha) {
            alpha = temp.value;
            best = allStates[i];
            best.value = temp.value;
            pLine->moves[0] = best;
            memcpy(pLine->moves + 1, currentLine.moves, currentLine.length * sizeof(State));
            pLine->length = currentLine.length + 1;
            if( temp.value >= beta){
                numbCuts++;
                allStates[i].value = beta;//break
                best = allStates[i];
                pLine->moves[0] = best;
                if(numbTries < MAXNODES) addToTrie(evals,best);
                free(allStates);
                return best;
            }
            
	    
        }
    }
    free(allStates);
    if(numbTries < MAXNODES) addToTrie(evals,best);
    //printBoard(best);  Seg fault for one case.. best never initialised? - fixed through initializing best with currentGame -> better solution needed. (check for flag if best has changed before adding to trie?
    return best;
}

State negaMax2(State currentGame, int depth, int alpha, int beta, trie evals,Line* pLine){
    Line currentLine;
    currentLine.length = 0;
    (cnt)++;
  
    if(isInTrie(evals,&currentGame)){
        return currentGame;
    }
    
    int i,numbAllStates;
    State temp,best;
    State* allStates;
    best.value = alpha;
    
    if( depth == 0 || currentGame.moveNumber == 36 ) {
        numbEvals++;
        currentGame.value = evaluateBoard(currentGame,1);
        if(numbTries < MAXNODES) addToTrie(evals,currentGame);
        pLine->length = 0;
        return currentGame;
    }
    
    numbAllStates = allPossibleStates(currentGame, &allStates);
    for( i = 0; i < numbAllStates; i++) {
        allStates[i].value = evaluateBoard(allStates[i],1);
    }
    
    quickSort(allStates,0,numbAllStates-1);
    
    for( i = 0; i < numbAllStates; i++ ) {
        if( depth == MAXDEPTH){
            printf("Loading: %.2lf%% \t\t Alpha: %d Beta: %d\n", (double) 100* (i+1)/ numbAllStates,alpha, beta);
        }
        if( allStates[i].value < -160000000){
            allStates[i].value *= -1;
            best = allStates[i];
            pLine->moves[0] = best;
            free(allStates);
            return best;
        }
        if( allStates[i].value > 160000000){
            temp = allStates[i];
        } else{
            temp = negaMax2(allStates[i], depth -1, -beta,-alpha,evals,&currentLine);
        }
        temp.value *= -1;
        
        if( temp.value > alpha) {
            alpha = temp.value;
            best = allStates[i];
            best.value = temp.value;
            pLine->moves[0] = best;
            memcpy(pLine->moves + 1, currentLine.moves, currentLine.length * sizeof(State));
            pLine->length = currentLine.length + 1;
            if( temp.value >= beta){
           	numbCuts++;
           	allStates[i].value = beta;//break
           	best = allStates[i];
          	if(numbTries < MAXNODES) addToTrie(evals,best);
           	free(allStates);
            	return best;
       	    }
        }
    }
    free(allStates);
    if(numbTries < MAXNODES) addToTrie(evals,best);
    return best;
}


void printVariation(int length, State* variations){
    //int i;
   // for(i=0;i<length;i++){
        printBoard(variations[0]);
    //}
}

void iterDeep(State* currentGame, trie evals, int depth){
    int i,lowBound,highBound,j,x;
    //int  aspirationWindow = 50;
    Variation principal;
    lowBound = -INFINITY;
    highBound = INFINITY;
    Line pLine;
    pLine.length = 0;
    principal.length = 0;
    State tempGame;
    for(i = 1; i <= depth;i++){
        evals = newEmptyTrie();
        printf("Depth %d\n",i);

	currentGame->marker = 0;
       	tempGame = negaMax(*currentGame, i, lowBound, highBound,evals,&pLine,principal,0);

        printf("Value of that move: %d \nPrincipal variation of depth %d: \n",tempGame.value,i);
        //printVariation(pLine.length, pLine.moves);
        
        principal.part[principal.length] = pLine;
        (principal.length)++;
		printBoard(principal.part[principal.length-1].moves[0]);
		
    }
    
    *currentGame = pLine.moves[0];
    freeTrie(evals);
}




void botMove(State* currentGame){
    clock_t start, end;
    double cpu_time_used;

    sort = 0;
    Line pLine;
    pLine.length = 0;
    int c;
    numbTrieNodes = numbTries = numbCuts = numbEvals = cnt = 0;
    trie evals;
    evals = newEmptyTrie();
    Variation principal;
    principal.length = 0;
    printf("Which bot?\n");
    scanf("%d",&c);
   
 
    start = clock();
    switch(c){
        case 1:
            iterDeep(currentGame,evals,MAXDEPTH);
            break;
        case 2:
            *currentGame = negaMax2(*currentGame, MAXDEPTH,-INFINITY,INFINITY,evals,&pLine);
            break;
	case 3:
	 	*currentGame = negaMax(*currentGame, MAXDEPTH,-INFINITY,INFINITY,evals,&pLine,principal,0);
		break;
	 case 4:
            *currentGame = negaMax(*currentGame, MAXDEPTH,-INFINITY,INFINITY,evals,&pLine,principal,0);
            break;
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    freeTrie(evals);
    printf("Calculated in %.2lf\n",cpu_time_used);
    printf("Count: %d Cuts: %d Evals: %d  Sorts: %d TrieEntries: %d Expected Value in %d turns: %d\n",cnt,numbCuts,numbEvals,sort,numbTries,MAXDEPTH,currentGame->value);
}
