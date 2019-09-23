#include <stdbool.h>
#include <stdint.h>
#include "fnqueue.h"
#include "critical.h"

#define FNQUEUE_LENGTH 16

typedef void (*task)();

static task fnqueue_functions[FNQUEUE_LENGTH];
static uint8_t fnqueue_head,
	fnqueue_tail,
	fnqueue_count;

bool fnqueue_init(void)
{
	fnqueue_head = 0;
	fnqueue_tail = 0;
	fnqueue_count = 0;
    
    return true;
}

bool fnqueue_run(void)
{
    critical_begin();
    if (fnqueue_count > 0)
    {
        critical_end();

        fnqueue_functions[fnqueue_head]();
        critical_begin();
        fnqueue_head = (fnqueue_head + 1) % FNQUEUE_LENGTH;
        fnqueue_count--;
    }
    critical_end();

	return 1;
}

bool fnqueue_add(void (*function)(void))
{
	bool result = false;
	
    critical_begin();
    if (fnqueue_count < FNQUEUE_LENGTH)
    {
        fnqueue_functions[fnqueue_tail] = function;
        fnqueue_tail = (fnqueue_tail + 1) % FNQUEUE_LENGTH;
        fnqueue_count++;
        result = true;
    }
    critical_end();
    return result;
}