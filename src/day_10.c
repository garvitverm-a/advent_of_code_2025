#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <z3.h>

#ifdef TEST
#    define SIZE 3
#    define INPUT "../inputs/day_10_test.txt"
#else
#    define SIZE 158
#    define INPUT "../inputs/day_10.txt"
#endif

#define MAX_BUTTON 13
#define MAX_JOLTAGE 10

#define u64 unsigned long long
#define u8 uint8_t
#define u16 uint16_t

u64 part_1(u16 machines[SIZE], u16 buttons[SIZE][MAX_BUTTON], u8 button_counts[SIZE]);
u64 part_2(u16 buttons[SIZE][MAX_BUTTON],
           u8 button_counts[SIZE],
           u16 joltages[SIZE][MAX_JOLTAGE],
           u8 joltage_counts[SIZE]);

int main(void)
{
    FILE *input = fopen(INPUT, "r");
    if (!input)
    {
        fprintf(stderr, "failed to open file\n");
        return 1;
    }

    u16 machines[SIZE]              = {0};
    u16 buttons[SIZE][MAX_BUTTON]   = {0};
    u8 button_counts[SIZE]          = {0};
    u16 joltages[SIZE][MAX_JOLTAGE] = {0};
    u8 joltage_counts[SIZE]         = {0};

    for (u16 *machine = machines; machine < machines + SIZE; machine++)
    {
        u16 *button_line  = buttons[machine - machines];
        u8 *button_count  = &button_counts[machine - machines];
        u16 *joltage_line = joltages[machine - machines];
        u8 *joltage_count = &joltage_counts[machine - machines];

        char *line  = NULL;
        size_t cap  = 0;
        ssize_t len = getline(&line, &cap, input);

        if (len <= 1)
        {
            free(line);
            break;
        }

        char *p        = line;
        *machine       = 0;
        *button_count  = 0;
        *joltage_count = 0;

        while (isspace(*p))
        {
            p++;
        }

        if (*p != '[')
        {
            fprintf(stderr, "Expected '[' at start of line\n");
            free(line);
            return 1;
        }
        p++;

        int machine_bits = 0;
        while (*p != ']' && *p != '\0')
        {
            *machine = (*machine << 1) | (*p++ == '#');
            machine_bits++;
        }

        if (*p != ']')
        {
            fprintf(stderr, "Expected ']' after machine state\n");
            free(line);
            return 1;
        }
        p++;

        while (*p != '\0')
        {
            while (isspace(*p))
            {
                p++;
            }

            if (*p == '\0')
            {
                break;
            }

            if (*p == '(')
            {
                if (*button_count >= MAX_BUTTON)
                {
                    fprintf(stderr, "Too many buttons\n");
                    free(line);
                    return 1;
                }

                p++;
                button_line[*button_count] = 0;

                while (*p != ')' && *p != '\0')
                {
                    while (isspace(*p))
                    {
                        p++;
                    }

                    if (!isdigit(*p))
                    {
                        fprintf(stderr, "Expected digit in button, got '%c'\n", *p);
                        free(line);
                        return 1;
                    }

                    int bit_pos = 0;
                    while (isdigit(*p))
                    {
                        bit_pos = bit_pos * 10 + (*p - '0');
                        p++;
                    }

                    button_line[*button_count] |= 1 << (machine_bits - 1 - bit_pos);

                    while (isspace(*p))
                    {
                        p++;
                    }

                    if (*p == ',')
                    {
                        p++;
                    }
                }

                if (*p != ')')
                {
                    fprintf(stderr, "Expected ')' after button\n");
                    free(line);
                    return 1;
                }
                p++;
                (*button_count)++;
            }
            else if (*p == '{')
            {
                p++;

                while (*p != '}' && *p != '\0')
                {
                    while (isspace(*p))
                    {
                        p++;
                    }

                    if (*p == '}')
                    {
                        break;
                    }

                    if (!isdigit(*p))
                    {
                        fprintf(stderr, "Expected digit in joltages, got '%c'\n", *p);
                        free(line);
                        return 1;
                    }

                    if (*joltage_count >= MAX_JOLTAGE)
                    {
                        fprintf(stderr, "Too many joltages\n");
                        free(line);
                        return 1;
                    }

                    joltage_line[*joltage_count] = 0;
                    while (isdigit(*p))
                    {
                        joltage_line[*joltage_count] =
                            joltage_line[*joltage_count] * 10 + (*p - '0');
                        p++;
                    }
                    (*joltage_count)++;

                    while (isspace(*p))
                    {
                        p++;
                    }

                    if (*p == ',')
                    {
                        p++;
                    }
                }

                if (*p != '}')
                {
                    fprintf(stderr, "Expected '}' after joltages\n");
                    free(line);
                    return 1;
                }
                p++;
            }
            else
            {
                fprintf(stderr, "Unexpected character: '%c'\n", *p);
                free(line);
                return 1;
            }
        }

        free(line);
    }
    u64 answer_1 = part_1(machines, buttons, button_counts);
    u64 answer_2 = part_2(buttons, button_counts, joltages, joltage_counts);

    printf("%llu %llu\n", answer_1, answer_2);

#ifdef TEST
    assert(answer_1 == 7);
    assert(answer_2 == 33);
#endif

    fclose(input);

    return 0;
}

#define min(a, b) ((a) > (b) ? (b) : (a))

u8 recurse_1(u16 machine, u16 *buttons, u8 button_count, u8 curr)
{
    if (machine == 0)
    {
        return curr;
    }
    if (button_count == 0)
    {
        return MAX_BUTTON;
    }

    u8 exclude = recurse_1(machine, buttons + 1, button_count - 1, curr);
    u8 include = recurse_1(machine ^ *buttons, buttons + 1, button_count - 1, curr + 1);

    return min(exclude, include);
}

u64 part_1(u16 machines[SIZE], u16 buttons[SIZE][MAX_BUTTON], u8 button_counts[SIZE])
{
    u64 presses = 0;

    for (u16 *machine = machines; machine < machines + SIZE; machine++)
    {
        u16 *buttons_line = buttons[machine - machines];
        u8 button_count   = button_counts[machine - machines];

        presses += recurse_1(*machine, buttons_line, button_count, 0);
    }

    return presses;
}

int cmp(u16 *state, u16 *joltages, u8 joltage_count)
{
    int all_equal = 0;
    for (u8 i = 0; i < joltage_count; i++)
    {
        if (state[i] > joltages[i])
        {
            return -1;
        }
        all_equal += joltages[i] - state[i];
    }
    return all_equal;
}

// i wanted no C dependencies.
// i hate this. i didn't want this.
// i was forced to try this. it works.
u64 part_2(u16 button_lines[SIZE][MAX_BUTTON],
           u8 button_counts[SIZE],
           u16 joltage_lines[SIZE][MAX_JOLTAGE],
           u8 joltage_counts[SIZE])
{
    Z3_context ctx;
    {
        Z3_config cfg = Z3_mk_config();
        ctx           = Z3_mk_context(cfg);
        Z3_del_config(cfg);
    }

    u64 presses = 0;

    for (u8 i = 0; i < SIZE; i++)
    {
        u16 *buttons     = button_lines[i];
        u8 button_count  = button_counts[i];
        u16 *joltages    = joltage_lines[i];
        u8 joltage_count = joltage_counts[i];

        Z3_optimize opt = Z3_mk_optimize(ctx);
        Z3_optimize_inc_ref(ctx, opt);

        Z3_ast btn_vars[MAX_BUTTON];
        Z3_sort int_sort = Z3_mk_int_sort(ctx);
        Z3_ast zero      = Z3_mk_int64(ctx, 0, int_sort);

        for (u8 b = 0; b < button_count; b++)
        {
            char name[16];
            snprintf(name, sizeof(name), "b_%d", b);
            Z3_symbol s = Z3_mk_string_symbol(ctx, name);
            btn_vars[b] = Z3_mk_const(ctx, s, int_sort);

            Z3_optimize_assert(ctx, opt, Z3_mk_ge(ctx, btn_vars[b], zero));
        }

        for (u8 k = 0; k < joltage_count; k++)
        {
            Z3_ast terms[MAX_BUTTON];
            u64 num_terms = 0;

            for (u8 b = 0; b < button_count; b++)
            {
                u8 bit = (buttons[b] >> (joltage_count - 1 - k)) & 1;

                if (bit)
                {
                    terms[num_terms++] = btn_vars[b];
                }
            }

            if (num_terms > 0)
            {
                Z3_ast sum_node    = Z3_mk_add(ctx, num_terms, terms);
                Z3_ast target_node = Z3_mk_int64(ctx, joltages[k], int_sort);
                Z3_optimize_assert(ctx, opt, Z3_mk_eq(ctx, sum_node, target_node));
            }
            else if (joltages[k] != 0)
            {
                Z3_ast target_node = Z3_mk_int64(ctx, joltages[k], int_sort);
                Z3_optimize_assert(ctx, opt, Z3_mk_eq(ctx, zero, target_node));
            }
        }

        Z3_ast total_sum = Z3_mk_add(ctx, button_count, btn_vars);
        Z3_optimize_minimize(ctx, opt, total_sum);

        if (Z3_optimize_check(ctx, opt, 0, NULL) == Z3_L_TRUE)
        {
            Z3_model model = Z3_optimize_get_model(ctx, opt);
            Z3_ast result_ast;

            if (Z3_model_eval(ctx, model, total_sum, Z3_L_TRUE, &result_ast))
            {
                __int64_t res_val;
                if (Z3_get_numeral_int64(ctx, result_ast, &res_val))
                {
                    presses += (u64)res_val;
                }
            }
        }

        Z3_optimize_dec_ref(ctx, opt);
    }

    Z3_del_context(ctx);

    return presses;
}
#endif
