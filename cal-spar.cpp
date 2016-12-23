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
using namespace std;

CalSpar::CalSpar()
{
}
///! MICROWAVE & RF CIRCUITS: Analysis, Design, Fabrication, & Measurement 7-11
vector<complex_t> CalSpar::SOLCal(vector<complex_t>& rawS1P, vector<complex_t>& shortS1P, vector<complex_t>& openS1P, vector<complex_t>& loadS1P, vector<double>& f)
{
    vector <complex_t> calS1P;
	complex_t M1, M2, P1, P2, Q1, Q2, A, B, C;
	

	calS1P.resize(rawS1P.size());			//output of function
    for(int i=0;i<(int)rawS1P.size();i++)
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
		complex_t Zsp=complex_t(0,2*M_PI*f[i]*(L0+L1*f[i]+L2*pow(f[i],2)+L3*pow(f[i],3)));
		complex_t Gsp=((Zsp-(complex_t(50,0))/(Zsp+complex_t(50,0))));
		double wl=c/f[i];
        Gsp = Gsp * exp(complex_t(0,2*M_PI*(LengthShort/wl))); //apply electrical length
        Gsp = Gsp * pow(10,-1*LossdBShort/20) * pow(10,(-1*LossdBHzShort*f[i])/20); //apply Loss (dB) and Loss (dB/Hz)
        //Gsp = complex_t(-1,0);
		                
		               
		
		// - - - - - - - - - - model for open
		double C0=m_CalStd->C0;
		double C1=m_CalStd->C1;
		double C2=m_CalStd->C2;
		double C3=m_CalStd->C3;
		double LengthOpen=m_CalStd->LengthOpen; //Electrical length in m
		double LossdBOpen=m_CalStd->LossdBOpen;
		double LossdBHzOpen=m_CalStd->LossdBHzOpen;
		
		complex_t Zop=complex_t(0,-1)/	complex_t(2*M_PI*f[i]*(C0+C1*f[i]+C2*pow(f[i],2)+C3*pow(f[i],3)),0);// impedance for open
		complex_t Gop=((Zop-complex_t(50,0))/(Zop+complex_t(50,0)));
        if(2*M_PI*f[i]*(C0+C1*f[i]+C2*pow(f[i],2)+C3*pow(f[i],3))==0)
            Gop=complex_t(1,0);
        Gop = Gop * exp(complex_t(0,2*M_PI*(LengthOpen/wl))); //apply electrical length
        Gop = Gop * pow(10,-1*LossdBOpen/20) * pow(10,(-1*LossdBHzOpen*f[i])/20); //apply Loss (dB) and Loss (dB/Hz)
        //Gop = complex_t(1,0);
		//- - - - - - - - - - model for load - - - - - - - - - - - - - - -
		double Ll=m_CalStd->Ll;
		double Rl=m_CalStd->Rl;
		complex_t Zlp=complex_t(Rl,2*M_PI*f[i]*Ll);
		complex_t Glp=((Zlp-complex_t(50,0))/(Zlp+complex_t(50,0)));
        //Glp = complex_t(0,0);

        //Apply standards to s-parameter.
		M1=shortS1P[i]-openS1P[i];
		M2=openS1P[i]-loadS1P[i];
		P1=Gsp-Gop;
		P2=Gop-Glp;
		Q1=(Gop*openS1P[i]) - (Gsp*shortS1P[i]);
		Q2=(Glp*loadS1P[i]) - (Gop*openS1P[i]);
		A=((Q2*M1)-(Q1*M2))/((P1*Q2)-(P2*Q1));
		C=((P2*M1)-(P1*M2))/((P2*Q1)-(P1*Q2));
		B=loadS1P[i]+(Glp*loadS1P[i]*C)-(Glp*A);
		calS1P[i]=(rawS1P[i]-B)/((complex_t(-1,0)*C*rawS1P[i])+A);
	}
	return calS1P;
}

vector<complex_t> CalSpar::ThroughIsolationCal(vector<complex_t>& rawS1P, vector<complex_t>& throughS1P, vector<complex_t>& isolationS1P, vector<double>& f)
{
	double LengthThrough=m_CalStd->LengthThrough;
	double LossdBThrough=m_CalStd->LossdBThrough;
	double LossdBHzThrough=m_CalStd->LossdBHzThrough;
	double c = 299792458;
	
	vector<complex_t> S1P = rawS1P;
    for(int i=0;i<(int)rawS1P.size();i++)
	{
		double wl=c/f[i];
		complex_t Gth(1,0);
		Gth = Gth * exp(complex_t(0,2*M_PI*(LengthThrough/wl))); //apply electrical length
		Gth = Gth * pow(10,-1*LossdBThrough/20) * pow(10,(-1*LossdBHzThrough*f[i])/20); //apply Loss (dB) and Loss (dB/Hz)	
		S1P[i]=(rawS1P[i]-isolationS1P[i])/(throughS1P[i]-isolationS1P[i]);
		S1P[i]/=Gth; //apply Calibration Standard
		
		
	}
	return S1P;
}


spar_t CalSpar::Cal(spar_t& S, cal_t& cal)
{
	spar_t Sc;
	Sc=S;
    if(cal.P1ShortDone&&cal.P1OpenDone&&cal.P1LoadDone&&S.S.size()>0)
	{
		Sc.S[0][0]=SOLCal(S.S[0][0], cal.P1Short, cal.P1Open, cal.P1Load,cal.f);
	}
	else 
	{
		//cout<<"Warning: Port 1 uncalibrated"<<endl;
	}
    if(cal.P2ShortDone&&cal.P2OpenDone&&cal.P2LoadDone&&S.S.size()>1)
	{
		Sc.S[1][1]=SOLCal(S.S[1][1], cal.P2Short, cal.P2Open, cal.P2Load,cal.f);
	}
	else 
	{
		//cout<<"Warning: Port 2 uncalibrated"<<endl;
	}
    if(cal.P3ShortDone&&cal.P3OpenDone&&cal.P3LoadDone&&S.S.size()>2)
	{
		Sc.S[2][2]=SOLCal(S.S[2][2], cal.P3Short, cal.P3Open, cal.P3Load,cal.f);
	}
	else 
	{
		//cout<<"Warning: Port 3 uncalibrated"<<endl;
	}
    if(cal.P4ShortDone&&cal.P4OpenDone&&cal.P4LoadDone&&S.S.size()>2)
	{
		Sc.S[3][3]=SOLCal(S.S[3][3], cal.P4Short, cal.P4Open, cal.P4Load,cal.f);
	}
	else 
	{
		//cout<<"Warning: Port 2 uncalibrated"<<endl;
	}

    if(cal.P12ThroughDone&&S.S.size()>1)
	{
		Sc.S[1][0]=ThroughIsolationCal(S.S[1][0], cal.Through21, cal.Isolation21, cal.f);
		Sc.S[0][1]=ThroughIsolationCal(S.S[0][1], cal.Through12, cal.Isolation12, cal.f);
	}
    if(cal.P13ThroughDone&&S.S.size()>2)
	{
		Sc.S[2][0]=ThroughIsolationCal(S.S[2][0], cal.Through31, cal.Isolation31, cal.f);
		Sc.S[0][2]=ThroughIsolationCal(S.S[0][2], cal.Through13, cal.Isolation13, cal.f);
	}
    if(cal.P14ThroughDone&&S.S.size()>2)
	{
		Sc.S[3][0]=ThroughIsolationCal(S.S[3][0], cal.Through41, cal.Isolation41, cal.f);
		Sc.S[0][3]=ThroughIsolationCal(S.S[0][3], cal.Through14, cal.Isolation14, cal.f);
	}
    if(cal.P23ThroughDone&&S.S.size()>2)
	{
		Sc.S[2][1]=ThroughIsolationCal(S.S[2][1], cal.Through32, cal.Isolation32, cal.f);
		Sc.S[1][2]=ThroughIsolationCal(S.S[1][2], cal.Through23, cal.Isolation23, cal.f);
	}
    if(cal.P24ThroughDone&&S.S.size()>2)
	{
		Sc.S[3][1]=ThroughIsolationCal(S.S[3][1], cal.Through42, cal.Isolation42, cal.f);
		Sc.S[1][3]=ThroughIsolationCal(S.S[1][3], cal.Through24, cal.Isolation24, cal.f);
	}
    if(cal.P34ThroughDone&&S.S.size()>2)
	{
		Sc.S[3][2]=ThroughIsolationCal(S.S[3][2], cal.Through43, cal.Isolation43, cal.f);
		Sc.S[2][3]=ThroughIsolationCal(S.S[2][3], cal.Through34, cal.Isolation34, cal.f);
	}
	
	return Sc;
}

vector<complex_t> CalSpar::Cal(vector<complex_t>& S, int index1, int index2, cal_t& cal)
{
	vector<complex_t> Sc;
	Sc=S;
	if(cal.P1ShortDone&&cal.P1OpenDone&&cal.P1LoadDone&&index1==0&&index2==0)
	{
		Sc=SOLCal(S, cal.P1Short, cal.P1Open, cal.P1Load,cal.f);
	}
	else 
	{
		//cout<<"Warning: Port 1 uncalibrated"<<endl;
	}
	if(cal.P2ShortDone&&cal.P2OpenDone&&cal.P2LoadDone&&index1==1&&index2==1)
	{
		Sc=SOLCal(S, cal.P2Short, cal.P2Open, cal.P2Load,cal.f);
	}
	else 
	{
		//cout<<"Warning: Port 2 uncalibrated"<<endl;
	}
	if(cal.P3ShortDone&&cal.P3OpenDone&&cal.P3LoadDone&&index1==2&&index2==2)
	{
		Sc=SOLCal(S, cal.P3Short, cal.P3Open, cal.P3Load,cal.f);
	}
	else 
	{
		//cout<<"Warning: Port 3 uncalibrated"<<endl;
	}
	if(cal.P4ShortDone&&cal.P4OpenDone&&cal.P4LoadDone&&index1==3&&index2==3)
	{
		Sc=SOLCal(S, cal.P4Short, cal.P4Open, cal.P4Load,cal.f);
	}
	else 
	{
		//cout<<"Warning: Port 4 uncalibrated"<<endl;
	}

	if(cal.P12ThroughDone)
	{
		if(index1==1&&index2==0)Sc=ThroughIsolationCal(S, cal.Through21, cal.Isolation21, cal.f);
		if(index1==0&&index2==1)Sc=ThroughIsolationCal(S, cal.Through12, cal.Isolation12, cal.f);
	}
	if(cal.P13ThroughDone)
	{
		if(index1==2&&index2==0)Sc=ThroughIsolationCal(S, cal.Through31, cal.Isolation31, cal.f);
		if(index1==0&&index2==2)Sc=ThroughIsolationCal(S, cal.Through13, cal.Isolation13, cal.f);
	}
	if(cal.P14ThroughDone)
	{
		if(index1==3&&index2==0)Sc=ThroughIsolationCal(S, cal.Through41, cal.Isolation41, cal.f);
		if(index1==0&&index2==3)Sc=ThroughIsolationCal(S, cal.Through14, cal.Isolation14, cal.f);
	}
	if(cal.P23ThroughDone)
	{
		if(index1==2&&index2==1)Sc=ThroughIsolationCal(S, cal.Through32, cal.Isolation32, cal.f);
		if(index1==1&&index2==2)Sc=ThroughIsolationCal(S, cal.Through23, cal.Isolation23, cal.f);
	}
	if(cal.P24ThroughDone)
	{
		if(index1==3&&index2==1)Sc=ThroughIsolationCal(S, cal.Through42, cal.Isolation42, cal.f);
		if(index1==1&&index2==3)Sc=ThroughIsolationCal(S, cal.Through24, cal.Isolation24, cal.f);
	}
	if(cal.P34ThroughDone)
	{
		if(index1==3&&index2==2)Sc=ThroughIsolationCal(S, cal.Through43, cal.Isolation43, cal.f);
		if(index1==2&&index2==3)Sc=ThroughIsolationCal(S, cal.Through34, cal.Isolation34, cal.f);
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
	cal.Isolation21.resize(NOP,complex_t(0,0));
	cal.Isolation12.resize(NOP,complex_t(0,0));
	cal.Through31.resize(NOP,complex_t(1,0));
	cal.Through13.resize(NOP,complex_t(1,0));
	cal.Isolation31.resize(NOP,complex_t(0,0));
	cal.Isolation13.resize(NOP,complex_t(0,0));
	cal.Through41.resize(NOP,complex_t(1,0));
	cal.Through14.resize(NOP,complex_t(1,0));
	cal.Isolation41.resize(NOP,complex_t(0,0));
	cal.Isolation14.resize(NOP,complex_t(0,0));
	cal.Through32.resize(NOP,complex_t(1,0));
	cal.Through23.resize(NOP,complex_t(1,0));
	cal.Isolation32.resize(NOP,complex_t(0,0));
	cal.Isolation23.resize(NOP,complex_t(0,0));
	cal.Through42.resize(NOP,complex_t(1,0));
	cal.Through24.resize(NOP,complex_t(1,0));
	cal.Isolation42.resize(NOP,complex_t(0,0));
	cal.Isolation24.resize(NOP,complex_t(0,0));
	cal.Through43.resize(NOP,complex_t(1,0));
	cal.Through34.resize(NOP,complex_t(1,0));
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
        temp_cal.Isolation42.push_back(SpIsolation43[temp_cal.f[i]]);
        temp_cal.Isolation43.push_back(SpIsolation43[temp_cal.f[i]]);
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

#endif



