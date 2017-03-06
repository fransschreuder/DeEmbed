<Qucs Schematic 0.0.19>
<Properties>
  <View=0,0,916,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=dut.dat>
  <DataDisplay=dut.dpl>
  <OpenDisplay=1>
  <Script=dut.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.ID -20 14 SUB>
  <.PortSym -30 0 1 0>
  <.PortSym 30 0 2 0>
  <Line -20 -10 40 0 #000080 2 1>
  <Line 20 -10 0 20 #000080 2 1>
  <Line -20 10 40 0 #000080 2 1>
  <Line -20 -10 0 20 #000080 2 1>
  <Line -30 0 10 0 #000080 2 1>
  <Line 20 0 10 0 #000080 2 1>
</Symbol>
<Components>
  <L L1 0 500 490 17 -26 0 1 "770.7pH" 1 "" 0>
  <GND * 0 500 520 0 0 0 0>
  <C C1 0 570 410 -27 10 0 0 "603.2fF" 1 "" 0 "neutral" 0>
  <L L2 0 640 490 17 -26 0 1 "548.3pH" 1 "" 0>
  <GND * 0 640 520 0 0 0 0>
  <C C2 0 710 410 -27 10 0 0 "603.2fF" 1 "" 0 "neutral" 0>
  <L L3 0 780 490 17 -26 0 1 "770.7pH" 1 "" 0>
  <GND * 0 780 520 0 0 0 0>
  <Amp X1 1 580 240 -26 27 0 0 "10" 1 "49 Ohm" 0 "51 Ohm" 0 "0 dB" 0>
  <Port P3 1 430 240 -23 12 0 0 "1" 1 "analog" 0>
  <Port P2 1 770 240 4 -58 0 2 "2" 1 "analog" 0>
  <C C3 1 570 140 -27 10 0 0 "133.2fF" 1 "" 0 "neutral" 0>
</Components>
<Wires>
  <500 410 500 460 "" 0 0 0 "">
  <640 410 640 460 "" 0 0 0 "">
  <780 410 780 460 "" 0 0 0 "">
  <500 410 540 410 "" 0 0 0 "">
  <600 410 640 410 "" 0 0 0 "">
  <640 410 680 410 "" 0 0 0 "">
  <740 410 780 410 "" 0 0 0 "">
  <430 240 520 240 "" 0 0 0 "">
  <610 240 670 240 "" 0 0 0 "">
  <520 140 540 140 "" 0 0 0 "">
  <520 240 550 240 "" 0 0 0 "">
  <520 140 520 240 "" 0 0 0 "">
  <600 140 670 140 "" 0 0 0 "">
  <670 240 770 240 "" 0 0 0 "">
  <670 140 670 240 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 540 570 12 #000000 0 "Chebyshev high-pass filter \n 5GHz cutoff, pi-type, \n impedance matching 50 Ohm">
</Paintings>
