#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct Free_List_Node Free_List_Node;
struct Free_List_Node
{
    Free_List_Node *next;
};

typedef struct Pool
{
    unsigned char *buf;
    size_t len;
    size_t chunk_size;
    Free_List_Node *head;
    int count;

} Pool;

void pool_free_all(Pool *p)
{
    size_t chunk_count = p->len / p->chunk_size;
    size_t i;
    for (i = 0; i < chunk_count; i++)
    {
        void *ptr = &p->buf[i * p->chunk_size];

        Free_List_Node *node = (Free_List_Node *)ptr;
        p->count++;
        node->next = p->head;
        p->head = node;
    }
}

void *pool_aloc(Pool *p)
{
    // assert(p->head == NULL && "No memory left");
    Free_List_Node *node = p->head;

    p->head = p->head->next;
    p->count--;
    memset(node, 0, p->chunk_size);
    return node;
}

void pool_free(Pool *p, void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
    Free_List_Node *node;

    void *start = p->buf;
    void *end = &p->buf[p->len];
    if (!(start <= ptr && ptr < end))
    {
        assert(0 && "Memory is out of bounds of the buffer in this pool");
        return;
    }
    node = (Free_List_Node *)ptr;
    node->next = p->head;
    p->head = node;
    p->count++;
}

void pool_init(Pool *p, size_t length, size_t chunk_size, void *buf)
{
    p->count = 0;
    assert(chunk_size >= sizeof(Free_List_Node) && "Chunk size is too small");
    assert(length >= chunk_size && "Buffer length smaller than chunk size");

    p->buf = (unsigned char *)buf;
    p->len = length;
    p->chunk_size = chunk_size;
    p->head = NULL;

    pool_free_all(p);
}

int main()
{
    char buf[1000];
    Pool p;
    pool_init(&p, 1000, 10, buf);
    char *a = (char *)pool_aloc(&p);

    char *b = (char *)pool_aloc(&p);

    printf("size of free list %d \n", p.count);
    pool_free(&p, a);
    printf("size of free list %d \n", p.count);
    char *c = (char *)pool_aloc(&p);
    printf("size of free list %d \n", p.count);
}
