// xnotify.c -- thread exit notification functions
#include <stdlib.h>
#include <string.h>
#include <thr/xthreads.h>

#include <Windows.h>

#define NITEMS 20

typedef struct { // data for condition-variable slot
    _Thrd_t id;
    _Mtx_t mtx;
    _Cnd_t cnd;
    int* res;
} at_thread_exit_data;

typedef struct at_thread_exit_block { // block of condition-variable slots
    at_thread_exit_data data[NITEMS];
    int num_used;
    struct at_thread_exit_block* next;
} at_thread_exit_block;

static at_thread_exit_block thread_exit_data;

void _Lock_at_thread_exit_mutex(void);
void _Unlock_at_thread_exit_mutex(void);

void _Cnd_register_at_thread_exit(
    _Cnd_t cnd, _Mtx_t mtx, int* p) { // register condition variable and mutex for cleanup at thread exit
    // find block with available space
    at_thread_exit_block* block = &thread_exit_data;

    _Lock_at_thread_exit_mutex();
    while (block != 0) { // loop through list of blocks
        if (block->num_used == NITEMS) { // block is full; move to next block and allocate
            if (block->next == 0) {
                block->next = (at_thread_exit_block*) calloc(1, sizeof(at_thread_exit_block));
            }

            block = block->next;
        } else { // found block with available space
            int i;

            for (i = 0; i < NITEMS; ++i) { // find empty slot
                if (block->data[i].mtx == 0) { // store into empty slot
                    block->data[i].id._Id = GetCurrentThreadId();
                    block->data[i].mtx    = mtx;
                    block->data[i].cnd    = cnd;
                    block->data[i].res    = p;
                    ++block->num_used;
                    break;
                }
            }
            block = 0;
        }
    }
    _Unlock_at_thread_exit_mutex();
}

void _Cnd_unregister_at_thread_exit(_Mtx_t mtx) { // unregister condition variable/mutex for cleanup at thread exit
    // find condition variables waiting for this thread to exit
    at_thread_exit_block* block = &thread_exit_data;

    _Lock_at_thread_exit_mutex();
    while (block != 0) { // loop through list of blocks
        int i;
        for (i = 0; block->num_used != 0 && i < NITEMS; ++i) {
            if (block->data[i].mtx == mtx) { // release slot
                block->data[i].mtx = 0;
                --block->num_used;
            }
        }

        block = block->next;
    }
    _Unlock_at_thread_exit_mutex();
}

void _Cnd_do_broadcast_at_thread_exit(void) { // notify condition variables waiting for this thread to exit
    // find condition variables waiting for this thread to exit
    at_thread_exit_block* block        = &thread_exit_data;
    const unsigned int currentThreadId = _Thrd_id();

    _Lock_at_thread_exit_mutex();
    while (block != 0) { // loop through list of blocks
        int i;
        for (i = 0; block->num_used != 0 && i < NITEMS; ++i) {
            if (block->data[i].mtx != 0 && block->data[i].id._Id == currentThreadId) { // notify and release slot
                if (block->data[i].res) {
                    *block->data[i].res = 1;
                }
                _Mtx_unlock(block->data[i].mtx);
                _Cnd_broadcast(block->data[i].cnd);
                block->data[i].mtx = 0;
                --block->num_used;
            }
        }

        block = block->next;
    }
    _Unlock_at_thread_exit_mutex();
}

/*
 * This file is derived from software bearing the following
 * restrictions:
 *
 * (c) Copyright William E. Kempf 2001
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation. William E. Kempf makes no representations
 * about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
