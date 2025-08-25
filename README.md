# VVFD
 Variable VFD display driver for any electromagnet based VFD.
**PREREQUISTES**
the VFD must use have a current going across the pixel pins and the grid pins. from our research smaller and cheaper VFDs do not do this, instead they rely on FET tech to generate a magnetic field.


 **HOW THIS WORKS**
 it detects an increase in current across different pins, and will make a logic table accordingly. this is still in development so it might take a while to get up and running.