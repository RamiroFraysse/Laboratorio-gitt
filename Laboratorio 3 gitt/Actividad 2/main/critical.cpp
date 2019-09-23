#include "stdint.h"
#include "avr/io.h"
#include "critical.h"
#include "Arduino.h"


static volatile int anidamiento;

void critical_begin(void)
{
    // Verificar que las interrupciones no fueron interrumpidas por fuera del
    // mÃ³dulo de regiÃ³n crÃ­tica. Esto puede suceder cuando se estÃ¡ ejecutando
    // una rutina de interrupciÃ³n. En este caso, no deshabilitar las
    // interrupciones

    // Lectura atÃ³mica del registro
    uint8_t status_register = SREG;
    cli();

    // Si las interrupciones estaban habilitadas, y aÃºn no fueron
    // deshabilitadas por el mÃ³dulo de regiÃ³n crÃ­tica
    // o estÃ¡n deshabilitadas pero por el mÃ³dulo de regiÃ³n crÃ­tica
    //       incrementar el anidamiento.
    if ((anidamiento == 0) && ((status_register & (0x80)) != 0)
            || (anidamiento != 0) && ((status_register & (0x80)) == 0))
    {
        anidamiento++;
    }
    // Si las interrupciones estaban deshabilitadas, y no fuÃ© el mÃ³dulo de regiÃ³n
    // crÃ­tica, no hacer nada.
}

void critical_end(void)
{
    uint8_t status_register = SREG;
    cli();

    if (anidamiento > 0) {
        anidamiento--;
        if (anidamiento == 0)
            sei();
    }
    else if ((status_register & 0x80) != 0)
        sei();
}
