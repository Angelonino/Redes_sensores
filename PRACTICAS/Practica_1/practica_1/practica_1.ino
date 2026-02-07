#include <Nano33BLE_System.h>
#include <NRF52_MBED_TimerInterrupt.h>
#include <nrf_timer.h>
#include <Arduino.h>

#define pin_ADC A0
#define pin_LED LED_BUILTIN
//LED AZUL ==> LEDB; está negado
//LED ROJO ==> LEDR
int V_ADC=0;
float V_real=0.0;
int V_int=0;
char buffer[64];
int ADCint_cont=0;
//Instanciación del TImer (en este caso Timer 3)
NRF52_MBED_Timer ITimer(NRF_TIMER_3);

//Flag de interrupción para la lectura del ADC
volatile bool readADC_flag = false; //Se usa 'volatile' porque se debe cambiar en la interrupción

//En este caso, la forma más correcta de manejar las interrupciones es mediante flags, y que en el proceso principal se ejecuten las tareas necesarias
//esto se debe a que no se trabaja en arquitecturas "Bare Metal" (Sin SO), en estas arquitecturas de más bajo nivel se trabaja con Callbacks y ejecutar
//las rutinas de control entera dentro de la interrupción. Esto se hace más en control y DSPs
//En arduino Nano 33 BLE se trabaja con una capa de SO, el Mbed SO, y además se tiene el Stack de Bluetooth. Las funciones Arduino como analogRead, Serial print, utilizan
//drivers de Mbed para operar, los cuales utilizan "Mutexes" para bloquear a la cpu mientras operan y evitar que otras funciones accedan al mismo recurso a la vez
//Las interrupciones de Timer (ISR) necesitan un Mutex y NO PUEDEN ESPERAR. Si el Mutex está ocupado por otra función, se provocará un Hard Fault, que cuelga la CPU

//Por otra parte, la CPU se comparte con el BLE, y sus interrupciones son las de mayor prioridad. Los Serial.print, Analog Read son muy lentos. Si la interrupción es muy lenta,
//o bloquea las interrupciones, puede hacer que se caiga la conexión bluetooth o se corrompa el stack del BLE

//Se puede también trabajar por hilos (Threads) para trabajar de forma más profesional y limpia. Pero eso es más complicado, no se hará en este caso
void TimerHandler(){
  //Levantamos flag de la interrupción ÚNICAMENTE
  readADC_flag=true;
}
//Función para hacer un parpadeo rápido de un LED para indicar la actividad
void fast_blink (int blinking_LED){
  digitalWrite(blinking_LED, HIGH);
  delay(50); //A pesar de tener un delay aquí, como la tarea de ADC es de 1seg, y el contador del timer no se detiene, no debería ser un problema
  digitalWrite(blinking_LED, LOW);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //Baudrate
  Serial.println("Comunicación serie iniciada");
  pinMode(pin_LED, OUTPUT);
  pinMode(pin_ADC, INPUT);
  analogReadResolution(12);
  // Intervalo en microsegundos. 1 segundo = 1,000,000 us.
  // attachInterruptInterval llama internamente a setFrequency
  //
  if (ITimer.attachInterruptInterval(1000000, TimerHandler))
  {
    Serial.print(F("Timer iniciado correctamente en: "));
    Serial.println(F("NRF_TIMER3"));
  }
  else
  {
    Serial.println(F("Error al iniciar el Timer"));
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  
  if (readADC_flag){ 
    
    readADC_flag=false; //bajamos el flag de inmediato
    ADCint_cont++;
    if (ADCint_cont>=10){
      ADCint_cont=0;
      V_ADC=analogRead(pin_ADC);
      //V_real=(V_ADC*3.3)/1023; //precisión del ADC de 10 bits
      V_int=(int)((long)V_ADC*3300)/4095; //una vez configurada la resolución, podemos aprovechar los 12 bits que tiene el ADC del Nano 33 BLE
      V_real=(float)V_int/1000.0;
      
      sprintf(buffer, "Input Voltage: %d mV", V_int);
      Serial.println(buffer);
      Serial.print("Tiempo: ");
      Serial.println(millis()); //en esta función se muestra el tiempo en milisegundos que ha transcurrido; útil para comprobar que se ejecute cada 1seg
      fast_blink(pin_LED);
    }
  }
}



