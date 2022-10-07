#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

int verifyZero(char *arg)
{
    int i = 0;
    if (arg[0] == '-')
        i = 1;
    for (; arg[i] != 0; i++)
    {
        if (!isdigit(arg[i]))
            return 0;
    }
    return 1;
}

int playerMove(int *player, int *dir, int boardX, int boardY)
{
    int newPlayerX = player[3] + dir[0];
    int newPlayerY = player[2] + dir[1];

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

int main(int argc, char **argv)
{
    srand(time(NULL));
    int boardX = atoi(argv[1]);
    int boardY = atoi(argv[2]);

    if (boardX <= 0 || boardY <= 0)
    {
        printf("%d", boardX);
        printf("Invalid arguments: the desired dimension of the board is invalid\n");
        return EXIT_FAILURE;
    }
    /* player and monster are represented by arrays in the following manner:
     * [prevY, prevX, currY, currX]
     */
    int *player = malloc(4 * sizeof(int));
    player[0] = -1;
    player[1] = -1;
    player[2] = atoi(argv[4]);
    player[3] = atoi(argv[3]);

    if (player[2] == 0)
    {
        int zero = verifyZero(argv[4]);
        if (!zero)
        {
            printf("Invalid arguments: the starting coordinates of the player are invalid\n");
            free(player);
            return EXIT_FAILURE;
        }
    }
    if (player[3] == 0)
    {
        int zero = verifyZero(argv[3]);
        if (!zero)
        {
            printf("Invalid arguments: the starting coordinates of the player are invalid\n");
            free(player);
            return EXIT_FAILURE;
        }
    }

    if (player[2] < 0 || player[3] < 0 || player[2] >= boardY || player[3] >= boardX)
    {
        printf("Invalid arguments: the starting coordinates of the player are invalid\n");
        free(player);
        return EXIT_FAILURE;
    }

    int *goal = malloc(2 * sizeof(int));
    goal[0] = atoi(argv[6]);
    goal[1] = atoi(argv[5]);

    if (goal[0] < 0 || goal[1] < 0 || goal[0] >= boardY || goal[1] >= boardX)
    {
        printf("Invalid arguments: the starting coordinates of the goal are invalid\n");
        free(player);
        free(goal);
        return EXIT_FAILURE;
    }

    if (goal[0] == 0)
    {
        int zero = verifyZero(argv[6]);
        if (!zero)
        {
            printf("Invalid arguments: the starting coordinates of the goal are invalid\n");
            free(player);
            free(goal);
            return EXIT_FAILURE;
        }
    }
    if (goal[1] == 0)
    {
        int zero = verifyZero(argv[5]);
        if (!zero)
        {
            printf("Invalid arguments: the starting coordinates of the goal are invalid\n");
            free(player);
            free(goal);
            return EXIT_FAILURE;
        }
    }

    int *monster = malloc(4 * sizeof(int));
    monster[0] = -1;
    monster[1] = -1;
    monster[2] = atoi(argv[8]);
    monster[3] = atoi(argv[7]);

    if (monster[2] == 0)
    {
        int zero = verifyZero(argv[8]);
        if (!zero)
        {
            printf("Invalid arguments: the starting coordinates of the player are invalid\n");
            free(player);
            free(goal);
            free(monster);
            return EXIT_FAILURE;
        }
    }
    if (monster[3] == 0)
    {
        int zero = verifyZero(argv[7]);
        if (!zero)
        {
            printf("Invalid arguments: the starting coordinates of the player are invalid\n");
            free(player);
            free(goal);
            free(monster);
            return EXIT_FAILURE;
        }
    }

    if (monster[2] < 0 || monster[3] < 0 || monster[2] >= boardY || monster[3] >= boardX)
    {
        printf("Invalid arguments: the starting coordinates of the monster are invalid\n");
        free(player);
        free(monster);
        free(goal);
        return EXIT_FAILURE;
    }

    if (monster[2] == goal[0] && monster[3] == goal[1])
    {
        printf("Invalid arguments: the starting coordinates of the goal and monster are tha same\n");
        free(player);
        free(monster);
        free(goal);
        return EXIT_FAILURE;
    }

    if (monster[2] == player[2] && monster[3] == player[3])
    {
        printf("monster wins!\n");
        free(player);
        free(monster);
        free(goal);
        return EXIT_SUCCESS;
    }

    if (player[2] == goal[0] && player[3] == goal[1])
    {
        printf("player wins!\n");
        free(player);
        free(monster);
        free(goal);
        return EXIT_SUCCESS;
    }

    char **board = malloc(boardY * sizeof(char *));
    for (int i = 0; i < boardY; i++)
    {
        board[i] = malloc(boardX * sizeof(char));
        for (int j = 0; j < boardX; j++)
        {
            board[i][j] = '.';
        }
    }
    board[goal[0]][goal[1]] = 'G';
    updateBoard(board, player, monster);
    printBoard(board, boardX, boardY);
    int breakpt = 0;
    while (1)
    {
        char *input = malloc(2 * sizeof(char));
        int size = 2;
        int idx = 0;
        while (1)
        {
            if (scanf("%c", input + idx) == EOF)
            {
                breakpt = 1;
                break;
            }
            if (input[idx] == '\r' || input[idx] == '\n')
            {
                input[idx] = '\0';
                break;
            }
            idx++;
            if (idx == size)
            {
                char *temp = malloc(2 * size * sizeof(char));
                for (int i = 0; i < size; i++)
                    temp[i] = input[i];
                free(input);
                input = temp;
                size *= 2;
            }
        }

        if (breakpt)
        {
            free(input);
            break;
        }

        if (idx > 1)
        {
            printf("invalid input\n");
            free(input);
            continue;
        }
        int *dir;
        if (*input == 'N')
        {
            dir = malloc(2 * sizeof(int));
            dir[0] = 0;
            dir[1] = 1;
        }
        else if (*input == 'E')
        {
            dir = malloc(2 * sizeof(int));
            dir[0] = 1;
            dir[1] = 0;
        }
        else if (*input == 'S')
        {
            dir = malloc(2 * sizeof(int));
            dir[0] = 0;
            dir[1] = -1;
        }
        else if (*input == 'W')
        {
            dir = malloc(2 * sizeof(int));
            dir[0] = -1;
            dir[1] = 0;
        }
        else
        {
            printf("invalid input\n");
            free(input);
            continue;
        }
        int playerMoveStatus = playerMove(player, dir, boardX, boardY);
        free(dir);
        if (!playerMoveStatus)
        {
            printf("invalid move\n");
            free(input);
            continue;
        }
        free(input);
        if (goal[0] == player[2] && goal[1] == player[3])
        {
            printf("player wins!\n");
            break;
        }
        if (monster[2] == player[2] && monster[3] == player[3])
        {
            printf("monster wins!\n");
            break;
        }
        char monsterMoveState = monsterMove(monster, player, goal);
        if (monsterMoveState != '\0')
            printf("monster moves %c\n", monsterMoveState);
        else
            printf("monster forfeits turn\n");
        if (monster[2] == player[2] && monster[3] == player[3])
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
