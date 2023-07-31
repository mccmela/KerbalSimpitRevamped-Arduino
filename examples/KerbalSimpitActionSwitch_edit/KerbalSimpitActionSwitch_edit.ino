/* KerbalSimpitActionSwitch
   A demonstration of using two switches to command both SAS and RCS.
   In this example, the KSP state is monitored to update the SAS and RCS
   to match the switch state. This ensure that when using the keyboard or when
   switching vessels, the game will still match the switch positions.
   In addition, the LED_BUILTIN matches the SAS state.

*/
#include "KerbalSimpit.h" //deleted const int ledPin = 13 from Stage.Demo

//BUTTONS - 10 pins
const int STAGE_PIN = 2;
const int RCS_SWITCH_PIN = 3; // the pin used for controlling RCS.
const int GEAR_PIN = 5;
const int SAS_SWITCH_PIN = 7; // the pin used for controlling SAS.
const int LIGHTS_PIN = 4;
// const int solar panels
// const int ladder
// const int chutes? 
const int BRAKES_PIN = 8;
const int ABORT_PIN = 6;

//ANALOG - 7 pins
// throttle
// translation
// rotation

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
//pinMode(SOLAR_PIN, INPUT_PULLUP)
//pinMode(ladder)
//pinMode(chutes)
  pinMode(BRAKES_PIN, INPUT_PULLUP);
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
  bool brakes_switch_state = digitalRead(BRAKES_PIN);

  // Update the BRAKES to match the state, only if a change is needed to avoid
  // spamming commands.
  if(brakes_switch_state && !(currentActionStatus & BRAKES_ACTION)){
    mySimpit.printToKSP("Pump the brakes, turbo.", PRINT_TO_SCREEN);
    mySimpit.activateAction(BRAKES_ACTION);
  }
  if(!brakes_switch_state && (currentActionStatus & BRAKES_ACTION)){
    mySimpit.printToKSP("Punch it!", PRINT_TO_SCREEN);
    mySimpit.deactivateAction(BRAKES_ACTION);
  }

//Anyway to get it to print on screen "Energize?" when we press button?
// Read the state of the STAGING switch into a local variable.
  int reading_stage = digitalRead(STAGE_PIN);

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
        mySimpit.printToKSP("Energize!", PRINT_TO_SCREEN);
      }
    }
  }

  // Set the LED to match the state of the button.
  digitalWrite(LED_BUILTIN, stageState);

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastStageState = reading_stage;

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
