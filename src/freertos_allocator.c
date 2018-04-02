
#include <stdlib.h>

#include "allocator.h"
#include "FreeRTOS.h"

void *gen_alloc(size_t size) {
    return pvPortMalloc(size);
}

void gen_free(void *p) {
    vPortFree(p);
}
