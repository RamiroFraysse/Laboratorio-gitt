#include <LiquidCrystal.h>
#include "device.h"
#include "fnqueue.h"
#include "driverADC.h"

#define Vcc 5.0f
#define Vref 5.0f
#define pinSensorTemp 1 

//Inicializa la library con los numeros de interfaces de los pines.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int brightness = 204; //brillo del display LCD
volatile int positionCounter=0;// scroll del mensaje inicial

//Identifican las celdas del lcd
const int numRows = 2;
const int numCols = 16;

int cs,cs_mediciones; //representan centesimas de segundo
const int analogOutPin = 10; // Analog output pin that the LED brightness is attached to

int estadoActual = 0; //0 mensaje inicial, 1 Temperatura actual, 2 Temp max y minima, 3 Temp promedio, 4 Ajuste de brillo.
volatile int timerON = 0; //estado del timer

//Representan el arreglo de temperaturas de donde se extrae el promedio
int cantMediciones = 0;
int mediciones[100];
int posLibre = 0;
//Representan la temperatura minima y maxima registrada desde el inicio
int maxTemp = 0;
int minTemp = 100;
//Temperatura actual
int valorTemp;


void setup() {
  //Setup timer2
  cli();
	  //set timer2 interrupt at 100Hz (Interrupciones cada 0,01s)
	  TCCR2A = 0;// set entire TCCR2A register to 0
	  TCCR2B = 0;// same for TCCR2B
	  TCNT2  = 0;//initialize counter value to 0
	  // set compare match register for 100Hz increments (0,01s)
	  OCR2A = 155.25;// = (16*10^6) / (1024*100) - 1 (must be <256)
	  // turn on CTC mode
	  TCCR2A |= (1 << WGM01);  
	  // Set CS21 bit for 1024 prescaler
	  TCCR2B |= (1 << CS20) | (1 << CS21)| (1 << CS22);  
	  // enable timer compare interrupt
	  TIMSK2 |= (1 << OCIE2A);
  sei();

  // Setup LCD
  pinMode(analogOutPin, OUTPUT);
  lcd.begin(numCols,numRows);
  analogWrite(analogOutPin, brightness); //Controla intensidad backlight  
  
  //Setup driverADC
  adc_cfg cfg_sensorTemp;
  cfg_sensorTemp.canal = pinSensorTemp;
  cfg_sensorTemp.func_callback = adcToTemp;
  adc_init(&cfg_sensorTemp); 

  //Inicialización de contadores de tiempo  
  cs=0;  
  cs_mediciones=0;
 
  //Asociación de funciones al callback
  key_down_callback(select_key_down, TECLA_SELECT);
  key_down_callback(left_key_down, TECLA_LEFT);
  key_down_callback(right_key_down, TECLA_RIGHT);
  key_down_callback(up_key_down, TECLA_UP);
  key_down_callback(down_key_down, TECLA_DOWN);
  
  key_up_callback(select_key_up, TECLA_SELECT);
  key_up_callback(left_key_up, TECLA_LEFT);
  key_up_callback(right_key_up, TECLA_RIGHT);
  key_up_callback(up_key_up, TECLA_UP);
  key_up_callback(down_key_up, TECLA_DOWN);

  //Inicialización driverTeclado
  teclado_init();
  //Inicialización de cola de funciones
  fnqueue_init();
  
  imprimirInicio();
    
  Serial.begin(9600);
}


//presionar tecla select
void select_key_down(){}
//presionar tecla left
void left_key_down(){}
//presionar tecla right
void right_key_down(){}
//presionar tecla up
void up_key_down(){}
//presionar tecla down
void down_key_down(){}
//soltar tecla down
void down_key_up(){
switch(estadoActual){
  case 0:
    estadoActual = 1;
    cleanDisplay();
    break;
  case 4:    
    if (brightness>0)
       brightness -= 51;
    delay(100);
    analogWrite(10, brightness);
    break;
}
}

//soltar tecla up
void up_key_up(){  
switch(estadoActual){
 case 0:
    estadoActual = 1;
    cleanDisplay();
    break;
  case 4:    
    if (brightness<255)
        brightness += 51;
        delay(100);
    analogWrite(10, brightness);
    break;   
}
  mostrarEstado();
}

//soltar tecla left
void left_key_up(){
   if(estadoActual <= 1){
    estadoActual = 4;
  }
  else{
    estadoActual -= 1;
  }
  mostrarEstado();
  }
//soltar tecla right
void right_key_up(){
   if(estadoActual >= 4){
    estadoActual = 1;
  }
  else{
    estadoActual += 1;
  }
  mostrarEstado();
  }

//soltar tecla select
void select_key_up(){}

//limpia el display LCD
void cleanDisplay()
{ 
  lcd.clear();
}

//Convierte el valor del ADC en grados centígrados
void adcToTemp(int adc_value){
  float Vin = adcToVin(adc_value);
  
  valorTemp=Vin;

  //Agrega temperatura al arreglo cada 15 centésimas de segundo
  if(cs_mediciones>14)
  {     
    agregarMedicion(valorTemp);
    cs_mediciones=0;
  }
  
  //Registra cambios en la temperatura mínima
  if(valorTemp < minTemp){
    minTemp = valorTemp;
  }
  //Registra cambios en la temperatura máxima
  if(valorTemp > maxTemp){
    maxTemp = valorTemp;
  }
  

}

//Esta funcion calcula el equivalente a grados para el adc pasado por parametro.
float adcToVin(float adcValue){  
  float Vin = (adcValue * Vref)/1024;  
  float grados = Vin/0.01;  
  return grados;  
}

//Inserta medición en el arreglo
void agregarMedicion(int tempValue){
  Serial.println("Cant mediciones "+String(cantMediciones));
  mediciones[posLibre] = tempValue;
  posLibre += 1;
  if(posLibre == 100){
    posLibre = 0;
  }
  if(cantMediciones < 100){
    cantMediciones += 1;
  }
  
}

//Calcula el promedio de las mediciones
float obtenerPromedio(){
  float suma = 0.0f;
  for(int i=0; i < cantMediciones; i++){
    suma += mediciones[i];
  }
  if (cantMediciones == 0){
    return 0;
  }
  else{
    return suma/(float)cantMediciones;
  }
}


//Imprime en display el mensaje inicial
void imprimirInicio(void)
{
  lcd.setCursor(0, 0);
  lcd.print("Sistemas Embebidos-2do Cuatrimestre 2019");
  lcd.setCursor(0, 1);
  lcd.print("Laboratorio 2 - Com: Fraysse / Carignano");
 
  if (positionCounter <24)
  {    
      // scroll one position left:
      lcd.scrollDisplayLeft();     
      positionCounter++;
      // wait a bit:
      delay(200);   
  }
  else if(estadoActual==0){
    if (positionCounter<48 && estadoActual==0){
      lcd.scrollDisplayRight();
      positionCounter++;
      delay(200);
    }
    else 
    {
    estadoActual=1;
    cleanDisplay();
    mostrarEstado();
    timerON=1;
    }
  }
  
}

//Imprime en display información del estado actual del sistema
void mostrarEstado(){
  switch(estadoActual){
    case 1: // Temperatura actual
      // disable timer compare interrupt
      //TIMSK2 &= ~(1 << OCIE2A);
      lcd.setCursor(0,0);
      //lcd.pri("1-2-3-4-5-6-7-8-");
      lcd.print("   Temp actual   ");
     // timerON = 0;
      break;
    case 2: // Temperatura max y min
      lcd.setCursor(0,0);
      //lcd.pri("1-2-3-4-5-6-7-8-");
      lcd.print("    Temp:       ");      
      break;
    case 3: // Temperatura promedio
      lcd.setCursor(0,0);
      //lcd.pri("1-2-3-4-5-6-7-8-");
      lcd.print(" Temp promedio  ");      
      break;
    case 4: // Ajuste dimmer
      lcd.setCursor(0,0);
      //lcd.pri("1-2-3-4-5-6-7-8-");
      lcd.print(" Ajuste dimmer  ");      
      break;
  }
}

//Imprime en display información sobre el brillo del mismo
void imprimirBrillo(){  
  lcd.setCursor(0, 1);
  //7 espacios en blanco
  lcd.print("       ");
  //1, 2 o 3
  lcd.print(brightness*100/255);
  //Si son 3 cifras
  if(brightness == 255){
    lcd.setCursor(10, 1);
    //6 caracteres mas
    lcd.print("%     ");
  }
   else//son 2 cifras
     if(brightness*100/255 > 9){
       lcd.setCursor(9, 1);
       lcd.print("%     ");
     }
     else{//es de 1 cifra
       lcd.setCursor(8, 1);
       lcd.print("%     ");
     }
}

//Funcion llamada por la rutina de interrupciones.
void procesarTimer(){
  
  if(timerON){
        cs += 1;
        cs_mediciones +=1;
        if(cs >= 100){          
          cs = 0;            
      }      
  }  
}

//Rutina del timer2
ISR(TIMER2_COMPA_vect){
  fnqueue_add(modoActual);
  fnqueue_add(procesarTimer);
}

void modoActual(){

switch(estadoActual){

    case 0:
      //Imprime en display el mensaje inicial  
      imprimirInicio(); 
     
      break;
    case 1:
         // Imprime temperatura actual cada 1 segundo para una mejor visualización
         
       if(cs >= 99){

        lcd.setCursor(0,1);
        lcd.print("     "+String(valorTemp)+" C");
        // Inserta caracteres al final para centrar valor
        lcd.print("     ");
        
      }
      break;
     case 2:
      lcd.setCursor(0,1);
      lcd.print("Max "+String(maxTemp)+"  Min "+String(minTemp));
      break;
     case 3:
      //Imprime temperatura promedio cada 1 segundo para una mejor visualización
        if (cs>=99)
        {
          lcd.setCursor(0,1);
          lcd.print("     " + String(obtenerPromedio()) + "        ");
        }
        break;
     case 4:
      lcd.setCursor(0,1);
      imprimirBrillo();
      break;
  }
}

//  principal
void loop() {
   fnqueue_run();
}
