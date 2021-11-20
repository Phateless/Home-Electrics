
// Phate 3.0

#include <EEPROM.h> // EEPROM Address:1 = Power Failure.  Value 3 Not active last start. Value 4 Was active and running last start.
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"


Adafruit_LiquidCrystal lcd(0);

int powerState;                // variable to hold the Power state


//INPUTS
const int Light = 4; // Light Switch (Momentary)
const int Start = 6; // Start Switch (Momentary)
const int Stop = 7; // Stop Switch (Momentary)
const int Water = 9;  // Water Switch (Toggle)
const int Air = 10; // Air Switch (Toggle)
const int Emergency = 8; // Emergency Switch (Toggle)


bool LightState = LOW;
bool Relay_LightState = HIGH;
bool latch = LOW;
//OUTPUTS
const int Relay1 =  A0;     // Light Relay 1
const int Relay2 =  A1;     // Fan Relay 2
const int Relay3 =  A2;     // Pump Relay 3
const int Relay4 =  A3;     // Hot Plate Relay 4

const int buzzer = 5;       //Active Powered Buzzer


//defining House Icon
byte housechar1[8] = {B00000, B00001, B00011, B00011, //Row 0, Col 0
                      B00111, B01111, B01111, B11111,
                     };
byte housechar2[8] = {B11111, B11111, B11100, B11100, //Row 1, Col 0
                      B11100, B11100, B11100, B11100,
                     };
byte housechar3[8] = {B00000, B10010, B11010, B11010, //ROW 0, Col 1
                      B11110, B11110, B11110, B11111,
                     };
byte housechar4[8] = {B11111, B11111, B11111, B10001, //Row 1, Col 1
                      B10001, B10001, B11111, B11111,
                     };

//Defining Humidity Icon
byte humchar1[8] = {B00000, B00001, B00011, B00011,
                    B00111, B01111, B01111, B11111,
                   };
byte humchar2[8] = {B11111, B11111, B11111, B01111,
                    B00011, B00000, B00000, B00000,
                   };
byte humchar3[8] = {B00000, B10000, B11000, B11000,
                    B11100, B11110, B11110, B11111,
                   };
byte humchar4[8] = {B11111, B11111, B11111, B11110,
                    B11100, B00000, B00000, B00000,
                   };




//Defining FAN
// First Frame
byte wind1[] = {
  B00000,
  B00000,
  B00000,
  B00011,
  B00111,
  B00111,
  B00111,
  B00011
};

byte wind2[] = {
  B11111,
  B11111,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte wind3[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B01110,
  B11111,
  B11111
};

byte wind4[] = {
  B11000,
  B11100,
  B11100,
  B11100,
  B11000,
  B00000,
  B00000,
  B00000,
};




void setup() {

  Serial.begin(9600);         // initialize serial


  digitalWrite(Relay1, HIGH); // Light Off Initial Startup
  Relay_LightState = HIGH;
  digitalWrite(Relay2, HIGH); // Fan Off Initial Startup
  digitalWrite(Relay3, HIGH); // Pump Off Initial Startup
  digitalWrite(Relay4, HIGH); // Hot Plate Off Initial Startup

  lcd.begin(16, 2);  // Set LCD's number of columns and rows:

  lcd.clear();
  lcd.createChar(1, housechar1);
  lcd.createChar(2, housechar2);
  lcd.createChar(3, housechar3);
  lcd.createChar(4, housechar4);
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.setCursor(1, 0);
  lcd.write(3);
  lcd.setCursor(1, 1);
  lcd.write(4);
  lcd.setCursor(3, 0);
  lcd.print("Home");
  lcd.setCursor(3, 1);
  lcd.print("Electrics");
  delay(2000);
  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Not in Service");


  delay(2000);
  lcd.setBacklight(LOW);

  pinMode(Light, INPUT); // initialize the pushbutton pin as an input:
  pinMode(Start, INPUT); // initialize the pushbutton pin as an input:
  pinMode(Stop, INPUT); // initialize the pushbutton pin as an input:
  pinMode(Water, INPUT); // initialize the pushbutton pin as an input:
  pinMode(Air, INPUT); // initialize the pushbutton pin as an input:
  pinMode(Emergency, INPUT); // initialize the pushbutton pin as an input:

  pinMode(Relay1, OUTPUT); // Light Relay
  pinMode(Relay2, OUTPUT); // Fan Relay
  pinMode(Relay3, OUTPUT); // Pump Relay
  pinMode(Relay4, OUTPUT); // Hot Plate Relay

  pinMode(buzzer, OUTPUT); // initialize the Buzzer pin as output



}
void Start_Routine(void);
void Stop_Routine(void);
void Emergency_Routine(void);
bool Start_ButtonState = LOW , Stop_ButtonState = LOW , Emergency_ButtonState = LOW; // variable for reading the pushbutton status
bool Buzzer_status = LOW; //Buzzer state
void(* resetFunc) (void) = 0; //declare reset function @ address 0
void Light_Routine(void);

void loop() {

  Emergency_ButtonState = digitalRead(Emergency);  // read the state of the pushbutton value:


  light_control();
  delay(100);


  if ( Emergency_ButtonState == LOW) {
    Start_ButtonState = digitalRead(Start);  // read the state of the pushbutton value:
    Stop_ButtonState  = digitalRead(Stop);  // read the state of the pushbutton value:

    if (Start_ButtonState) Start_Routine();
    else  digitalWrite(Relay2, HIGH);    // turn off Relay 2 Fan pin A1

    if (Stop_ButtonState) Stop_Routine();
    else digitalWrite(Relay3, HIGH);    // turn off Relay 3 pump pin A2

  }
  else  Emergency_Routine_2();

   powerState = EEPROM.read(1);
   if(powerState == 3) {
    digitalWrite(Relay2, HIGH);
    digitalWrite(Relay3, HIGH);
   } 
   if(powerState == 4) {

  light_control();
  //TURN ON RELAY 1 Light PIN A0
  digitalWrite(Relay1, LOW);
  Relay_LightState = LOW;

  //TURN OFF RELAY 4 HOT PLATE PIN A3
  digitalWrite(Relay4, HIGH);

  //TURN ON RELAY 2 FAN PIN A1
  digitalWrite(Relay2, LOW);

  //TURN ON RELAY 3 PUMP PIN A2
  digitalWrite(Relay3, LOW);
  light_control();
  //PRINT "Shutting Down"
  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Shutting Down");
  Serial.println("Shutting Down");

  //DELAY 2 SECONDS
  for (int i = 0; i < 20; i++) {
    light_control();
    delay(100);
  }

  // PRINT * MINUTE COUNTDOWN
  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(3, 0);
  lcd.print("15 Minute");   
  lcd.setCursor(1, 1);
  lcd.print("Shutdown Delay");
  light_control();
  //DELAY 2 SECONDS
  for (int i = 0; i < 20; i++) {
    light_control();
    delay(100);
  }
  light_control();
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("Minutes Left");
  int counter = 0;
  int j = 5;
  int min = 15; // ***********Countdown Setting************15 mins******


  lcd.setCursor(7, 0);
  lcd.print(min);
  Serial.println(min);
  /* Modified section */
  for (int i = 0; i < 15; i++) //15min  // *************Countdown Setting*************15 mins****
  {



    for (int i = 0; i < 600; i++) //1 min = 60*100ms =60000ms = 60sec  // *************Countdown Setting*************15 mins****
    {
      light_control();
      Emergency_ButtonState = digitalRead(Emergency);
      if (Emergency_ButtonState == HIGH)Emergency_Routine_2();
      // light_control();
      if ((min - counter) < 10)
      {
        lcd.setCursor(7, 0);
        lcd.print(min - counter);
        lcd.setCursor(8, 0);
        lcd.print(" ");
        Serial.println(min - counter);
      }
      else {
        lcd.setCursor(7, 0);
        lcd.print(min - counter);
        Serial.println(min - counter);
      }
      delay(100); // 60 sec delay
    }

    counter++;
  }


  //TURN OFF RELAY 2 FAN PIN A1
  digitalWrite(Relay2, HIGH);

  //TURN OFF RELAY 3 PUMP PIN A2
  digitalWrite(Relay3, HIGH);

  //PRINT "Shutdown Complete"
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Shutdown");
  lcd.setCursor(4, 1);
  lcd.print("Complete");

  //DELAY 2 SECONDS
  for (int i = 0; i < 20; i++) {
    light_control();
    delay(100);
  }

  //TURN OFF RELAY 1 Light PIN A0
  digitalWrite(Relay1, HIGH);
  Relay_LightState = HIGH;

  EEPROM.write(1, 3);                            //##################POWER FAILURE OUT OF OPERATION#####################
  
  //DELAY 2 SECONDS
  // delay(2000);

  //RESET ARDUINO
  resetFunc();  //call reset
  light_control();
  Emergency_ButtonState = digitalRead(Emergency);
  if (Emergency_ButtonState == HIGH)Emergency_Routine_2();
   }

  
}



void Start_Routine(void) {

  digitalWrite(Relay1, LOW); //TURN ON RELAY 1 Light PIN A0
  Relay_LightState = LOW;
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(3, 0);
  lcd.print("30 Second");
  lcd.setCursor(1, 1);
  lcd.print("Startup Delay");
  Serial.println("Startup Delay");
  light_control();
  for (int i = 0; i < 20; i++) {
    light_control();
    delay(100);
  }


  digitalWrite(Relay2, LOW);  //TURN ON RELAY 2 FAN PIN A1

  // PRINT 30 SEC COUNTDOWN
  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(2, 1);
  lcd.print("Seconds Left");
  Serial.println("Seconds Left");
  lcd.setCursor(7, 0);
  for (int i = 30; i >= 0; i-- ) // Countdown Setting      *************Startup Timing*********30 Seconds*********

  {
    light_control();
    Emergency_ButtonState = digitalRead(Emergency);
    if (Emergency_ButtonState == HIGH)Emergency_Routine_2();



    /*Added code*/
    if (i < 10)
    {
      lcd.setCursor(7, 0);
      lcd.print(i);
      lcd.setCursor(8, 0);
      lcd.print(" ");
      Serial.println(i);
    }
    else {
      lcd.setCursor(7, 0);
      lcd.print(i);
      Serial.println(i);
    }
    /*End of added code*/


    //DELAY 1 SECOND
    for (int i = 0; i < 10; i++) {
      light_control();
      delay(100);
    }

  }

  //CHECK WATER IS ON? PIN 10
  if (digitalRead(Water)) //yes
  {
    if_water_on();
  }
  else  //no
  {
    if_water_off();
  }

}

void if_water_on()
{
  //PRINT FIRST LINE OF LCD "Water Level = OK"
  light_control();
  lcd.clear();
  lcd.createChar(1, humchar1);
  lcd.createChar(2, humchar2);
  lcd.createChar(3, humchar3);
  lcd.createChar(4, humchar4);
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.setCursor(1, 0);
  lcd.write(3);
  lcd.setCursor(1, 1);
  lcd.write(4);
  lcd.setCursor(4, 0);
  lcd.print("Water=OK");
  light_control();
  //DELAY 2 SECONDS
  for (int i = 0; i < 20; i++) {
    light_control();
    delay(100);
  }

  //TURN ON RELAY 3 PUMP, PIN A2 After Checking Water Level
  digitalWrite(Relay3, LOW);

  //CHECK AIR IS ON? PIN 9
  if (digitalRead(Air)) //yes
  {
    if_air_on();
  }
  else //no
  {
    if_air_off();
  }
}

void if_air_on()
{
  //PRINT SECOND LINE OF LCD "Air Flow = OK"
  light_control();
  // lcd.clear();
  lcd.createChar(5 , wind1); //Numbering should start at 1, not 0
  lcd.createChar(6 , wind2);
  lcd.createChar(7 , wind3);
  lcd.createChar(8 , wind4);

  lcd.createChar(1, humchar1);
  lcd.createChar(2, humchar2);
  lcd.createChar(3, humchar3);
  lcd.createChar(4, humchar4);
  light_control();
  lcd.setCursor(14, 0);
  lcd.write(5);
  lcd.setCursor(14, 1);
  lcd.write(6);
  lcd.setCursor(15, 0);
  lcd.write(7);
  lcd.setCursor(15, 1);
  lcd.write(8);
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.setCursor(1, 0);
  lcd.write(3);
  lcd.setCursor(1, 1);
  lcd.write(4);
  lcd.setCursor(4, 0);
  lcd.print("Water=OK");
  lcd.setCursor(5, 1);
  lcd.print("Air=OK");
  light_control();
  //DELAY 2 SECONDS
  for (int i = 0; i < 20; i++) {
    light_control();
    delay(100);
  }
  light_control();
  //2 SECONDS DELAY
  //DELAY 2 SECONDS
  for (int i = 0; i < 20; i++) {
    light_control();
    delay(100);
  }
  light_control();
  //TURN ON RELAY 4 HOT PLATE PIN A3
  digitalWrite(Relay4, LOW);

  EEPROM.write(1, 4);                            //##################POWER FAILURE IN OPERATION#####################

  //PRINT "Service Available"
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Service");
  lcd.setCursor(3, 1);
  lcd.print("Available");

  //IF WATER PIN 10 OR AIR PIN 9 GO OFF THEN EMERGENCY SHUTDOWN
  while ((digitalRead(Air) == HIGH) && (digitalRead(Water) == HIGH) && (digitalRead(Emergency) == LOW)) {
    delay(100);
    light_control();
    Stop_ButtonState = digitalRead(Stop);
    if (Stop_ButtonState) Stop_Routine();
  }
  Emergency_Routine_2();

}

void if_air_off()
{
  light_control();
  //PRINT "No Air flow"
  lcd.clear();
  lcd.createChar(1 , wind1); //Numbering should start at 1, not 0
  lcd.createChar(2 , wind2);
  lcd.createChar(3 , wind3);
  lcd.createChar(4 , wind4);
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.setCursor(1, 0);
  lcd.write(3);
  lcd.setCursor(1, 1);
  lcd.write(4);
  lcd.setCursor(6, 0);
  lcd.print("Check"); // Print a message to the LCD.
  lcd.setCursor(5, 1);
  lcd.print("Air Flow");
  light_control();

  //BUZZER ON
  tone(buzzer, 500);

  //WAIT 10 SECONDS FOR THE AIR PIN 9 TO COME ON
  int count = 0;
  while ((digitalRead(Air) == LOW) && (count <= 7000) ) {
    light_control();
    tone(buzzer, 500);
    for (int i = 0; i < 5 ; i++) {
      light_control();
      delay(50);
    }
    light_control();
    noTone(buzzer);
    for (int i = 0; i < 5 ; i++) {
      light_control();
      delay(50);
    }
    count++;

  }

  //IF AIR PIN 9 COMES ON WITHIN 10 SECONDS THEN BUZZER OFF AND JUMP TO "if_air_on" ROUTINE
  if (digitalRead(Air)) {
    light_control();
    noTone(buzzer);
    lcd.clear();
    if_air_on(); //if air button turns on within 10 seconds then it will go to "if_air_on" routine
  }

  //IF AIR PIN 9 DOESN'T COME ON THEN EMERGENCY SHUTDOWN
  else { //if air button does not turns on within 10 seconds then it will go to "Emergency_Routine" routine
    noTone(buzzer);
    Emergency_Routine();
  }

}

void if_water_off()
{
  //PRINT "No Water Flow"
  lcd.begin(16, 2);
  lcd.createChar(1, humchar1);
  lcd.createChar(2, humchar2);
  lcd.createChar(3, humchar3);
  lcd.createChar(4, humchar4);
  light_control();
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.setCursor(1, 0);
  lcd.write(3);
  lcd.setCursor(1, 1);
  lcd.write(4);
  lcd.setCursor(6, 0);
  lcd.print("Check"); // Print a message to the LCD.
  lcd.setCursor(3, 1);
  lcd.print("Water Level");
  light_control();

  //BUZZER ON
  tone(buzzer, 500);

  //WAIT 10 SECONDS FOR THE WATER PIN 10 TO COME ON
  int count = 0;
  while ((digitalRead(Water) == LOW) && (count <= 7000) ) {
    light_control();
    tone(buzzer, 500);
    for (int i = 0; i < 5 ; i++) {
      light_control();
      delay(50);
    }
    light_control();
    noTone(buzzer);
    for (int i = 0; i < 5 ; i++) {
      light_control();
      delay(50);
    }
    count++;
  }

  //IF WATER PIN 10 COMES ON WITHIN 10 SECONDS THEN BUZZER OFF AND JUMP TO "if_water_on" ROUTINE
  if (digitalRead(Water)) {
    noTone(buzzer);
    if_water_on(); //if air button turns on within 10 seconds then it will go to "if_air_on" routine
  }

  //IF WATER PIN 10 DOESN'T COME ON THEN EMERGENCY SHUTDOWN
  else { //if air button does not turns on within 10 seconds then it will go to "Emergency_Routine" routine
    noTone(buzzer);
    Emergency_Routine();
  }

}


void Stop_Routine(void) {
  light_control();
  //TURN ON RELAY 1 Light PIN A0
  digitalWrite(Relay1, LOW);
  Relay_LightState = LOW;

  //TURN OFF RELAY 4 HOT PLATE PIN A3
  digitalWrite(Relay4, HIGH);

  //TURN ON RELAY 2 FAN PIN A1
  digitalWrite(Relay2, LOW);

  //TURN ON RELAY 3 PUMP PIN A2
  digitalWrite(Relay3, LOW);
  light_control();
  //PRINT "Shutting Down"
  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Shutting Down");
  Serial.println("Shutting Down");

  //DELAY 2 SECONDS
  for (int i = 0; i < 20; i++) {
    light_control();
    delay(100);
  }

  // PRINT * MINUTE COUNTDOWN
  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(3, 0);
  lcd.print("15 Minute");   
  lcd.setCursor(1, 1);
  lcd.print("Shutdown Delay");
  light_control();
  //DELAY 2 SECONDS
  for (int i = 0; i < 20; i++) {
    light_control();
    delay(100);
  }
  light_control();
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("Minutes Left");
  int counter = 0;
  int j = 5;
  int min = 15; // ***********Countdown Setting************15 mins******


  lcd.setCursor(7, 0);
  lcd.print(min);
  Serial.println(min);
  /* Modified section */
  for (int i = 0; i < 15; i++) //15min  // *************Countdown Setting*************15 mins****
  {



    for (int i = 0; i < 600; i++) //1 min = 60*100ms =60000ms = 60sec  // *************Countdown Setting*************15 mins****
    {
      light_control();
      Emergency_ButtonState = digitalRead(Emergency);
      if (Emergency_ButtonState == HIGH)Emergency_Routine_2();
      // light_control();
      if ((min - counter) < 10)
      {
        lcd.setCursor(7, 0);
        lcd.print(min - counter);
        lcd.setCursor(8, 0);
        lcd.print(" ");
        Serial.println(min - counter);
      }
      else {
        lcd.setCursor(7, 0);
        lcd.print(min - counter);
        Serial.println(min - counter);
      }
      delay(100); // 60 sec delay
    }

    counter++;
  }

  //TURN OFF RELAY 2 FAN PIN A1
  digitalWrite(Relay2, HIGH);

  //TURN OFF RELAY 3 PUMP PIN A2
  digitalWrite(Relay3, HIGH);

  //PRINT "Shutdown Complete"
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Shutdown");
  lcd.setCursor(4, 1);
  lcd.print("Complete");

  //DELAY 2 SECONDS
  for (int i = 0; i < 20; i++) {
    light_control();
    delay(100);
  }

  //TURN OFF RELAY 1 Light PIN A0
  digitalWrite(Relay1, HIGH);
  Relay_LightState = HIGH;

  EEPROM.write(1, 3);                            //##################POWER FAILURE OUT OF OPERATION#####################
  
  //DELAY 2 SECONDS
  // delay(2000);

  //RESET ARDUINO
  resetFunc();  //call reset
  light_control();
  Emergency_ButtonState = digitalRead(Emergency);
  if (Emergency_ButtonState == HIGH)Emergency_Routine_2();
}


void Emergency_Routine(void) {

  //TURN OFF RELAY 4 Hot Plate PIN A3
  digitalWrite(Relay4, HIGH);

  //TURN OFF RELAY 2 FAN PIN A1
  digitalWrite(Relay2, HIGH);

  //TURN OFF RELAY 3 PUMP PIN A2
  digitalWrite(Relay3, HIGH);

  EEPROM.update(1, 3);                            //##################POWER FAILURE OUT OF OPERATION#####################

  //PRINT "Emergency Shutdown"
  lcd.clear();
  lcd.createChar(5 , wind1); //Numbering should start at 1, not 0
  lcd.createChar(6 , wind2);
  lcd.createChar(7 , wind3);
  lcd.createChar(8 , wind4);

  lcd.createChar(1, humchar1);
  lcd.createChar(2, humchar2);
  lcd.createChar(3, humchar3);
  lcd.createChar(4, humchar4);
  light_control();
  lcd.setCursor(14, 0);
  lcd.write(5);
  lcd.setCursor(14, 1);
  lcd.write(6);
  lcd.setCursor(15, 0);
  lcd.write(7);
  lcd.setCursor(15, 1);
  lcd.write(8);
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.setCursor(1, 0);
  lcd.write(3);
  lcd.setCursor(1, 1);
  lcd.write(4);
  lcd.setCursor(3, 0);
  lcd.print("Check Air");
  lcd.setCursor(3, 1);
  lcd.print("Check Water");


  //BUZZER ON PIN11
  tone(buzzer, 500);

  //STAYS THIS WAY UNTILL EMERGENCY SHUTDOWN RESET PIN 8
  while (digitalRead(Emergency) == LOW) {
    tone(buzzer, 500);
    delay(250);
    noTone(buzzer);
    delay(250);

  }
  delay(400);
  while (digitalRead(Emergency) == HIGH) {
    tone(buzzer, 500);
    delay(250);
    noTone(buzzer);
    delay(250);
  }
  //RESET ARDUINO
  resetFunc();  //call reset
}



void Emergency_Routine_2(void) {

  lcd.setBacklight(HIGH);

  //TURN OFF RELAY 4 Hot Plate PIN A3
  digitalWrite(Relay4, HIGH);
  light_control();
  //TURN ON RELAY 1 Light PIN A0
  digitalWrite(Relay1, LOW);
  Relay_LightState = LOW;

  //TURN ON RELAY 2 FAN PIN A1
  digitalWrite(Relay2, LOW);

  //TURN ON RELAY 3 PUMP PIN A2
  digitalWrite(Relay3, LOW);

  EEPROM.update(1, 3);                            //##################POWER FAILURE OUT OF OPERATION#####################

  //PRINT "Emergency Shutdown"
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Emergency");
  lcd.setCursor(4, 1);
  lcd.print("Shutdown");
  light_control();
  //BUZZER ON PIN11
  tone(buzzer, 500);

  //STAYS THIS WAY UNTILL EMERGENCY SHUTDOWN RESET PIN 8
  while (digitalRead(Emergency) == LOW) {
    light_control();
    tone(buzzer, 500);
    for (int i = 0; i < 5 ; i++) {
      light_control();
      delay(50);
    }

    light_control();
    noTone(buzzer);
    for (int i = 0; i < 5 ; i++) {
      light_control();
      delay(50);
    }

  }
  //4 SECONDS DELAY
  for (int i = 0; i < 4 ; i++) {
    light_control();
    delay(100);
  }


  while (digitalRead(Emergency) == HIGH) {
    light_control();

    tone(buzzer, 500);
    for (int i = 0; i < 5 ; i++) {
      light_control();
      delay(50);
    }

    light_control();
    noTone(buzzer);
    for (int i = 0; i < 5 ; i++) {
      light_control();
      delay(50);
    }
  }
  light_control();
  //RESET ARDUINO
  resetFunc();  //call reset
}


void light_control(void) {
  if (!LightState) {
    latch = LOW;
  }

  LightState =  digitalRead(Light);
  if (LightState && !latch) {
    latch = HIGH;
    Serial.println("The button is pressed");

    // toggle state of LED
    Relay_LightState = !Relay_LightState;

    // control LED arccoding to the toggleed sate
    digitalWrite(Relay1, Relay_LightState);
  }
}
