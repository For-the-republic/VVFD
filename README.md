# VVFD
 Variable VFD display driver for any electromagnet based VFD.
 made by Sam Ramsay and Luke Quigley
# PREREQUISTES

## It must be an "E" type VFD

from our research, there are two main types of VFDs. the first type is what we call "E" type. these are VFDs that generate the segment magnetic field with an electromagnet, and therefore require a current across it to work.

The other type is "F" type. these seem to use FET technology to generate the magnetic field for the segments. this creates a bit of a problem in how we sense grid/segment combonations, and so you cant tell if there is one or not. for that reason, it's impossible to automate "F" type VFDs with our method.

## how to tell which type you have
the best way's to tell which type it is are with age and size. most early and high end VFDs are E-type, while cheaper ones tend to be F-type. if its designed for small systems, and wont be that birght you are looking at an F-type, whereas audio amp and car VFDs tend to be E-type.

if you are still unsure, connect up an ammeter across the grid/segment pins and measure the current. if you notice an increase when a segment lights up, its an E-type. if there is no increase, its a F-type.


# how this works
 it detects an increase in current across different pins, and will make a logic table accordingly.
  this is still in development so it might take a while to get up and running.

# setup process and PCB information
the PCB is designed to be cascaded for extra modules to fit your VFD size. each module can have 32 pins, and only requires the cascade/vfd driver components. the original board will still need the drivers, but no other ones will.
## setup information
the two values you will need to change  are:
- PINCOUNT
- CHUNKSIZE
PINCOUNT isthe number of pin slots used, going from the top right of the image down. CHUNKSIZE is the maximum number of segments it will test at once. for safety this is set to 1, but can be set higher for greater speed.

compile this code to your arduino, and have it plugged in to accept serial out. the arduino will print your matrix to the serial out, so its kinda important.

to set the base conditions, the the arduino runs it will go into setup mode for one minute. this gives you time to ajust the variable resistors, and make sure everything is working. **DO NOT CHANGE THE RESISTORS AFTER ONE MINUTE**, or else the ammeters wont have a base value to test off of.
![pic](/Electronics/PCBPic.png)

once this mode is done, just leave it. 
## when it has finished
once it has completed making the pinout table, it will cycle through every valid combonation and send a matrix to the serial out of the arduino.
