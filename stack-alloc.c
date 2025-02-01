#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct Stack
{
    unsigned char *buffer;
    size_t length;
    size_t curr_offset;
    size_t prev_offset;

} Stack;

typedef struct Header
{
    size_t prev_offset;

} Header;

void init_stack(Stack *s, char *buf, size_t length)
{
    s->buffer = buf;
    s->length = length;
    s->curr_offset = 0;
    s->prev_offset = 0;
}

void *allocate(Stack *s, size_t size)
{
    Header *header;

    assert(s->curr_offset + size + sizeof(Header) <= s->length);
    s->prev_offset = s->curr_offset;
    s->curr_offset += sizeof(Header);
    header = (Header *)&s->buffer[s->curr_offset];
    header->prev_offset = s->prev_offset;

    void *ptr = &s->buffer[s->curr_offset];
    s->curr_offset += size;
    memset(ptr, 0, size);
}

void stack_free(Stack *s, void *ptr)
{
    uintptr_t start, end, curr_adr;
    Header *header;

    start = (uintptr_t)s->buffer;
    end = start + (uintptr_t)s->length;
    curr_adr = (uintptr_t)ptr;

    if (!(start <= curr_adr && curr_adr <= end))
    {
        assert(0 && "Out of bounds memory address passed to stack allocator");
        return;
    }
    if (curr_adr > start + (uintptr_t)s->curr_offset)
    {
        // memory alread freed or not used
        return;
    }
    s->curr_offset = s->prev_offset;
    header = (Header *)&s->buffer[s->curr_offset];
    s->prev_offset = header->prev_offset;
}

int main()
{
    Header h;
    Stack s;
    char initial_buf[256];
    init_stack(&s, initial_buf, 256);

    char *p = allocate(&s, 4);
    char *b = allocate(&s, 4);

    strcpy(p, "aaa");
    strcpy(b, "bbb");
    printf("Values: %s, %s \n", p, b);
    printf("Current offset %ld \n", s.curr_offset);
    stack_free(&s, b);
    printf("Current offset %ld \n", s.curr_offset);
    stack_free(&s, p);
    printf("Current offset %ld \n", s.curr_offset);
}