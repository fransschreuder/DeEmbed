/*
 * DeEmbed
 * Copyright (C) Frans Schreuder 2016 <info@schreuderelectronics.com>
 *
software is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _VNA_MATH_H_
#define _VNA_MATH_H_
#include <complex>
#include "typedefs.h"
#include <QString>
using namespace std;


///! Converts a complex impedance into an S-parameter (normalized to 1)
complex_t ZtoS(complex_t Z);
///! Converts an S-parameter int a complex impedance
complex_t StoZ(complex_t S);
///! Calculates VSWR from an S-parameter
double StoVSWR(complex_t S);
///! Degrees to radians and vise versa
double RadtoDeg(double rad);
double DegtoRad(double deg);

///! Creates a QString from a frequency with correct unit (MHz, GHz, etc)
QString FreqToString(double f, int DecimalPlaces=3);
double StringToFreq(QString freqstring);

///! converts a double (alwayd in dBm) into a string with dBm, W, or mW
QString AmpToString(double amp, AmpType type);
double StringToAmp(const QString &ampstring);

#endif // _VNA_MATH_H_
