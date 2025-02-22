#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SIZE 3000

char Heap[SIZE];

typedef struct Header
{
    size_t size;
    bool free;

} Header;

void *alloc(char *heap, size_t length)
{
    Header *h = (Header *)&heap[0];
    size_t offset = 0;
    while (true)
    {
        if (h->free == true && h->size >= length)
        {
            h = (Header *)&heap[offset];

            if (h->size > length)
            {
                int next_offset = offset + (length + sizeof(Header));
                Header *newBlock = (Header *)&heap[next_offset];
                newBlock->size = h->size - length - sizeof(Header);
                newBlock->free = true;
            }
            h->size = length;
            h->free = false;
            void *p = &heap[offset + sizeof(Header)];
            memset(p, 0, length);
            return p;
        }
        if (offset + sizeof(Header) >= SIZE)
            break;

        offset += (h->size + sizeof(Header));
        h = (Header *)&heap[offset];
    }
    return NULL;
}

void free(void *p)
{
    Header *h = (Header *)((char *)p - sizeof(Header));

    char *heap_base = (char *)Heap;
    size_t next_offset = (char *)h - heap_base + sizeof(Header) + h->size;

    h->free = true;

    if (next_offset + sizeof(Header) <= SIZE)
    {
        Header *nextBlock = (Header *)&heap_base[next_offset];
        if (nextBlock->free)
        {

            h->size += sizeof(Header) + nextBlock->size;
        }
    }
}

void init(char *heap)
{
    Header *h = (Header *)&heap[0];
    h->free = true;
    h->size = SIZE - sizeof(Header);
}

int main()
{
    init(Heap);
    char *p = (char *)alloc(Heap, 1000);
    char *b = (char *)alloc(Heap, 1000);

    strncpy(p, "aaaa", 10);
    strncpy(b, "bvbbb", 10);
    printf("%s, %s", p, b);

    // this doesn't work because coalescing looks only ahead. If I want to look behind as well, I need a duplicated header after the block.
    // free(p);
    // free(b);
    free(b);
    free(p);
    char *c = (char *)alloc(Heap, 2000);
    strncpy(c, "ccccc", 10);
    printf("%s", c);
}
