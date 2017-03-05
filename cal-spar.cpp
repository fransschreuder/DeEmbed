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

#include "cal-spar.h"
#include <iostream>
#include "typedefs.h"
#include "spline.h"
#include "vna-math.h"
#include <QDebug>
using namespace std;

#define METHOD_AGILENT

CalSpar::CalSpar()
{
}

complex_t CalSpar::stdShort(double f)
{
    // - - - - - - - - - - model for short - - - - - - - - - - - - - -
    double L0=m_CalStd->L0;
    double L1=m_CalStd->L1;
    double L2=m_CalStd->L2;
    double L3=m_CalStd->L3;
    double LengthShort=m_CalStd->LengthShort; //Electrical length in m
    double LossdBShort=m_CalStd->LossdBShort;
    double LossdBHzShort=m_CalStd->LossdBHzShort;
    double c = 299792458;
    complex_t Zsp=complex_t(0,2*M_PI*f*(L0+L1*f+L2*pow(f,2)+L3*pow(f,3)));
    complex_t Gsp=ZtoS(Zsp/complex_t(50,0));
    double wl=c/f;
    Gsp = Gsp * exp(complex_t(0,2*M_PI*(LengthShort/wl))); //apply electrical length
    Gsp = Gsp * pow(10,-1*LossdBShort/20) * pow(10,(-1*LossdBHzShort*f)/20); //apply Loss (dB) and Loss (dB/Hz)
    return Gsp;
}

complex_t CalSpar::stdOpen(double f)
{
    // - - - - - - - - - - model for open
    double C0=m_CalStd->C0;
    double C1=m_CalStd->C1;
    double C2=m_CalStd->C2;
    double C3=m_CalStd->C3;
    double LengthOpen=m_CalStd->LengthOpen; //Electrical length in m
    double LossdBOpen=m_CalStd->LossdBOpen;
    double LossdBHzOpen=m_CalStd->LossdBHzOpen;
    double c = 299792458;
    double wl=c/f;
    complex_t Zop=complex_t(0,-1)/	complex_t(2*M_PI*f*(C0+C1*f+C2*pow(f,2)+C3*pow(f,3)),0);// impedance for open
    complex_t Gop=ZtoS(Zop/complex_t(50,0));
    if(2*M_PI*f*(C0+C1*f+C2*pow(f,2)+C3*pow(f,3))==0)
        Gop=complex_t(1,0);
    Gop = Gop * exp(complex_t(0,2*M_PI*(LengthOpen/wl))); //apply electrical length
    Gop = Gop * pow(10,-1*LossdBOpen/20) * pow(10,(-1*LossdBHzOpen*f)/20); //apply Loss (dB) and Loss (dB/Hz)
    //Gop = complex_t(1,0);
    return Gop;
}

complex_t CalSpar::stdLoad(double f)
{
    //- - - - - - - - - - model for load - - - - - - - - - - - - - - -
    double Ll=m_CalStd->Ll;
    double Rl=m_CalStd->Rl;
    complex_t Zlp=complex_t(Rl,2*M_PI*f*Ll);
    complex_t Glp=ZtoS(Zlp/complex_t(50,0));
    //Glp = complex_t(0,0);
    return Glp;
}

complex_t CalSpar::stdThrough(double f)
{
    double LengthThrough=m_CalStd->LengthThrough;
    double LossdBThrough=m_CalStd->LossdBThrough;
    double LossdBHzThrough=m_CalStd->LossdBHzThrough;
    double c = 299792458;
    double wl=c/f;
    complex_t Gth(1,0);
    Gth = Gth * exp(complex_t(0,2*M_PI*(LengthThrough/wl))); //apply electrical length
    Gth = Gth * pow(10,-1*LossdBThrough/20) * pow(10,(-1*LossdBHzThrough*f)/20); //apply Loss (dB) and Loss (dB/Hz)
    return Gth;
}

void CalSpar::Port1ErrorTerms(complex_t G1, complex_t G2, complex_t G3, complex_t Gm1, complex_t Gm2, complex_t Gm3, complex_t* e00, complex_t* e11, complex_t *de)
{
    /**
     * Method from Agilent: http://www2.electron.frba.utn.edu.ar/~jcecconi/Bibliografia/04%20-%20Param_S_y_VNA/Network_Analyzer_Error_Models_and_Calibration_Methods.pdf
     * Gm1==e00+G1*Gm1*e11-G1*de
     * Gm2==e00+G2*Gm2*e11-G2*de
     * Gm3==e00+G3*Gm3*e11-G3*de
     * Inverting the 3 equations below gives the resulting equations for e00, e11 and de below.
     * solve([Gm1==e00+G1*Gm1*e11-G1*de, Gm2==e00+G2*Gm2*e11-G2*de, Gm3==e00+G3*Gm3*e11-G3*de], e00, e11, de)
     * e00 = ((G3*Gm2 - G3*Gm3)*G2*Gm1 - (G2*Gm2*Gm3 - G3*Gm2*Gm3 + (G3*Gm2 - G2*Gm3)*Gm1)*G1)/(((G2 - G3)*Gm1 - G2*Gm2 + G3*Gm3)*G1 + (G3*Gm2 - G3*Gm3)*G2)
     * e11 = ((G2 - G3)*Gm1 - G1*(Gm2 - Gm3) + G3*Gm2 - G2*Gm3)/(((G2 - G3)*Gm1 - G2*Gm2 + G3*Gm3)*G1 + (G3*Gm2 - G3*Gm3)*G2)
     * de = -(G1*Gm1*(Gm2 - Gm3) + G2*Gm2*Gm3 - G3*Gm2*Gm3 - (G2*Gm2 - G3*Gm3)*Gm1)/(((G2 - G3)*Gm1 - G2*Gm2 + G3*Gm3)*G1 + (G3*Gm2 - G3*Gm3)*G2)
     **/

    ///Directivity
    *e00 = ((G3*Gm2 - G3*Gm3)*G2*Gm1 - (G2*Gm2*Gm3 - G3*Gm2*Gm3 + (G3*Gm2 - G2*Gm3)*Gm1)*G1)/(((G2 - G3)*Gm1 - G2*Gm2 + G3*Gm3)*G1 + (G3*Gm2 - G3*Gm3)*G2);
    ///Port 2 match
    *e11 = ((G2 - G3)*Gm1 - G1*(Gm2 - Gm3) + G3*Gm2 - G2*Gm3)/(((G2 - G3)*Gm1 - G2*Gm2 + G3*Gm3)*G1 + (G3*Gm2 - G3*Gm3)*G2);
    ///de is e00*e11 - (e10*e01)
    *de = -(G1*Gm1*(Gm2 - Gm3) + G2*Gm2*Gm3 - G3*Gm2*Gm3 - (G2*Gm2 - G3*Gm3)*Gm1)/(((G2 - G3)*Gm1 - G2*Gm2 + G3*Gm3)*G1 + (G3*Gm2 - G3*Gm3)*G2);
}

vector<complex_t> CalSpar::SOLCal(vector<complex_t>& rawS1P, vector<complex_t>& shortS1P, vector<complex_t>& openS1P, vector<complex_t>& loadS1P, vector<double>& f)
{
    vector <complex_t> calS1P;

	

	calS1P.resize(rawS1P.size());			//output of function
    for(int i=0;i<(int)rawS1P.size();i++)
	{
        complex_t Gsp = stdShort(f[i]);
        complex_t Gop = stdOpen(f[i]);
        complex_t Glp = stdLoad(f[i]);
        complex_t e00, e11, de;
        Port1ErrorTerms(Gsp, Gop, Glp, shortS1P[i], openS1P[i], loadS1P[i], &e00, &e11, &de);
        calS1P[i] = (rawS1P[i]-e00)/((rawS1P[i]*e11)-de);
	}
    return calS1P;
}

void CalSpar::TwoPortCal(vector<complex_t> rawS21, vector<complex_t> throughS21, vector<complex_t> isolationS21,vector<complex_t> reflectS21,
                                               vector<complex_t> rawS12, vector<complex_t> throughS12, vector<complex_t> isolationS12,vector<complex_t> reflectS12,
                                               vector<complex_t> rawS11, vector<complex_t> shortS11, vector<complex_t> openS11, vector<complex_t> loadS11,
                                               vector<complex_t> rawS22, vector<complex_t> shortS22, vector<complex_t> openS22, vector<complex_t> loadS22,
                                               vector<double> f,
                                               vector<complex_t>* calS21, vector<complex_t>* calS12, vector<complex_t>* calS11, vector<complex_t>* calS22)
{

    *calS21 = rawS21;
    *calS12 = rawS12;
    *calS11 = rawS11;
    *calS22 = rawS22;

    for(int i=0;i<(int)rawS21.size();i++)
	{
        complex_t Gth = stdThrough(f[i]);
        complex_t Gsp = stdShort(f[i]);
        complex_t Gop = stdOpen(f[i]);
        complex_t Glp = stdLoad(f[i]);
        //S1P[i]=(rawS21[i]-isolationS21[i])/(throughS21[i]-isolationS21[i]);
        //S1P[i]/=Gth; //apply Calibration Standard
		
        //S1P[i] =
        complex_t e00, e11, de_P1;
        Port1ErrorTerms(Gsp, Gop, Glp, shortS11[i], openS11[i], loadS11[i], &e00, &e11, &de_P1);

        complex_t e33, e22, de_P2;
        Port1ErrorTerms(Gsp, Gop, Glp, shortS22[i], openS22[i], loadS22[i], &e33, &e22, &de_P2);

        //complex_t S11 = (rawS11[i]-e00)/((rawS11[i]*e11)-de);

        complex_t e10e01 = -de_P1+(e11*e00);
        complex_t e23e32 = -de_P2+(e22*e33);

        complex_t e30 = isolationS21[i];
        complex_t e03 = isolationS12[i];

        complex_t E22 = ((reflectS21[i]/Gth)-e00)/(((reflectS21[i]/Gth)*e11)-de_P1);
        complex_t e10e32 = ((throughS21[i]/Gth) - e30)*(complex_t(1,0)-(e11*E22));

        complex_t E11 = ((reflectS12[i]/Gth)-e33)/(((reflectS12[i]/Gth)*e22)-de_P2);
        complex_t e23e01 = ((throughS12[i]/Gth) - e03)*(complex_t(1,0)-(e22*E11));

        complex_t D = (complex_t(1,0)+((rawS11[i]-e00)/(e10e01))*e11)*
                      (complex_t(1,0)+((rawS22[i]-e33)/(e23e32))*e22)-
                      ((rawS21[i]-e30)/e10e32)*
                      ((rawS12[i]-e03)/e23e01)*
                       E22*E11;

        (*calS21)[i] = (((rawS21[i]-e30)/(e10e32))*(complex_t(1,0)+((rawS22[i]-e33)/(e23e32))*(e22-E22)))/D;
        (*calS12)[i] = (((rawS12[i]-e03)/(e23e01))*(complex_t(1,0)+((rawS11[i]-e00)/(e10e01))*(e11-E11)))/D;


        (*calS11)[i] = (((rawS11[i]-e00)/(e10e01))*(complex_t(1,0)+((rawS22[i]-e33)/(e23e32)*e22))-(E22 * ((rawS21[i]-e30)/(e10e32))*((rawS12[i]-e03)/(e23e01))))/ D;
        (*calS22)[i] = (((rawS22[i]-e33)/(e23e32))*(complex_t(1,0)+((rawS11[i]-e00)/(e10e01)*e11))-(E11 * ((rawS12[i]-e03)/(e23e01))*((rawS21[i]-e30)/(e10e32))))/ D;
		
	}

}


///! Executes a Through (+isolation) calibration on a 3-port trace, including SOL cal (S21, S12, S31, S13, S32, S23 etc)
void CalSpar::ThreePortCal(                    vector<complex_t> rawS12, vector<complex_t> throughS12, vector<complex_t> isolationS12,vector<complex_t> reflectS12,
                                      vector<complex_t> rawS13, vector<complex_t> throughS13, vector<complex_t> isolationS13,vector<complex_t> reflectS13,
                                      vector<complex_t> rawS21, vector<complex_t> throughS21, vector<complex_t> isolationS21,vector<complex_t> reflectS21,
                                      vector<complex_t> rawS23, vector<complex_t> throughS23, vector<complex_t> isolationS23,vector<complex_t> reflectS23,
                                      vector<complex_t> rawS31, vector<complex_t> throughS31, vector<complex_t> isolationS31,vector<complex_t> reflectS31,
                                      vector<complex_t> rawS32, vector<complex_t> throughS32, vector<complex_t> isolationS32,vector<complex_t> reflectS32,

                                      vector<complex_t> rawS11, vector<complex_t> shortS11, vector<complex_t> openS11, vector<complex_t> loadS11,
                                      vector<complex_t> rawS22, vector<complex_t> shortS22, vector<complex_t> openS22, vector<complex_t> loadS22,
                                      vector<complex_t> rawS33, vector<complex_t> shortS33, vector<complex_t> openS33, vector<complex_t> loadS33,
                                      vector<double> f,
                                      vector<complex_t>* calS12, vector<complex_t>* calS13,
                                      vector<complex_t>* calS21, vector<complex_t>* calS23,
                                      vector<complex_t>* calS31, vector<complex_t>* calS32,

                                      vector<complex_t>* calS11, vector<complex_t>* calS22, vector<complex_t>* calS33)
{
    *calS12 = rawS12;
    *calS13 = rawS13;
    *calS21 = rawS21;
    *calS23 = rawS23;
    *calS31 = rawS31;
    *calS32 = rawS32;
    *calS11 = rawS11;
    *calS22 = rawS22;
    *calS33 = rawS33;

    for(int i=0;i<(int)f.size();i++)
    {
        complex_t Gth = stdThrough(f[i]);
        complex_t Gsp = stdShort(f[i]);
        complex_t Gop = stdOpen(f[i]);
        complex_t Glp = stdLoad(f[i]);
        complex_t e44, e11, de_P1;
        Port1ErrorTerms(Gsp, Gop, Glp, shortS11[i], openS11[i], loadS11[i], &e44, &e11, &de_P1);

        complex_t e55, e22, de_P2;
        Port1ErrorTerms(Gsp, Gop, Glp, shortS22[i], openS22[i], loadS22[i], &e55, &e22, &de_P2);

        complex_t e66, e33, de_P3;
        Port1ErrorTerms(Gsp, Gop, Glp, shortS33[i], openS33[i], loadS33[i], &e66, &e33, &de_P3);

        complex_t e14e41 = -de_P1+(e11*e44);
        complex_t e25e52 = -de_P2+(e22*e55);
        complex_t e36e63 = -de_P3+(e33*e66);

        complex_t e54 = isolationS21[i];
        complex_t e45 = isolationS12[i];
        complex_t e64 = isolationS31[i];
        complex_t e46 = isolationS13[i];
        complex_t e65 = isolationS32[i];
        complex_t e56 = isolationS23[i];

        complex_t ee11  = ((reflectS12[i]/Gth)-e55)/(((reflectS12[i]/Gth)*e22)-de_P2);
        complex_t eee11 = ((reflectS13[i]/Gth)-e66)/(((reflectS13[i]/Gth)*e22)-de_P3);
        complex_t ee22  = ((reflectS21[i]/Gth)-e44)/(((reflectS21[i]/Gth)*e11)-de_P1);
        complex_t eee22 = ((reflectS23[i]/Gth)-e66)/(((reflectS23[i]/Gth)*e33)-de_P3);
        complex_t ee33  = ((reflectS31[i]/Gth)-e44)/(((reflectS31[i]/Gth)*e11)-de_P1);
        complex_t eee33 = ((reflectS32[i]/Gth)-e55)/(((reflectS32[i]/Gth)*e22)-de_P2);

        complex_t e14e52 = ((throughS21[i]/Gth) - e54)*(complex_t(1,0)-(e11*ee22));
        complex_t e25e41 = ((throughS12[i]/Gth) - e45)*(complex_t(1,0)-(e22*ee11));
        complex_t e14e63 = ((throughS31[i]/Gth) - e64)*(complex_t(1,0)-(e11*ee33));
        complex_t e36e41 = ((throughS13[i]/Gth) - e46)*(complex_t(1,0)-(e33*eee11));
        complex_t e25e63 = ((throughS32[i]/Gth) - e65)*(complex_t(1,0)-(e22*eee33));
        complex_t e36e52 = ((throughS23[i]/Gth) - e56)*(complex_t(1,0)-(e33*eee22));

        complex_t D = (complex_t(1,0)+((rawS11[i]-e44)/(e14e41))*e11)*
                      (complex_t(1,0)+((rawS22[i]-e55)/(e25e52))*e22)*
                      (complex_t(1,0)+((rawS33[i]-e66)/(e36e63))*e33)
                      -((rawS21[i]-e54)/e14e52)*
                      ((rawS12[i]-e45)/e25e41)*
                       ee22*ee11
                      -((rawS31[i]-e64)/e14e63)*
                      ((rawS13[i]-e46)/e36e41)*
                       ee33*eee11-
                      -((rawS32[i]-e65)/e25e63)*
                      ((rawS23[i]-e56)/e36e52)*
                       eee33*eee22;

        (*calS21)[i] = (((rawS21[i]-e54)/(e14e52))*(complex_t(1,0)+((rawS22[i]-e55)/(e25e52))*(e22-ee22)))/D;
        (*calS12)[i] = (((rawS12[i]-e45)/(e25e41))*(complex_t(1,0)+((rawS11[i]-e44)/(e14e41))*(e11-ee11)))/D;
        (*calS31)[i] = (((rawS31[i]-e64)/(e14e63))*(complex_t(1,0)+((rawS33[i]-e66)/(e36e63))*(e33-ee33)))/D;
        (*calS13)[i] = (((rawS13[i]-e46)/(e36e41))*(complex_t(1,0)+((rawS11[i]-e44)/(e14e41))*(e11-eee11)))/D;
        (*calS32)[i] = (((rawS32[i]-e65)/(e25e63))*(complex_t(1,0)+((rawS33[i]-e66)/(e36e63))*(e33-eee33)))/D;
        (*calS23)[i] = (((rawS23[i]-e56)/(e36e52))*(complex_t(1,0)+((rawS22[i]-e55)/(e25e52))*(e22-eee22)))/D;


        (*calS11)[i] = (((rawS11[i]-e44)/(e14e41))*
                        (complex_t(1,0)+((rawS22[i]-e55)/(e25e52)*e22))*
                        (complex_t(1,0)+((rawS33[i]-e66)/(e36e63)*e33))
                        -(ee22 * ((rawS21[i]-e54)/(e14e52))*((rawS12[i]-e45)/(e25e41)))
                        -(ee33 * ((rawS31[i]-e64)/(e14e63))*((rawS13[i]-e46)/(e36e41))) )/ D;
#warning TODO  S22 and S33
        (*calS22)[i] = (((rawS11[i]-e44)/(e14e41))*
                        (complex_t(1,0)+((rawS22[i]-e55)/(e25e52)*e22))*
                        (complex_t(1,0)+((rawS33[i]-e66)/(e36e63)*e33))
                        -(ee22 * ((rawS21[i]-e54)/(e14e52))*((rawS12[i]-e45)/(e25e41)))
                        -(ee33 * ((rawS31[i]-e64)/(e14e63))*((rawS13[i]-e46)/(e36e41))) )/ D;

        (*calS33)[i] = (((rawS11[i]-e44)/(e14e41))*
                        (complex_t(1,0)+((rawS22[i]-e55)/(e25e52)*e22))*
                        (complex_t(1,0)+((rawS33[i]-e66)/(e36e63)*e33))
                        -(ee22 * ((rawS21[i]-e54)/(e14e52))*((rawS12[i]-e45)/(e25e41)))
                        -(ee33 * ((rawS31[i]-e64)/(e14e63))*((rawS13[i]-e46)/(e36e41))) )/ D;


    }
}

///! Executes a Through (+isolation) calibration on a 4-port trace, including SOL cal (S21, S12, S31, S13, S32, S23 etc)
void CalSpar::FourPortCal(                     vector<complex_t> rawS12, vector<complex_t> throughS12, vector<complex_t> isolationS12,vector<complex_t> reflectS12,
                                      vector<complex_t> rawS13, vector<complex_t> throughS13, vector<complex_t> isolationS13,vector<complex_t> reflectS13,
                                      vector<complex_t> rawS14, vector<complex_t> throughS14, vector<complex_t> isolationS14,vector<complex_t> reflectS14,
                                      vector<complex_t> rawS21, vector<complex_t> throughS21, vector<complex_t> isolationS21,vector<complex_t> reflectS21,
                                      vector<complex_t> rawS23, vector<complex_t> throughS23, vector<complex_t> isolationS23,vector<complex_t> reflectS23,
                                      vector<complex_t> rawS24, vector<complex_t> throughS24, vector<complex_t> isolationS24,vector<complex_t> reflectS24,
                                      vector<complex_t> rawS31, vector<complex_t> throughS31, vector<complex_t> isolationS31,vector<complex_t> reflectS31,
                                      vector<complex_t> rawS32, vector<complex_t> throughS32, vector<complex_t> isolationS32,vector<complex_t> reflectS32,
                                      vector<complex_t> rawS34, vector<complex_t> throughS34, vector<complex_t> isolationS34,vector<complex_t> reflectS34,
                                      vector<complex_t> rawS41, vector<complex_t> throughS41, vector<complex_t> isolationS41,vector<complex_t> reflectS41,
                                      vector<complex_t> rawS42, vector<complex_t> throughS42, vector<complex_t> isolationS42,vector<complex_t> reflectS42,
                                      vector<complex_t> rawS43, vector<complex_t> throughS43, vector<complex_t> isolationS43,vector<complex_t> reflectS43,

                                      vector<complex_t> rawS11, vector<complex_t> shortS11, vector<complex_t> openS11, vector<complex_t> loadS11,
                                      vector<complex_t> rawS22, vector<complex_t> shortS22, vector<complex_t> openS22, vector<complex_t> loadS22,
                                      vector<complex_t> rawS33, vector<complex_t> shortS33, vector<complex_t> openS33, vector<complex_t> loadS33,
                                      vector<complex_t> rawS44, vector<complex_t> shortS44, vector<complex_t> openS44, vector<complex_t> loadS44,
                                      vector<double> f,
                                      vector<complex_t>* calS12, vector<complex_t>* calS13, vector<complex_t>* calS14,
                                      vector<complex_t>* calS21, vector<complex_t>* calS23, vector<complex_t>* calS24,
                                      vector<complex_t>* calS31, vector<complex_t>* calS32, vector<complex_t>* calS34,
                                      vector<complex_t>* calS41, vector<complex_t>* calS42, vector<complex_t>* calS43,

                                      vector<complex_t>* calS11, vector<complex_t>* calS22, vector<complex_t>* calS33, vector<complex_t>* calS44)
{
#warning TODO

    *calS12 = rawS12;
    *calS13 = rawS13;
    *calS14 = rawS14;
    *calS21 = rawS21;
    *calS23 = rawS23;
    *calS24 = rawS24;
    *calS31 = rawS31;
    *calS32 = rawS32;
    *calS34 = rawS34;
    *calS41 = rawS41;
    *calS42 = rawS42;
    *calS43 = rawS43;
    *calS11 = rawS11;
    *calS22 = rawS22;
    *calS33 = rawS33;
    *calS44 = rawS44;
}




spar_t CalSpar::Cal(spar_t& S, cal_t& cal)
{
	spar_t Sc;
	Sc=S;
    QStringList warningStrings;
    if(S.S.size()==1)
    {
        if(cal.P1ShortDone&&cal.P1OpenDone&&cal.P1LoadDone)
        {
            Sc.S[0][0]=SOLCal(S.S[0][0], cal.P1Short, cal.P1Open, cal.P1Load,cal.f);
        }
        else
        {
            warningStrings.push_back("S11");
        }
    }
    if(S.S.size()>1&&cal.P12ThroughDone==false)
    {
        if(cal.P2ShortDone&&cal.P2OpenDone&&cal.P2LoadDone)
        {
            Sc.S[1][1]=SOLCal(S.S[1][1], cal.P2Short, cal.P2Open, cal.P2Load,cal.f);
        }
        else
        {
            warningStrings.push_back("S22");
        }
    }
    if(S.S.size()>2&&cal.P23ThroughDone==false)
    {
        if(cal.P3ShortDone&&cal.P3OpenDone&&cal.P3LoadDone)
        {
            Sc.S[2][2]=SOLCal(S.S[2][2], cal.P3Short, cal.P3Open, cal.P3Load,cal.f);
        }
        else
        {
            warningStrings.push_back("S33");
        }
    }
    if(S.S.size()>3&&cal.P34ThroughDone==false)
    {
        if(cal.P4ShortDone&&cal.P4OpenDone&&cal.P4LoadDone)
        {
            Sc.S[3][3]=SOLCal(S.S[3][3], cal.P4Short, cal.P4Open, cal.P4Load,cal.f);
        }
        else
        {
            warningStrings.push_back("S44");
        }
    }
    if(S.S.size()==2)
    {
        if(cal.P12ThroughDone)
        {
            TwoPortCal(S.S[1][0], cal.Through21, cal.Isolation21, cal.Reflect21,
                                S.S[0][1], cal.Through12, cal.Isolation12, cal.Reflect12,
                                S.S[0][0], cal.P1Short, cal.P1Open, cal.P1Load,
                                S.S[1][1], cal.P2Short, cal.P2Open, cal.P2Load,cal.f,
                                &(Sc.S[1][0]), &(Sc.S[0][1]), &(Sc.S[0][0]), &(Sc.S[1][1]));

        }
        else
        {
            warningStrings.push_back("S21");
            warningStrings.push_back("S12");
        }
    }
    if(S.S.size()==3)
    {
        if(cal.P12ThroughDone&&cal.P13ThroughDone&&cal.P23ThroughDone)
        {
            ThreePortCal(   S.S[0][1], cal.Through12, cal.Isolation12, cal.Reflect12,
                            S.S[0][2], cal.Through13, cal.Isolation13, cal.Reflect13,
                            S.S[1][0], cal.Through21, cal.Isolation21, cal.Reflect21,
                            S.S[1][2], cal.Through23, cal.Isolation23, cal.Reflect23,
                            S.S[2][0], cal.Through31, cal.Isolation31, cal.Reflect31,
                            S.S[2][1], cal.Through32, cal.Isolation32, cal.Reflect32,

                            S.S[0][0], cal.P1Short, cal.P1Open, cal.P1Load,
                            S.S[1][1], cal.P2Short, cal.P2Open, cal.P2Load,
                            S.S[2][2], cal.P3Short, cal.P3Open, cal.P3Load,cal.f,

                            &(Sc.S[0][1]), &(Sc.S[0][2]),
                            &(Sc.S[1][0]), &(Sc.S[1][2]),
                            &(Sc.S[2][0]), &(Sc.S[2][1]),

                            &(Sc.S[0][0]), &(Sc.S[1][1]), &(Sc.S[2][2]));
        }
        else
        {
            warningStrings.push_back("S12");
            warningStrings.push_back("S13");
            warningStrings.push_back("S21");
            warningStrings.push_back("S23");
            warningStrings.push_back("S31");
            warningStrings.push_back("S32");
        }
    }
    if(S.S.size()>3)
    {
        if(     cal.P12ThroughDone&&cal.P13ThroughDone&&cal.P14ThroughDone&&
                cal.P23ThroughDone&&cal.P24ThroughDone&&
                cal.P34ThroughDone)
        {
            FourPortCal(   S.S[0][1], cal.Through12, cal.Isolation12, cal.Reflect12,
                            S.S[0][2], cal.Through13, cal.Isolation13, cal.Reflect13,
                            S.S[0][3], cal.Through14, cal.Isolation14, cal.Reflect14,
                            S.S[1][0], cal.Through21, cal.Isolation21, cal.Reflect21,
                            S.S[1][2], cal.Through23, cal.Isolation23, cal.Reflect23,
                            S.S[1][3], cal.Through24, cal.Isolation24, cal.Reflect24,
                            S.S[2][0], cal.Through31, cal.Isolation31, cal.Reflect31,
                            S.S[2][1], cal.Through32, cal.Isolation32, cal.Reflect32,
                            S.S[2][3], cal.Through34, cal.Isolation34, cal.Reflect34,
                            S.S[3][0], cal.Through41, cal.Isolation41, cal.Reflect41,
                            S.S[3][1], cal.Through42, cal.Isolation42, cal.Reflect42,
                            S.S[3][2], cal.Through43, cal.Isolation43, cal.Reflect43,
                            S.S[0][0], cal.P1Short, cal.P1Open, cal.P1Load,
                            S.S[1][1], cal.P2Short, cal.P2Open, cal.P2Load,
                            S.S[2][2], cal.P3Short, cal.P3Open, cal.P3Load,
                            S.S[3][3], cal.P4Short, cal.P4Open, cal.P4Load,cal.f,

                            &(Sc.S[0][1]), &(Sc.S[0][2]), &(Sc.S[0][3]),
                            &(Sc.S[1][0]), &(Sc.S[1][2]), &(Sc.S[1][3]),
                            &(Sc.S[2][0]), &(Sc.S[2][1]), &(Sc.S[2][3]),
                            &(Sc.S[3][0]), &(Sc.S[3][1]), &(Sc.S[3][2]),

                            &(Sc.S[0][0]), &(Sc.S[1][1]), &(Sc.S[2][2]), &(Sc.S[3][3]));
        }
        else
        {
            warningStrings.push_back("S12");
            warningStrings.push_back("S13");
            warningStrings.push_back("S14");
            warningStrings.push_back("S21");
            warningStrings.push_back("S23");
            warningStrings.push_back("S24");
            warningStrings.push_back("S31");
            warningStrings.push_back("S32");
            warningStrings.push_back("S34");
            warningStrings.push_back("S41");
            warningStrings.push_back("S42");
            warningStrings.push_back("S43");
        }
    }

    if(warningStrings.size()>0)
    {
        emit CalibrationWarning(warningStrings.join(',')+" not calibrated");
    }
	return Sc;
}



cal_t CalSpar::DefaultCal(double startfreq,double stopfreq,int NOP)
{
	cal_t cal;
	double freqstep=(stopfreq-startfreq)/((double)(NOP-1));
	for(int i=0;i<NOP;i++)
		cal.f.push_back(startfreq+(((double)i)*freqstep));
	cal.P1ShortDone=false;
	cal.P1OpenDone=false;
	cal.P1LoadDone=false;
	cal.P2ShortDone=false;
	cal.P2OpenDone=false;
	cal.P2LoadDone=false;
	cal.P3ShortDone=false;
	cal.P3OpenDone=false;
	cal.P3LoadDone=false;
	cal.P4ShortDone=false;
	cal.P4OpenDone=false;
	cal.P4LoadDone=false;
	cal.P12ThroughDone=false;
	cal.P13ThroughDone=false;
	cal.P14ThroughDone=false;
	cal.P23ThroughDone=false;
	cal.P24ThroughDone=false;
	cal.P34ThroughDone=false;
    cal.P12IsolationDone=false;
    cal.P13IsolationDone=false;
    cal.P14IsolationDone=false;
    cal.P23IsolationDone=false;
    cal.P24IsolationDone=false;
    cal.P34IsolationDone=false;

	cal.P1Short.resize(NOP,complex_t(-1,0));
	cal.P1Open.resize(NOP,complex_t(1,0));
	cal.P1Load.resize(NOP,complex_t(0,0));
	cal.P2Short.resize(NOP,complex_t(-1,0));
	cal.P2Open.resize(NOP,complex_t(1,0));
	cal.P2Load.resize(NOP,complex_t(0,0));
	cal.P3Short.resize(NOP,complex_t(-1,0));
	cal.P3Open.resize(NOP,complex_t(1,0));
	cal.P3Load.resize(NOP,complex_t(0,0));
	cal.P4Short.resize(NOP,complex_t(-1,0));
	cal.P4Open.resize(NOP,complex_t(1,0));
	cal.P4Load.resize(NOP,complex_t(0,0));
	cal.Through21.resize(NOP,complex_t(1,0));
	cal.Through12.resize(NOP,complex_t(1,0));
    cal.Reflect21.resize(NOP,complex_t(0,0));
    cal.Reflect12.resize(NOP,complex_t(0,0));
    cal.Isolation21.resize(NOP,complex_t(0,0));
	cal.Isolation12.resize(NOP,complex_t(0,0));
    cal.Through31.resize(NOP,complex_t(1,0));
	cal.Through13.resize(NOP,complex_t(1,0));
    cal.Reflect31.resize(NOP,complex_t(0,0));
    cal.Reflect13.resize(NOP,complex_t(0,0));
    cal.Isolation31.resize(NOP,complex_t(0,0));
	cal.Isolation13.resize(NOP,complex_t(0,0));
    cal.Through41.resize(NOP,complex_t(1,0));
	cal.Through14.resize(NOP,complex_t(1,0));
    cal.Reflect41.resize(NOP,complex_t(0,0));
    cal.Reflect14.resize(NOP,complex_t(0,0));
    cal.Isolation41.resize(NOP,complex_t(0,0));
	cal.Isolation14.resize(NOP,complex_t(0,0));
    cal.Through32.resize(NOP,complex_t(1,0));
	cal.Through23.resize(NOP,complex_t(1,0));
    cal.Reflect23.resize(NOP,complex_t(0,0));
    cal.Reflect32.resize(NOP,complex_t(0,0));
    cal.Isolation32.resize(NOP,complex_t(0,0));
	cal.Isolation23.resize(NOP,complex_t(0,0));
    cal.Through42.resize(NOP,complex_t(1,0));
	cal.Through24.resize(NOP,complex_t(1,0));
    cal.Reflect42.resize(NOP,complex_t(0,0));
    cal.Reflect24.resize(NOP,complex_t(0,0));
    cal.Isolation42.resize(NOP,complex_t(0,0));
	cal.Isolation24.resize(NOP,complex_t(0,0));
    cal.Through43.resize(NOP,complex_t(1,0));
	cal.Through34.resize(NOP,complex_t(1,0));
    cal.Reflect43.resize(NOP,complex_t(0,0));
    cal.Reflect34.resize(NOP,complex_t(0,0));
    cal.Isolation43.resize(NOP,complex_t(0,0));
	cal.Isolation34.resize(NOP,complex_t(0,0));
    return cal;
}
#ifndef CLI_ONLY
bool CalSpar::TrimCal(cal_t& cal, double startfreq, double stopfreq, int NOP, cal_t *targetCal)
{
    cal_t temp_cal;
	temp_cal.f.resize(NOP);
	temp_cal.P1ShortDone=cal.P1ShortDone;
	temp_cal.P1OpenDone=cal.P1OpenDone;
	temp_cal.P1LoadDone=cal.P1LoadDone;
	temp_cal.P2ShortDone=cal.P2ShortDone;
	temp_cal.P2OpenDone=cal.P2OpenDone;
	temp_cal.P2LoadDone=cal.P2LoadDone;
	temp_cal.P12ThroughDone=cal.P12ThroughDone;
	temp_cal.P13ThroughDone=cal.P13ThroughDone;
	temp_cal.P14ThroughDone=cal.P14ThroughDone;
	temp_cal.P23ThroughDone=cal.P23ThroughDone;
	temp_cal.P24ThroughDone=cal.P24ThroughDone;
	temp_cal.P34ThroughDone=cal.P34ThroughDone;
    temp_cal.P12IsolationDone=cal.P12IsolationDone;
    temp_cal.P13IsolationDone=cal.P13IsolationDone;
    temp_cal.P14IsolationDone=cal.P14IsolationDone;
    temp_cal.P23IsolationDone=cal.P23IsolationDone;
    temp_cal.P24IsolationDone=cal.P24IsolationDone;
    temp_cal.P34IsolationDone=cal.P34IsolationDone;
    qDebug()<<cal.f.size();

	Spline<double, complex_t> SpP1Short(cal.f, cal.P1Short);
	Spline<double, complex_t> SpP1Open(cal.f, cal.P1Open);
	Spline<double, complex_t> SpP1Load(cal.f, cal.P1Load);
	Spline<double, complex_t> SpP2Short(cal.f, cal.P2Short);
	Spline<double, complex_t> SpP2Open(cal.f, cal.P2Open);
	Spline<double, complex_t> SpP2Load(cal.f, cal.P2Load);
	Spline<double, complex_t> SpP3Short(cal.f, cal.P3Short);
	Spline<double, complex_t> SpP3Open(cal.f, cal.P3Open);
	Spline<double, complex_t> SpP3Load(cal.f, cal.P3Load);
	Spline<double, complex_t> SpP4Short(cal.f, cal.P4Short);
	Spline<double, complex_t> SpP4Open(cal.f, cal.P4Open);
	Spline<double, complex_t> SpP4Load(cal.f, cal.P4Load);
	Spline<double, complex_t> SpThrough12(cal.f, cal.Through12);
	Spline<double, complex_t> SpThrough13(cal.f, cal.Through13);
	Spline<double, complex_t> SpThrough14(cal.f, cal.Through14);
	Spline<double, complex_t> SpThrough21(cal.f, cal.Through21);
	Spline<double, complex_t> SpThrough23(cal.f, cal.Through23);
	Spline<double, complex_t> SpThrough24(cal.f, cal.Through24);
	Spline<double, complex_t> SpThrough31(cal.f, cal.Through31);
	Spline<double, complex_t> SpThrough32(cal.f, cal.Through32);
	Spline<double, complex_t> SpThrough34(cal.f, cal.Through34);
	Spline<double, complex_t> SpThrough41(cal.f, cal.Through41);
	Spline<double, complex_t> SpThrough42(cal.f, cal.Through42);
	Spline<double, complex_t> SpThrough43(cal.f, cal.Through43);
    
    Spline<double, complex_t> SpIsolation12(cal.f, cal.Isolation12);
	Spline<double, complex_t> SpIsolation13(cal.f, cal.Isolation13);
	Spline<double, complex_t> SpIsolation14(cal.f, cal.Isolation14);
	Spline<double, complex_t> SpIsolation21(cal.f, cal.Isolation21);
	Spline<double, complex_t> SpIsolation23(cal.f, cal.Isolation23);
	Spline<double, complex_t> SpIsolation24(cal.f, cal.Isolation24);
	Spline<double, complex_t> SpIsolation31(cal.f, cal.Isolation31);
	Spline<double, complex_t> SpIsolation32(cal.f, cal.Isolation32);
	Spline<double, complex_t> SpIsolation34(cal.f, cal.Isolation34);
	Spline<double, complex_t> SpIsolation41(cal.f, cal.Isolation41);
	Spline<double, complex_t> SpIsolation42(cal.f, cal.Isolation42);
	Spline<double, complex_t> SpIsolation43(cal.f, cal.Isolation43);

    Spline<double, complex_t> SpReflect12(cal.f, cal.Reflect12);
    Spline<double, complex_t> SpReflect13(cal.f, cal.Reflect13);
    Spline<double, complex_t> SpReflect14(cal.f, cal.Reflect14);
    Spline<double, complex_t> SpReflect21(cal.f, cal.Reflect21);
    Spline<double, complex_t> SpReflect23(cal.f, cal.Reflect23);
    Spline<double, complex_t> SpReflect24(cal.f, cal.Reflect24);
    Spline<double, complex_t> SpReflect31(cal.f, cal.Reflect31);
    Spline<double, complex_t> SpReflect32(cal.f, cal.Reflect32);
    Spline<double, complex_t> SpReflect34(cal.f, cal.Reflect34);
    Spline<double, complex_t> SpReflect41(cal.f, cal.Reflect41);
    Spline<double, complex_t> SpReflect42(cal.f, cal.Reflect42);
    Spline<double, complex_t> SpReflect43(cal.f, cal.Reflect43);
    
	
	
	double freqstep=(stopfreq-startfreq)/((double)(NOP-1));
	for(int i=0;i<NOP;i++)
	{
		
		temp_cal.f[i]=startfreq+((double)i)*freqstep;
		if(temp_cal.f[i]<cal.f[0])
		{
			*targetCal = DefaultCal(startfreq,stopfreq,NOP);
			return false;
		}
		if(temp_cal.f[i]>cal.f.back())
		{
			*targetCal =  DefaultCal(startfreq,stopfreq,NOP);
			return false;
		}
		temp_cal.P1Short.push_back(SpP1Short[temp_cal.f[i]]);
		temp_cal.P1Open.push_back(SpP1Open[temp_cal.f[i]]);
		temp_cal.P1Load.push_back(SpP1Load[temp_cal.f[i]]);
		temp_cal.P2Short.push_back(SpP2Short[temp_cal.f[i]]);
		temp_cal.P2Open.push_back(SpP2Open[temp_cal.f[i]]);
		temp_cal.P2Load.push_back(SpP2Load[temp_cal.f[i]]);
		temp_cal.P3Short.push_back(SpP3Short[temp_cal.f[i]]);
		temp_cal.P3Open.push_back(SpP3Open[temp_cal.f[i]]);
		temp_cal.P3Load.push_back(SpP3Load[temp_cal.f[i]]);
		temp_cal.P4Short.push_back(SpP4Short[temp_cal.f[i]]);
		temp_cal.P4Open.push_back(SpP4Open[temp_cal.f[i]]);
		temp_cal.P4Load.push_back(SpP4Load[temp_cal.f[i]]);
		temp_cal.Through12.push_back(SpThrough12[temp_cal.f[i]]);
		temp_cal.Through13.push_back(SpThrough13[temp_cal.f[i]]);
		temp_cal.Through14.push_back(SpThrough14[temp_cal.f[i]]);
		temp_cal.Through21.push_back(SpThrough21[temp_cal.f[i]]);
		temp_cal.Through23.push_back(SpThrough23[temp_cal.f[i]]);
		temp_cal.Through24.push_back(SpThrough24[temp_cal.f[i]]);
		temp_cal.Through31.push_back(SpThrough31[temp_cal.f[i]]);
		temp_cal.Through32.push_back(SpThrough32[temp_cal.f[i]]);
		temp_cal.Through34.push_back(SpThrough34[temp_cal.f[i]]);
		temp_cal.Through41.push_back(SpThrough41[temp_cal.f[i]]);
		temp_cal.Through42.push_back(SpThrough42[temp_cal.f[i]]);
		temp_cal.Through43.push_back(SpThrough43[temp_cal.f[i]]);
		temp_cal.Isolation12.push_back(SpIsolation12[temp_cal.f[i]]);
        temp_cal.Isolation13.push_back(SpIsolation13[temp_cal.f[i]]);
        temp_cal.Isolation14.push_back(SpIsolation14[temp_cal.f[i]]);
        temp_cal.Isolation21.push_back(SpIsolation21[temp_cal.f[i]]);
        temp_cal.Isolation23.push_back(SpIsolation23[temp_cal.f[i]]);
        temp_cal.Isolation24.push_back(SpIsolation24[temp_cal.f[i]]);
        temp_cal.Isolation31.push_back(SpIsolation31[temp_cal.f[i]]);
        temp_cal.Isolation32.push_back(SpIsolation32[temp_cal.f[i]]);
        temp_cal.Isolation34.push_back(SpIsolation34[temp_cal.f[i]]);
        temp_cal.Isolation41.push_back(SpIsolation41[temp_cal.f[i]]);
        temp_cal.Isolation42.push_back(SpIsolation42[temp_cal.f[i]]);
        temp_cal.Isolation43.push_back(SpIsolation43[temp_cal.f[i]]);
        temp_cal.Reflect12.push_back(SpReflect12[temp_cal.f[i]]);
        temp_cal.Reflect13.push_back(SpReflect13[temp_cal.f[i]]);
        temp_cal.Reflect14.push_back(SpReflect14[temp_cal.f[i]]);
        temp_cal.Reflect21.push_back(SpReflect21[temp_cal.f[i]]);
        temp_cal.Reflect23.push_back(SpReflect23[temp_cal.f[i]]);
        temp_cal.Reflect24.push_back(SpReflect24[temp_cal.f[i]]);
        temp_cal.Reflect31.push_back(SpReflect31[temp_cal.f[i]]);
        temp_cal.Reflect32.push_back(SpReflect32[temp_cal.f[i]]);
        temp_cal.Reflect34.push_back(SpReflect34[temp_cal.f[i]]);
        temp_cal.Reflect41.push_back(SpReflect41[temp_cal.f[i]]);
        temp_cal.Reflect42.push_back(SpReflect42[temp_cal.f[i]]);
        temp_cal.Reflect43.push_back(SpReflect43[temp_cal.f[i]]);
	}
	*targetCal = temp_cal;
	return true;
	
}

tCalStd CalSpar::DefaultCalStd(void)
{
	tCalStd c;
	c.Name="Default";
    c.L0=2.0765e-12;
    c.L1=-108.54e-24;
    c.L2=2.1705e-33;
    c.L3=-.01e-42;
    c.LengthShort=0.0001;
    c.LossdBShort=0.01;
    c.LossdBHzShort=1e-14;
    c.C0=49.433e-15;
    c.C1=-310.13e-27;
    c.C2=23.168e-36;
    c.C3=-.15966e-45;
    c.LengthOpen=0.0001;
    c.LossdBOpen=0.01;
    c.LossdBHzOpen=1e-14;
    c.Ll=0;
    c.Rl=50;
    c.LengthThrough=0;
    c.LossdBThrough=0.1;
    c.LossdBHzThrough=1e-12;
	return c;
}

tCalStd CalSpar::IdealCalStd(void)
{
    tCalStd c;
    c.Name="Ideal";
    c.L0=0;
    c.L1=0;
    c.L2=0;
    c.L3=0;
    c.LengthShort=0;
    c.LossdBShort=0;
    c.LossdBHzShort=0;
    c.C0=0;
    c.C1=0;
    c.C2=0;
    c.C3=0;
    c.LengthOpen=0;
    c.LossdBOpen=0;
    c.LossdBHzOpen=0;
    c.Ll=0;
    c.Rl=50;
    c.LengthThrough=0;
    c.LossdBThrough=0;
    c.LossdBHzThrough=0;
    return c;
}

#endif



