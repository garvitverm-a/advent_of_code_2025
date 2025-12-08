#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TEST
#    define SIZE 16
#    define INPUT "../inputs/day_7_test.txt"
#else
#    define SIZE 142
#    define INPUT "../inputs/day_7.txt"
#endif

#define u64 unsigned long long

bool visited[SIZE][SIZE] = {0};
u64 cache[SIZE][SIZE]    = {0};

u64 part_1(char contents[SIZE][SIZE], int x, int y);
u64 part_2(char contents[SIZE][SIZE], int x, int y);

int main(void)
{
    FILE *input = fopen(INPUT, "r");
    if (!input)
    {
        fprintf(stderr, "failed to open file\n");
        return 1;
    }

    char buffer[SIZE][SIZE] = {0};

    {
        int idx         = 0;
        char *buf       = NULL;
        size_t line_cap = 0;
        while (getline(&buf, &line_cap, input) != -1 && idx != SIZE)
        {
            memcpy(buffer[idx], buf, SIZE);
            idx += 1;
        }
    }

    u64 answer_1 = part_1(buffer, 0, SIZE / 2 - 1);
    u64 answer_2 = part_2(buffer, 0, SIZE / 2 - 1);

    printf("%llu %llu\n", answer_1, answer_2);

#ifdef TEST
    assert(answer_1 == 21 && printf);
    assert(answer_2 == 40);
#endif

    fclose(input);

    return 0;
}

u64 part_1(char contents[SIZE][SIZE], int x, int y)
{
    if (x >= SIZE || x < 0 || y >= SIZE || y < 0)
    {
        return 0;
    }

    while (x < SIZE)
    {
        if (contents[x][y] == '^' && !visited[x][y])
        {
            visited[x][y] = true;
            u64 left      = part_1(contents, x, y - 1);
            u64 right     = part_1(contents, x, y + 1);
            return 1 + left + right;
        }
        else if (contents[x][y] == '^')
        {
            break;
        }
        x++;
    }

    return 0;
}

u64 part_2(char contents[SIZE][SIZE], int x, int y)
{
    if (x >= SIZE || x < 0 || y >= SIZE || y < 0)
    {
        return 1;
    }

    if (cache[x][y] != 0)
    {
        return cache[x][y];
    }

    if (contents[x][y] == '^')
    {
        u64 left    = part_2(contents, x + 1, y - 1);
        u64 right   = part_2(contents, x + 1, y + 1);
        cache[x][y] = left + right;
    }
    else
    {
        cache[x][y] = part_2(contents, x + 1, y);
    }

    return cache[x][y];
}
