# College of the Holy Cross: Cosmic Ray Telescope Programs for Research
I have decided to make these programs open-source because I hope they may be useful to others; it is possible that others could be interested in this research.

These programs are not under active development; I am no longer conducting this research at the College of the Holy Cross. However, I think others will continue to expand upon this research in following years. Therefore, please address any concerns related to this research or its programs to our faculty advisor for this research, Prof. Tomohiko Narita tnarita@holycross.edu

# AnalyzeCRData Gui.py
This program serves several data anlysis purposes. Most importantly, it is used for processing the hexadecimal output from telescope recordings. Data collection with the cosmic ray telescopes is time-sensitive because detector dead-time affects our results, so it is better to process the data afterwards. This program serves as a combination of many features that could be separate programs. See documentation for more details.

External libraries can be installed using the pip command-line installer:
$pip install openpyxl
$pip install PySimpleGUI

# CRTLfinal.ino
This sketch was written to let the Arduino handle all circuit board processing. Links to libraries that were used are inside of the sketch commenting.
