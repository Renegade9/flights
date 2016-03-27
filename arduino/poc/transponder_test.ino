/*
   Transponder Test

   Test of reading the binary coded octal pins from the transponder's
   rotary switches and displaying the squawk code.

   Each switch has 3 input pins, a binary coded representation of the
   digit.  Four switches total, so we are using 12 pins and they
   are currently plugged into the Arduino pins 2-13.  Note that pin 13
   is connected to a internal LED on the Arduino Uno, so a pull-up resistor
   was wired for this pin.  The other pins use the internal pull-up capability
   (by setting pin mode to INPUT_PULLUP).
*/

const byte num_digits = 4;
const byte pins_per_digit = 3;

// record pin assignment to the switches
const byte inputPins[num_digits][pins_per_digit] = {
  {2, 3, 4}, // switch one
  {5, 6, 7}, // switch two
  {8, 9, 10}, // switch three
  {11, 12, 13} // switch four
};

byte pinValues[num_digits][pins_per_digit];

String lastSquawk = "";

void setup() {

  Serial.begin(9600);

  // set the pin modes
  for (int digit = 0; digit < num_digits; digit++) {
    for (int bitPosition = 0; bitPosition < pins_per_digit; bitPosition++) {
      pinMode(inputPins[digit][bitPosition], INPUT_PULLUP);
    }
  }

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

void loop() {
  String squawk = read_squawk();
  if (squawk != lastSquawk) {
    Serial.print("Squawk=");
    Serial.println(squawk);
    lastSquawk = squawk;
  }
  delay(200);
}
