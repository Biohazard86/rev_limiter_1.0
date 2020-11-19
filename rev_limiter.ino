/*
Biohazard_86

REV. limiter by davidbarrios@usal.es

TESTED in:
BMW E36 316i M40B16 to TRIONIC 5 ECU

Most of the car dashboards works with 2 or 4 pulses per rev. 

BMW E36 6 cyl cluster uses 3 pulses per rev

21krpm / 60secs = 350 rev per sec 
1/350hz = 2.8571*10^-3 
In the worst case we have to read a pulse every 2.8571ms (Period) or 350 pulses every second = 350Hz (Frecuency).
Arduino allows up to 8Mhz input/output frecuency


*/


// Define the pulses per revolution 
#define pulses_rev  2   // Change this! always is 2 or 4 pulses per rev.
#define max_rpm_output_allowed 7100 // vtec boys doesnt like this... hehehe 


//Define the pins
const int inputPin = 9; //read the buton
const int pinRELE = 7;  // to the rele
const int outputPin = 5;  //To turn on the button
int input_rpm = 2;   // In the pin 13 of the arduino we have the ECU output from the Saab ECU


//Variable
double actual_rpm = 0;
unsigned long time_per_pulse_micro = 0; //microseconds
double time_per_pulse_ms = 0; //miliseconds
double frek = 0;
unsigned int time_1 = 0;
unsigned int time_2 = 0;
int counter = 0;
volatile bool flag = false;
long value_a0;
long max_rpm;
int value;
int cut_off_time = 10;  // time that Arduino has to pass in to the if to can cut the engine.


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void setup() {
    Serial.begin(9600);   //Serial port
    pinMode(input_rpm, INPUT);  //We define the pin 2 called input_rpm as an input
    pinMode(inputPin, INPUT);   // Buton pin
    pinMode(pinRELE, OUTPUT);   // Rele pin
    pinMode(outputPin, OUTPUT); // buton turn on pin
    
    // Serial output
    Serial.print("\n\n\n---------------------------------------------\n");
    Serial.print("         REV. LIMITER  1.5k to 6.7k rpm \n");
    Serial.print("               Biohazard_86\n");
    Serial.print("--------------------------------------------\n");
    counter = 0;  
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//========================================================================================
//this function calculates the actual rpm.
void rpm_fun() {
  if(counter == 0){ //if is the first time we call this function
    time_1=micros();  // save the time in microseconds
    counter++;        // add 1 to the counter to dont enter in the if in the next time we call rpm_fun
    flag=false;       // flag to doesnt show the rpm yet
  }
  else{               //2nd time we call rpm_fun
    counter = 0;      // set 0 to counter to the next time we call rpm_fun to enter in the if
    time_2=micros();  // save the time 
    flag=true;        // turn on the flag to can show the rpm
    } 
  }
//========================================================================================

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// User choose the max rpm (potentiometer) when press the button 
long choose_max_rpm(long value_a0){   // value_a0 is the potentiometer value
    if(value_a0 < 68){
        return 6700; 
     }
    if((value_a0 >= 68) && (value_a0 < 123) ){
        //return 1600; 
        return 6400;
     }
     if((value_a0 >= 123) && (value_a0 < 178) ){
        //return 1900;
         return 6100;
     }
     if((value_a0 >= 178) && (value_a0 < 233) ){
        //return 2200; 
        return 5800;
     }
     if((value_a0 >= 233) && (value_a0 < 288) ){
        //return 2500; 
         return 5500;
     }
     if((value_a0 >= 288) && (value_a0 < 343) ){
        //return 2800; 
        return 5200;
     }
     if((value_a0 >= 343) && (value_a0 < 398) ){
        //return 3100; 
        return 4900;
        
     }
     if((value_a0 >= 398) && (value_a0 < 453) ){
        //return 3400; 
        return 4600;
     }
     if((value_a0 >= 453) && (value_a0 < 508) ){
        //return 3700; 
         return 4300;
     }
     if((value_a0 >= 508) && (value_a0 < 563) ){
        //return 4000; 
        return 4000;
     }
     if((value_a0 >= 563) && (value_a0 < 618) ){
        //return 4300; 
         return 3700;
     }
     if((value_a0 >= 618) && (value_a0 < 673) ){
        //return 4600; 
         return 3400;
     }
     if((value_a0 >= 673) && (value_a0 < 728) ){
        //return 4900; 
         return 3100;
     }
     if((value_a0 >= 728) && (value_a0 < 783) ){
        //return 5200; 
        return 2800;
     }
     if((value_a0 >= 783) && (value_a0 < 838) ){
        //return 5500; 
        return 2500;
     }
     if((value_a0 >= 838) && (value_a0 < 893) ){
        //return 5800; 
        return 2200;
     }
     if((value_a0 >= 893) && (value_a0 < 938) ){
        //return 6100; 
        return 1900;
     }
     if((value_a0 >= 938) && (value_a0 < 993) ){
        //return 6400; 
        return 1500;
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
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void loop() {
  // We read the A0 value (potentiometer)
  value_a0 = analogRead(A0);  // save that value in value_a0
  max_rpm = choose_max_rpm(value_a0);

  
  
  attachInterrupt(0, rpm_fun, RISING);  // 0 is pin 2 (only 2 and 3 pins can handle interrupts); calls rpm_fun; RISING to trigger when the pin goes from low to high; 
  if(flag){
    detachInterrupt(0); // turn off interrupts
    time_per_pulse_micro = (time_2 - time_1); //  calculate the time per pulse in micro seconds
    time_per_pulse_ms = time_per_pulse_micro / 1000.00;   // calculate the time per pulse in micro seconds   
    frek = pow((time_per_pulse_ms / 1000), -1); // calculates the frecuency 
    actual_rpm = (frek * 60)/ pulses_rev; // calculates the actual rpm of the engine
      
      // Serial prints:
      //------------------------------------------------
      Serial.print(actual_rpm);   //Shows the actual rpm
      Serial.print("  ");       // nothing
      Serial.print(max_rpm);      // Shows the max rpm allowed by the potentiometer
      
      Serial.print("\n");
      //------------------------------------------------
    flag=false;
    }

    
    
  if(actual_rpm > max_rpm){   //if rpm > max_rpm (potentiometer) then we can read the button
    digitalWrite(outputPin, HIGH);  // power on the button
    value = digitalRead(inputPin);  //read the button
    digitalWrite(outputPin, LOW);   //power off
    if(value == 1){ //if the buton is on then cut off the rele
////////
        if(cut_off_time == 10){ // if cut_off_time is 10, Arduino can cuts negative side of the coil
         digitalWrite(pinRELE, LOW);  //cut off
         cut_off_time = 0;    // puts cut_off_time to 0
        }
        cut_off_time++; //Add 1 to cutt off time
      }
    else{   // else 
        digitalWrite(pinRELE, HIGH); // dont cut off 
      }
    
  } //end if
  else{ //if the rpm < max_rpm (potentiometer) dont cut the rele
      digitalWrite(pinRELE, HIGH); //dont cut the rele
    }


}
