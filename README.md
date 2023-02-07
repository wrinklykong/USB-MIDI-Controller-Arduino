# USB-MIDI-Controller-Arduino
MIDI plug and play Controller made using a SparkFun Pro Micro.

///// FUNCTIONALITY /////

The 3 Potentiometers at the top map to MIDI Channels N through N+2 (Dependant on Channel Select Value). The three buttons map to C2, C3, C4 on the Piano Roll.
By holding down the L and R buttons, Knob Selection Mode is engaged allowing the user to move left or right to increase the Channel Select Value.
  
///// LIBRARIES /////

MidiUSB.h
MIDI.h

///// EXPERIENCE /////

This was my first ever arduino project, and I am glad that it worked out as well as it did. I used this project as an entry point to other future Arduino projects I have in mind as a MIDI controller seemed simple to program, and it was lol. I learned everything in this project from the ground up, including the Breadboard wiring, Buttons, Potentiometers, Led-emitting-diods, MIDI structure, PCB, etc.

///// ISSUES I RAN INTO /////

Ableton Live will not handle the CC change unless it has "Remote" checked in the MIDI Preferences tab, please do so if you are using Ableton. LEDs can be extremely bright if you connect them to a Digital Channel. To avoid this, I suggest connecting them to a PWM-able channel to avoid the LEDs from blinding you during use.

///// TODO /////

Create PCB design, create a diagram, add photos etc.
