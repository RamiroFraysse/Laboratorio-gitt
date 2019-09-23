#include "driverADC.h"
#include "Arduino.h"
#include "fnqueue.h"

volatile uint8_t canalActual = 0;
volatile uint8_t cantCanales = 0;
volatile uint8_t low, high;


struct data_canal{
  uint8_t canal;
  void (*func_callback)(int);
  int dataFromADC = 0;
};

struct data_canal *datos_canal[6];

uint8_t alreadyCall = 1;

static int deboucing=0;

bool reemplazarCanal(adc_cfg *cfg);

int adc_init(adc_cfg *cfg){
    int exito = 0;
    int tensionRef =1;
    uint8_t canalAux = cfg->canal;
    if(canalAux >= 0 && canalAux <6){
        if(cfg->func_callback){
            if(!reemplazarCanal(cfg)){
              datos_canal[cantCanales] = (struct data_canal *) malloc(sizeof(struct data_canal));
              datos_canal[cantCanales]->canal = canalAux;
              datos_canal[cantCanales]->func_callback = cfg->func_callback;  
              if(cantCanales < 5){
                cantCanales += 1;
              }
            }
            exito = 1;
        }
    }
    if(alreadyCall && exito){    
        alreadyCall = 0; //Para no modificar los registros cada vez que se llama a adc_init().        
        cli();        
			ADMUX |= canalAux;     
			ADMUX |= (tensionRef << 6);
			   
			//PRR – Power Reduction Register
			//Bit 0 – PRADC: Power Reduction ADC
			//Writing a logic one to this bit shuts down the ADC. The ADC must be disabled before shut down.
			PRR &= ~(1 << PRADC);
			
			//ADCSRA – ADC Control and Status Register A
			//Bit 7 – ADEN: ADC Enable
			//Por defecto el adc esta apagado para consumir menos,entonces lo prende poniendo el bit a 1.  
			ADCSRA |= (1 << ADEN);
			//Configura el prescaler en 128 para que trabaje con la frecuencia maxima del ADC que es de 125khz
			ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
			//Bit 3 – ADIE:habilita las intrerrupciones del adc.
			ADCSRA |= (1 << ADIE);
			
			//Bit 5 – ADATE: ADC Auto Trigger Enable para que en modo free running comience la conversión.
			ADCSRA |= (1 << ADATE);
					
			ADCSRB &= ~(1 << ADTS2) & ~(1 << ADTS1) & ~(1 << ADTS0);
			
			//Inicializar Conversion
			//Bit 6 – ADSC: ADC Start Conversion
			ADCSRA |= (1 << ADSC);			
        sei();        
    }

    Serial.begin(9600);
    return exito;
}

bool reemplazarCanal(adc_cfg *cfg){
  // Busca si ya existe un canal configurado con el mismo numero que cfg->canal y lo reemplaza. 
  // retorna true si lo encontro, false en caso contrario.
   int i = 0;
   uint8_t canal = cfg->canal;
   while(i < cantCanales){
    if(datos_canal[i]->canal == canal){
      datos_canal[i]->canal = canal;
      datos_canal[i]->func_callback = cfg->func_callback;
      return true;
    }
    i += 1;
   }
   return false;
}

uint8_t buscarPosCanal(uint8_t canal){
   int i = 0;
   while(i < cantCanales){
    if(datos_canal[i]->canal == canal){
      return i;
    }
    i += 1;
   }
   return 10;
}

void procesarAdc(){  
  uint8_t canal=canalActual;
  if(canal >= 0 && canal <6){
    int aux;
    uint8_t pos = buscarPosCanal(canal);
    if(pos != 10){      
	  cli();
		aux = datos_canal[pos]->dataFromADC;      
	  sei();
      
      datos_canal[pos]->func_callback(aux);
    }
  }  
}

ISR(ADC_vect){ //ADC conversion complete  

   deboucing++;
  if(deboucing==100)
  {
	//Lee el valor del canal
    uint8_t low, high;
    low = ADCL;
    high = ADCH;
    datos_canal[canalActual]->dataFromADC = (high << 8) | low;
    canalActual = (canalActual + 1) % cantCanales;
  
	//Configura el siguiente canal a leer
    high = ADMUX & 0b11110000;
    ADMUX = high | datos_canal[canalActual]->canal; 
  
    ADCSRA |= (1 << ADSC);    
  }
  if(deboucing==200){
    fnqueue_add(procesarAdc);
    
  }
  if(deboucing == 300){
    deboucing=0;
  }

}
