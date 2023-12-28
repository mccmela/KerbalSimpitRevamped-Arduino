/* Copie from the KerbalSimpitActionSwitch, initally. If there are three /// 
eventually remove when the buttons are in place.

*/
#include "KerbalSimpit.h" //deleted const int ledPin = 13 from Stage.Demo

//BUTTONS - 10 pins CYRRENTLY HAVE 2 LATCHING WHITE PINS UNWIRED / LATCHING (PINS 7 & 6)

const int RCS_SWITCH_PIN = 13; // the pin used for controlling RCS. Latching
const int SAS_SWITCH_PIN = 12; // the pin used for controlling SAS. Latching
const int LIGHTS_PIN = 11; //Latching
const int GEAR_PIN = 10; //Latching
/// const int solar panels 
/// const int ladder
/// const int chutes? 
///const int BRAKES_PIN = 8;
const int STAGE_PIN = 4; // Momentary
const int ABORT_PIN = 3; //now the map button

//ANALOG - 7 PINS
const int THROTTLE_PIN = A15; // the pin used for controlling throttle
const int PITCH_PIN = A14;
const int ROLL_PIN = A13;    // the pin used for controlling roll
const int YAW_PIN = A12;     // the pin used for controlling yaw
const int TRANSX_PIN = A3;
const int TRANSY_PIN = A2;
const int TRANSZ_PIN = A1;

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int stageState;             // the current reading from the input pin
int lastStageState = LOW;   // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50; 

//Store the current action status, as recevied by simpit.
byte currentActionStatus = 0;

// Declare a KerbalSimpit object that will communicate using the "Serial" device.
KerbalSimpit mySimpit(Serial);

void setup() {
  // Open the serial connection.
  Serial.begin(115200);

  // Set up the build in LED, and turn it on.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  

  // Set up the switches with builtin pullup.
  pinMode(STAGE_PIN, INPUT_PULLUP);
  pinMode(RCS_SWITCH_PIN, INPUT_PULLUP);
  pinMode(GEAR_PIN, INPUT_PULLUP);
  pinMode(SAS_SWITCH_PIN, INPUT_PULLUP);
  pinMode(LIGHTS_PIN, INPUT_PULLUP);
///pinMode(SOLAR_PIN, INPUT_PULLUP)
///pinMode(ladder)
///pinMode(chutes)
 /// pinMode(BRAKES_PIN, INPUT_PULLUP);
  pinMode(ABORT_PIN, INPUT_PULLUP); 

  // This loop continually attempts to handshake with the plugin.
  // It will keep retrying until it gets a successful handshake.
  while (!mySimpit.init()) {
    delay(100);
  }
  // Turn off the built-in LED to indicate handshaking is complete.
  digitalWrite(LED_BUILTIN, LOW);
  // Display a message in KSP to indicate handshaking is complete.
  mySimpit.printToKSP("Connected", PRINT_TO_SCREEN);
  // Sets our callback function. The KerbalSimpit library will
  // call this function every time a packet is received.
  mySimpit.inboundHandler(messageHandler);
  // Send a message to the plugin registering for the Action status channel.
  // The plugin will now regularly send Action status  messages while the
  // flight scene is active in-game.
  mySimpit.registerChannel(ACTIONSTATUS_MESSAGE);
}

void loop() {
  // Check for new serial messages.
  mySimpit.update();

  // Get the SAS switch state
  bool sas_switch_state = digitalRead(SAS_SWITCH_PIN);

  // Update the SAS to match the state, only if a change is needed to avoid
  // spamming commands.
  if(sas_switch_state && !(currentActionStatus & SAS_ACTION)){
    mySimpit.printToKSP("Activate SAS!", PRINT_TO_SCREEN);
    mySimpit.activateAction(SAS_ACTION);
  }
  if(!sas_switch_state && (currentActionStatus & SAS_ACTION)){
    mySimpit.printToKSP("Desactivate SAS!", PRINT_TO_SCREEN);
    mySimpit.deactivateAction(SAS_ACTION);
  }

  // Get the RCS switch state
  bool rcs_switch_state = digitalRead(RCS_SWITCH_PIN);

  // Update the RCS to match the state, only if a change is needed to avoid
  // spamming commands.
  if(rcs_switch_state && !(currentActionStatus & RCS_ACTION)){
    mySimpit.printToKSP("Activate RCS!", PRINT_TO_SCREEN);
    mySimpit.activateAction(RCS_ACTION);
  }
  if(!rcs_switch_state && (currentActionStatus & RCS_ACTION)){
    mySimpit.printToKSP("Desactivate RCS!", PRINT_TO_SCREEN);
    mySimpit.deactivateAction(RCS_ACTION);
  }

  // Get the GEAR switch state
  bool gear_switch_state = digitalRead(GEAR_PIN);

  // Update the GEAR to match the state, only if a change is needed to avoid
  // spamming commands.
  if(gear_switch_state && !(currentActionStatus & GEAR_ACTION)){
    mySimpit.printToKSP("Deploy Gear!", PRINT_TO_SCREEN);
    mySimpit.activateAction(GEAR_ACTION);
  }
  if(!gear_switch_state && (currentActionStatus & GEAR_ACTION)){
    mySimpit.printToKSP("Retract Gear!", PRINT_TO_SCREEN);
    mySimpit.deactivateAction(GEAR_ACTION);
  }

// Get the LIGHTS switch state
  bool lights_switch_state = digitalRead(LIGHTS_PIN);

  // Update the LIGHTS to match the state, only if a change is needed to avoid
  // spamming commands.
  if(lights_switch_state && !(currentActionStatus & LIGHT_ACTION)){
    mySimpit.printToKSP("ILLUMINATION!", PRINT_TO_SCREEN);
    mySimpit.activateAction(LIGHT_ACTION);
  }
  if(!lights_switch_state && (currentActionStatus & LIGHT_ACTION)){
    mySimpit.printToKSP("Get Dark!", PRINT_TO_SCREEN);
    mySimpit.deactivateAction(LIGHT_ACTION);
  }

  // Get the ABORT switch state
  bool abort_switch_state = digitalRead(ABORT_PIN);

  // Update the ABORT to match the state, only if a change is needed to avoid
  // spamming commands.
  if(abort_switch_state && !(currentActionStatus & ABORT_ACTION)){
    mySimpit.printToKSP("SHUT IT DOWN!", PRINT_TO_SCREEN);
    mySimpit.activateAction(ABORT_ACTION);
  }
  if(!abort_switch_state && (currentActionStatus & ABORT_ACTION)){
    mySimpit.printToKSP("RUN IT BACK!", PRINT_TO_SCREEN);
    mySimpit.deactivateAction(ABORT_ACTION);
  }

  // Get the BRAKES switch state
  ///bool brakes_switch_state = digitalRead(BRAKES_PIN);

  // Update the BRAKES to match the state, only if a change is needed to avoid
  // spamming commands.
  ///if(brakes_switch_state && !(currentActionStatus & BRAKES_ACTION)){
    ///mySimpit.printToKSP("Pump the brakes, turbo.", PRINT_TO_SCREEN);
    ///mySimpit.activateAction(BRAKES_ACTION);
 /// }
 /// if(!brakes_switch_state && (currentActionStatus & BRAKES_ACTION)){
   /// mySimpit.printToKSP("Punch it!", PRINT_TO_SCREEN);
   /// mySimpit.deactivateAction(BRAKES_ACTION);
 /// }

//Anyway to get it to print on screen "Energize?" when we press button?
// Read the state of the STAGING switch into a local variable.
  int reading_stage = digitalRead(STAGE_PIN);
  ///bool intiate = false;
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading_stage != lastStageState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading_stage != stageState) {
      stageState = reading_stage;

      // If the new button state is HIGH, that means the button
      // has just been pressed.
      if (stageState == HIGH) {
        // Send a message to the plugin activating the Stage
        // action group. The plugin will then activate the
        // next stage.
        mySimpit.activateAction(STAGE_ACTION);
           /// if (intiate == false) { 
        mySimpit.printToKSP("Energize!", PRINT_TO_SCREEN);
           ///   intiate = true;
       // }
        
      }
    }
  }

  // Set the LED to match the state of the button.
  digitalWrite(LED_BUILTIN, stageState);

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastStageState = reading_stage;


//JOYSTICKS 
throttleMessage throttle_msg;
  // Read the value of the potentiometer
  int reading = analogRead(THROTTLE_PIN);
  // Convert it in KerbalSimpit Range
  throttle_msg.throttle = map(reading, 0, 1023, 0, INT16_MAX);
  // Send the message
  mySimpit.send(THROTTLE_MESSAGE, throttle_msg);

  //Rotation
  rotationMessage rot_msg;
  // Read the values of the potentiometers
  int reading_pitch = analogRead(PITCH_PIN);
  int reading_roll = analogRead(ROLL_PIN);
  int reading_yaw = analogRead(YAW_PIN);

  // Convert them in KerbalSimpit range
  int16_t pitch = map(reading_pitch, 0, 1023, INT16_MIN, INT16_MAX);
    if ((pitch > -3200) && (pitch < 3200)) {
     rot_msg.setPitch(0);
     ///mySimpit.printToKSP("Pitch Dead Zone",PRINT_TO_SCREEN);
    } 
    else {
      rot_msg.setPitch(pitch);
      ///mySimpit.printToKSP("Pitch Alive Zone",PRINT_TO_SCREEN);
    }
  int16_t roll = map(-reading_roll, -1023, 0, INT16_MIN, INT16_MAX);
    if ((roll > -3200) && (roll < 3200)) {
      rot_msg.setRoll(0);
      ///mySimpit.printToKSP("Roll Dead Zone",PRINT_TO_SCREEN);
    } 
    else {
      rot_msg.setRoll(roll);
      ///mySimpit.printToKSP("Roll Alive Zone",PRINT_TO_SCREEN);
     }
  int16_t yaw = map(reading_yaw, 0, 1023, INT16_MIN, INT16_MAX);
    if ((yaw > -3200) && (yaw < 3200)) {
      rot_msg.setYaw(0);
      ///mySimpit.printToKSP("Yaw Dead Zone",PRINT_TO_SCREEN);
    } 
    else {
      rot_msg.setYaw(yaw);
      ///mySimpit.printToKSP("Yaw Alive Zone",PRINT_TO_SCREEN);
     }
  // Put those values in the message
  
  // Send the message
  mySimpit.send(ROTATION_MESSAGE, rot_msg);
  ///mySimpit.printToKSP(String(pitch), PRINT_TO_SCREEN); ///String(reading_pitch)

  //Translation
  translationMessage trans_msg; ////?????
  // Read the values of the potentiometers
  int reading_X = analogRead(TRANSX_PIN);
  int reading_Y = analogRead(TRANSY_PIN);
  int reading_Z = analogRead(TRANSZ_PIN);


  // Convert them in KerbalSimpit range
  int16_t x = map(reading_X, 0, 1023, INT16_MIN, INT16_MAX);
    if ((x > -3200) && (x < 3200)) {
     trans_msg.setX(0);
     ///mySimpit.printToKSP("X Dead Zone",PRINT_TO_SCREEN);
    } 
    else {
      trans_msg.setX(x);
      ///mySimpit.printToKSP("X Alive Zone",PRINT_TO_SCREEN);
    }

  int16_t y = map(reading_Y, 0, 1023, INT16_MIN, INT16_MAX);
    if ((y > -3200) && (y < 3200)) {
     trans_msg.setY(0);
     ///mySimpit.printToKSP("X Dead Zone",PRINT_TO_SCREEN);
    } 
    else {
      trans_msg.setY(y);
      ///mySimpit.printToKSP("X Alive Zone",PRINT_TO_SCREEN);
    }
    
  int16_t z = map(reading_Z, 0, 1023, INT16_MIN, INT16_MAX);
    if ((z > -3200) && (z < 3200)) {
     trans_msg.setZ(0);
     ///mySimpit.printToKSP("X Dead Zone",PRINT_TO_SCREEN);
    } 
    else {
      trans_msg.setZ(z);
      ///mySimpit.printToKSP("X Alive Zone",PRINT_TO_SCREEN);
    }
  
  // Send the message
  mySimpit.send(TRANSLATION_MESSAGE, trans_msg);
  ///mySimpit.printToKSP(String(reading_pitch));
}

void messageHandler(byte messageType, byte msg[], byte msgSize) {
  switch(messageType) {
  case ACTIONSTATUS_MESSAGE:
    // Checking if the message is the size we expect is a very basic
    // way to confirm if the message was received properly.
    if (msgSize == 1) {
      currentActionStatus = msg[0];

      //Let the LED_BUILIN match the current SAS state
      if(currentActionStatus & SAS_ACTION){
        digitalWrite(LED_BUILTIN, HIGH);
      } else {
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
    break;
  }
}
