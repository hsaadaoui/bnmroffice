/***************************************************************************
*  File: simenergygap.cpp                                                  *
*  Description: performs energy gap simulation and fitting.                *
*  Author: Hassan Saadaoui                                                 *
*  e-mail: saadaoui@triumf.ca                                              *
*  $Id: mainwindow.h 2014-12-20 $                                          *
*  Copyright (C) 2014 by Hassan Saadaoui                                   *
***************************************************************************/
/***************************************************************************
    This file is part of BnmrOffice.

    BnmrOffice is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BnmrOffice  is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BnmrOffice .  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

/***************************************************************************
List of Changes:
----------------------------------------------------------------------------
TYPE  DATE        AUTHOR  DESCRIBE
***************************************************************************/



#include <iostream>                             //Header file for outputting data
#include <fstream>                              //Header file for outputting data
#include <math.h>                               //Header file for math operations
#include <cassert>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
using namespace std;                             // Declared for output files
#include "constants.h"
#include "energygap.h"

const double    kB    =   0.08617384436; //kB in meV/K;
const double    PI    =   3.14159265358979323846;   /* pi */

double ClassGAP::operator ()(double x) const
{
  if(fct() == "Sigma_vs_temp"  )
    return sigmaT(x,gap(),sigmab(),par());
  if(fct() == "Sigma_vs_field"  )
    return sigmaB(x,sigmab(),par());
  if(fct() == "Lambda_vs_temp" )
    return LambdaT(x,gap(),par());
}


//This formula is found from fitting Bc2 vs T.
double Bc2(double T,double Tc)
{
  if( T < Tc )
    return 7865*pow(1.0-pow(T/Tc,1.075),0.877);
  else
    return       0.0;
}

//This formula is WHH curve that fits Bc2 (found from sigma B vs. B) vs T:
double Hc2(double T,double Tc)
{
  double bc2 = 10000.0*(0.7016 - 0.02843*T -0.02873*T*T + 0.001852*T*T*T);
  if (T < Tc)
    return bc2;
  else
    return 0.0;
}

//This is RhoSwave formulat taken from Prozorov et al.
double RhoSwave(double T,double D0,double Tc)
{
  //Numerical parameters
  int      N  = 10000;
  double   Ec = 100.0;
  double   dE = 1.0/N;
  double  integral = 0.0;

  double   DT = D0*tanh(1.82*pow(1.018*(Tc/T - 1.0),0.51));//Gap for s-wave
  for (double E = 0.0; E < 1.0; E += dE )
    integral += pow(cosh(sqrt(Ec*Ec*E*E+DT*DT)/(2.0*kB*T)),-2.0);
  return    1.0 -  dE*Ec*integral/(2.0*kB*T);//== Rho
}

//This is RhoSwave formulat taken from Prozorov et al.
double RhoDwave(double T,double D0,double Tc)
{
  //Numerical parameters
  int    N  = 2000;
  double Ec = 100.0;
  double dE = 1.0/N;
  double df = 2*PI/N;
  double g  = 4.0/3.0;

  double rho      = 0.0;
  double integral = 0.0;
  double DT       = 0.0;

  for (double f = 0.0; f < 1.0; f += df )
    {
      double phi = f*2.0*PI;
      DT = g*D0*cos(2.0*phi)*tanh(1.82*pow(1.018*(Tc/T - 1.0),0.51));//Gap for d-wave
      for (double E= 0.0; E < 1.0; E += dE )
        integral += cos(phi)*cos(phi)*pow(cosh(sqrt(Ec*Ec*E*E+DT*DT)/(2.0*kB*T)),-2.0);
    }
  return  1.0 -  df*dE*Ec*integral/(kB*T);//== Rho
}



//This is RhoSwave formulat taken from Prozorov et al.
double RhoDirty(double T,double D0,double Tc)
{
  double   DT = D0*tanh(1.82*pow(1.018*(Tc/T - 1.0),0.51));//Gap for s-wave
  return    (DT/D0)*tanh(DT/(2.0*kB*T));//== Rho
}



//This is RhoSwave formulat taken from Prozorov et al
//D0=1.19*Tc, a =0.38, g=1.43*cos(2phi) +0.43*cos(6*phi)
double RhoDwaveNonMono(double T,double D0,double Tc)
{
  //Numerical parameters
  int    N  = 2000;
  double Ec = 100.0;
  double dE = 1.0/N;
  double df = 2*PI/N;
  double a  = 0.38;

  double rho      = 0.0;
  double integral = 0.0;
  double DT       = 0.0;

  for (double f = 0.0; f < 1.0; f += df )
    {
      double phi = f*2.0*PI;
      DT = D0*tanh((PI*Tc/D0)*pow(a*(Tc/T - 1.0),0.5))*(1.43*cos(2.0*phi)+0.43*cos(6*phi));//Gap for d-wave
      for (double E= 0.0; E < 1.0; E += dE )
        integral += cos(phi)*cos(phi)*pow(cosh(sqrt(Ec*Ec*E*E+DT*DT)/(2.0*kB*T)),-2.0);
    }
  return  1.0 -  df*dE*Ec*integral/(kB*T);//== Rho
}




double rhoGap(double T,string gapChoice, vector<double> par)
{
    double D0       =  par.at(0);
    double Tc       =  par.at(1);
    double B        =  par.at(2);
    double Bc2      =  par.at(3);
    double b        =  B/Bc2;
    double rho      =  0.0;

    if ( T < Tc && b < 1.0)
    {
        if(gapChoice.compare("s_wave") == 0)
            rho      =  RhoSwave(T,D0,Tc);
        if(gapChoice.compare("d_wave") == 0)
            rho      =  RhoDwave(T,D0,Tc);
        if(gapChoice.compare("d_wave_nonmonotonic") == 0 )
            rho      =  RhoDwaveNonMono(T,D0,Tc);
        if(gapChoice.compare("s_wave_dirty") == 0 )
            rho      =  RhoDirty(T,D0,Tc);
    }
    return rho;
}

//Return sigma vs B using London or GL models
double sigmaBML(double b, double lambda)
{
  return 48200*(1.0 - b)*sqrt(1.0+3.9*pow(1.0-b,2.0))/(lambda*lambda);
}

//Originally it was  return 48300*(1.0 - b)*(1.0+1.39*pow(1.0-b,2.0))/(lambda*lambda);
double sigmaBGL(double b, double lambda)
{
  return  48200*(1.0 - b)*(1.0+1.21*pow(1.0-sqrt(b),3.0))/(lambda*lambda);
}

//Returns sigma vs B using GL or ML models
double sigmaB(double B,string sigmaBChoice, vector<double> par)
{
  double BC2      =  par.at(0);
  double lambda   =  par.at(1);
  double sigmaB   =  0.0;
  if( B < BC2 )
    {
      if( sigmaBChoice == "Ginsburg_Landau" ) //Ginsburg Landau model
        sigmaB    =  sigmaBGL(B/BC2,lambda);
      if( sigmaBChoice == "Modified_London" ) //Modified London model
        sigmaB    =  sigmaBML(B/BC2,lambda);
    }
  else
    sigmaB = 0.0;
  return sigmaB;
}

//returns sigmaT vs T of s or d wave functions
double sigmaT(double T,string gapChoice, string sigmaBChoice,vector<double> par)
{
  double B        =  par.at(2);
  double sigma_ns =  par.at(5);
  vector<double> par2;
  par2.push_back(par.at(3));
  par2.push_back(par.at(4));
  double rho      =  rhoGap(T,gapChoice,par);
  double sigma_0  =  sigmaB(B,sigmaBChoice,par2);
  return sqrt(sigma_ns*sigma_ns + sigma_0*sigma_0*rho*rho);
}

//returns lambda vs T of s or d wave functions
double LambdaT(double T,string gapChoice,vector<double> par)
{
    double lambda0   =  par.at(4);
    double rho       =  rhoGap(T,gapChoice,par);
    double output = 0.0;
    if(rho != 0.0 )
        output  = lambda0/sqrt(rho) ;
    return output;
}
