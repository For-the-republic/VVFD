# VVFD
 Variable VFD display driver for any electromagnet based VFD.
 made by Sam Ramsay and Luke Quigley
# PREREQUISTES

## It must have a leakage current
VFDs are funny little things, as even though the have vacumn in the name, most are not a pure vacumn. instead they have a small amount of gas in them. this is only true for larger ( or more delacite ones), and is the main operation which we are measuring to find the anode connections.

## how to tell if you have a leakage current
the best way to tell if it has a leakage current is by the datasheet. if it has **both** a current draw on the segment and grid, you have a leakage. 

the next best way to tell is by the size of the VFD. smaller VFDs tend to be pure vacumns, and dont have a leakage current. if it is a larger (or older) VFD like those found in audio amplifiers, car radios ect, it most likely has a leakage current.

the third and final way to tell is to manually check. if there is a leakage current, **there will be a small current increase when a valid grid/segment combination is found**. this is by far the best way to tell, however can be quite finnicky and so we reccomend the other ways first.


# how this works
 it detects a leakage in current across different pins, and will make an anode connection table accordingly.
  this is still in development so it might take a while to get up and running.

# setup process and PCB information
the PCB is designed to be cascaded for extra modules to fit your VFD size. each module can have 32 pins, and only requires the cascade/vfd driver components. the original board will still need the drivers, but no other ones will.
## setup information
the two values you will need to change  are:
- PINCOUNT
PINCOUNT is the number of shift registers used, starting from the main module and cascading to the right. we need to know this so we can properly push the right bits.

compile this code to your arduino, and have it plugged in to accept serial out. the arduino will print your matrix to the serial out, so its kinda important.

to set the base conditions, the the arduino runs it will go into setup mode for one minute. this gives you time to ajust the variable resistors, and make sure everything is working. **DO NOT CHANGE THE RESISTORS AFTER ONE MINUTE**, or else the ammeters wont have a base value to test off of.
![pic](/Electronics/PCBPic.png)

once this mode is done, just leave it. 
## when it has finished
once it has completed making the pinout table, it will cycle through every valid combonation and send a matrix to the serial out of the arduino.


## a note about pinouts
**if in doubt, use the pinout schematic/PCB provided**
- the AC generator pins must be both PWN pins
- the ammeter pins must be analouge
- all other pins should combos should be fine
