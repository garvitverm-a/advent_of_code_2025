#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef TEST
#    define MAX_ITERS 10
#    define SIZE 20
#    define INPUT "../inputs/day_8_test.txt"
#else
#    define MAX_ITERS 1000
#    define SIZE 1000
#    define INPUT "../inputs/day_8.txt"
#endif

#define f64 double
#define i64 long long
#define u64 unsigned i64

struct vec3
{
    int x, y, z;
};

struct edge
{
    size_t src_idx, dest_idx;
    u64 distance;
};

struct disjoint_union
{
    size_t parent[SIZE];
    u64 size[SIZE];
};

struct disjoint_union dju_new()
{
    struct disjoint_union new = {0};
    for (size_t i = 0; i < SIZE; i++)
    {
        new.parent[i] = i;
        new.size[i]   = 1;
    }
    return new;
}

size_t dju_find(struct disjoint_union *set, size_t i)
{
    if (set->parent[i] == i)
    {
        return i;
    }
    size_t ancestor = dju_find(set, set->parent[i]);
    set->parent[i]  = ancestor;
    return ancestor;
}

void dju_unite(struct disjoint_union *set, size_t i1, size_t i2)
{
    size_t parent_1 = dju_find(set, i1);
    size_t parent_2 = dju_find(set, i2);

    if (parent_1 == parent_2)
    {
        return;
    }

    u64 new_size = set->size[parent_1] + set->size[parent_2];

    if (set->size[parent_1] < set->size[parent_2])
    {
        set->size[parent_2]   = new_size;
        set->size[parent_1]   = 0;
        set->parent[parent_1] = parent_2;
    }
    else if (set->size[parent_1] > set->size[parent_2])
    {
        set->size[parent_1]   = new_size;
        set->size[parent_2]   = 0;
        set->parent[parent_2] = parent_1;
    }
    else
    {
        set->size[parent_2]   = new_size;
        set->size[parent_1]   = 0;
        set->parent[parent_1] = parent_2;
    }
}

bool dju_in_same_set(struct disjoint_union *set, size_t i1, size_t i2)
{
    return dju_find(set, i1) == dju_find(set, i2);
}

u64 vec_distance_sqr(struct vec3 r1, struct vec3 r2)
{
    u64 dx  = abs(r1.x - r2.x);
    u64 dx2 = dx * dx;
    u64 dy  = abs(r1.y - r2.y);
    u64 dy2 = dy * dy;
    u64 dz  = abs(r1.z - r2.z);
    u64 dz2 = dz * dz;

    return dx2 + dy2 + dz2;
}

int compare_edge(const void *a, const void *b)
{
    const struct edge *edge_1 = a;
    const struct edge *edge_2 = b;

    if (edge_1->distance < edge_2->distance)
    {
        return -1;
    }
    if (edge_2->distance < edge_1->distance)
    {
        return 1;
    }
    return 0;
}

u64 part_1(struct vec3 positions[SIZE]);
u64 part_2(struct vec3 positions[SIZE]);

int main(void)
{
    FILE *input = fopen(INPUT, "r");
    if (!input)
    {
        fprintf(stderr, "failed to open file\n");
        return 1;
    }

    struct vec3 positions[SIZE] = {0};

    {
        struct vec3 *p = positions;
        while (fscanf(input, " %d,%d,%d", &p->x, &p->y, &p->z) == 3 && p != positions + SIZE)
        {
            p++;
        }
        assert(p == positions + SIZE);
    }

    u64 answer_1 = part_1(positions);
    u64 answer_2 = part_2(positions);

    printf("%llu %llu\n", answer_1, answer_2);

#ifdef TEST
    assert(answer_1 == 40);
    assert(answer_2 == 25272);
#endif

    fclose(input);

    return 0;
}

u64 part_1(struct vec3 positions[SIZE])
{
    struct disjoint_union ds = dju_new();

    struct edge *edges = calloc(SIZE * SIZE, sizeof(struct edge));
    size_t edge_count  = 0;

    for (size_t i = 0; i < SIZE - 1; i++)
    {
        for (size_t j = i + 1; j < SIZE; j++)
        {
            u64 distance        = vec_distance_sqr(positions[i], positions[j]);
            edges[edge_count++] = (struct edge){.distance = distance, .src_idx = i, .dest_idx = j};
        }
    }

    qsort(edges, edge_count, sizeof(struct edge), compare_edge);

    for (size_t i = 0; i < MAX_ITERS && i < edge_count; i++)
    {
        struct edge edge = edges[i];
        if (dju_in_same_set(&ds, edge.src_idx, edge.dest_idx))
        {
            continue;
        }
        dju_unite(&ds, edge.src_idx, edge.dest_idx);
    }

    free(edges);

    u64 max1 = 0;
    u64 max2 = 0;
    u64 max3 = 0;
#ifdef DEBUG
    u64 total_size        = 0;
    u64 total_connections = 0;
#endif

    for (size_t i = 0; i < SIZE; i++)
    {
        u64 current_num = ds.size[i];
#ifdef DEBUG
        printf("%llu ", current_num);

        total_size += current_num;
        i64 connections = current_num - 1;
        total_connections += connections > 0 ? connections : 0;
#endif

        if (current_num > max1)
        {
            max3 = max2;
            max2 = max1;
            max1 = current_num;
        }
        else if (current_num > max2)
        {
            max3 = max2;
            max2 = current_num;
        }
        else if (current_num > max3)
        {
            max3 = current_num;
        }
    }

#ifdef DEBUG
    printf("\n");

    printf("total size: %llu, total connections: %llu\n", total_size, total_connections);

    assert(total_size == SIZE);
    assert(total_connections <= MAX_ITERS);
    printf("%llu, %llu, %llu\n", max1, max2, max3);
#endif
    return max1 * max2 * max3;
}

bool edge_equal(struct edge edge_1, struct edge edge_2)
{
    return edge_1.src_idx == edge_2.src_idx && edge_1.dest_idx == edge_2.dest_idx;
}

u64 part_2(struct vec3 positions[SIZE])
{
    struct disjoint_union ds = dju_new();

    struct edge *edges = calloc(SIZE * SIZE, sizeof(struct edge));
    size_t edge_count  = 0;

    for (size_t i = 0; i < SIZE - 1; i++)
    {
        for (size_t j = i + 1; j < SIZE; j++)
        {
            u64 distance        = vec_distance_sqr(positions[i], positions[j]);
            edges[edge_count++] = (struct edge){.distance = distance, .src_idx = i, .dest_idx = j};
        }
    }

    qsort(edges, edge_count, sizeof(struct edge), compare_edge);

    struct edge last_edge = edges[0];

    for (size_t i = 0; i < edge_count; i++)
    {
        struct edge edge = edges[i];
        if (dju_in_same_set(&ds, edge.src_idx, edge.dest_idx))
        {
            continue;
        }
        dju_unite(&ds, edge.src_idx, edge.dest_idx);
        if (!edge_equal(last_edge, edge))
        {
            last_edge = edge;
        }
    }

    u64 result = (u64)positions[last_edge.dest_idx].x * (u64)positions[last_edge.src_idx].x;

    free(edges);

    return result;
}
