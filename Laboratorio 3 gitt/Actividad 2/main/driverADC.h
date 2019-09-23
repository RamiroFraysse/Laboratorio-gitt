//#include "Arduino.h"
typedef unsigned char uint8_t;

typedef struct adc_config adc_cfg;
struct adc_config{
    uint8_t canal; //A0 = 0, A1 = 1,..., A5 = 5
    void (*func_callback)(int);
};

int adc_init(adc_cfg *cfg);

void adc_loop(uint8_t canal);
