# temp_display_7seg
this is a simple project that demonstrates how to drive 7 segment displays and update them smoothly to the human eye with no external driver components.

it reads the temp and relative humidity from a si7021 over i2c then displays the values alternating after some seconds.

its done on a fairly large random pic18f mcu i had, but the key of the trick is using an interrupt to update the segments.

the math to calculate the digits is straightforward and does several modulo, but it does run fast enough (at least at most clock rates) to visibly skip with 
lots of room to spare. this could probably be improved or unrolled a bit too

