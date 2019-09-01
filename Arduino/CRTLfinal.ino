/*
-----------------------------
CRTelescopeLoggerLeo.ino
Author: A.Murphy
Email: almurp20@g.holycross.edu
Updated: 6/26/19
GPS Data Logger for Cosmic Ray Telescopes

This program is intended to be used for an Arduino Leonardo connected
to the electronics board of cosmic ray detection telescopes 
at the College of the Holy Cross. 
We do not use the Arduino's built-in timing system because it
is not precise enough for our application. Also an RTC clock
has to occasionally be re-synchronized (is off about 2-3 seconds
per day) and does not come with the added capability to measure
lattitude and longitude. Instead, we use the GPS Pulse-Per-Second
(PPS) output to synchronize clocks on our electronics boards.

This program was made possible through the open-source work
of others. Bill Greiman wrote the SDFat library available here:
https://github.com/greiman/SdFat
that allows this program to write to the SD shield. We use his custom
version for optimizations.

Nick Gammon edited the ReceiveOnlySoftwareSerial file
this program uses to only listen to the GPS without
(pointlessly) reserving a pin for tx (data coming from the GPS):
https://forum.arduino.cc/index.php?topic=112013.0

PIN USAGE:
D0 and D1 are intentionally not used because they could
conflict with serial data coming from a computer device. 
These pins should be the last pins that are used, if they
are needed.
  
D2 - D9 are 8 pins used for clock binary inputs.
D10 is used by the SD chipselect, but SD shield needs special soldering  
D11 is now used for GPS TX to be compatible with the Leonardo
D12 - 13 are NOT used, but lines on the SD shield need to be scratched out to use them.

A0 is for reading trigger signal from clocks.
A1 is now used for a button that saves all of the data on the SD card.
A2 is used for GPS PPS
A3 and A4 are for dataselect lines to read from clocks
A5 is an output held at 5V for the low-to-clear signal for clocks

MAINTENANCE:
If testing without the GPS shield, comment out processGPSString() and 
other GPS functions otherwise the code will wait indefinitely for signals
to arrive from the GPS!

If the SD card is plugged in to the Arduino while it has power, 
the reset button on the microcontroller must be pressed!

The Leonardo reset button does not properly cause the program to restart.
-----------------------------
*/
#include <ReceiveOnlySoftwareSerial.h>
#include "SdFat.h"
#define fileBase "F"
String fileEnd = ".txt";
char fileName[10]= fileBase "0.txt"; // Six or less chars before file extension allowed
const uint8_t chipSelect = 10;
#define error(msg) sd.errorHalt(F(msg)) // Error messages stored in Flash,
                                        // do not delete that line, but do not use the error
                                        // functions because they use D0 and D1 for IO.
SdFat sd;
SdFile file;

ReceiveOnlySoftwareSerial gpsserial(11); // GPS UART Serial object reference

// Declare all of these variables.
char printBuffer[63]; // Create array that is more than long enough to hold the GPS string
int readPins[8] = {2, 3, 4, 5, 6, 7, 8, 9}; // Array of pins to use as inputs for clocks
byte value = 0;
word halfString1 = 0;
word halfString2 = 0;
int fileLength = 5000; // Is frequently updated.

// Wait for a GPS string that starts with the expected sequence:
// $ G P R M C
// Assumes serial connection with GPS has already been initiated.
void waitGPRMC(ReceiveOnlySoftwareSerial &input) {
  char startString[7] = {'$', 'G', 'P', 'R', 'M', 'C', '\0'};
  char checkString[7];
  checkString[6] = '\0';
  int numMatched = 0;
  while (numMatched < 6) {
    // Wait for serial data
    while(!input.available());
    checkString[numMatched] = input.read();
    if (checkString[numMatched] != startString[numMatched]) {
      numMatched = 0;
      memset(checkString, '0', numMatched);
    } else {
      numMatched++;
    } // end else
  } // end while
} // end waitGPRMC

// Function to quickly read from 8 clock inputs, assuming pin 1 is the most significant bit
byte read8Pins(int readPins[]) {
  byte btes = 0;
  // For the following hypothetical binary number:
  // 10010100
  // The '1' on the left is from digitalRead(pin 2), while the '0' on the right would be from pin 9.
  for (int n = 7; n >= 0; n--) {
    btes |= (digitalRead(readPins[7 - n]) << n);
  }
  return btes;
}

// Uses global char array printBuffer[]
// Note that this program has to get the time from the clocks
// even though there wasn't a detection
// that way the gps string can be compared to the 
void processGPSString(ReceiveOnlySoftwareSerial &input) {
  memset(printBuffer, "0", sizeof(printBuffer)); // "Initialize" the printbuffer to several 0's
  input.begin(9600); // GPS's only Baud Rate.  
  waitGPRMC(input); // Wait for a string starting with GPRMC
  for (int i = 0; i < 62; i++) {
    while((!input.available()));
    printBuffer[i] = input.read();
  }
  input.end(); // Saves power?
  printBuffer[sizeof(printBuffer) - 1] = '\0'; // Try null-terminating the GPS string buffer
                                               // to stop random program crashes. 
  while(file.print(printBuffer) == -1); // Write the GPS string to the SD card.                                                                                               
}

// Assumes file has been opened, and that it will be
// closed/saved afterwards when needed. Assumes that variables:
// halfString1, halfString2, and values
// have already been declared and initialized.
void processTimeStamp() {
  value = 0;
  halfString1 = 0;
  halfString2 = 0;
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  //delay(1);
  value = read8Pins(readPins);
  halfString1 = halfString1 + (value << 8);
  
  digitalWrite(A3, HIGH);
  digitalWrite(A4, LOW);
  //delay(1);
  value = read8Pins(readPins);
  halfString1 = halfString1 + value;
  
  digitalWrite(A3, LOW);
  digitalWrite(A4, HIGH);
  //delay(1);
  value = read8Pins(readPins);
  halfString2 = halfString2 + (value << 8);

  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  //delay(1);
  value = read8Pins(readPins);
  halfString2 = halfString2 + value;
  digitalWrite(A3, LOW); // HAVE TO BRING THESE BACK TO LOW AT THE END
  digitalWrite(A4, LOW); // HAVE TO BRING THESE BACK TO LOW AT THE END
  
  // These string conversion lines are a workaround for file.print()
  // truncating the leading zeros in each word!
  // There is no formatted printing with the file class.
  // By the way these take about 0.0 - 0.2ms
  String bytevalues = String(halfString1, HEX);
  String bytevalues1 = String(halfString2, HEX);
  while (bytevalues.length() < 4) {
    String zero = String("0");
    while(!zero.concat(bytevalues));
    bytevalues = String(zero);
  }
  
  while (bytevalues1.length() < 4) {
    String zero1 = String("0");
    while(!zero1.concat(bytevalues1));
    bytevalues1 = String(zero1);
  }
  
  // Print hex data to the file. We might as well use while loops in case each function fails.
  // Functions are likely to fail when the sd is not given enough current, voltage, 
  // or too high of a high speed is used.
  while (file.print(bytevalues) == -1);
  while (file.println(bytevalues1) == -1);
  // Send clear pulse
  digitalWrite(A5, LOW);
  digitalWrite(A5, HIGH);  
}
/*
 * ███████████████████████████████████████
 *                          MAIN PROGRAM                     
 * ███████████████████████████████████████                          
 */
void setup() {
  if (!sd.begin(chipSelect, SD_SCK_MHZ(5))) {
    // The Arduino board has to be reset to properly call this function again (tested!)
    // So, we infinitely wait (or flash an LED if the user should know).
    while(true);
  }
  delay(1000); // Add stability. Online users say SD card needs time to initialize after the call.

  // Pin setup
  pinMode(A3, OUTPUT);    // Outputs dataselect0 for reading from clocks
  pinMode(A4, OUTPUT);    // Outputs dataselect1 for reading from clocks
  pinMode(A5, OUTPUT);    // Outputs low-to-clear pulse for clocks. This pin is kept high at 5V
  digitalWrite(A3, LOW);  // These lines start low
  digitalWrite(A4, LOW);  // These lines start low
  digitalWrite(A5, HIGH); // Keep the clear signal high for now
}

void loop() {
  while (analogRead(A2) < 400); // Wait for 3.3V logic PPS signal to start updating clocks.
                                // The maximum value is 1024 for 5V logic.
                                // the maximum value is then 675 for 3.3V logic.

  // Find an unused file name.
  // NOTE: This process WILL take longer for filenames with larger n.
  // file.open accepts character array filename as well, but it is easier
  // to concatenate strings than assign varying characters.
  int n = 0;
  while (sd.exists(fileName)) {
    String temp = fileBase;
    temp.concat(n);
    temp.concat(fileEnd);
    temp.toCharArray(fileName, sizeof(fileName));
    n += 1;
  }
  
  // These "file" functions return 0 or -1 when they do not work, 
  // so we use while loops to wait for them to work.
  // Otherwise we just throw detections away whenever the SD makes mistakes,
  // instead of letting file write errors become a part of the dead time.
  // Create the file whose name is the unused file name.
  while (!file.open(fileName, O_CREAT | O_APPEND | O_WRITE));
  processGPSString(gpsserial);
  processTimeStamp();
  
  // Read all 8 pins four times, shifting bits appropriately
  // This assumes the first bit read from pin 1 is the most significant bit.
  // andsa the first 8 bits read are all more significant than subsequent 8-pin reads.
  // Small delays added to let the clocks update.
  // If we don't bitshift into larger word variables, simply printing the bytes themselves
  // is NOT equivalent.
  for (int i = 0; i < fileLength; i++) { 
    // analogRead() consumes 100microseconds, or happens 10,000 times a second. 
    // digitalRead() consumes only 4microseconds, but this will not compile anymore.
    while (analogRead(A0) < 266) {
      if (analogRead(A1) > 700) {
        processGPSString(gpsserial);
        processTimeStamp();
        while(file.close() == -1);
        while(true);
      }
    }
    processTimeStamp();
  } // end for(i) loop for file
  while(file.close() == -1);
} // end void loop
