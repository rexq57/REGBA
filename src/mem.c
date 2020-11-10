#include "mem.h"

#include "type.h"

struct REMEM* remem_create(void) {
    return (struct REMEM*)malloc(sizeof(struct REMEM));
}

void remem_delete(struct REMEM* mem) {
    free(mem);
}

void remem_init(struct REMEM* mem) {
    
}
