/* 
 TeensyG430

 Interface a home-built flight simulator Garmin 430-like switch panel with X-Plane
 via the Teensy FlightSim Library.

 Hardware consists of:

   a) Two position rocker switch (two buttons effectively)
      Functions:  [v RNG ^] (Zoom In and Zoom Out)

   b) Four pushbuttons on the right side of the unit
      Functions:  Direct, Menu, Clear, Enter

   c) Five pushbuttons under the LCD screen.
      Functions:  CDI, OBS, MSG, FPL (Flight Plan), PROC (Procedures)

   d) Dual Rotary Encoder with an integrated pushbutton.
      Chapter Down/Up, Page Down/Up, Cursor mode

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
#include <Encoder.h>

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

// dual rotary encoder + pushbutton
const int innerKnobPin1 = 14;
const int innerKnobPin2 = 15;
const int outerKnobPin1 = 16;
const int outerKnobPin2 = 17;
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

FlightSimCommand chapterDn;
FlightSimCommand chapterUp;
FlightSimCommand pageDn;
FlightSimCommand pageUp;
FlightSimCommand pressCursor;

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

Encoder innerKnob = Encoder(innerKnobPin1, innerKnobPin2);
Encoder outerKnob = Encoder(outerKnobPin1, outerKnobPin2);
Bounce cursorButton = Bounce(cursorPin, debounceMillis);

long innerKnobValuePrev=0;    // for detecting rotary position change
long outerKnobValuePrev=0;    // for detecting rotary position change


// setup runs once
void setup() {

  // Serial.begin(9600);

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

  chapterDn = XPlaneRef("sim/GPS/g430n1_chapter_dn");
  chapterUp = XPlaneRef("sim/GPS/g430n1_chapter_up");
  pageDn = XPlaneRef("sim/GPS/g430n1_page_dn");
  pageUp = XPlaneRef("sim/GPS/g430n1_page_up");

  pressCursor = XPlaneRef("sim/GPS/g430n1_cursor");

}



// loop runs repetitively, as long as power is on (duh)
void loop() {
  FlightSim.update(); // causes X-Plane's changes to be received


  if (rangeInButton.update()) {
    if (rangeInButton.fallingEdge()) {
      zoomIn = 1;
    } else {
      zoomIn = 0;
    }
  }
  if (rangeOutButton.update()) {
    if (rangeOutButton.fallingEdge()) {
      zoomOut = 1;
    } else {
      zoomOut = 0;
    }
  }


  // read the rotary encoder, if it's changed, write appropriate commands
  // we're getting two pulses per click, so only act on "even" changes

  long innerKnobValue = innerKnob.read();
  if (innerKnobValue != innerKnobValuePrev) {
    int delta = innerKnobValue - innerKnobValuePrev;
    if (delta != 0 && delta % 2 == 0)  {
      if (delta > 0) {
        pageDn = 1;
        pageDn = 0;
      } else if (delta < 0)  {
        pageUp = 1;
        pageUp = 0;
      }
      innerKnobValuePrev = innerKnobValue;
      delay(100); // the real Garmin is not so responsive!
    }
  }
  long outerKnobValue = outerKnob.read();
  if (outerKnobValue != outerKnobValuePrev) {
    int delta = outerKnobValue - outerKnobValuePrev;
    if (delta != 0 && delta % 2 == 0)  {
      if (delta > 0) {
        chapterDn = 1;
        chapterUp = 0;
      } else if (delta < 0)  {
        chapterUp = 1;
        chapterUp = 0;
      }
      outerKnobValuePrev = outerKnobValue;
      delay(100); // the real Garmin is not so responsive!
    }
  }

  // Command Buttons on the right hand side of the unit
  if (directButton.update()) {
    if (directButton.fallingEdge()) {
      pressDirect = 1;
    } else {
      pressDirect = 0;
    }
  }
  if (menuButton.update()) {
    if (menuButton.fallingEdge()) {
      pressMenu = 1;
    } else {
      pressMenu = 0;
    }
  }
  if (clrButton.update()) {
    if (clrButton.fallingEdge()) {
      pressClr = 1;
    } else {
      pressClr = 0;
    }
  }
  if (entButton.update()) {
    if (entButton.fallingEdge()) {
      pressEnt = 1;
    } else {
      pressEnt = 0;
    }
  }


  // Toggle buttons at the bottom of the screen.  No difference in coding,
  // can ignore the distinction.
  
  if (cdiButton.update()) {
    if (cdiButton.fallingEdge()) {
      toggleCdi = 1;
    } else {
      toggleCdi = 0;
    }
  }
  if (obsButton.update()) {
    if (obsButton.fallingEdge()) {
      toggleObs = 1;
    } else {
      toggleObs = 0;
    }
  }
  if (msgButton.update()) {
    if (msgButton.fallingEdge()) {
      toggleMsg = 1;
    } else {
      toggleMsg = 0;
    }
  }
  if (fplButton.update()) {
    if (fplButton.fallingEdge()) {
      toggleFpl = 1;
    } else {
      toggleFpl = 0;
    }
  }
  if (procButton.update()) {
    if (procButton.fallingEdge()) {
      toggleProc = 1;
    } else {
      toggleProc = 0;
    }
  }

  if (cursorButton.update()) {
    if (cursorButton.fallingEdge()) {
      pressCursor = 1;
    } else {
      pressCursor = 0;
    }
  }

}


