#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <frequencyToNote.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>

#define button1     2
#define button2     3
#define button3     4
#define sbutton     5

#define pot1        A0
#define pot2        A1

#define led         9

#define numButtons  3
#define numPots     2

// Potentiometer/Knob variables
int potVal              = 0;
int pots[]              = {pot1, pot2};
int potMap[]            = {0x01, 0x02, 0x03};
int lastValPots[]       = {0,0};

// Button variables
int buttons[]           = {button1, button2, button3};      // button maps to Pro Micro
int lastStateButtons[]  = {0,0,0};                          // holds the previous value to not flood the MIDI
int buttonMap[]         = {pitchC2, pitchC3, pitchC4};      // Holds note information to be sent (C3,C4,C5 etc)
int buttonState         = 0;                                // current state of button being checked
// button to turn on the number thing
int selectButton        = sbutton;
int prevSelectButtonVal = 0;
int curSelectButtonVal  = 0;
int selected            = 0;          // 0 is false, 1 is true :)
int ccSelected          = 0;          // values between [0,12], used with the select button (12+3 == 15, 16 channels gonna be used lolz :)

void decrementCC() {
  if ( ccSelected>0 ) {
    ccSelected--;
    Serial.print("Decreased: ");
    Serial.println(ccSelected);
  }
}
void incrementCC() {
  if ( ccSelected<12 ) {
    ccSelected++;
    Serial.print("Increased: ");
    Serial.println(ccSelected);
  }
}

void readButtons() {

  curSelectButtonVal = digitalRead(sbutton);
  if ( curSelectButtonVal != prevSelectButtonVal && curSelectButtonVal) {
    selected = !selected;                             // bit inversion, flips the switch
  }
  prevSelectButtonVal = curSelectButtonVal;
  if ( selected ) {                                   // then we treat buttons as a selector for knobs
    digitalWrite(led, HIGH);
    // check left button, if its selected then decrement the CC
    buttonState = digitalRead(buttons[0]);            // left select
    if ( buttonState != lastStateButtons[0] && buttonState ) {        // only updates when it changes, dec the cc
      decrementCC();
    }
    lastStateButtons[0] = buttonState;
    // check right button, if its selected then increase the CC
    buttonState = digitalRead(buttons[2]);            // left select
    if ( buttonState != lastStateButtons[2] && buttonState ) {        // only updates when it changes, inc the cc
      incrementCC();
    }
    lastStateButtons[2] = buttonState;

  }
  else {                                              // we treat them like buttons :)
    digitalWrite(led, LOW);
    int i = 0;
    for ( i = 0; i < numButtons; i++ ) {
      buttonState = digitalRead(buttons[i]);
      if ( buttonState != lastStateButtons[i] ) {
        Serial.println(buttonState);
        // send midi info
        if ( buttonState == 1 ) {
          noteOn(0, buttonMap[i], 127);
          MidiUSB.flush();
          Serial.println("sent note on");
        }
        else {
          noteOff(0, buttonMap[i], 0);
          MidiUSB.flush();
          Serial.println("OFF!");
        }
      }
      lastStateButtons[i] = buttonState;
    }

  }

}


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

void readPots() {
  int i = 0;
  for ( i = 0; i < numPots; i++ ) {
    potVal = analogRead(pots[i]);
    potVal = potVal >> 3;                               // 128 is still possible, may have to change, if != 0, sub 1? maybe lol
    if ( potVal != lastValPots[i] ) {
      controlChange(1,i+ccSelected,potVal);
      MidiUSB.flush();
      Serial.print("Send pot update:");
      Serial.println(potVal);
    }
    lastValPots[i] = potVal;
  }
}

void setup() {

  int i = 0;
  for ( i = 0; i < numButtons; i++ ) {
    pinMode(buttons[i],INPUT);       // Since this is a digital pin, set all buttons to be input
  }
  pinMode(sbutton,INPUT);
  pinMode(led, OUTPUT);

}

void loop() {

  readPots();
  readButtons();
  delay(2);           // cuz of the MhZ or whatever manE IDK !!! LOL

}
