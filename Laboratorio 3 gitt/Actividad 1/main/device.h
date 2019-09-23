#define TECLA_RIGHT 0
#define TECLA_UP 1
#define TECLA_DOWN 2
#define TECLA_LEFT 3
#define TECLA_SELECT 4

void teclado_init(void);


//Asociar la funcion handler al evento de presionar la tecla.
void key_down_callback(void (*handler)(), int tecla);

//Asociar la funcion handler al evento de soltar la tecla.
void key_up_callback(void (*handler)(), int tecla);
