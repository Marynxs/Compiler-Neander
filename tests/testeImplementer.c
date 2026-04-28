#include <stdio.h>
#include "../src/neanderImplementer/neanderImplementer.h"

int main() {
    NeanderImplementer cpu;

    neander_init(&cpu);

    if (!neander_load_mem_file(&cpu, "saida.mem")) {
        printf("Erro ao carregar saida.mem\n");
        return 1;
    }

    printf("Estado inicial:\n");
    neander_print_state(&cpu);

    printf("\nExecutando...\n");
    neander_run(&cpu, 10000, 0);

    printf("\nEstado final:\n");
    neander_print_state(&cpu);

    printf("\nMemoria relevante:\n");
    neander_print_memory(&cpu, 0, 120);

    return 0;
}