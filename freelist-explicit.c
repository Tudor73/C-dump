#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SIZE 3000

char Heap[SIZE];

typedef struct FreeListNode FreeListNode;
struct FreeListNode
{
    size_t size;
    FreeListNode *next;
};

typedef struct FreeList
{
    void *data;
    size_t size;
    size_t used;

    FreeListNode *head;
} FreeList;

void free_list_free_all(FreeList *fl)
{
    fl->used = 0;
    FreeListNode *first_node = (FreeListNode *)fl->data;
    first_node->size = fl->size;
    first_node->next = NULL;
    fl->head = first_node;
}

void free_list_init(FreeList *fl, void *data, size_t size)
{
    fl->data = data;
    fl->size = size;
    free_list_free_all(fl);
}

void *heap_alloc(FreeList *fl, size_t length)
{
    // first fit
    FreeListNode *node = fl->head;
    FreeListNode *prev = NULL;

    while (node != NULL)
    {
        if (node->size >= length)
        {
            size_t remaining = node->size - length - sizeof(FreeListNode);

            if (remaining > 0)
            {
                FreeListNode *new_node = (FreeListNode *)((char *)node + sizeof(FreeListNode) + length);
                new_node->size = remaining;
                new_node->next = node->next;

                node->size = length;
                node->next = new_node;
            }
            if (prev)
            {
                prev->next = node->next;
            }
            else
            {
                fl->head = node->next;
            }
            fl->used += length;
            return (void *)((char *)node + sizeof(FreeListNode));
        }
        prev = node;
        node = node->next;
    }
    return NULL;
}
void heap_free(FreeList *fl, void *p)
{
    FreeListNode *free_node = (FreeListNode *)((char *)p - sizeof(FreeListNode));
    FreeListNode *node;
    FreeListNode *prev = NULL;

    node = fl->head;
    while (node)
    {
        if (free_node < node)
        {
            free_node->next = node;
            if (prev)
            {
                prev->next = free_node;
            }
            else
            {
                fl->head = free_node;
            }
            break;
        }
        prev = node;
        node = node->next;
    }
    if (free_node->next != NULL && ((char *)free_node + free_node->size + sizeof(FreeListNode)) == (char *)free_node->next)
    {
        free_node->size += free_node->next->size + sizeof(FreeListNode);
        free_node->next = free_node->next->next;
    }
    if (prev != NULL && ((char *)prev + sizeof(FreeListNode) + prev->size) == (char *)free_node)
    {
        prev->size += free_node->size + sizeof(FreeListNode);
        prev->next = free_node->next;
    }
}

void free_list_dump(FreeList *fl)
{
    FreeListNode *node = fl->head;
    while (node)
    {
        printf("size: %ld, next: %p  \n", node->size, (void *)node->next);
        node = node->next;
    }
    printf("##########\n");
}

int main()
{
    FreeList fl;
    char Heap[SIZE];
    free_list_init(&fl, Heap, SIZE);
    char *p = (char *)heap_alloc(&fl, 1000);
    free_list_dump(&fl);
    char *b = (char *)heap_alloc(&fl, 1000);
    free_list_dump(&fl);

    strncpy(p, "aaaa", 10);
    strncpy(b, "bvbbb\n", 10);
    printf("%s, %s", p, b);
    heap_free(&fl, b);
    free_list_dump(&fl);
    heap_free(&fl, p);
    free_list_dump(&fl);

    return 0;
}