#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef TEST
#    define SIZE 8
#    define INPUT "../inputs/day_9_test.txt"
#else
#    define SIZE 496
#    define INPUT "../inputs/day_9.txt"
#endif

#define i64 long long
#define u64 unsigned i64

struct vec2
{
    int x, y;
};

u64 part_1(struct vec2 positions[SIZE]);
u64 part_2(struct vec2 positions[SIZE]);

int main(void)
{
    FILE *input = fopen(INPUT, "r");
    if (!input)
    {
        fprintf(stderr, "failed to open file\n");
        return 1;
    }

    struct vec2 positions[SIZE] = {0};

    {
        struct vec2 *p = positions;
        while (fscanf(input, " %d,%d", &p->x, &p->y) == 2 && p != positions + SIZE)
        {
            p++;
        }
        assert(p == positions + SIZE);
    }

    u64 answer_1 = part_1(positions);
    u64 answer_2 = part_2(positions);

    printf("%llu %llu\n", answer_1, answer_2);

#ifdef TEST
    assert(answer_1 == 50);
    assert(answer_2 == 24);
#endif

    fclose(input);

    return 0;
}

#define min(a, b) ((a) > (b) ? (b) : (a))
#define max(a, b) ((a) > (b) ? (a) : (b))

u64 part_1(struct vec2 positions[SIZE])
{
    u64 max_area = 0;

    for (struct vec2 *pi = positions; pi < positions + SIZE - 1; pi++)
    {
        for (struct vec2 *pj = pi + 1; pj < positions + SIZE; pj++)
        {
            int x1 = min(pi->x, pj->x), y1 = min(pi->y, pj->y);
            int x2 = max(pi->x, pj->x), y2 = max(pi->y, pj->y);

            u64 dx = x2 - x1 + 1;
            u64 dy = y2 - y1 + 1;

            if (dx * dy > max_area)
            {
                max_area = dx * dy;
            }
        }
    }

    return max_area;
}
u64 part_2(struct vec2 positions[SIZE])
{
    u64 max_area = 0;

    for (struct vec2 *pi = positions; pi < positions + SIZE - 1; pi++)
    {
        for (struct vec2 *pj = pi + 1; pj < positions + SIZE; pj++)
        {
            int x1 = min(pi->x, pj->x), y1 = min(pi->y, pj->y);
            int x2 = max(pi->x, pj->x), y2 = max(pi->y, pj->y);

            u64 dx = x2 - x1 + 1;
            u64 dy = y2 - y1 + 1;

            if (dx * dy > max_area)
            {
                struct vec2 *pk = positions;
                for (; pk < positions + SIZE; pk++)
                {
                    struct vec2 *pl = (pk == positions + SIZE - 1) ? positions : pk + 1;
                    if (x2 > min(pk->x, pl->x) && y2 > min(pk->y, pl->y) &&
                        max(pk->x, pl->x) > x1 && max(pk->y, pl->y) > y1)
                    {
                        break;
                    }
                }
                max_area = (pk == positions + SIZE) * dx * dy + (pk != positions + SIZE) * max_area;
            }
        }
    }

    return max_area;
}
