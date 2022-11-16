#include <stdio.h>
#include "tas_alloc.h"

char *get_tas()
{
    static char tas[SIZE] = {SIZE - 1, FREE_BLOCK};

    return tas;
}

t_strategy *get_strategy()
{
    static t_strategy strategy = &first_fit;

    return &strategy;
}

int *get_first_libre()
{
    static int index;

    return &index;
}

char *fragmentation(unsigned size, int index)
{
    int rest;
    char *tas;
    char *ret;

    tas = get_tas();
    rest = tas[index] - size - 1;
    if (!rest)
        size++;
    set_first_libre(size, index, rest);
    tas[index] = size;
    tas[index + 1] = 0;
    ret = tas + index + 1;
    return ret;
}

char *tas_malloc(unsigned int size)
{
    char *tas;
    t_strategy strategy;
    char *ret;
    int libre;

    tas = get_tas();
    strategy = *get_strategy();
    libre = (*strategy)(size);
    if (!size)
        return (tas + libre + 1);
    ret = NULL;
    if (libre != -1)
        ret = fragmentation(size, libre);
    return ret;
}

/**
 * @brief this function merge consecutive free block.
 *
 * TODO: optimization:
 *          at the moment: run through all free block.
 *          need to be: check only the previous and next block.
 *
 */
void defragmentation()
{
    char *tas;
    int size;
    int libre;
    int next_libre;

    tas = get_tas();
    libre = *get_first_libre();
    while (tas[libre + 1] != -1)
    {
        next_libre = tas[libre + 1];
        size = (tas + next_libre) - (tas + libre) - 1;
        if (size == tas[libre])
        {
            tas[libre] += tas[next_libre] + 1;
            tas[libre + 1] = tas[next_libre + 1];
            continue;
        }
        libre = tas[libre + 1];
    }
}

int get_previous_index(int index)
{
    char *tas;
    int first_libre;
    int previous_libre;

    tas = get_tas();
    first_libre = *get_first_libre();
    previous_libre = *get_first_libre();
    while (first_libre < index)
    {
        if (previous_libre != first_libre)
        {
            previous_libre = first_libre;
        }
        first_libre = tas[first_libre + 1];
    }
    return previous_libre;
}

/**
 * @brief this function add at address +1, the index of the next free block
 *        and save the first free block.
 *
 * @param ptr, address of memory that need to be free.
 */
void tas_free(char *ptr)
{
    char *tas;
    int *first_libre;
    int index;
    int previous_index;

    if (ptr)
    {
        tas = get_tas();
        first_libre = get_first_libre();
        index = ptr - tas - 1;
        if (index == *first_libre)
            return;
        if (index < *first_libre)
        {
            *ptr = *first_libre;
            *first_libre = index;
        }
        else
        {
            previous_index = get_previous_index(index);
            *ptr = tas[previous_index + 1];
            tas[previous_index + 1] = index;
        }
        defragmentation();
    }
}

void show_tas()
{
    char *tas;
    int *first_libre;
    int index;
    int size;

    tas = get_tas();
    first_libre = get_first_libre();
    printf("first bloc libre: %d\n", *first_libre);
    printf("show tas:\n");
    index = 0;
    while (index < SIZE)
    {
        size = tas[index];
        printf("    index: %d, size = %d, content: %.*s\n", index, size, size, tas + index + 1);
        index += size + 1;
    }
}