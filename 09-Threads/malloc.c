#include <stddef.h>
#include "malloc.h"
#include "os.h"

typedef struct chunk {
    struct chunk *next;
    struct chunk *prev;
    int size;
    void *ptr;
} chunk_t;

#define OFFSET (sizeof(chunk_t) - sizeof(void*))
#define ALGIN 8

static unsigned char heaps[MAX_HEAPS];
static unsigned char *program_break = heaps;

static chunk_t *head = NULL;
static chunk_t *freelist = NULL;

static inline int align_up(int size)
{
    int mask = ALGIN - 1;
    return ((size - 1) | mask) + 1;
}

static void *sbrk(unsigned int nbytes)
{
	if (program_break + nbytes >= heaps
	    && program_break + nbytes < heaps + MAX_HEAPS) {
		unsigned char *previous_pb = program_break;
		program_break += nbytes;
		return (void *) previous_pb;
	}
	return (void *) -1;
}

void *malloc(unsigned int size)
{
	if(size == 0)
		return NULL;

	size = align_up(size);

    if(!head){
        chunk_t *tmp = sbrk(align_up(sizeof(chunk_t)));

        head = tmp;
        head->next = NULL;
        head->prev = NULL;
        head->ptr = NULL;
        head->size = 0;
    }

    if(!freelist){
        chunk_t *tmp = sbrk(align_up(sizeof(chunk_t)));

        freelist = tmp;
        freelist->next = NULL;
        freelist->prev = NULL;
        freelist->ptr = NULL;
        freelist->size = 0;
    }
	
	chunk_t *cur = head;
	while(cur->next)
		cur = cur->next;

    chunk_t *best_fit_chunk = NULL;
    chunk_t *alloc;

    if (!freelist->next) {
        /* If no more chunks in the free chunk list,
           allocate best_fit_chunk as NULL. */
        alloc = best_fit_chunk;
    } else {
        chunk_t *fh = freelist;
        /* record the size of the chunk */
        int bsize = 0;

        while (fh->next) {
            if (fh->size >= size && !best_fit_chunk) {
                /* first time setting fh as best_fit_chunk */
                best_fit_chunk = fh;
                bsize = fh->size;
            } else if (fh->size >= size && best_fit_chunk &&
                       (fh->size < bsize)) {
                /* If there is a smaller chunk available,
                   replace it with this. */
                best_fit_chunk = fh;
                bsize = fh->size;
            }
            fh = fh->next;
        }

        /* a suitable chunk has been found */
        if (best_fit_chunk) {
            /* remove the chunk from the freelist */
            if (best_fit_chunk->prev) {
                chunk_t *tmp = best_fit_chunk->prev;
                tmp->next = best_fit_chunk->next;
            } else
                freelist = best_fit_chunk->next;

            if (best_fit_chunk->next) {
                chunk_t *tmp = best_fit_chunk->next;
                tmp->prev = best_fit_chunk->prev;
            }
        }
        alloc = best_fit_chunk;
    }

	if(!alloc){
        alloc = sbrk(size);
        alloc->size = size;
    }

	cur->next = alloc;
	alloc->prev = cur;

	cur = alloc;
	cur->next = NULL;
	cur->size = size;
	cur->ptr = cur + OFFSET;

	return cur->ptr;
}

void free(void *ptr)
{
	if(ptr == NULL)
		return;

	chunk_t *cur = head;

	while(cur->ptr != ptr)
		 cur = cur->next;
	
	chunk_t *prev;
    if (cur->prev != NULL) {
        prev = cur->prev;
        prev->next = cur->next;
    } else
        head = cur->next;

    chunk_t *next;
    if (cur->next != NULL) {
        next = cur->next;
        next->prev = cur->prev;
    } else
        prev->next = NULL;

	/* Insert Head in freelist_head */
    cur->next = freelist;
    cur->prev = NULL;
    freelist->prev = cur;
    freelist = cur;
}
