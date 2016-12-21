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

#include "vna-math.h"
#include <QString>


double StringToFreq(QString freqstring)
{
    double freq;
    double multiplier=1;
    QString temp=freqstring;
    temp=temp.toUpper();
    if(temp.replace("KHZ", "")>0)multiplier=1000;
    if(temp.replace("MHZ", "")>0)multiplier=1000000;
    if(temp.replace("GHZ", "")>0)multiplier=1000000000;
    temp.replace(" ","");
    freq = temp.toDouble();
    freq*=multiplier;
    return freq;
}

QString FreqToString(double f, int DecimalPlaces)
{
    double fTemp=f;
    while(fTemp>1e3)fTemp/=1e3;
    if(((fTemp-floor(fTemp))<0.1)&&DecimalPlaces==1)DecimalPlaces=0;
    QString s;
    QString format=s.sprintf("%%1.%dlf",DecimalPlaces);
    if(f>1e9)return s.sprintf(((format+" GHz").toUtf8().data()),f/1e9);
    if(f>1e6)return s.sprintf(((format+" MHz").toUtf8().data()),f/1e6);
    if(f>1e3)return s.sprintf(((format+" kHz").toUtf8().data()),f/1e3);
    return s.sprintf((format+" Hz").toUtf8().data(),f);
}



double StringToAmp(const QString &ampstring)
{
    double amp;
    QString temp = ampstring;
    AmpType ampType = dBm;
    temp=temp.toUpper();
    if(temp.replace("MW", "")>0)ampType = mW;
    if(temp.replace("W", "")>0)ampType = W;
    if(temp.replace("DBM", "")>0)ampType = dBm;
    amp = temp.toDouble();
    switch(ampType)
    {
        case mW:
            amp = 10*log10(amp);
            break;
        case W:
            amp = 10*log10(amp*1000);
            break;
        case dBm:
            //already in dBm, do nothing;
            break;
    }
    return amp;
}

QString AmpToString(double amp, AmpType type)
{
    double amptemp=amp;
    QString AmpString;
    QString s;
    switch(type)
    {
        case mW:
            amptemp = pow(10,amptemp)/10;
            AmpString = s.sprintf("%1.1lf mW", amptemp);
            break;
        case W:
            amptemp = pow(10,amptemp)/10000;
            AmpString = s.sprintf("%1.1lf W", amptemp);
            break;
        case dBm:
            AmpString = s.sprintf("%1.1lf dBm", amptemp);
            break;
    }
    return AmpString;
}

complex_t ZtoS(complex_t Z)
{
    return (Z-complex_t(1,0))/(Z+complex_t(1,0));
}

complex_t StoZ(complex_t S)
{
    return (complex_t(1,0)+S)/(complex_t(1,0)-S);
}

double StoVSWR(complex_t S)
{
    double as=abs(S);
    return (1+as)/(1-as);
}

double RadtoDeg(double rad)
{
    return rad*(180/M_PI);
}

double DegtoRad(double deg)
{
    return deg/(180/M_PI);
}


