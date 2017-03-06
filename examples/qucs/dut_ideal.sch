<Qucs Schematic 0.0.19>
<Properties>
  <View=0,0,1068,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=dut_ideal.dat>
  <DataDisplay=dut_ideal.dpl>
  <OpenDisplay=1>
  <Script=dut_ideal.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <Pac P1 1 160 280 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 160 310 0 0 0 0>
  <.SP SP1 1 200 380 0 77 0 0 "lin" 1 "1 GHz" 1 "10 GHz" 1 "2001" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Pac P2 1 740 260 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 740 290 0 0 0 0>
  <Sub SUB1 1 510 230 -20 14 0 0 "dut.sch" 0>
</Components>
<Wires>
  <160 230 160 250 "" 0 0 0 "">
  <160 230 480 230 "" 0 0 0 "">
  <540 230 740 230 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
