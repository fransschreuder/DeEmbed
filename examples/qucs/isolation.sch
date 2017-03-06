<Qucs Schematic 0.0.19>
<Properties>
  <View=0,0,1068,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=isolation.dat>
  <DataDisplay=isolation.dpl>
  <OpenDisplay=1>
  <Script=isolation.m>
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
  <COAX Line1 1 380 230 -26 16 0 0 "2.29" 1 "0.022e-6" 0 "1" 0 "2.95 mm" 0 "0.9 mm" 0 "750 mm" 1 "4e-4" 0 "26.85" 0>
  <Pac P1 1 160 280 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 160 310 0 0 0 0>
  <.SP SP1 1 200 380 0 77 0 0 "lin" 1 "1 GHz" 1 "10 GHz" 1 "2001" 1 "no" 0 "1" 0 "2" 0 "no" 0 "no" 0>
  <Pac P2 1 740 260 18 -26 0 1 "2" 1 "50 Ohm" 1 "0 dBm" 0 "1 GHz" 0 "26.85" 0>
  <GND * 1 740 290 0 0 0 0>
  <COAX Line2 1 660 230 -26 16 0 0 "2.29" 1 "0.022e-6" 0 "1" 0 "2.95 mm" 0 "0.9 mm" 0 "750 mm" 1 "4e-4" 0 "26.85" 0>
  <SUBST Subst1 1 380 420 -30 24 0 0 "4.3" 1 "1.6 mm" 1 "35 um" 1 "2e-4" 1 "0.022e-6" 1 "0.15e-6" 1>
  <MGAP MS1 0 530 230 -26 15 0 0 "Subst1" 1 "3.6 mm" 1 "3.6 mm" 1 "10 mm" 1 "Hammerstad" 0 "Kirschning" 0>
</Components>
<Wires>
  <160 230 350 230 "" 0 0 0 "">
  <160 230 160 250 "" 0 0 0 "">
  <690 230 740 230 "" 0 0 0 "">
  <410 230 500 230 "" 0 0 0 "">
  <560 230 630 230 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
