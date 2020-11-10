#include "cpu.h"

struct RECPU* recpu_create(void) {
    return (struct RECPU*)malloc(sizeof(struct RECPU));
}

void recpu_delete(struct RECPU* cpu) {
    free(cpu);
}

void recpu_init(struct RECPU* cpu, struct REBUS* bus) {
    
    
};
