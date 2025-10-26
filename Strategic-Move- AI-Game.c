#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define BOARD_SIZE 7
#define PLAYER_X 'X'
#define PLAYER_O 'O'
#define EMPTY '-'

typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    int p1MoveableSpace;
    int p2MoveableSpace;
} GameState;

typedef struct {
    int row;
    int col;
} Coordinate;

// Function to manually place pieces on the board
void manuallyPlacePieces(GameState *state, int numPiecesPerPlayer, int player) {
    printf("Player %d, enter the initial positions of your pieces (format: e.g b6..):\n", player);

    for (int piece = 0; piece < numPiecesPerPlayer; ++piece) {
        char position[3];
        printf("Piece %d: ", piece + 1);
        scanf("%s", position);

        int row = position[0] - 'a';
        int col = position[1] - '1';

        if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || state->board[row][col] != EMPTY) {
            printf("Invalid position. Try again.\n");
            --piece;  // Retry placing the current piece
        } else {
            state->board[row][col] = (player == 1) ? PLAYER_X : PLAYER_O;

            // Update movable spaces
            if (player == 1) {
                state->p1MoveableSpace += 4;  // Assuming all pieces can move in all directions initially
            } else {
                state->p2MoveableSpace += 4;
            }
        }
    }
}

// Function to initialize the game state
void initializeGame(GameState *state, int numPiecesPerPlayer, int *userPlayer) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            state->board[i][j] = EMPTY;
        }
    }

    // Prompt user to choose Player X or O
    printf("Do you want to play with X or O: ");
    char playerChoice;
    scanf(" %c", &playerChoice);

    if (playerChoice == 'X' || playerChoice == 'x') {
        *userPlayer = 1;
    } else if(playerChoice == 'O' || playerChoice == 'o'){
        *userPlayer = 2;
    }else{
        printf("Please enter a valid input.");
        printf("Do you want to play with X or O: ");
        char playerChoice;
        scanf(" %c", &playerChoice);
    }

    // Ask the user whether to place pieces manually or randomly
    int manualPlacement;
    printf("Do you want to place your pieces manually or randomly? \n (1 for manually, 0 for randomly): ");
    scanf("%d", &manualPlacement);

    // Initialize pieces for both players
    for (int player = 1; player <= 2; ++player) {
        if (player == *userPlayer && manualPlacement) {
            manuallyPlacePieces(state, numPiecesPerPlayer, player);
        } else {
            for (int piece = 0; piece < numPiecesPerPlayer; ++piece) {
                int row, col;
                do {
                    row = rand() % BOARD_SIZE;
                    col = rand() % BOARD_SIZE;
                } while (state->board[row][col] != EMPTY);

                state->board[row][col] = (player == 1) ? PLAYER_X : PLAYER_O;

                // Update movable spaces
                if (player == 1) {
                    state->p1MoveableSpace += 4;  // Assuming all pieces can move in all directions initially
                } else {
                    state->p2MoveableSpace += 4;
                }
            }
        }
    }
}

// Function to print the game board
void printBoard(GameState *state) {
    printf("   1 2 3 4 5 6 7\n");
    printf("   ---------------\n");

    for (int i = 0; i < BOARD_SIZE; ++i) {
        printf("%c |", 'a' + i);

        for (int j = 0; j < BOARD_SIZE; ++j) {
            printf(" %c", state->board[i][j]);
        }

        printf("\n");
    }
}

// Function to check if a move is valid
int isValidMove(GameState *state, Coordinate from, Coordinate to, char currentPlayer) {
    if (from.row < 0 || from.row >= BOARD_SIZE || from.col < 0 || from.col >= BOARD_SIZE ||
        to.row < 0 || to.row >= BOARD_SIZE || to.col < 0 || to.col >= BOARD_SIZE) {
        return 0; // Out of bounds
    }

    if (state->board[from.row][from.col] != currentPlayer) {
        return 0; // Trying to move the opponent's piece
    }

    if (state->board[to.row][to.col] != EMPTY) {
        return 0; // Destination is not empty
    }

    // Check if moving to one of the four neighbors
    int dx = abs(from.row - to.row);
    int dy = abs(from.col - to.col);

    return (dx == 0 && dy == 1) || (dx == 1 && dy == 0);
}

// Heuristic function for evaluation
int heuristic(GameState *state, char currentPlayer) {
    int score = 0;

    // Evaluation function can take into account the number of pieces in each cell
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (state->board[i][j] == PLAYER_X) {
                score -= 1; // Negative score for PLAYER_X
            } else if (state->board[i][j] == PLAYER_O) {
                score += 2; // Positive score for PLAYER_O
            }
        }
    }

    // Add the number of movable spaces for each player
    score += (currentPlayer == PLAYER_X) ? state->p1MoveableSpace : state->p2MoveableSpace;

    return score;
}

// Function to make a move on the board
void makeMove(GameState *state, Coordinate from, Coordinate to) {
    state->board[to.row][to.col] = state->board[from.row][from.col];
    state->board[from.row][from.col] = EMPTY;
}

// Function to check if the game is over
int isGameOver(GameState *state) {
    if (state->p1MoveableSpace == state->p2MoveableSpace) {
        printf("Player 1's and Player 2's movable spaces: %d %d\n", state->p1MoveableSpace, state->p2MoveableSpace);
        return 1; // Draw
    } else if (state->p1MoveableSpace == 0 || state->p2MoveableSpace > state->p1MoveableSpace) {
        printf("Player 1's and Player 2's movable spaces: %d %d\n", state->p1MoveableSpace, state->p2MoveableSpace);
        return 2; // Player 2 wins
    } else if (state->p2MoveableSpace == 0 || state->p1MoveableSpace > state->p2MoveableSpace) {
        printf("Player 1's and Player 2's movable spaces: %d %d\n", state->p1MoveableSpace, state->p2MoveableSpace);
        return 3; // Player 1 wins
    }

    return 0;
}


// Minimax algorithm for AI move selection
int minimax(GameState *state, int depth, int maximizingPlayer, int alpha, int beta, char currentPlayer,
            int p1MoveableSpace, int p2MoveableSpace) {
    if (depth == 0 || isGameOver(state)) {
        return heuristic(state, currentPlayer);
    }
    if (maximizingPlayer) {
        int maxEval = INT_MIN;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (state->board[i][j] == PLAYER_O) {
                    Coordinate from = {i, j};

                    for (int x = -1; x <= 1; ++x) {
                        for (int y = -1; y <= 1; ++y) {
                            Coordinate to = {i + x, j + y};

                            if (isValidMove(state, from, to, PLAYER_O)) {
                                GameState newState = *state;
                                makeMove(&newState, from, to);

                                int eval = minimax(&newState, depth - 1, 0, alpha, beta, PLAYER_X,
                                                   newState.p1MoveableSpace, newState.p2MoveableSpace);
                                maxEval = (eval > maxEval) ? eval : maxEval;
                                alpha = (alpha > eval) ? alpha : eval;

                                if (beta <= alpha)
                                    break;
                            }
                        }
                    }
                }
            }
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (state->board[i][j] == PLAYER_X) {
                    Coordinate from = {i, j};

                    for (int x = -1; x <= 1; ++x) {
                        for (int y = -1; y <= 1; ++y) {
                            Coordinate to = {i + x, j + y};

                            if (isValidMove(state, from, to, PLAYER_X)) {
                                GameState newState = *state;
                                makeMove(&newState, from, to);

                                int eval = minimax(&newState, depth - 1, 1, alpha, beta, PLAYER_O,
                                                   newState.p1MoveableSpace, newState.p2MoveableSpace);
                                minEval = (eval < minEval) ? eval : minEval;
                                beta = (beta < eval) ? beta : eval;

                                if (beta <= alpha)
                                    break;
                            }
                        }
                    }
                }
            }
        }
        return minEval;
    }
}

// Function for AI to make a move
void aiMakeMove(GameState *state) {
    int bestVal = INT_MIN;
    Coordinate bestMoveFrom, bestMoveTo;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (state->board[i][j] == PLAYER_O) {
                Coordinate from = {i, j};

                for (int x = -1; x <= 1; ++x) {
                    for (int y = -1; y <= 1; ++y) {
                        Coordinate to = {i + x, j + y};

                        if (isValidMove(state, from, to, PLAYER_O)) {
                            GameState newState = *state;
                            makeMove(&newState, from, to);

                            int p1MoveableSpace = 0, p2MoveableSpace = 0;

                            for (int i = 0; i < BOARD_SIZE; ++i) {
                                for (int j = 0; j < BOARD_SIZE; ++j) {
                                    if (newState.board[i][j] == PLAYER_X) {
                                        if (i > 0 && newState.board[i - 1][j] == EMPTY) p1MoveableSpace++;
                                        if (i < BOARD_SIZE - 1 && newState.board[i + 1][j] == EMPTY) p1MoveableSpace++;
                                        if (j > 0 && newState.board[i][j - 1] == EMPTY) p1MoveableSpace++;
                                        if (j < BOARD_SIZE - 1 && newState.board[i][j + 1] == EMPTY) p1MoveableSpace++;
                                    } else if (newState.board[i][j] == PLAYER_O) {
                                        if (i > 0 && newState.board[i - 1][j] == EMPTY) p2MoveableSpace++;
                                        if (i < BOARD_SIZE - 1 && newState.board[i + 1][j] == EMPTY) p2MoveableSpace++;
                                        if (j > 0 && newState.board[i][j - 1] == EMPTY) p2MoveableSpace++;
                                        if (j < BOARD_SIZE - 1 && newState.board[i][j + 1] == EMPTY) p2MoveableSpace++;
                                    }
                                }
                            }

                            int moveVal = (state->p1MoveableSpace - p1MoveableSpace) - (state->p2MoveableSpace - p2MoveableSpace);

                            if (moveVal > bestVal) {
                                bestMoveFrom = from;
                                bestMoveTo = to;
                                bestVal = moveVal;
                            }
                        }
                    }
                }
            }
        }
    }

    printf("Computer moves the piece at %c%d to %c%d\n", 'a' + bestMoveFrom.row, bestMoveFrom.col + 1, 'a' + bestMoveTo.row,
           bestMoveTo.col + 1);

    makeMove(state, bestMoveFrom, bestMoveTo);

    // Update movable spaces after the move
    state->p1MoveableSpace = 0;
    state->p2MoveableSpace = 0;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (state->board[i][j] == PLAYER_X) {
                if (i > 0 && state->board[i - 1][j] == EMPTY) state->p1MoveableSpace++;
                if (i < BOARD_SIZE - 1 && state->board[i + 1][j] == EMPTY) state->p1MoveableSpace++;
                if (j > 0 && state->board[i][j - 1] == EMPTY) state->p1MoveableSpace++;
                if (j < BOARD_SIZE - 1 && state->board[i][j + 1] == EMPTY) state->p1MoveableSpace++;
            } else if (state->board[i][j] == PLAYER_O) {
                if (i > 0 && state->board[i - 1][j] == EMPTY) state->p2MoveableSpace++;
                if (i < BOARD_SIZE - 1 && state->board[i + 1][j] == EMPTY) state->p2MoveableSpace++;
                if (j > 0 && state->board[i][j - 1] == EMPTY) state->p2MoveableSpace++;
                if (j < BOARD_SIZE - 1 && state->board[i][j + 1] == EMPTY) state->p2MoveableSpace++;
            }
        }
    }
}

// Function for Player1 to make a move
void userMakeMove(GameState *state, int currentPlayer) {
    char fromStr[3], toStr[3];
    Coordinate from, to;

    while (1) {
        printf("Choose piece to move: ");
        scanf("%s", fromStr);

        from.row = fromStr[0] - 'a';
        from.col = fromStr[1] - '1';

        printf("Choose the new position for %s: ", fromStr);
        scanf("%s", toStr);

        to.row = toStr[0] - 'a';
        to.col = toStr[1] - '1';

        if (isValidMove(state, from, to, (currentPlayer == 1) ? PLAYER_X : PLAYER_O)) {
            makeMove(state, from, to);
            break;
        } else {
            printf("Invalid move. Try again.\n");
        }
    }
}

int main() {
    // Seed the random number generator
    srand(time(NULL));
    GameState game;
    int numPiecesPerPlayer, maxTurns, userPlayer;

    printf("Enter the number of pieces per player: ");
    scanf("%d", &numPiecesPerPlayer);

    printf("Enter the maximum number of turns: ");
    scanf("%d", &maxTurns);

    initializeGame(&game, numPiecesPerPlayer, &userPlayer);
    int currentPlayer = (userPlayer == 1) ? 1 : 2;
    int turn = 0;

    while (1) {
        printBoard(&game);

        if (currentPlayer == userPlayer) {
            userMakeMove(&game, currentPlayer);
        } else {
            aiMakeMove(&game);
        }

        currentPlayer = (currentPlayer == 1) ? 2 : 1;
        turn++;

        if (turn >= maxTurns) {
            int gameResult = isGameOver(&game);
            if (gameResult != 0) {
                printBoard(&game);
                if (gameResult == 1) {
                    printf("Game Over. It's a draw!\n");
                } else if (gameResult == 2) {
                    printf("Game Over. Player 2 (O) wins!\n");
                } else if (gameResult == 3) {
                    printf("Game Over. Player 1 (X) wins!\n");
                }
                break;
            }
        }
    }
    return 0;
}
