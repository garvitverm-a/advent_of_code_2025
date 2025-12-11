#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef TEST
#    define SIZE 13 + 1  // out node
#    define INPUT "../inputs/day_11_test.txt"
#else
#    define SIZE 543 + 1  // out node
#    define INPUT "../inputs/day_11.txt"
#endif

#define u32 uint32_t
#define u64 uint64_t

// every node is 3 letters. this can be used to make a single number of size 4 bytes
#define node_name(node) ((((u32)node[0]) << 16) | (((u32)node[1]) << 8) | (u32)node[2])

#define src_1 node_name("you")
#define src_2 node_name("svr")
#define dac node_name("dac")
#define fft node_name("fft")
#define dest node_name("out")

bool visited[SIZE] = {0};

struct graph
{
    u32 nodes[SIZE];
    size_t neighbours[SIZE][SIZE];
    size_t neighbour_counts[SIZE];
};

size_t graph_find(const struct graph *g, u32 node)
{
    size_t idx = 0;
    // NOTE: this can iterate over garbage values, but if graph has been set to all 0s, it won't
    // matter since a node's name can't be all 0s
    for (; idx < SIZE && g->nodes[idx] != node; idx++)
        ;
    return idx;
}

void graph_add_edge(struct graph *g, u32 source, u32 destination)
{
    size_t i = graph_find(g, source);
    size_t j = graph_find(g, destination);

    assert(i < SIZE && j < SIZE);

    g->neighbours[i][g->neighbour_counts[i]++] = j;
}

u64 part_1(const struct graph *g, size_t source_idx, size_t output_idx);
u64 part_2(const struct graph *g, size_t source_idx, size_t output_idx);

int main(void)
{
    FILE *input = fopen(INPUT, "r");
    if (!input)
    {
        fprintf(stderr, "failed to open file\n");
        return 1;
    }

    struct graph g      = {0};
    size_t source_1_idx = 0;
    size_t source_2_idx = 0;
    size_t output_idx   = 0;

    size_t node_count = 0;

    for (size_t i = 0; i < SIZE; i++)
    {
        char *line       = NULL;
        size_t line_cap  = 0;
        ssize_t line_len = getline(&line, &line_cap, input);

        if (line_len < 5)
        {
            free(line);
            break;
        }

        char *p           = line;
        ssize_t remaining = line_len;

        char node[3];
        if (sscanf(p, "%c%c%c", &node[0], &node[1], &node[2]) != 3)
        {
            fprintf(stderr, "malformed input\n");
            free(line);
            exit(1);
        }

        u32 source_node = node_name(node);

        if (graph_find(&g, source_node) == SIZE)
        {
            if (node_count >= SIZE)
            {
                fprintf(stderr, "too many nodes\n");
                free(line);
                exit(1);
            }
            if (node_name(node) == src_1)
            {
                source_1_idx = node_count;
            }
            if (node_name(node) == src_2)
            {
                source_2_idx = node_count;
            }
            g.nodes[node_count++] = source_node;
        }

        p += 3;
        remaining -= 3;

        while (remaining > 0 && *p != '\n')
        {
            while (remaining > 0 && *p != '\n' && !isalpha(*p))
            {
                p++;
                remaining--;
            }

            if (remaining == 0 || *p == '\n')
            {
                break;
            }

            if (remaining < 3)
            {
                fprintf(stderr, "malformed input: incomplete node name\n");
                free(line);
                exit(1);
            }

            if (sscanf(p, "%c%c%c", &node[0], &node[1], &node[2]) != 3)
            {
                fprintf(stderr, "malformed input: couldn't parse destination node\n");
                free(line);
                exit(1);
            }

            u32 dest_node = node_name(node);

            if (graph_find(&g, dest_node) == SIZE)
            {
                if (node_count >= SIZE)
                {
                    fprintf(stderr, "too many nodes\n");
                    free(line);
                    exit(1);
                }

                if (node_name(node) == src_1)
                {
                    source_1_idx = node_count;
                }
                if (node_name(node) == src_2)
                {
                    source_2_idx = node_count;
                }

                if (dest_node == dest)
                {
                    output_idx = node_count;
                }
                g.nodes[node_count++] = dest_node;
            }

            graph_add_edge(&g, source_node, dest_node);

            p += 3;
            remaining -= 3;
        }

        free(line);
    }

    u64 answer_1 = part_1(&g, source_1_idx, output_idx);
    u64 answer_2 = part_2(&g, source_2_idx, output_idx);

#ifdef TEST
    assert(answer_1 == 8);
    assert(answer_2 == 2);
#endif

    printf("%llu %llu\n", answer_1, answer_2);

    fclose(input);

    return 0;
}

u64 part_1(const struct graph *g, size_t source_idx, size_t output_idx)
{
    if (source_idx == output_idx)
    {
        return 1;
    }

    u64 storage[SIZE]   = {0};
    bool computed[SIZE] = {0};
    size_t nodes[SIZE]  = {0};
    size_t edges[SIZE]  = {0};
    int sp              = 0;

    nodes[0]             = source_idx;
    storage[output_idx]  = 1;
    computed[output_idx] = true;

    while (sp >= 0)
    {
        size_t u            = nodes[sp];
        size_t neighbor_idx = edges[sp];

        if (u == output_idx || neighbor_idx >= g->neighbour_counts[u])
        {
            computed[u] = true;
            sp--;
            storage[nodes[sp]] += (sp >= 0) * storage[u];
        }
        else
        {
            edges[sp]++;
            size_t v = g->neighbours[u][neighbor_idx];

            if (computed[v])
            {
                storage[u] += storage[v];
            }
            else
            {
                sp++;
                nodes[sp] = v;
                edges[sp] = 0;
            }
        }
    }

    return storage[source_idx];
}

u64 part_2(const struct graph *g, size_t source_idx, size_t output_idx)
{
    size_t dac_idx = graph_find(g, dac);
    size_t fft_idx = graph_find(g, fft);

    // only one of these 2 is non zero since it's acyclic
    u64 dac_to_fft = part_1(g, dac_idx, fft_idx);
    u64 fft_to_dac = part_1(g, fft_idx, dac_idx);

    if (dac_to_fft)
    {
        u64 src_to_dac = part_1(g, source_idx, dac_idx);
        u64 fft_to_out = part_1(g, fft_idx, output_idx);
        return (src_to_dac * dac_to_fft * fft_to_out);
    }
    else /* if (fft_to_dac) */
    {
        u64 src_to_fft = part_1(g, source_idx, fft_idx);
        u64 dac_to_out = part_1(g, dac_idx, output_idx);

        return (src_to_fft * fft_to_dac * dac_to_out);
    }
}
