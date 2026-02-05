#define pin_ADC 4

int V_ADC=0;
float V_real=0.0;
int V_int=0;
char buffer[125];

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  V_ADC=analogRead(pin_ADC);
  V_real=(V_ADC*3.3)/4095; //precisión del ADC de 12bits
  V_int=(V_ADC*3300)/4095;
  sprintf(buffer, "Input Voltage: %d", V_int);
  delay(1000);
}
