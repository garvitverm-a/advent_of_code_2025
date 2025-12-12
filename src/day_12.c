#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#define NUM_SHAPES 6
#define SIZE 1000
#define INPUT "../inputs/day_12.txt"

#define u8 uint8_t
#define u64 uint64_t

typedef char shape[3][3];
struct region
{
    u8 x, y;
    u8 requirements[NUM_SHAPES];
} regions[SIZE]          = {0};
shape shapes[NUM_SHAPES] = {0};
u8 areas[NUM_SHAPES]     = {0};

u64 part_1(void);

int main(void)
{
    FILE *input = fopen(INPUT, "r");
    if (!input)
    {
        fprintf(stderr, "failed to open file\n");
        return 1;
    }

    for (u8 i = 0; i < NUM_SHAPES; i++)
    {
        fscanf(input, " %*s");
        for (u8 j = 0; j < 3; j++)
        {
            fscanf(input, " %c%c%c", &shapes[i][j][0], &shapes[i][j][1], &shapes[i][j][2]);
            areas[i] +=
                (shapes[i][j][0] == '#') + (shapes[i][j][1] == '#') + (shapes[i][j][2] == '#');
        }
    }

    for (size_t i = 0; i < SIZE; i++)
    {
        int width, length;
        fscanf(input, "%dx%d:", &width, &length);
        regions[i].x = (u8)width, regions[i].y = (u8)length;
        for (u8 j = 0; j < NUM_SHAPES; j++)
        {
            int req;
            fscanf(input, " %d", &req);
            regions[i].requirements[j] = (u8)req;
        }
    }

    printf("%llu\n", part_1());

    fclose(input);

    return 0;
}

u64 part_1()
{
    u64 result = 0;
    for (size_t i = 0; i < SIZE; i++)
    {
        u64 shape_area  = (u64)regions[i].x * (u64)regions[i].y;
        u64 requirement = 0;
        for (u8 j = 0; j < NUM_SHAPES; j++)
        {
            requirement += areas[j] * regions[i].requirements[j];
        }
        result += shape_area > requirement;
    }
    return result;
}
