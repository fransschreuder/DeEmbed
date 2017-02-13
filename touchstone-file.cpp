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

#include "touchstone-file.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#ifdef WIN32
#include "shlwapi.h"
/*int strcasecmp(const char* first, const char* second)
{
 return lstrcmpiA(first,second);
}
#define M_PI 3.1415*/
#endif
TouchstoneFile::TouchstoneFile ()
{
    setlocale(LC_NUMERIC, "C");
}

TouchstoneFile::~TouchstoneFile ()
{
}

bool TouchstoneFile::Save(spar_t& spar, const char* filename)
{
cout<<"Save 1"<<endl;
    time_t timer = time(NULL);
    ofstream File;
    File.open(filename);
    bool fileOpened = false;

    if(File.good())fileOpened = true;
    char line[4096];
    if(fileOpened)
    {
        File<<"!Created with vna, "<<ctime(&timer)<<endl;
        File<<"# Hz S RI R 50"<<endl;
    }
    else
    {
        cout<<"!Created with vna, "<<ctime(&timer)<<endl;
        cout<<"# Hz S RI R 50"<<endl;
    }
    for(int i=0;i<(int)spar.f.size();i++)
    {
        if(spar.S[0].size()==1)
            sprintf(line,"%lf\t%le\t%le",spar.f[i],spar.S[0][0][i].real(),spar.S[0][0][i].imag());
        else if (spar.S[0].size()==2)
            sprintf(line,"%lf\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le",spar.f[i],
                    spar.S[0][0][i].real(),spar.S[0][0][i].imag(),
                    spar.S[1][0][i].real(),spar.S[1][0][i].imag(),
                    spar.S[0][1][i].real(),spar.S[0][1][i].imag(),
                    spar.S[1][1][i].real(),spar.S[1][1][i].imag());
        else if (spar.S[0].size()==4)
            sprintf(line,"%lf\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\n\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\n\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\n\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le",spar.f[i],
                    spar.S[0][0][i].real(),spar.S[0][0][i].imag(),
                    spar.S[0][1][i].real(),spar.S[0][1][i].imag(),
                    spar.S[0][2][i].real(),spar.S[0][2][i].imag(),
                    spar.S[0][3][i].real(),spar.S[0][3][i].imag(),
                    spar.S[1][0][i].real(),spar.S[1][0][i].imag(),
                    spar.S[1][1][i].real(),spar.S[1][1][i].imag(),
                    spar.S[1][2][i].real(),spar.S[1][2][i].imag(),
                    spar.S[1][3][i].real(),spar.S[1][3][i].imag(),
                    spar.S[2][0][i].real(),spar.S[2][0][i].imag(),
                    spar.S[2][1][i].real(),spar.S[2][1][i].imag(),
                    spar.S[2][2][i].real(),spar.S[2][2][i].imag(),
                    spar.S[2][3][i].real(),spar.S[2][3][i].imag(),
                    spar.S[3][0][i].real(),spar.S[3][0][i].imag(),
                    spar.S[3][1][i].real(),spar.S[3][1][i].imag(),
                    spar.S[3][2][i].real(),spar.S[3][2][i].imag(),
                    spar.S[3][3][i].real(),spar.S[3][3][i].imag());
        if(fileOpened)File<<line<<endl;
        else cout<<line<<endl;
    }
    if(fileOpened)File.close();
    return fileOpened;

}

spar_t TouchstoneFile::Load2P(const char* filename)
{
    ifstream File;
    spar_t spar;
    File.open(filename);
    if(!File.good())return spar;

    spar.S.resize(2);
    for(int i=0; i<2; i++)
        spar.S[i].resize(2);
    FUnit fUnit=GHz;
    SFormat sFormat=MA;
    double R;
    double freq, firstS11, secondS11,firstS21, secondS21,firstS12, secondS12,firstS22, secondS22;

    char rawline[4096];
    char* line;
    while(File.getline(rawline,4095))
    {
        line=rawline;
        while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        if(line[0]=='!'||line[0]=='\0')continue; //ignore comment lines or emty lines
        if(line[0]=='#')
        {
            while(*line != '\0' && (isspace(*line)||*line=='#'))line++; //strip leading '#' and spaces

        //	# <frequency unit> <parameter> <format> R <n>
            char sFUnit[256], sSFormat[256],  dontcare[256];
            sscanf(line, "%s %s %s %s %lf", sFUnit, dontcare, sSFormat, dontcare, &R);
            if(strcasecmp(sFUnit,"GHZ")==0)fUnit=GHz;
            if(strcasecmp((sFUnit),"MHZ")==0)fUnit=MHz;
            if(strcasecmp((sFUnit),"KHZ")==0)fUnit=KHz;
            if(strcasecmp((sFUnit),"HZ")==0)fUnit=Hz;

            if(strcasecmp((sSFormat),"DB")==0)sFormat=DB;
            if(strcasecmp((sSFormat),"MA")==0)sFormat=MA;
            if(strcasecmp((sSFormat),"RI")==0)sFormat=RI;
            continue;
        }
        sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf", &freq, &firstS11, &secondS11, &firstS21, &secondS21, &firstS12, &secondS12, &firstS22, &secondS22);
        if(fUnit==GHz)freq*=1e9;
        if(fUnit==MHz)freq*=1e6;
        if(fUnit==KHz)freq*=1e3;
        if(spar.f.size()>0&&freq<spar.f.back()){cout<<"Ignoring Noise Parameters in file"<<endl;break;} //either error or noise parameters
        spar.f.push_back(freq);
        complex_t tempS11;
        if(sFormat==DB)tempS11=complex_t(pow(10,firstS11/20),0)*exp(complex_t(0,secondS11)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS11=complex_t(firstS11,0)*exp(complex_t(0,secondS11)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS11=complex_t(firstS11,secondS11);
        spar.S[0][0].push_back(tempS11);
        complex_t tempS21;
        if(sFormat==DB)tempS21=complex_t(pow(10,firstS21/20),0)*exp(complex_t(0,secondS21)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS21=complex_t(firstS21,0)*exp(complex_t(0,secondS21)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS21=complex_t(firstS21,secondS21);
        spar.S[1][0].push_back(tempS21);
        complex_t tempS12;
        if(sFormat==DB)tempS12=complex_t(pow(10,firstS12/20),0)*exp(complex_t(0,secondS12)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS12=complex_t(firstS12,0)*exp(complex_t(0,secondS12)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS12=complex_t(firstS12,secondS12);
        spar.S[0][1].push_back(tempS12);
        complex_t tempS22;
        if(sFormat==DB)tempS22=complex_t(pow(10,firstS22/20),0)*exp(complex_t(0,secondS22)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS22=complex_t(firstS22,0)*exp(complex_t(0,secondS22)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS22=complex_t(firstS22,secondS22);
        spar.S[1][1].push_back(tempS22);
    }
    File.close();
    bool is2port=false;
    bool is4port=false;
    for(int i=0;i<(int)spar.S[0][0].size();i++)
    {
        for (int nx=0;nx<2;nx++)
        {
            for (int ny=0;ny<2;ny++)
            if(abs(spar.S[nx][ny][i])>1e-20 && nx*ny>0)
            {
                //if(nx>1 || ny >1)is4port=true;
                is2port=true;
                i=spar.S[0][0].size();
            }
        }
    }
    if(!is4port)
    {
        spar.S.resize(2);
        spar.S[0].resize(2);
        spar.S[1].resize(2);
    }
    if(!is2port)
    {
        spar.S.resize(1);
        spar.S[0].resize(1);
    }

    return spar;
}

spar_t TouchstoneFile::Load3P(const char* filename)
{
    ifstream File;
    spar_t spar;
    File.open(filename);
    if(!File.good())return spar;

    spar.S.resize(3);
    for(int i=0; i<3; i++)
        spar.S[i].resize(3);
    FUnit fUnit=GHz;
    SFormat sFormat=MA;
    double R;
    double freq, firstS11, secondS11,firstS12, secondS12, firstS13, secondS13,
                 firstS21, secondS21,firstS22, secondS22, firstS23, secondS23,
                 firstS31, secondS31,firstS32, secondS32, firstS33, secondS33;

    char rawline[4096];
    char* line;
    while(File.getline(rawline,4095))
    {
        line=rawline;
        while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        if(line[0]=='!'||line[0]=='\0')continue; //ignore comment lines or emty lines
        if(line[0]=='#')
        {
            while(*line != '\0' && (isspace(*line)||*line=='#'))line++; //strip leading '#' and spaces

        //	# <frequency unit> <parameter> <format> R <n>
            char sFUnit[256], sSFormat[256],  dontcare[256];
            sscanf(line, "%s %s %s %s %lf", sFUnit, dontcare, sSFormat, dontcare, &R);
            if(strcasecmp(sFUnit,"GHZ")==0)fUnit=GHz;
            if(strcasecmp((sFUnit),"MHZ")==0)fUnit=MHz;
            if(strcasecmp((sFUnit),"KHZ")==0)fUnit=KHz;
            if(strcasecmp((sFUnit),"HZ")==0)fUnit=Hz;

            if(strcasecmp((sSFormat),"DB")==0)sFormat=DB;
            if(strcasecmp((sSFormat),"MA")==0)sFormat=MA;
            if(strcasecmp((sSFormat),"RI")==0)sFormat=RI;
            continue;
        }
        sscanf(line, "%lf %lf %lf %lf %lf %lf %lf", &freq, &firstS11, &secondS11, &firstS12, &secondS12, &firstS13, &secondS13);

        if(!File.getline(rawline,4095))break;
        line=rawline;
        while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        while(line[0]=='!'||line[0]=='\0') //ignore comment lines or empty lines
        {
            if(!File.getline(rawline,4095))break;
            line=rawline;
            while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        }
        sscanf(line, "%lf %lf %lf %lf %lf %lf", &firstS21, &secondS21, &firstS22, &secondS22, &firstS23, &secondS23);

        if(!File.getline(rawline,4095))break;
        line=rawline;
        while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        while(line[0]=='!'||line[0]=='\0') //ignore comment lines or empty lines
        {
            if(!File.getline(rawline,4095))break;
            line=rawline;
            while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        }
        sscanf(line, "%lf %lf %lf %lf %lf %lf", &firstS31, &secondS31, &firstS32, &secondS32, &firstS33, &secondS33);

        if(fUnit==GHz)freq*=1e9;
        if(fUnit==MHz)freq*=1e6;
        if(fUnit==KHz)freq*=1e3;
        if(spar.f.size()>0&&freq<spar.f.back()){cout<<"Ignoring Noise Parameters in file"<<endl;break;} //either error or noise parameters
        spar.f.push_back(freq);
        complex_t tempS11;
        if(sFormat==DB)tempS11=complex_t(pow(10,firstS11/20),0)*exp(complex_t(0,secondS11)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS11=complex_t(firstS11,0)*exp(complex_t(0,secondS11)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS11=complex_t(firstS11,secondS11);
        spar.S[0][0].push_back(tempS11);
        complex_t tempS12;
        if(sFormat==DB)tempS12=complex_t(pow(10,firstS12/20),0)*exp(complex_t(0,secondS12)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS12=complex_t(firstS12,0)*exp(complex_t(0,secondS12)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS12=complex_t(firstS12,secondS12);
        spar.S[0][1].push_back(tempS12);
        complex_t tempS13;
        if(sFormat==DB)tempS13=complex_t(pow(10,firstS13/20),0)*exp(complex_t(0,secondS13)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS13=complex_t(firstS13,0)*exp(complex_t(0,secondS13)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS13=complex_t(firstS13,secondS13);
        spar.S[0][2].push_back(tempS13);
        complex_t tempS21;
        if(sFormat==DB)tempS21=complex_t(pow(10,firstS21/20),0)*exp(complex_t(0,secondS21)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS21=complex_t(firstS21,0)*exp(complex_t(0,secondS21)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS21=complex_t(firstS21,secondS21);
        spar.S[1][0].push_back(tempS21);
        complex_t tempS22;
        if(sFormat==DB)tempS22=complex_t(pow(10,firstS22/20),0)*exp(complex_t(0,secondS22)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS22=complex_t(firstS22,0)*exp(complex_t(0,secondS22)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS22=complex_t(firstS22,secondS22);
        spar.S[1][1].push_back(tempS22);
        complex_t tempS23;
        if(sFormat==DB)tempS23=complex_t(pow(10,firstS23/20),0)*exp(complex_t(0,secondS23)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS23=complex_t(firstS23,0)*exp(complex_t(0,secondS23)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS23=complex_t(firstS23,secondS23);
        spar.S[1][2].push_back(tempS23);
        complex_t tempS31;
        if(sFormat==DB)tempS31=complex_t(pow(10,firstS31/20),0)*exp(complex_t(0,secondS31)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS31=complex_t(firstS31,0)*exp(complex_t(0,secondS31)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS31=complex_t(firstS31,secondS31);
        spar.S[2][0].push_back(tempS31);
        complex_t tempS32;
        if(sFormat==DB)tempS32=complex_t(pow(10,firstS32/20),0)*exp(complex_t(0,secondS32)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS32=complex_t(firstS32,0)*exp(complex_t(0,secondS32)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS32=complex_t(firstS32,secondS32);
        spar.S[2][1].push_back(tempS32);
        complex_t tempS33;
        if(sFormat==DB)tempS33=complex_t(pow(10,firstS33/20),0)*exp(complex_t(0,secondS33)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS33=complex_t(firstS33,0)*exp(complex_t(0,secondS33)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS33=complex_t(firstS33,secondS33);
        spar.S[2][2].push_back(tempS33);
    }
    File.close();
    bool is2port=false;
    bool is3port=false;
    for(int i=0;i<(int)spar.S[1].size();i++)
    {
        for (int nx=0;nx<3;nx++)
        {
            for (int ny=0;ny<3;ny++)
            if(abs(spar.S[nx][ny][i])>1e-20 && nx*ny>0)
            {
                if(nx>1 || ny >1)is3port=true;
                is2port=true;
                i=spar.S[1].size();
            }
        }
    }
    if(!is3port)
    {
        spar.S.resize(2);
        spar.S[0].resize(2);
        spar.S[1].resize(2);
    }
    if(!is2port)
    {
        spar.S.resize(1);
        spar.S[0].resize(1);
    }

    return spar;
}

spar_t TouchstoneFile::Load4P(const char* filename)
{
    ifstream File;
    spar_t spar;
    File.open(filename);
    if(!File.good())return spar;

    spar.S.resize(4);
    for(int i=0; i<4; i++)
        spar.S[i].resize(4);
    FUnit fUnit=GHz;
    SFormat sFormat=MA;
    double R;
    double freq, firstS11, secondS11,firstS12, secondS12, firstS13, secondS13,firstS14, secondS14,
                 firstS21, secondS21,firstS22, secondS22, firstS23, secondS23,firstS24, secondS24,
                 firstS31, secondS31,firstS32, secondS32, firstS33, secondS33,firstS34, secondS34,
                 firstS41, secondS41,firstS42, secondS42, firstS43, secondS43,firstS44, secondS44;

    char rawline[4096];
    char* line;
    while(File.getline(rawline,4095))
    {
        line=rawline;
        while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        if(line[0]=='!'||line[0]=='\0')continue; //ignore comment lines or emty lines
        if(line[0]=='#')
        {
            while(*line != '\0' && (isspace(*line)||*line=='#'))line++; //strip leading '#' and spaces

        //	# <frequency unit> <parameter> <format> R <n>
            char sFUnit[256], sSFormat[256],  dontcare[256];
            sscanf(line, "%s %s %s %s %lf", sFUnit, dontcare, sSFormat, dontcare, &R);
            if(strcasecmp(sFUnit,"GHZ")==0)fUnit=GHz;
            if(strcasecmp((sFUnit),"MHZ")==0)fUnit=MHz;
            if(strcasecmp((sFUnit),"KHZ")==0)fUnit=KHz;
            if(strcasecmp((sFUnit),"HZ")==0)fUnit=Hz;

            if(strcasecmp((sSFormat),"DB")==0)sFormat=DB;
            if(strcasecmp((sSFormat),"MA")==0)sFormat=MA;
            if(strcasecmp((sSFormat),"RI")==0)sFormat=RI;
            continue;
        }
        sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf", &freq, &firstS11, &secondS11, &firstS12, &secondS12, &firstS13, &secondS13, &firstS14, &secondS14);

        if(!File.getline(rawline,4095))break;
        line=rawline;
        while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        while(line[0]=='!'||line[0]=='\0') //ignore comment lines or empty lines
        {
            if(!File.getline(rawline,4095))break;
            line=rawline;
            while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        }
        sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf", &firstS21, &secondS21, &firstS22, &secondS22, &firstS23, &secondS23, &firstS24, &secondS24);

        if(!File.getline(rawline,4095))break;
        line=rawline;
        while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        while(line[0]=='!'||line[0]=='\0') //ignore comment lines or empty lines
        {
            if(!File.getline(rawline,4095))break;
            line=rawline;
            while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        }
        sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf", &firstS31, &secondS31, &firstS32, &secondS32, &firstS33, &secondS33, &firstS34, &secondS34);

        if(!File.getline(rawline,4095))break;
        line=rawline;
        while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        while(line[0]=='!'||line[0]=='\0') //ignore comment lines or empty lines
        {
            if(!File.getline(rawline,4095))break;
            line=rawline;
            while(*line != '\0' && isspace(*line))line++; //strip leading spaces
        }
        sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf", &firstS41, &secondS41, &firstS42, &secondS42, &firstS43, &secondS43, &firstS44, &secondS44);


        if(fUnit==GHz)freq*=1e9;
        if(fUnit==MHz)freq*=1e6;
        if(fUnit==KHz)freq*=1e3;
        if(spar.f.size()>0&&freq<spar.f.back()){cout<<"Ignoring Noise Parameters in file"<<endl;break;} //either error or noise parameters
        spar.f.push_back(freq);
        complex_t tempS11;
        if(sFormat==DB)tempS11=complex_t(pow(10,firstS11/20),0)*exp(complex_t(0,secondS11)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS11=complex_t(firstS11,0)*exp(complex_t(0,secondS11)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS11=complex_t(firstS11,secondS11);
        spar.S[0][0].push_back(tempS11);
        complex_t tempS12;
        if(sFormat==DB)tempS12=complex_t(pow(10,firstS12/20),0)*exp(complex_t(0,secondS12)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS12=complex_t(firstS12,0)*exp(complex_t(0,secondS12)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS12=complex_t(firstS12,secondS12);
        spar.S[0][1].push_back(tempS12);
        complex_t tempS13;
        if(sFormat==DB)tempS13=complex_t(pow(10,firstS13/20),0)*exp(complex_t(0,secondS13)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS13=complex_t(firstS13,0)*exp(complex_t(0,secondS13)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS13=complex_t(firstS13,secondS13);
        spar.S[0][2].push_back(tempS13);
        complex_t tempS14;
        if(sFormat==DB)tempS14=complex_t(pow(10,firstS14/20),0)*exp(complex_t(0,secondS14)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS14=complex_t(firstS14,0)*exp(complex_t(0,secondS14)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS14=complex_t(firstS14,secondS14);
        spar.S[0][3].push_back(tempS14);
        complex_t tempS21;
        if(sFormat==DB)tempS21=complex_t(pow(10,firstS21/20),0)*exp(complex_t(0,secondS21)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS21=complex_t(firstS21,0)*exp(complex_t(0,secondS21)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS21=complex_t(firstS21,secondS21);
        spar.S[1][0].push_back(tempS21);
        complex_t tempS22;
        if(sFormat==DB)tempS22=complex_t(pow(10,firstS22/20),0)*exp(complex_t(0,secondS22)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS22=complex_t(firstS22,0)*exp(complex_t(0,secondS22)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS22=complex_t(firstS22,secondS22);
        spar.S[1][1].push_back(tempS22);
        complex_t tempS23;
        if(sFormat==DB)tempS23=complex_t(pow(10,firstS23/20),0)*exp(complex_t(0,secondS23)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS23=complex_t(firstS23,0)*exp(complex_t(0,secondS23)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS23=complex_t(firstS23,secondS23);
        spar.S[1][2].push_back(tempS23);
        complex_t tempS24;
        if(sFormat==DB)tempS24=complex_t(pow(10,firstS24/20),0)*exp(complex_t(0,secondS24)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS24=complex_t(firstS24,0)*exp(complex_t(0,secondS24)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS24=complex_t(firstS24,secondS24);
        spar.S[1][3].push_back(tempS24);
        complex_t tempS31;
        if(sFormat==DB)tempS31=complex_t(pow(10,firstS31/20),0)*exp(complex_t(0,secondS31)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS31=complex_t(firstS31,0)*exp(complex_t(0,secondS31)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS31=complex_t(firstS31,secondS31);
        spar.S[2][0].push_back(tempS31);
        complex_t tempS32;
        if(sFormat==DB)tempS32=complex_t(pow(10,firstS32/20),0)*exp(complex_t(0,secondS32)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS32=complex_t(firstS32,0)*exp(complex_t(0,secondS32)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS32=complex_t(firstS32,secondS32);
        spar.S[2][1].push_back(tempS32);
        complex_t tempS33;
        if(sFormat==DB)tempS33=complex_t(pow(10,firstS33/20),0)*exp(complex_t(0,secondS33)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS33=complex_t(firstS33,0)*exp(complex_t(0,secondS33)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS33=complex_t(firstS33,secondS33);
        spar.S[2][2].push_back(tempS33);
        complex_t tempS34;
        if(sFormat==DB)tempS34=complex_t(pow(10,firstS34/20),0)*exp(complex_t(0,secondS34)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS34=complex_t(firstS34,0)*exp(complex_t(0,secondS34)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS34=complex_t(firstS34,secondS34);
        spar.S[2][3].push_back(tempS34);
        complex_t tempS41;
        if(sFormat==DB)tempS41=complex_t(pow(10,firstS41/20),0)*exp(complex_t(0,secondS41)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS41=complex_t(firstS41,0)*exp(complex_t(0,secondS41)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS41=complex_t(firstS41,secondS41);
        spar.S[3][0].push_back(tempS41);
        complex_t tempS42;
        if(sFormat==DB)tempS42=complex_t(pow(10,firstS42/20),0)*exp(complex_t(0,secondS42)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS42=complex_t(firstS42,0)*exp(complex_t(0,secondS42)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS42=complex_t(firstS42,secondS42);
        spar.S[3][1].push_back(tempS42);
        complex_t tempS43;
        if(sFormat==DB)tempS43=complex_t(pow(10,firstS43/20),0)*exp(complex_t(0,secondS43)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS43=complex_t(firstS43,0)*exp(complex_t(0,secondS43)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS43=complex_t(firstS43,secondS43);
        spar.S[3][2].push_back(tempS43);
        complex_t tempS44;
        if(sFormat==DB)tempS44=complex_t(pow(10,firstS44/20),0)*exp(complex_t(0,secondS44)*complex_t(M_PI/180,0));
        if(sFormat==MA)tempS44=complex_t(firstS44,0)*exp(complex_t(0,secondS44)*complex_t(M_PI/180,0));
        if(sFormat==RI)tempS44=complex_t(firstS44,secondS44);
        spar.S[3][3].push_back(tempS44);
    }
    File.close();
    bool is2port=false;
    bool is4port=false;
    for(int i=0;i<(int)spar.S[1].size();i++)
    {
        for (int nx=0;nx<4;nx++)
        {
            for (int ny=0;ny<4;ny++)
            if(abs(spar.S[nx][ny][i])>1e-20 && nx*ny>0)
            {
                if(nx>1 || ny >1)is4port=true;
                is2port=true;
                i=spar.S[1].size();
            }
        }
    }
    if(!is4port)
    {
        spar.S.resize(2);
        spar.S[0].resize(2);
        spar.S[1].resize(2);
    }
    if(!is2port)
    {
        spar.S.resize(1);
        spar.S[0].resize(1);
    }

    return spar;
}

