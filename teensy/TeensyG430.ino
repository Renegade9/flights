/* 
 TeensyG430

 Interface a home-built flight simulator Garmin 430-like switch panel with X-Plane
 via the Teensy FlightSim Library.

 Hardware consists of:
   a) Range IN and OUT rocker switch (two buttons effectively)
   b) Four pushbuttons on the right side of the unit
   c) Five pushbuttons under the LCD screen.
   d) Dual Rotary Encoder with an integrated pushbutton.

 The LCD screen is out of scope for this interface.  That has an HDMI input and
 there is a software solution/hack to get the G430 display on the small LCD.

 The COM/NAV switches were not included due to lack of available "real estate"
 on the hardware enclosure, since the LCD screen I am using is larger than the
 Garmin 430 screen.  So the switch panel covers the GPS functions only.
 I think that actually makes it a Garmin 400, but since the software simulates
 a 430, I'm calling it that.  I'll make another box for the radio tuner knobs.

 https://buy.garmin.com/en-US/US/in-the-air/discontinued/gps-400w/prod298.html

 Brute force programming, I could make it more elegant by creating arrays
 of the buttons, datarefs, state checks ... but this approach is better at
 self-documenting.
 
 Created 03 April 2016 by Chris Saulit
 This sample code is in the public domain.
 */
 
#include <Bounce.h>

// pin assignments
const int rangeInPin = 2;
const int rangeOutPin = 3;

const int directPin = 4;
const int menuPin = 5;
const int clrPin = 6;
const int entPin = 7;

const int procPin = 8;
const int fplPin = 9;
const int msgPin = 10;
const int obsPin = 11;
const int cdiPin = 12;

const int cursorPin = 18;

// Special variables to access the transponder datarefs

// range in and out pushbuttons
FlightSimCommand zoomIn;
FlightSimCommand zoomOut;

// other pushbuttons
FlightSimCommand pressDirect;
FlightSimCommand pressMenu;
FlightSimCommand pressClr;
FlightSimCommand pressEnt;
FlightSimCommand toggleCdi;
FlightSimCommand toggleObs;
FlightSimCommand toggleMsg;
FlightSimCommand toggleFpl;
FlightSimCommand toggleProc;

FlightSimCommand pressCursor;

/*
 * D2+ sim/GPS/g430n1_cursor
D3+ sim/GPS/g430n1_chapter_dn
D4+ sim/GPS/g430n1_chapter_up 
D5+ sim/GPS/g430n1_page_dn
D6+ sim/GPS/g430n1_page_up 
 */

// Use the debounce functions for our push-buttons
const int debounceMillis = 5;
Bounce rangeInButton = Bounce(rangeInPin, debounceMillis);
Bounce rangeOutButton = Bounce(rangeOutPin, debounceMillis);

Bounce directButton = Bounce(directPin, debounceMillis);
Bounce menuButton = Bounce(menuPin, debounceMillis);
Bounce clrButton = Bounce(clrPin, debounceMillis);
Bounce entButton = Bounce(entPin, debounceMillis);

Bounce cdiButton = Bounce(cdiPin, debounceMillis);
Bounce obsButton = Bounce(obsPin, debounceMillis);
Bounce msgButton = Bounce(msgPin, debounceMillis);
Bounce fplButton = Bounce(fplPin, debounceMillis);
Bounce procButton = Bounce(procPin, debounceMillis);

Bounce cursorButton = Bounce(cursorPin, debounceMillis);


// setup runs once
void setup() {

  pinMode(rangeInPin, INPUT_PULLUP);
  pinMode(rangeOutPin, INPUT_PULLUP);

  pinMode(directPin, INPUT_PULLUP);
  pinMode(menuPin, INPUT_PULLUP);
  pinMode(clrPin, INPUT_PULLUP);
  pinMode(entPin, INPUT_PULLUP);

  pinMode(cdiPin, INPUT_PULLUP);
  pinMode(obsPin, INPUT_PULLUP);
  pinMode(msgPin, INPUT_PULLUP);
  pinMode(fplPin, INPUT_PULLUP);
  pinMode(procPin, INPUT_PULLUP);

  pinMode(cursorPin, INPUT_PULLUP);

  // associate variables to X-Plane Datarefs
  zoomIn = XPlaneRef("sim/GPS/g430n1_zoom_in");
  zoomOut = XPlaneRef("sim/GPS/g430n1_zoom_out");

  pressDirect = XPlaneRef("sim/GPS/g430n1_direct");
  pressMenu = XPlaneRef("sim/GPS/g430n1_menu");
  pressClr = XPlaneRef("sim/GPS/g430n1_clr");
  pressEnt = XPlaneRef("sim/GPS/g430n1_ent");

  toggleCdi = XPlaneRef("sim/GPS/g430n1_cdi");
  toggleObs = XPlaneRef("sim/GPS/g430n1_obs");
  toggleMsg = XPlaneRef("sim/GPS/g430n1_msg");
  toggleFpl = XPlaneRef("sim/GPS/g430n1_fpl");
  toggleProc = XPlaneRef("sim/GPS/g430n1_proc");

  pressCursor = XPlaneRef("sim/GPS/g430n1_cursor");

}

// loop runs repetitively, as long as power is on (duh)
void loop() {
  FlightSim.update(); // causes X-Plane's changes to be received

  // With the pushbuttons, I'm using the internal pull-up resistors, so
  // the "open" reading will be high, and when pressed, will go low,
  // so we check for "fallingEdge" to detect button presses.

  rangeInButton.update();
  if (rangeInButton.fallingEdge()) {
    zoomIn = 1;
  }

  rangeOutButton.update();
  if (rangeOutButton.fallingEdge()) {
    zoomOut = 1;
  }


  directButton.update();
  if (directButton.fallingEdge()) {
    pressDirect = 1;
  }
  menuButton.update();
  if (menuButton.fallingEdge()) {
    pressMenu = 1;
  }
  clrButton.update();
  if (clrButton.fallingEdge()) {
    pressClr = 1;
  }
  entButton.update();
  if (entButton.fallingEdge()) {
    pressEnt = 1;
  }

  cdiButton.update();
  if (cdiButton.fallingEdge()) {
    toggleCdi = 1;
  }
  obsButton.update();
  if (obsButton.fallingEdge()) {
    toggleObs = 1;
  }
  msgButton.update();
  if (msgButton.fallingEdge()) {
    toggleMsg = 1;
  }
  fplButton.update();
  if (fplButton.fallingEdge()) {
    toggleFpl = 1;
  }
  procButton.update();
  if (procButton.fallingEdge()) {
    toggleProc = 1;
  }

  cursorButton.update();
  if (cursorButton.fallingEdge()) {
    pressCursor = 1;
  }
}


