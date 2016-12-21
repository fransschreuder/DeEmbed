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

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

//#define CLI_ONLY

#include <complex>
#include <vector>
using namespace std;
typedef complex<double> complex_t;

///! Contains the S-parameter matrix and the frequency range
typedef  struct{
    vector<vector<vector<complex_t> > > S;
    vector<double> f;
}	spar_t;

///! Contains calibration (SOLTI) data.
typedef struct{
    vector<complex_t> P1Short;
    vector<complex_t> P1Open;
    vector<complex_t> P1Load;
    vector<complex_t> P2Short;
    vector<complex_t> P2Open;
    vector<complex_t> P2Load;
    vector<complex_t> P3Short;
    vector<complex_t> P3Open;
    vector<complex_t> P3Load;
    vector<complex_t> P4Short;
    vector<complex_t> P4Open;
    vector<complex_t> P4Load;
    vector<complex_t> Through12;
    vector<complex_t> Isolation12;
    vector<complex_t> Through13;
    vector<complex_t> Isolation13;
    vector<complex_t> Through14;
    vector<complex_t> Isolation14;
    vector<complex_t> Through21;
    vector<complex_t> Isolation21;
    vector<complex_t> Through23;
    vector<complex_t> Isolation23;
    vector<complex_t> Through24;
    vector<complex_t> Isolation24;
    vector<complex_t> Through31;
    vector<complex_t> Isolation31;
    vector<complex_t> Through32;
    vector<complex_t> Isolation32;
    vector<complex_t> Through34;
    vector<complex_t> Isolation34;
    vector<complex_t> Through41;
    vector<complex_t> Isolation41;
    vector<complex_t> Through42;
    vector<complex_t> Isolation42;
    vector<complex_t> Through43;
    vector<complex_t> Isolation43;

    vector<double> f;
    bool P1ShortDone;
    bool P1OpenDone;
    bool P1LoadDone;
    bool P2ShortDone;
    bool P2OpenDone;
    bool P2LoadDone;
    bool P3ShortDone;
    bool P3OpenDone;
    bool P3LoadDone;
    bool P4ShortDone;
    bool P4OpenDone;
    bool P4LoadDone;
    bool P12ThroughDone;
    bool P13ThroughDone;
    bool P14ThroughDone;
    bool P23ThroughDone;
    bool P24ThroughDone;
    bool P34ThroughDone;
    bool P12IsolationDone;
    bool P13IsolationDone;
    bool P14IsolationDone;
    bool P23IsolationDone;
    bool P24IsolationDone;
    bool P34IsolationDone;
}cal_t;

///! Strings for the names of the s-parameters
const char SparameterNames[4][4][4]=
{
    {"S11","S12","S13","S14"},
    {"S21","S22","S23","S24"},
    {"S31","S32","S33","S34"},
    {"S41","S42","S43","S44"}
};

///! Enum for the different types that the Chart can plot
typedef enum {DB_PLOT,PHASE_PLOT,DBPHASE_PLOT,MAG_PLOT,MAGPHASE_PLOT,VSWR_PLOT,SMITH_PLOT,POLAR_PLOT}SparType;

///! Amplitude can be in dBm, W or mW
typedef enum
{
    dBm, W, mW
}AmpType;

#endif //TYPEDEFS_H
