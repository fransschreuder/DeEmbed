#DeEmbed

DeEmbed is a GUI application that can read raw S-Parameter files in TouchStone format
There is support for 1 to 4 port s-parameter files.
The s-parameters can be de-embedded using the SOLTI (Short, Open, Load, Through, Isolation) method. 
If you create 1 or 2 port s-parameter files from your cables (through + isolation) and measure 1-port s-parameter files of a short, open and load, the original DUT can be shown in the different graphs as if they were measured without cables.

By default the application shows the data in a smith chart, but different charts are supported (Db, phase, magnitude, VSWR, polar and some combinations).

##Compiling
cd DeEmbed
mkdir build
cd build
qmake ..
make

