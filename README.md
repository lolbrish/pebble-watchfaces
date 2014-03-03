#pebble-watchfaces

Custom watch faces for pebble device with varrying vibration intervals, inspired loosely by <http://skreksto.re/products/durr>

## fuzzy_time_300s

This one is mostly done.

It's a basic fuzzy time watch face: based on the fuzzy time example from the pebble sdk + a very light vibration every 300s (5 minutes)

##fuzzy_time_300s_15m

**This one is still under development!**

Variation 1 on fuzzy_time_300s to remind me to look away from the computer at least once every 15 minutes (because my eye doctor says that's important!) 

As with fuzzy_time_300s, this one gives a light vibration every 5 minutes. 

In addition, every 15 minutes, it gives a stronger pulse. Wearer must tap the watch face to acknowledge this stronger pulse, or it will continue to pulse every 1 minute thereafter until wearer acknowledges with a tap. 

15 minute timer is continually reset by any large movements of the watch -- these are taken to be suggestions that the wearer is not in front of a computer. Also has the side effect of encouraging wrist/arm movement :)

15 minute timer is also continually reset if the watch is completely still -- taken to be a suggestion that no one is wearing the watch at all! 

dots on the right side of the watch indicate the current 