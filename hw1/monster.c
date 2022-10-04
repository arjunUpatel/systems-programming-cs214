#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int playerMove(int *player, int *dir, int boardX, int boardY)
{
    int newPlayerX = player[3] + dir[0];
    int newPlayerY = player[2] + dir[1];

    printf("newPlayerX: %d\n", newPlayerX);
    printf("newPlayerY: %d\n", newPlayerY);
    if (newPlayerY < 0 || newPlayerY >= boardY || newPlayerX < 0 || newPlayerX >= boardX)
        return 0;

    player[0] = player[2];
    player[1] = player[3];
    player[2] = newPlayerY;
    player[3] = newPlayerX;
    return 1;
}

char monsterMove(int *monster, int *player, int *goal)
{
    int xDist = player[3] - monster[3];
    int yDist = player[2] - monster[2];
    int xDistAbs = abs(xDist);
    int yDistAbs = abs(yDist);
    if (xDistAbs > yDistAbs)
    {
        if (xDist > 0 && !(monster[2] == goal[0] && monster[3] + 1 == goal[1]))
        {
            monster[0] = monster[2];
            monster[1] = monster[3];
            monster[3]++;
            return 'E';
        }
        if (xDist < 0 && !(monster[2] == goal[0] && monster[3] - 1 == goal[1]))
        {
            monster[0] = monster[2];
            monster[1] = monster[3];
            monster[3]--;
            return 'W';
        }
    }
    else if (xDistAbs < yDistAbs)
    {
        if (yDist > 0 && !(monster[2] + 1 == goal[0] && monster[3] == goal[1]))
        {
            monster[0] = monster[2];
            monster[1] = monster[3];
            monster[2]++;
            return 'N';
        }
        if (yDist < 0 && !(monster[2] - 1 == goal[0] && monster[3] == goal[1]))
        {
            monster[0] = monster[2];
            monster[1] = monster[3];
            monster[2]--;
            return 'S';
        }
    }
    else
    {
        int r = rand() % 2;
        if (r == 0)
        {
            // do stuff with x distance
            if (xDist > 0 && !(monster[2] == goal[0] && monster[3] + 1 == goal[1]))
            {
                monster[0] = monster[2];
                monster[1] = monster[3];
                monster[3]++;
                return 'E';
            }
            else if (xDist < 0 && !(monster[2] == goal[0] && monster[3] - 1 == goal[1]))
            {
                monster[0] = monster[2];
                monster[1] = monster[3];
                monster[3]--;
                return 'W';
            }
            else
            {
                if (yDist > 0 && !(monster[2] + 1 == goal[0] && monster[3] == goal[1]))
                {
                    monster[0] = monster[2];
                    monster[1] = monster[3];
                    monster[2]++;
                    return 'N';
                }
                if (yDist < 0 && !(monster[2] - 1 == goal[0] && monster[3] == goal[1]))
                {
                    monster[0] = monster[2];
                    monster[1] = monster[3];
                    monster[2]--;
                    return 'S';
                }
            }
        }
        else
        {
            // do stuff with y distance
            if (yDist > 0 && !(monster[2] + 1 == goal[0] && monster[3] == goal[1]))
            {
                monster[0] = monster[2];
                monster[1] = monster[3];
                monster[2]++;
                return 'N';
            }
            else if (yDist < 0 && !(monster[2] - 1 == goal[0] && monster[3] == goal[1]))
            {
                monster[0] = monster[2];
                monster[1] = monster[3];
                monster[2]--;
                return 'S';
            }
            else
            {
                if (xDist > 0 && !(monster[2] == goal[0] && monster[3] + 1 == goal[1]))
                {
                    monster[0] = monster[2];
                    monster[1] = monster[3];
                    monster[3]++;
                    return 'E';
                }
                if (xDist < 0 && !(monster[2] == goal[0] && monster[3] - 1 == goal[1]))
                {
                    monster[0] = monster[2];
                    monster[1] = monster[3];
                    monster[3]--;
                    return 'W';
                }
            }
        }
    }
    return '\0';
}

void updateBoard(char **board, int *player, int *monster)
{
    if (player[0] != -1)
    {
        board[player[0]][player[1]] = '.';
        board[monster[0]][monster[1]] = '.';
    }
    board[player[2]][player[3]] = 'P';
    board[monster[2]][monster[3]] = 'M';
}

void printBoard(char **board, int boardX, int boardY)
{
    for (int i = boardY - 1; i >= 0; i--)
    {
        for (int j = 0; j < boardX; j++)
        {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    // save inputs and create data
    int boardX = atoi(argv[1]);
    int boardY = atoi(argv[2]);

    /* player and monster are represented by arrays in the following manner:
     * [prevX, prevY, currX, currY]
     */
    int *player = malloc(4 * sizeof(int));
    player[0] = -1;
    player[1] = -1;
    player[2] = atoi(argv[4]);
    player[3] = atoi(argv[3]);

    int *goal = malloc(2 * sizeof(int));
    goal[0] = atoi(argv[6]);
    goal[1] = atoi(argv[5]);

    int *monster = malloc(4 * sizeof(int));
    monster[0] = -1;
    monster[1] = -1;
    monster[2] = atoi(argv[8]);
    monster[3] = atoi(argv[7]);

    // create game board
    char **board = malloc(boardY * sizeof(char *));
    for (int i = 0; i < boardY; i++)
    {
        board[i] = malloc(boardX * sizeof(char));
        for (int j = 0; j < boardX; j++)
        {
            board[i][j] = '.';
        }
    }
    printf("goal: %d %d\n", goal[0], goal[1]);
    board[goal[0]][goal[1]] = 'G';
    updateBoard(board, player, monster);
    printBoard(board, boardX, boardY);
    char input;

    while (!feof(stdin))
    {
        scanf(" %c", &input);
        if (feof(stdin))
            break;

        int *dir = malloc(2 * sizeof(int));
        if (input == 'N')
        {
            dir[0] = 0;
            dir[1] = 1;
        }
        else if (input == 'E')
        {
            dir[0] = 1;
            dir[1] = 0;
        }
        else if (input == 'S')
        {
            dir[0] = 0;
            dir[1] = -1;
        }
        else if (input == 'W')
        {
            dir[0] = -1;
            dir[1] = 0;
        }
        else
        {
            printf("invalid input\n");
            continue;
        }
        int playerMoveStatus = playerMove(player, dir, boardX, boardY);
        free(dir);
        if (!playerMoveStatus)
        {
            printf("invalid move\n");
            continue;
        }
        // ask when win conditions have to be checked, maybe you need to check here
        if (monster[2] == player[2] && monster[3] == player[3])
        {
            printf("monster wins!\n");
            break;
        }
        char monsterMoveState = monsterMove(monster, player, goal);
        // check monster win conditions
        if (monsterMoveState != '\0')
            printf("monster moves %c\n", monsterMoveState);
        else
            printf("monster forfeits turn\n"); // ask what to print if monster forfeits turn
        if (goal[0] == player[2] && goal[1] == player[3])
        {
            printf("player wins!\n");
            break;
        }
        else if (monster[2] == player[2] && monster[3] == player[3])
        {
            printf("monster wins!\n");
            break;
        }
        else
        {
            updateBoard(board, player, monster);
            printBoard(board, boardX, boardY);
        }
    }
    for (int i = 0; i < boardY; i++)
        free(board[i]);
    free(board);
    free(player);
    free(goal);
    free(monster);
    return EXIT_SUCCESS;
}
