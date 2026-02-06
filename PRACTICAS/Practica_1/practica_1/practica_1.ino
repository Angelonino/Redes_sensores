#define pin_ADC A0

int V_ADC=0;
float V_real=0.0;
int V_int=0;
char buffer[125];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //Baudrate
  Serial.println("Comunicación serie iniciada");
}

void loop() {
  // put your main code here, to run repeatedly:
  V_ADC=analogRead(pin_ADC);
  //V_real=(V_ADC*3.3)/1023; //precisión del ADC de 10 bits
  V_int=(V_ADC*3300)/1023;
  V_real=(float)V_int/1000.0;
  
  sprintf(buffer, "Input Voltage: %d", V_int);
  Serial.println(buffer);
  delay(1000);
}

