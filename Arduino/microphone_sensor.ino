// INCLUDE DEPENDENCIES
#include <ezButton.h>
#include <stdint.h>
#include <Servo.h>

// ========================================================================================================================================================================================
// MICROPHONE INITIALIZATIONS =============================================================================================================================================================
const int MIC_ANALOG_PIN = A0;
const int MIC_LED_PIN = 5;
const int LED_PIN_CALIBRATE = 5;
const int MIC_CALIBRATED_VAL_MODIFIER = 17;
const int MIC_NUM_OF_CALIBRATIONS = 100;
float MIC_RUNNING_SUM = 0;
int MIC_COUNTER = 0;
int MIC_RUNNING_SUM_CALC = 0;
int MIC_CALIBRATED_VAL;
int MIC_ANALOG_VAL;
String str1 = "Running_Sum = ";
String str2 = "MIC_ANALOG_VAL = ";
String str3 = "number_of_calibrations = ";
String str4 = "MIC_COUNTER = ";
String str5 = "MIC_CALIBRATED_VAL = ";
int MICROPHONE_SIGNAL = 0;

//========================================================================================================================================================================================
// BLUETOOTH INITIALIZATIONS =============================================================================================================================================================
int BLUETOOTH_LED = 13;
int BLUETOOTH_LED_STATE = 0;
char BLUETOOTH_RECEIVED = '0';
int BLUETOOTH_SIGNAL = 0;

//========================================================================================================================================================================================
// PEDAL INITIALIZATIONS =================================================================================================================================================================
ezButton limitSwitch(7);  // create ezButton object that attach to pin 7;
int PEDAL_SIGNAL = 0;

//========================================================================================================================================================================================
// MOTOR INITIALIZATIONS =================================================================================================================================================================
Servo servoD;  // servo moter (at the buttom) for page flipping
Servo servoU; // servo moter (at the top) to control another motor
int pos = 0;    // variable to store the servo position
int posU = 0;
int biMotorEn = 4;
bool isDown = false;  // variable to decide if the function upmotor is true of false
bool isRight = true;
bool rightF = false;
bool leftF = false;
void pageFlipper(void);
void uppermotor(void);
void pageSide(void);



//=========================================================================================================================================================================================
void setup(){ 
  Serial.begin(9600);
  
  // ======================================================================================================================================================================================
  // MICROPHONE SETUP =====================================================================================================================================================================
  pinMode(MIC_ANALOG_PIN, INPUT);
  pinMode(MIC_LED_PIN, OUTPUT);

  // ======================================================================================================================================================================================
  // BLUETOOTH SETUP ======================================================================================================================================================================
  pinMode(BLUETOOTH_LED, OUTPUT);
  digitalWrite(BLUETOOTH_LED, LOW);

  // ======================================================================================================================================================================================
  // PEDAL SETUP ==========================================================================================================================================================================
  limitSwitch.setDebounceTime(50); // set debounce time to 50 milliseconds

  // ======================================================================================================================================================================================
  // MOTOR SETUP ==========================================================================================================================================================================
  pinMode(2, INPUT);  // input from the button
  pinMode(8, INPUT);
  pinMode(4, OUTPUT);   // 1st Logic for Hbridge of DC motor
  digitalWrite(4, LOW);
  pinMode(6, OUTPUT);   // 2nd Logic for Hbridge of DC motor
  digitalWrite(6, LOW);
  pinMode(9, OUTPUT); // output for lower motor
  pinMode(10, OUTPUT);  // output for the upper motor
  servoD.attach(9);  // attach servoD to pin 9
  servoU.attach(10); // attach servoU to pin 
  servoD.write(0);
  servoU.write(45);
}



void loop() {
  MICROPHONE_SIGNAL = 0;
  BLUETOOTH_SIGNAL = 0;
  PEDAL_SIGNAL = 0;
  
  // =====================================================================================================================================================================================
  // MICROPHONE IMPLEMENTATION ===========================================================================================================================================================   
    MIC_ANALOG_VAL = analogRead(MIC_ANALOG_PIN);
    if ((MIC_ANALOG_VAL > (MIC_ANALOG_VAL + MIC_CALIBRATED_VAL_MODIFIER)) || (MIC_ANALOG_VAL < (MIC_ANALOG_VAL - MIC_CALIBRATED_VAL_MODIFIER))){
      MICROPHONE_SIGNAL = 1;
    }


  // =================================================================================================================================================================================
  // BLUETOOTH IMPLEMENTATION ========================================================================================================================================================
  if (Serial.available() >0){
    BLUETOOTH_RECEIVED = Serial.read();
  }
  if (BLUETOOTH_LED_STATE == 0 && BLUETOOTH_RECEIVED == '1'){
    digitalWrite(BLUETOOTH_LED,HIGH);
    BLUETOOTH_RECEIVED = 0;
    BLUETOOTH_LED_STATE = 1;
    BLUETOOTH_SIGNAL = 1;
  }
  if (BLUETOOTH_LED_STATE == 1 && BLUETOOTH_RECEIVED == '1'){
    digitalWrite(BLUETOOTH_LED ,LOW);
    BLUETOOTH_RECEIVED = 1;
    BLUETOOTH_LED_STATE = 0;
    BLUETOOTH_SIGNAL = 1;
  }


  // =================================================================================================================================================================================
  // PEDAL IMPLEMENTATION ============================================================================================================================================================
  limitSwitch.loop(); // MUST call the loop() function first
  if(limitSwitch.isPressed())
    Serial.println("The limit switch: UNTOUCHED -> TOUCHED");
  if(limitSwitch.isReleased())
    Serial.println("The limit switch: TOUCHED -> UNTOUCHED");
  int PEDAL_STATE = limitSwitch.getState();
  if(PEDAL_STATE == HIGH)
    Serial.println("The limit switch: UNTOUCHED");
  else
    Serial.println("The limit switch: TOUCHED");
    PEDAL_SIGNAL = 1;


  // =================================================================================================================================================================================
  // MOTOR IMPLEMENTATION ============================================================================================================================================================
  // IF CONDITIONAL BLOCK --------------------------------------------------------------------------
  if (BLUETOOTH_SIGNAL == 1){
      if((rightF && isRight) || (leftF && !isRight)){
        pageFlipper(); 
      }
      else if((rightF && !isRight) || (leftF && isRight)){
        pageSide();
        pageFlipper();
      }
      if(digitalRead(2) == HIGH){
        leftF = true;
        delay(1000);
      }
      if(digitalRead(8) == HIGH){
        rightF = true;
        delay(1000);
      }
  }
  else if (MICROPHONE_SIGNAL == 1 || PEDAL_SIGNAL == 1){
      if((rightF && isRight) || (leftF && !isRight)){
        pageFlipper(); 
      }
      else if((rightF && !isRight) || (leftF && isRight)){
        pageSide();
        pageFlipper();
      }
      if(digitalRead(2) == HIGH){
        leftF = true;
        delay(1000);
      }
      if(digitalRead(8) == HIGH){
        rightF = true;
        delay(1000);
      }    
  }
}


void pageFlipper(void){
  uppermotor();
  delay(500);
  digitalWrite(biMotorEn, HIGH);
  delay(500);
  digitalWrite(biMotorEn, LOW);
  if(isRight){
    for (pos = 0; pos <= 160; pos += 1) { // goes from 0 degrees to 180 degrees
      if(pos == 45){
        uppermotor();
      }
      servoD.write(pos);              // tell servo to go to position in variable 'pos'
      delay(3);                       // waits 15ms for the servo to reach the position
    }
    delay(1000);
    for (pos = 160; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      servoD.write(pos);              // tell servo to go to position in variable 'pos'
      delay(3);                       // waits 15ms for the servo to reach the position
    }
    rightF = false;
  }else{
    for (pos = 180; pos >= 20; pos -= 1) { // goes from 180 degrees to 0 degrees
      if(pos == 135){
        uppermotor();
      }
      servoD.write(pos);              // tell servo to go to position in variable 'pos'
      delay(3);                       // waits 15ms for the servo to reach the position
    }
    delay(1000);
    for (pos = 20; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      servoD.write(pos);              // tell servo to go to position in variable 'pos'
      delay(3);                       // waits 15ms for the servo to reach the position
    }
    leftF = false;
  }
}


void uppermotor(void){
  if(isRight){
    if(isDown){  // turn the upper motor down
      for(posU = 0; posU <= 45; posU++){   // adds increment angle for servoU
        servoU.write(posU);   // start motor according to pos
        delay(3);
      }
    }else{  // turn the upper motor up
      for(posU = 45; posU >= 0; posU--){
        servoU.write(posU);
        delay(3);
      }
    }
  }else{
    if(isDown){  // turn the upper motor down
      for(posU = 180; posU >= 135; posU--){   // adds increment angle for servoU
        servoU.write(posU);   // start motor according to pos
        delay(3);
      }
    }else{  // turn the upper motor up
      for(posU = 135; posU <= 180; posU++){
        servoU.write(posU);
        delay(3);
      }
    }
  }
  isDown = !isDown;
}


void pageSide(void){
  if(isRight){
    for(posU=45; posU <= 135; posU++){   // adds increment angle for servoU
      servoU.write(posU);   // start motor according to pos
      delay(3);
    }
    for(pos=0; pos <= 180; pos++){   // adds increment angle for servoU
      servoD.write(pos);   // start motor according to pos
      delay(3);
    }
  }else{
    for(posU=135; posU >= 45; posU--){   // adds increment angle for servoU
      servoU.write(posU);   // start motor according to pos
      delay(3);
    }
    for(pos=180; pos >= 0 ; pos--){   // adds increment angle for servoU
      servoD.write(pos);   // start motor according to pos
      delay(3);
    }
  }
  biMotorEn = (biMotorEn == 4) ? 6 : 4;
  isRight = !isRight;
}
