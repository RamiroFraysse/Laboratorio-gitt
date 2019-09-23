#ifndef FNQUEUE_H
#define FNQUEUE_H

#include <stdbool.h>
#include <stdint.h>

bool fnqueue_init(void);
bool fnqueue_run(void);
bool fnqueue_add(void (*function)(void));

#endif