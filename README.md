# USB-MIDI-Controller-Arduino
MIDI plug and play Controller made using a SparkFun Pro Micro.

///// FUNCTIONALITY /////
The current functionality is that it will send MIDI note on/off information with the 3 buttons that are attached, and the two potentiometers will handle Control Changes.
By activating the button near the potentiometers, it will treat the buttons as a way to control which knob you control virtually:
  Clicking the left button moves the knobs to the left by 1, clicking the right moves the knobs to the right.
  This will give you control over 16 different knobs when used correctly.
  
///// LIBRARIES /////
MidiUSB.h
MIDI.h

///// EXPERIENCE /////
This was my first ever arduino project, and I am glad that it worked out as well as it did. I used this project as an entry point to other future Arduino projects I have in mind as a MIDI controller seemed simple to program, and it was lol. I learned everything in this project from the ground up, including the Breadboard wiring, Buttons, Potentiometers, Led-emitting-diods, MIDI structure, PCB, etc.

///// ISSUES I RAN INTO /////
Ableton Live will not handle the CC change unless it has "Remote" checked in the MIDI Preferences tab, please do so if you are using Ableton.

///// TODO /////
Create PCB design, create a diagram, add photos etc.
