/*

ARDUBEER v1.1  

Client: 

Glenn Adams 
EMAIL: glenn.adams@mweb.co.za

STAINLESS MARINE
www.microbrewingsa.co.za
083 709 6759
22 Fish Eagle Park
Kommetjie

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Coded by Rouan van der Ende
EMAIL: bitlabio@gmail.com
PHONE: 062 933 1183

------------------------------------------------------------------------------------*/

int pulsecount = 0;

int startButtonPin = 7;

int beerfillPin     = 10; //this should control the flow of beer

int co2Pin          = 11; //this should control the lifter co2 gas
int liftSolenoidPin = 12; //this should control the lifter co2 solenoid 

int flowPulsePin    = 3; //Arduino UNO interrupt only on pin 2 or 3.

//LEDS (PWM so we can wire up directly without resistors)
int powerLED = 9;     // ON WHEN SYSTEM HAS POWER
int readyLED = 6;     // ON WHEN SYSTEM IS READY FOR A NEW FILL
int flowLED = 5;      // ON WHEN SYSTEM IS BUSY FILLING

bool disableButton        = 0; //0 = not filling, 1 = filling.
bool measureflow    = 0;
/*------------------------------------------------------------------------------------*/

void setup() {

  //LEDS
  pinMode(powerLED, OUTPUT);
  pinMode(readyLED, OUTPUT);
  pinMode(flowLED, OUTPUT);
  pinMode(beerfillPin,OUTPUT);      
  pinMode(liftSolenoidPin,OUTPUT);  
  pinMode(co2Pin,OUTPUT);   

  //DEFAULT POSITIONS
  digitalWrite(liftSolenoidPin, HIGH); //keep lift solenoid high at the start
  digitalWrite(beerfillPin, LOW);      // close beer

  analogWrite(powerLED, 100); //0-255 brightness
  analogWrite(readyLED, 100);
  analogWrite(flowLED, 0);


         

  // beer flow sensor:
  pinMode(flowPulsePin, INPUT_PULLUP); 
  
  // this should run the flowpulse() function each time a pulse is detected.
  attachInterrupt(digitalPinToInterrupt(flowPulsePin), flowpulse, CHANGE); 
}

/*------------------------------------------------------------------------------------*/

void loop() {
  if (disableButton == false) {
    int buttonstate = digitalRead(startButtonPin);
    if (buttonstate == 1) {
      disableButton = true; //disables the button for now.
      startfill();

    }
  }
}

/*------------------------------------------------------------------------------------*/

void startfill() {
    // This function is called when the button is pushed to start the bottle fill process.

    digitalWrite(liftSolenoidPin, HIGH);  // lift solenoid open
    delay(6000);                          // delay 6 sec
    digitalWrite(co2Pin, HIGH);           // open CO2
    delay(6000);                          // delay 6 sec
    digitalWrite(co2Pin, LOW);            // close CO2
    // start beer fill
    digitalWrite(beerfillPin, HIGH);      // open beer
    pulsecount = 0;                       // reset counter;
    measureflow = true;                   // disables button. and starts to measure flow
    
}

/*------------------------------------------------------------------------------------*/

void flowpulse() {
  // This function is called each time the flow pulse is sensed.
  if (measureflow == true) {
    pulsecount++;
    analogWrite(flowLED, 25+((pulsecount%2)*75)); //should toggle between two brightness when filling on each pulse.
    if (pulsecount >= 12760 ) {
      // beer should be full now.
      pulsecount = 0; // zero counter   
      measureflow = false;   
      endfill();      // bottle full.
    }
  }
}

/*------------------------------------------------------------------------------------*/

void endfill() {
  // Stop beer flow:
  measureflow = false;
  analogWrite(flowLED, 0);              // turn off flow light
  digitalWrite(beerfillPin, LOW);       // turn off beer pin

  // After filling up do this:  
  delay(6000);                          // delay 6 sec
  digitalWrite(liftSolenoidPin, LOW);   //  lift solenoid close (lower bottle)
  delay(3000);                          // give it time to lower.
  disableButton = false;
}