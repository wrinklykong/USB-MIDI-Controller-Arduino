#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <frequencyToNote.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>

#define buttonL     4
#define buttonM     3
#define buttonR     2

#define pot1        A0
#define pot2        A1
#define pot3        A2

#define ledL        9
#define ledR        5
#define ledM        6

#define numButtons  3
#define numPots     3

// Potentiometer/Knob variables
int pots[]              = {pot1, pot2, pot3};
int lastValPots[]       = {0,0, 0};

// Button variables
int buttons[]           = {buttonL, buttonM, buttonR};      // button maps to Pro Micro
int curStateButtons[]   = {0,0,0};                          // holds the previous value to not flood the MIDI
int lastStateButtons[]  = {0,0,0};                          // holds the previous value to not flood the MIDI
int buttonMap[]         = {pitchC2, pitchC3, pitchC4};      // Holds note information to be sent (C3,C4,C5 etc)
int curButton           = 0;                                // Helper button from

// button to turn on the number thing
int ccSelected          = 0;          // values between [0,12], used with the select button (12+3 == 15, 16 channels gonna be used lolz :)

//special mode
int modeOn              = 0;
int modeVal             = 0;

//  MIDIUsb library functions that I added a flush to :)
void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

// to avoid notes from lingering while it is shifting between Funky Mode
void destroyNotes() {
  int i = 0;
  for ( i = 0; i < numButtons; i++ ) {
    noteOff(0, buttonMap[i], 0);
  }
}

// functions relating to the "Special Mode". They will
// change the value the knobs are mapped to, range goes from [0,14]
void decrementCC() {
  if ( ccSelected>0 ) {
    ccSelected--;
    //Serial.print("Decreased: ");
    //Serial.println(ccSelected);
    ledFunction();
  }
}
void incrementCC() {
  if ( ccSelected<12 ) {
    ccSelected++;
    //Serial.print("Increased: ");
    //Serial.println(ccSelected);
    ledFunction();
  }
}

// This function reads the button values, and will increase the CC
// if in the "Special Mode", or will just send a Note hit if it is not
void readButtons() {
  
    int i = 0;
    for ( i = 0; i < numButtons; i++ ) {
      curStateButtons[i] = digitalRead(buttons[i]);
      if ( curStateButtons[i] != lastStateButtons[i] ) {
        if ( modeOn == 0 ) {
          curButton = curStateButtons[i];
          // send midi info
          if ( curButton == 1 ) {
            noteOn(0, buttonMap[i], 127);
            MidiUSB.flush();
            //Serial.println("sent note on");
          }
          else {
            noteOff(0, buttonMap[i], 0);
            MidiUSB.flush();
            //Serial.println("OFF!");
          }
        }
      }
    }
    // Holding down the L and R buttons will activate the "Special Mode"
    if ( ( curStateButtons[0] == 1 && curStateButtons[2] == 1 ) ) {
      modeVal++;
      if ( modeVal > 254 ) {
        destroyNotes();
        modeVal = 0;
        // boolean issues, had to do it this way
        if ( modeOn == 0 ) {
          modeOn = 1;
          analogWrite(ledM, 100);
          ledFunction();
        }
        else {
          analogWrite(ledM, 0);
          analogWrite(ledL, 0);
          analogWrite(ledR, 0);
        }
      }
    }
    // --> funky mode is NOT TRYING TO be engaged
    else {
      modeVal = 0;
      // if mode is on, change based on button presses, otherwise do the thingy magizgys
      if ( modeOn == 1 ) {
        if ( ( curStateButtons[0] == 1 ) && ( lastStateButtons[0] == 0 ) ) {
          decrementCC();
        }
        if ( ( curStateButtons[2] == 1 ) && ( lastStateButtons[2] == 0 ) ) {
          incrementCC();
        }
      }
    }
    for ( i = 0; i < numButtons; i++ ) {
      lastStateButtons[i] = curStateButtons[i];
    }
}

// This reads the Potentiometers and sends MIDI information from its values.
// It will send to the specific chosen channel.
void readPots() {
  int i;
  int tmp = 0;
  int diff = 0;
  int help = 0;
  for ( i = 0; i < numPots; i++ ) {
    tmp = analogRead(pots[i]);
    if ( lastValPots[i] != tmp ) {
      diff = lastValPots[i] - tmp;
      if ( diff <= -6 || diff >= 6 ) {                       // Avoidance of miniscule movement, will only change when it goes 2 up, to limit noise
        // Serial.println(tmp);
        // send MIDI note information
        controlChange(1,i+ccSelected, (1023-tmp)>>3);               // Bitwise operation to divide by 8, 1023->127 max for MIDI Compliance
        //Serial.println(diff);
        MidiUSB.flush();
        //Serial.print("Send pot update:");
        //Serial.println(tmp);
        lastValPots[i] = tmp;                                // only updates when there is a change :)
      }
    }
  }                                               
}

// Changes the LED brightness depending on certain circumstances
//    Blue LEDs will turn on when it is in "Special Mode", and will
//    denote the edges of the available channels you can choose.
void ledFunction() {
  if ( ccSelected < 12 ) {
    analogWrite(ledR, 100);
  }   
  if ( ccSelected > 0 ) {
    analogWrite(ledL, 100);
  }
  if ( ccSelected == 0 ) {
    analogWrite(ledL, 0);           // edge case, if on first channel left LED no longer shines
  }
  if ( ccSelected == 12 ) {         // edge case, if on last channel right LED no longer shines
    analogWrite(ledR, 0);
  } 
}

void setup() {

  int i = 0;
  for ( i = 0; i < numButtons; i++ ) {
    pinMode(buttons[i],INPUT);       // Since this is a digital pin, set all buttons to be input
  }
  analogWrite(ledM, 35);

}

void loop() {

  readPots();
  readButtons();
  delay(2);           // for processor :)

}
