# LPD8-Expander
An application I've made for expanding the number of my Akai LPD8 controller from 32 to 2048, 128 CCs x 16 midi channels.

It works with any controller that has 8 buttons and 8 knobs.

Uses RtMidi library https://github.com/thestk/rtmidi

For info and instructions go to this page:
https://vibersoul.wordpress.com/2017/07/24/lpd8-expander/

# Compile
g++ -O0 -Wall -D__LINUX_ALSA__ -I "/usr/include/rtmidi" main.cpp -o LPD8-Expander -lrtmidi -lasound -lpthread
