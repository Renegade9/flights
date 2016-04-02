/* 
 TeensyTransponder

 Interface a hardware transponder with X-Plane via the Teensy FlightSim Library.

 X-Plane must have the Teensy plug-in installed.
 USB Type (in Arduino/Teensyduino IDE) must be set to "Flight Sim Controls"

 Transponder hardware being interfaced consists of:
 
   a) Four rotary switches that each produce a Binary Coded Octal output
      (3 bits).
   b) "Ident" button
   c) 4-position switch (not counting off):  Off-Sby-On-Alt-Tst
   d) Output LED to indicate transponder interrogation ("reply light")
 
 Created 30 March 2016 by Chris Saulit
 This sample code is in the public domain.
 */
 
#include <Bounce.h>

// Special variables to access the transponder datarefs

FlightSimInteger transponderLight;
FlightSimInteger transponderCode;
FlightSimCommand transponderIdent;

// Transponder Hardware Pins

const byte num_digits = 4;
const byte pins_per_digit = 3;

// record pin assignment to the switches
const byte inputPins[num_digits][pins_per_digit] = {
  {2, 3, 4}, // switch one
  {5, 6, 7}, // switch two
  {8, 9, 10}, // switch three
  {11, 12, 14} // switch four  note SKIPPING pin 13 (internal LED)
};

byte pinValues[num_digits][pins_per_digit];

const int identPin = 15;
const int replyLightPin = 16;

Bounce identButton = Bounce(identPin, 5);      // Ident Pushbutton, 5ms debounce

String lastSquawk = "";

// setup runs once
void setup() {
  
  pinMode(LED_BUILTIN, OUTPUT);  // pin 11 on Teensy 2.0, pin 6 on Teensy++ 2.0
  pinMode(replyLightPin, OUTPUT);

  transponderLight = XPlaneRef("sim/cockpit/radios/transponder_light");
  transponderLight.onChange(updateLED);

  // get the current transponder code setting
  transponderCode = XPlaneRef("sim/cockpit2/radios/actuators/transponder_code");

  // set up ident button dataref
  transponderIdent = XPlaneRef("sim/transponder/transponder_ident");


    // set the pin modes
  for (int digit = 0; digit < num_digits; digit++) {
    for (int bitPosition = 0; bitPosition < pins_per_digit; bitPosition++) {
      pinMode(inputPins[digit][bitPosition], INPUT_PULLUP);
    }
  }

  // ident pin
  pinMode(identPin, INPUT_PULLUP);
}

String read_squawk() {

  char zeroChar = '0';
  char squawk[num_digits + 1] = {'-', '-', '-', '-', '\0'};

  for (int digit = 0; digit < num_digits; digit++) {
    int digitValue = 0;
    for (int bitPosition = 0; bitPosition < pins_per_digit; bitPosition++) {
      int pinValue = digitalRead(inputPins[digit][bitPosition]);
      digitValue += pinValue << bitPosition;
    }
    squawk[digit] = digitValue + zeroChar;  // convert digit to ASCII
  }

  return squawk;
}

// loop runs repetitively, as long as power is on
void loop() {
  FlightSim.update(); // causes X-Plane's changes to be received

  // TODO:  Don't poll but make it interrupt driven
  
  String squawk = read_squawk(); // read hardware
  if (squawk != lastSquawk) {
    lastSquawk = squawk;
    // send it to X-Plane
    transponderCode = squawk.toInt();
  }

  identButton.update();
  if (identButton.fallingEdge()) {
    transponderIdent = 1;
  }
  if (identButton.risingEdge()) {
    transponderIdent = 0;
  }

}

// updateLED runs only when X-Plane changes transponderLight
void updateLED(long value) {
  if (value == 0) {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(replyLightPin, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(replyLightPin, HIGH);
  }
}

