# College of the Holy Cross: Cosmic Ray Telescope Programs for Research
I have decided to make these programs open-source because others could be interested in my work and this research.

# AnalyzeCRData GUI.py
This program serves several data anlysis purposes. Most importantly, it is used for processing the hexadecimal output from telescope recordings. Data collection with the cosmic ray telescopes is time-sensitive because detector dead-time affects our results, so it is better to process the data afterwards. This program serves as a combination of many features that could be separate programs. See documentation for more details.

External libraries can be installed using the pip command-line installer that comes with the Python installation:

$ pip install openpyxl

$ pip install PySimpleGUI

# CRTLfinal.ino
This sketch was written to let the Arduino Leonardo handle all circuit board processing. Links to the libraries that were used are inside of the sketch commenting. For this code to work with the Leonardo, we made several wiring changes to the Sparkfun SD shield. 
