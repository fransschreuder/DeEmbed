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

#ifndef CAL_SPAR_H
#define CAL_SPAR_H

#include "typedefs.h"
#include <QString>
#include <QObject>

///! A load is not a real load, neither is a short. This data object contains properties of realistic standards.
typedef struct
{
    QString Name;
	//short
	double L0, L1, L2, L3;
	double LengthShort, LossdBShort, LossdBHzShort;
	//open
	double C0, C1, C2, C3;
	double LengthOpen, LossdBOpen, LossdBHzOpen;
	//load
	double Ll, Rl;
	//through
	double LengthThrough, LossdBThrough, LossdBHzThrough;
	
} tCalStd;

class CalSpar: public QObject
{
    Q_OBJECT
public:
    ///! Executes a Short Open Load (1 port) calibration using a calibration set and cal std.
	vector<complex_t> SOLCal(vector<complex_t>& rawS1P, vector<complex_t>& shortS1P, vector<complex_t>& openS1P, vector<complex_t>& loadS1P, vector<double>& f);	
    ///! Executes a Through (+isolation) calibration on a 2-port trace (S21, S12, etc)
    void ThroughIsolationCal(vector<complex_t> rawS21, vector<complex_t> throughS21, vector<complex_t> isolationS21,vector<complex_t> reflectS21,
                                          vector<complex_t> rawS12, vector<complex_t> throughS12, vector<complex_t> isolationS12,vector<complex_t> reflectS12,
                                          vector<complex_t> rawS11, vector<complex_t> shortS11, vector<complex_t> openS11, vector<complex_t> loadS11,
                                          vector<complex_t> rawS22, vector<complex_t> shortS22, vector<complex_t> openS22, vector<complex_t> loadS22,
                                          vector<double> f,
                                          vector<complex_t>* calS21, vector<complex_t>* calS12, vector<complex_t>* calS11, vector<complex_t>* calS22);
    ///! Cal executes both SOLCal and ThroughIsolationCal on a dataset
	spar_t Cal(spar_t& S, cal_t& cal);
    ///! This overloaded Cal executes SOLCal and ThroughIsolationCal on a single trace with 2 indexes in the matrix.
    vector<complex_t> Cal(spar_t S, int index1, int index2, cal_t& cal);
    ///! Constructor, does nothing.
    CalSpar();
    ///! Returns an empty calibraion set with a given frequency range, this is the ideal / uncalibrated calset.
    static cal_t DefaultCal(double startfreq, double stopfreq, int NOP);
    ///! Executes a spline interpolation on the calibration set in order to match to a certain frequency range.
    static bool TrimCal(cal_t& cal, double startfreq, double stopfreq, int NOP, cal_t *targetCal);
    ///! Just some nice values for a realistic calibration kit if you don't have data for your own.
	static tCalStd DefaultCalStd(void);
    ///! Creates the pointer to the (external) calibration standard (usually in the main window).l
    void SetCalStd(tCalStd* CalStd)
	{
		m_CalStd=CalStd;
	}
    tCalStd *m_CalStd;
private:
    complex_t stdShort(double f);
    complex_t stdOpen(double f);
    complex_t stdLoad(double f);
    complex_t stdThrough(double f);
    void Port1ErrorTerms(complex_t G1, complex_t G2, complex_t G3, complex_t Gm1, complex_t Gm2, complex_t Gm3, complex_t* e00, complex_t* e11, complex_t *de);
signals:
    void CalibrationWarning(QString warning);
};

#endif // CAL_SPAR_H
