Document: DeEmbed
Title: Debian DeEmbed Manual
Author: Frans Schreuder
Abstract: DeEmbed

DeEmbed is a GUI application that can read raw S-Parameter files in TouchStone format There is support for 1 to 4 port s-parameter files. The s-parameters can be de-embedded using the SOLTI (Short, Open, Load, Through, Isolation) method. If you create 1 or 2 port s-parameter files from your cables (through + isolation) and measure 1-port s-parameter files of a short, open and load, the original DUT can be shown in the different graphs as if they were measured without cables.

By default the application shows the data in a smith chart, but different charts are supported (Db, phase, magnitude, VSWR, polar and some combinations).

The application automatically interpolates (spline) the calibration sets to the desired touchstone file. Note that the frequency range of the first touchstone file is leading, any file loaded afterwards will also be interpolated.
Section: Features

Show calibrated and uncalibrated S-parameters in a smith chart

Also other charts (like dB/Phase) are supported.

The calibration sets can be loaded as touchstone files. For two-port standards (Through / Isolation) either one (s1p) or two (s2p) port s-parameter files can be used. For s1p files S11 will be used as through/isolation standard, for s2p files S21 and S12 will be used.

The properties of the calibration standards can be modified, even after loading the data. The calibration standard is live applied to the graph while the dialog is still open, this way it is easy to find / fit the standard if the DUT is known.

The chart can be navigated using the mouse wheel:

    Scrolling zooms the smith chart and centers
    Scrolling moves a cartesian chart
    Ctrl+scrolling zooms a cartesian chart
    Shift+scrolling moves the right cartesian axis
    Shift+Ctrl+scrolling zooms the right cartesian axis
    A marker can be set by right clicking near to one of the traces.

Section: unknown

Format: debiandoc-sgml
Files: /usr/share/doc/DeEmbed/DeEmbed.sgml.gz

Format: postscript
Files: /usr/share/doc/DeEmbed/DeEmbed.ps.gz

Format: text
Files: /usr/share/doc/DeEmbed/DeEmbed.text.gz

Format: HTML
Index: /usr/share/doc/DeEmbed/html/index.html
Files: /usr/share/doc/DeEmbed/html/*.html
