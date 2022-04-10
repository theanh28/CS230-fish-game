#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Linked List Node
struct Node {
    int v;
    struct Node * next;
};
typedef struct Node node;

// Grid element
struct Grid {
    char v;
    struct Grid *up, *down,
                *left, *right,
                *upright, *upleft,
                *downright, *downleft;
};
typedef struct Grid grid;

/** helper function min(a, b) */
int min(int a, int b) { return a < b ? a : b; }
/**
    @param m, n - wanted size of the grid
    initialize the grid with malloc and build links between grid positions
    @return the game grid
*/
grid * initMap(int m, int n) {
    // initialize the map
    grid * map = (grid *)malloc(m * n * sizeof(grid));
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++ j) {
            // current position being looked into
            grid * cur = map + i * n + j;
            // small trick to get position value: (min of distances to 4 sides) + 1
            int dist = min(i, min(j, min(m - i - 1, n - j - 1))) + 1;
            cur->v = (char)dist + '0';

            // set up linking pointers
            cur->up = cur - n;
            cur->down = cur + n;
            cur->left = cur - 1;
            cur->right = cur + 1;
            cur->upleft = cur - n - 1;
            cur->upright = cur - n + 1;
            cur->downleft = cur + n - 1;
            cur->downright = cur + n + 1;
            // edge cases
            if (i == 0) {
                cur->up = NULL;
                cur->upleft = NULL;
                cur->upright = NULL;
            }
            if (i == m - 1) {
                cur->down = NULL;
                cur->downleft = NULL;
                cur->downright = NULL;
            }
            if (j == 0) {
                cur->left = NULL;
                cur->upleft = NULL;
                cur->downleft = NULL;
            }
            if (j == n - 1) {
                cur->right = NULL;
                cur->upright = NULL;
                cur->downright = NULL;
            }
        }
    // init player and AI starting position, update positions value
	grid * playerStart = map;
    grid * AIStart = map->right;
    playerStart->v = 'P';
    AIStart->v = 'A';
    return map;
}

/**
    @param map, m, n - the grid and its sizes
    print the grid
*/
void printBoard(grid * map, int m, int n) {
    for (int i = 0; i < 3; ++i) {
        printf("\n*********************\n");
    }
    printf("  ");
    // print the row, column number
    for(int i = 0; i < n; ++i) {
        printf("%2d", i + 1);
    }
    printf("\n");

    // pointer to leftmost position of each row on the grid
    grid * front = map, *tp;
    for(int i = 0; i < m; ++i) {
        printf("%2d", i + 1);
        tp = front;
        for (int j = 0; j < n; ++j) {
            // print value at a position then move temp pointer to the right
            printf("%2c", tp->v);
            tp = tp->right;
        }
        printf("\n");
        // front pointer move down to the leftmost position on next row
        front = front->down;
    }
}

/**
    @param startPos - pointer to player/AI current position
    check all 8 directions to see if they can move
    @return whether they can move
*/
bool checkMovable(grid * startPos) {
    // check all 8 direction to see if we can make any move
    // boolean res will be tested against all directions
    bool res = 0;
    // temp pointer to adjacent position
    grid * tp = startPos->up;
    // condition for adjacent position to be "movable to"
    res |= !(!tp || tp->v < '0' || tp->v > '9');
    tp = startPos->down;
    res |= !(!tp || tp->v < '0' || tp->v > '9');
    tp = startPos->left;
    res |= !(!tp || tp->v < '0' || tp->v > '9');
    tp = startPos->right;
    res |= !(!tp || tp->v < '0' || tp->v > '9');
    tp = startPos->upleft;
    res |= !(!tp || tp->v < '0' || tp->v > '9');
    tp = startPos->upright;
    res |= !(!tp || tp->v < '0' || tp->v > '9');
    tp = startPos->downleft;
    res |= !(!tp || tp->v < '0' || tp->v > '9');
    tp = startPos->downright;
    res |= !(!tp || tp->v < '0' || tp->v > '9');
    // res will be true if we can move in any direction
    return res;
}

/**
    @param cur - pointer to current position
    @param dir - direction string
    description as in @return
    @return pointer to the next position after moving 1 step in input direction
*/
grid * singleMove(grid * cur, char * dir) {
    // get pointer to an adjacent position to "cur" in input direction
    if (!strcmp(dir, "UP"))
        return cur->up;
    if (!strcmp(dir, "DOWN"))
        return cur->down;
    if (!strcmp(dir, "LEFT"))
        return cur->left;
    if (!strcmp(dir, "RIGHT"))
        return cur->right;
    if (!strcmp(dir, "UPLEFT"))
    	return cur->upleft;
    if (!strcmp(dir, "UPRIGHT"))
    	return cur->upright;
    if (!strcmp(dir, "DOWNLEFT"))
    	return cur->downleft;
    if (!strcmp(dir, "DOWNRIGHT"))
    	return cur->downright;
    return NULL;
}

// add new score to its Linked List
/**
    @param start - pointer to pointer of Linked List head node
    @param value - value to store in the new List node
    append node to score Linked List
*/
void appendNode(node ** start, int value) {
    // allocate heap memory for a new List node
    node * tp = (node *)malloc(sizeof (node));
    // add value to the node
    tp->v = value;
    tp->next = NULL;

    // if no node is in Linked List, set it as head node
    if (!(*start)) {
        *start = tp;
    } else { // else traverse to tail node and append
        node * iter = *start;
        while(iter->next) {
            iter = iter->next;
        }
        iter->next = tp;
    }
}

/**
    @param map - the grid
    @param startPos - pointer to pointer of player start position this turn
    @param resP - pointer to pointer of score Linked List
    ask for move command, validate and perform the move from players
    @return whether player has made a valid move
*/
bool processPlayerTurn(grid * map, grid ** startPos, node ** resP) {
    printf("Your move? (<DIRECTION> <STEPS>)\n");
    // direction string and steps number
    char * dir = (char *)malloc(100 * sizeof(char));
    int steps = 0;
    scanf("%s %d", dir, &steps);
    // 0 steps = stay still = invalid
    if (!steps) {
        printf("INVALID MOVE\n");
        free(dir);
        return false;
    }
    // we TRY to perform the move command, and curPos is the iterator through the grid in inputted direction
    grid * curPos = *startPos;

    for (int i = 0; i < steps; ++i) {
        // iterator makes a move in said direction
        curPos = singleMove(curPos, dir);
        // conditions to check if player CAN move to the position of the iterator curPos
        if (!curPos || curPos->v < '0' || curPos->v > '9') {
            printf("INVALID MOVE\n");
            free(dir);
            // the player cannot move to this position,
            // meaning it also cannot perform the move command
            return false;
        }
    }

    // the player is able to perform the move command
    // update score Linked List
    appendNode(resP, curPos->v);
    printf("THE PLAYER MOVED TO A %c-POINT POSITION", curPos->v);
    // update the grid value
    (*startPos)->v = '.';
    // update position of the player (move where "player" pointer points to)
    *startPos = curPos;
    curPos->v = 'P';
    free(dir);
    // return true that the player has made a valid move
    return true;
}
/**
    @param map - the grid
    @param m, n - sizes of the grid
    @param startPos - pointer to pointer of the player start position this turn
    @param resP - pointer to pointer of the score Linked List
    perform player turn, check if they can move and make moves
    @return whether the player has made a valid move
*/
bool playerTurn(grid * map, int m, int n, grid ** startPos, node ** resP) {
    if (!checkMovable(*startPos)) {
        printBoard(map, m, n);
        printf("\nTHE PLAYER CANNOT MAKE A VALID MOVE");
        // return false means the player cannot make a valid move
        return false;
    }
    // player may make invalid moves, in which cases we need to keep asking them to try again
    // loopFlag continues the "making move" loop until a valid move has been made
    bool loopFlag = 0;
    while(!loopFlag) {
        printBoard(map, m, n);
        printf("PLAYER TURN\n");
        // if player has made a valid move (the processPlayerTurn() returns true)
        // loopFlag becomes true and ends the loops, else the loop continues
        loopFlag = processPlayerTurn(map, startPos, resP);
    }
    // return true that the player has made a valid move
    return true;
}

/**
    @param a, b - pointer to positions on the grid
    helper function to get position with larger value
    @return position to the better position
*/
grid * betterPos(grid * a, grid * b) {
    if (!a)
        return b;
    if (!b)
        return a;
    // return pointer to whichever position with larger value
    return a->v > b->v ? a : b;
}
// keep moving toward 1 direction + return the best pos along the way
/**
    @param startPos - pointer to AI start position this turn
    @param dir - direction string
    the AI try moving toward 1 direction and pick up the best position
    @return res - pointer to best position
*/
grid * AITrial(grid * startPos, char * dir) {
    // the AI try going in 1 direction, and curPos = iterator of the AI
    grid * curPos = startPos;
    // initially, curPos is set to 1 step next to starting position in the direction
    curPos = singleMove(curPos, dir);
    // holder for pointer to best position in this direction
    grid * res = NULL;
    // conditions to check if the AI can move into curPos (the iterator) position on the grid
    while(!(!curPos || curPos->v < '0' || curPos->v > '9')) {
        // update best position along the way
        res = betterPos(res, curPos);
        // iterator move 1 more step in the direction
        curPos = singleMove(curPos, dir);
    }
    return res;
}
/**
    @param map - the grid
    @param startPos - pointer to pointer of AI start position this turn
    @param resA - pointer to pointer of AI score Linked List
    perform selecting the best move, make the move and update scores
*/
void processAITurn(grid * map, grid ** startPos, node ** resA) {
    // current position with best value
    grid * best = NULL;
    // and test it through all 8 direction to find the best position
    best = betterPos(best, AITrial(*startPos, "UP"));
    best = betterPos(best, AITrial(*startPos, "DOWN"));
    best = betterPos(best, AITrial(*startPos, "LEFT"));
    best = betterPos(best, AITrial(*startPos, "RIGHT"));
    best = betterPos(best, AITrial(*startPos, "UPLEFT"));
    best = betterPos(best, AITrial(*startPos, "UPRIGHT"));
    best = betterPos(best, AITrial(*startPos, "DOWNLEFT"));
    best = betterPos(best, AITrial(*startPos, "DOWNRIGHT"));

    // append value of the best position
    appendNode(resA, best->v);
    printf("\nTHE AI MOVED TO A %c-POINT POSITION", best->v);
    // change value of AI original position on the grid
    (*startPos)->v = '.';
    // make the position pointer point to new best position
    // aka AI has moved
    *startPos = best;
    // update new position value on the grid
    best->v = 'A';
}
/**
    @param map - the grid
    @param m, n - sizes of the grid
    @param startPos - pointer to pointer of AI start position
    @param resA - pointer to pointer of AI score Linked List
    perform AI turn: check if AI can move and make moves
    @return whether the AI did make a move
*/
bool AITurn(grid * map, int m, int n, grid ** startPos, node ** resA) {
    if (!checkMovable(*startPos)) {
        printBoard(map, m, n);
        printf("\nTHE AI CANNOT MAKE A VALID MOVE");
        // return false means the AI cannot make a valid move
        return false;
    }
    printBoard(map, m, n);
    printf("THE AI TURN\n");
    // the AI can make a valid move, and proceed to choose the best move
    processAITurn(map, startPos, resA);
    return true;
}

/**
    @param start - Linked List head pointer
    get sum score of player/AI, print score trace and free LinkedList memory
    @return sum - the total score
*/
int harvestScore(node * start) {
    int sum = 0;
    // cur is iterator through the Linked List, tp is a temp pointer for later use
    node *cur = start, *tp;
    while(cur) { // cur == NULL when the LinkedList ends
        // v is the value of the node, check struct Node above
        printf("%c", cur->v);
        sum += cur->v - '0';
        tp = cur;
        cur = cur->next;
        if (cur) {
            printf(" + ");
        } else {
            printf(" = ");
        }
        // free heap memory allocated for List node
        free(tp);
    }
    printf("%d", sum);
    return sum;
}
/**
    @param resA, resP - Linked List head pointers
    calculate scores and announce results
*/
void announceResult(node * resA, node * resP) {
    printf("\nANNOUNCE RESULT:\n");
    printf("\nPlayer\'s points:  ");
    // sum score of player
    int sumP = harvestScore(resP);
    printf("\nAI\'s points:      ");
    // sum score of AI
    int sumA = harvestScore(resA);
    if (sumA > sumP) {
        printf("\nAI wins\n");
    } else if (sumA == sumP) {
        printf("\nIT\'S A DRAW\n");
    } else {
        printf("\nPlayer wins\n");
    }
}

/**
    @param m, n - sizes of the map
    main loop of the game: initialize, keep looping player + AI turn til the end
*/
void mainProcess(int m, int n) {
    // initialize the grid and player/AI pointer
    grid * map = initMap(m, n);
    // pointer to starting position of player and AI
    grid * playerPos = map;
    grid * AIPos = map->right;

    // linked list for result of player and AI
    node *resA = NULL;
    node *resP = NULL;
    // both auto get +1 at their starting point
    appendNode(&resA, '1');
    appendNode(&resP, '1');
    bool loopFlag = 1;
    // if either can make a move, the game continues, else it ends
    // loopFlag will be True as long as someone can make a move
    while (loopFlag) {
        // if playerTurn() returns true, player has made a valid move
        loopFlag = playerTurn(map, m, n, &playerPos, &resP);
        // if AITurn() returns true, AI has made a valid move
        loopFlag |= AITurn(map, m, n, &AIPos, &resA);
        printBoard(map, m, n);
        printf("\n---THE GAME CONTINUES---\n");
        // if anyone has made a valid move, loopFlag stays as true,
        // continuing the game, or the flag is false and end
    }
    printBoard(map, m, n);
    announceResult(resA, resP);
    // free heap memory
    free(map);
}

int main(void) {
    char c;
    do {
        int m, n;
        printf("WELCOME TO THE FISH GAME...\n");
        printf("CHOOSE YOUR MAP SIZE PWEASE\n(IF YOU ARE MY INSTRUCTORS FROM CS230, type \"6 6\")\n");
        scanf("%d %d", &m, &n);
        mainProcess(m, n);
        printf("WANNA WASTE YOUR TIME HERE AGAIN?\n(Type \'Y\' or I will cry and shutdown)\n");
        scanf(" %c", &c);
    } while(c == 'Y');
    return 0;
}
