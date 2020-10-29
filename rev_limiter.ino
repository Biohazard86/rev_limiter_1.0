/*
Biohazard_86

Saab Trionic 5 to Bmw E36 cluster RPM conversion by davidbarrios@usal.es

TESTED in:
BMW E36 316i M40B16 to TRIONIC 5 ECU

Most of the Saab dashboards works with 2 or 4 pulses per rev. 

BMW E36 cluster uses 3 pulses per rev

Maximum pulses per rev is:
7000*2 = 14.000 in the Saab output of the ECU
7000*3 = 21.000 to the BMW cluster

21krpm / 60secs = 350 rev per sec 
1/350hz = 2.8571*10^-3 
In the worst case we have to read a pulse every 2.8571ms (Period) or 350 pulses every second = 350Hz (Frecuency).
Arduino allows up to 8Mhz input/output frecuency


*/


// Define the pulses per revolution of both cars
#define bmw_pulses_rev  3
#define saab_pulses_rev  2
#define max_rpm_output_allowed 7100 // to cant broke the E36 cluster putting 100k rpm on the cluster
int rev_limit_1 = 1800;
int rev_limit_2 = 5500;
int rev_limit_3 = 6100;
const int inputPin = 2;
const int pinRELE = 7;

//Define the pins
int input_saab = 2;   // In the pin 13 of the arduino we have the ECU output from the Saab ECU
int output_bmw = 3;    // In the pin 3 of the arduino we have the dashboard input

// We have differents lights for the rev limiter. For example, 1st for the max torque point. 2nd for the max hp output and 3rd if you're near the rpm limiter
int lightrevlim1 = 4;
int lightrevlim2 = 5;
int lightrevlim3 = 6;


//Variable
double actual_rpm = 0;
unsigned long time_per_pulse_micro = 0; //microseconds
double time_per_pulse_ms = 0; //miliseconds
double frek = 0;
unsigned int time_1 = 0;
unsigned int time_2 = 0;
int counter = 0;
int counter2 = 0;
unsigned int t_lights_1 = 0;
unsigned int t_lights_2 = 0;
volatile bool flag = false;
bool flag_serial = false;
bool flag_lights = false;
char data;
long value_a0;
long max_rpm;
int value;

void setup() {
  Serial.begin(9600);   //Serial port
   pinMode(inputPin, INPUT);
   pinMode(pinRELE, OUTPUT);
  pinMode(input_saab, INPUT);  //We define the pin 1 called input_saab as an input
  pinMode(output_bmw, OUTPUT);  //We define the pin 3 called output_bmw as an output

  //............

  pinMode(lightrevlim1, OUTPUT);  //led 1
  pinMode(lightrevlim2, OUTPUT);  // led 2
  pinMode(lightrevlim3, OUTPUT);  // led 3


  // Serial output
  Serial.print("\n\n\n---------------------------------------------\n");
  Serial.print("    SAAB Trionic 5 to BMW E36 cluster\n");
  Serial.print("               Biohazard_86\n");
  Serial.print("--------------------------------------------\n");


 counter = 0;  
}

void rpm_fun() {
  if(counter == 0){
    time_1=micros();
    counter++;
    flag=false;
  }
  else{
    counter = 0;
    time_2=micros();
    flag=true;
    }
  }


void lights_func(){
  if(actual_rpm > rev_limit_3){
      digitalWrite(lightrevlim3, HIGH);   //
    }
  else{
      digitalWrite(lightrevlim3, LOW);
    }
  
  

  if(actual_rpm > rev_limit_2){
      digitalWrite(lightrevlim2, HIGH);   //
    }
  else{
      digitalWrite(lightrevlim2, LOW);
    }
  
  


  if(actual_rpm > rev_limit_1){
      digitalWrite(lightrevlim1, HIGH);   //
    }
  else{
      digitalWrite(lightrevlim1, LOW);
    }
  
  

}

long choose_max_rpm(long value_a0){
    if(value_a0 < 68){
        return 1300; 
     }
    if((value_a0 >= 68) && (value_a0 < 123) ){
        return 1600; 
     }
     if((value_a0 >= 123) && (value_a0 < 178) ){
        return 1900; 
     }
     if((value_a0 >= 178) && (value_a0 < 233) ){
        return 2200; 
     }
     if((value_a0 >= 233) && (value_a0 < 288) ){
        return 2500; 
     }
     if((value_a0 >= 288) && (value_a0 < 343) ){
        return 2800; 
     }
     if((value_a0 >= 343) && (value_a0 < 398) ){
        return 3100; 
     }
     if((value_a0 >= 398) && (value_a0 < 453) ){
        return 3400; 
     }
     if((value_a0 >= 453) && (value_a0 < 508) ){
        return 3700; 
     }
     if((value_a0 >= 508) && (value_a0 < 563) ){
        return 4000; 
     }
     if((value_a0 >= 563) && (value_a0 < 618) ){
        return 4300; 
     }
     if((value_a0 >= 618) && (value_a0 < 673) ){
        return 4600; 
     }
     if((value_a0 >= 673) && (value_a0 < 728) ){
        return 4900; 
     }
     if((value_a0 >= 728) && (value_a0 < 783) ){
        return 5200; 
     }
     if((value_a0 >= 783) && (value_a0 < 838) ){
        return 5500; 
     }
     if((value_a0 >= 838) && (value_a0 < 893) ){
        return 5800; 
     }
     if((value_a0 >= 893) && (value_a0 < 948) ){
        return 6100; 
     }
     if((value_a0 >= 948) && (value_a0 < 1003) ){
        return 6400; 
     }
     if((value_a0 >= 1003) && (value_a0 < 1058) ){
        return 6700; 
     }

     /*
13  1300
68  1600
123 1900
178 2200
233 2500
288 2800
343 3100
398 3400
453 3700
508 4000
563 4300
618 4600
673 4900
728 5200
783 5500
838 5800
893 6100
948 6400
1003  6700
1058  7000

    */ 
  }


void loop() {
  // We read the A0 value
  value_a0 = analogRead(A0);
  value = digitalRead(inputPin);  //lectura digital de pin
  max_rpm = choose_max_rpm(value_a0);

  
  
  attachInterrupt(0, rpm_fun, RISING);
  if(flag){
    detachInterrupt(0);
    time_per_pulse_micro = (time_2 - time_1);
    
    time_per_pulse_ms = time_per_pulse_micro / 1000.00;
    
    frek = pow((time_per_pulse_ms / 1000), -1); 
    
    actual_rpm = (frek * 60)/ saab_pulses_rev;

    counter2++;
    if(counter2>0){
      //Serial.print("\n RREK:   ");
      //Serial.print("\n");
      //Serial.print(frek);
      
      //Serial.print("\n TPP micro:  ");
      //Serial.print("\n");
      //Serial.print(time_per_pulse_micro);

    
      //Serial.print("\n TPP ms:  ");
      //Serial.print("\n");
      //Serial.print(time_per_pulse_ms);

    
      //Serial.print("\n RPM:   ");
      Serial.print("\n");
      Serial.print(actual_rpm);
      //Imprimimos por el monitor serie
      Serial.print("\n RPM MAX >>>    ");
      Serial.print(max_rpm);
      Serial.print("\nStatus    ");
      Serial.print(value);  // 0 es activado
      Serial.print("\n\n\n --------------------------    ");

      counter2=0;
    }
    
    flag=false;
    }

    // 10 times per second
    if(flag_lights == false){
      t_lights_1 = millis();
      flag_lights = true;
      }
    if(flag_lights){
      if(t_lights_1 > 100){
        lights_func();
        flag_lights == false;
      }
      
      }

    
    if((actual_rpm > max_rpm) && (value == 0)){ // 0 es activado
      digitalWrite(pinRELE, HIGH);
      Serial.print("\n RELE ON");
      
      }
    if((actual_rpm < max_rpm) && (value == 0)){
      digitalWrite(pinRELE, LOW);
      Serial.print("\n RELE OFF");    
      }

      if(value == 1){
      digitalWrite(pinRELE, LOW);
      Serial.print("\n RELE OFF");    
      }
  

    

}
